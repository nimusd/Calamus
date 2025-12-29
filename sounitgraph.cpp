#include "sounitgraph.h"
#include <QDebug>
#include <QSet>
#include <cmath>

// Helper function to apply connection functions
static double applyConnectionFunction(double currentValue, double sourceValue,
                                     const QString& function, double weight)
{
    if (function == "passthrough") {
        // Source replaces input entirely (ignores weight)
        return sourceValue;
    } else if (function == "add") {
        // Input + source × weight
        return currentValue + sourceValue * weight;
    } else if (function == "multiply") {
        // Input × source × weight
        return currentValue * sourceValue * weight;
    } else if (function == "subtract") {
        // Input - source × weight
        return currentValue - sourceValue * weight;
    } else if (function == "replace") {
        // Weighted blend: input × (1-weight) + source × weight
        return currentValue * (1.0 - weight) + sourceValue * weight;
    } else if (function == "modulate") {
        // Bipolar modulation around input: input + (source - 0.5) × weight × range
        // Using range = 2.0 for full bipolar range
        return currentValue + (sourceValue - 0.5) * weight * 2.0;
    }

    // Unknown function, default to passthrough
    return sourceValue;
}

SounitGraph::SounitGraph(double sampleRate)
    : sampleRate(sampleRate)
    , hasValidSignalOutput(false)
{
}

void SounitGraph::buildFromCanvas(Canvas *canvas)
{
    qDebug() << "SounitGraph::buildFromCanvas - START";

    processors.clear();
    executionOrder.clear();
    signalOutputContainer = nullptr;
    hasValidSignalOutput = false;

    if (!canvas) {
        qDebug() << "SounitGraph::buildFromCanvas - canvas is null!";
        return;
    }

    qDebug() << "SounitGraph: Finding containers...";
    // Create processor data for each container
    QList<Container*> containers = canvas->findChildren<Container*>();
    qDebug() << "SounitGraph: Found" << containers.size() << "containers";

    for (Container *container : containers) {
        qDebug() << "  - Container:" << container->getName();
        ProcessorData data;
        data.container = container;
        processors[container] = data;
    }

    qDebug() << "SounitGraph: Computing execution order...";
    // Compute execution order
    computeExecutionOrder(canvas);

    qDebug() << "SounitGraph: Creating processors...";
    // Create processor instances
    createProcessors();

    qDebug() << "SounitGraph: Built graph with" << processors.size() << "containers";
    qDebug() << "SounitGraph: Execution order:" << executionOrder.size() << "containers";
    qDebug() << "SounitGraph: Valid signal output:" << hasValidSignalOutput;
}

void SounitGraph::computeExecutionOrder(Canvas *canvas)
{
    // Proper topological sort using Kahn's algorithm
    // This follows actual connections instead of fixed type-based ordering

    if (!canvas) {
        qDebug() << "SounitGraph::computeExecutionOrder - canvas is null";
        return;
    }

    // Build dependency graph
    // Key: container, Value: set of containers that depend on it (outgoing edges)
    QMap<Container*, QSet<Container*>> dependents;
    // Key: container, Value: number of incoming edges (dependencies)
    QMap<Container*, int> incomingEdgeCount;

    // Initialize all containers with zero incoming edges
    for (Container *container : processors.keys()) {
        incomingEdgeCount[container] = 0;
        dependents[container] = QSet<Container*>();
    }

    // Build the graph from connections
    const QVector<Canvas::Connection>& connections = canvas->getConnections();
    for (const Canvas::Connection &conn : connections) {
        // Connection goes from output to input, so fromContainer must execute before toContainer
        if (processors.contains(conn.fromContainer) && processors.contains(conn.toContainer)) {
            // Add edge: fromContainer → toContainer
            dependents[conn.fromContainer].insert(conn.toContainer);
            incomingEdgeCount[conn.toContainer]++;
        }
    }

    // Kahn's algorithm: Start with containers that have no dependencies
    QVector<Container*> queue;
    for (Container *container : processors.keys()) {
        if (incomingEdgeCount[container] == 0) {
            queue.append(container);
        }
    }

    // Process queue
    executionOrder.clear();
    while (!queue.isEmpty()) {
        // Remove a container with no incoming edges
        Container *current = queue.takeFirst();
        executionOrder.append(current);

        // Remove edges from current to its dependents
        for (Container *dependent : dependents[current]) {
            incomingEdgeCount[dependent]--;
            if (incomingEdgeCount[dependent] == 0) {
                queue.append(dependent);
            }
        }
    }

    // Check for cycles
    if (executionOrder.size() != processors.size()) {
        qDebug() << "SounitGraph::computeExecutionOrder - WARNING: Cycle detected!";
        qDebug() << "  Processed" << executionOrder.size() << "containers, but" << processors.size() << "total";
        qDebug() << "  Containers in cycle:";
        for (Container *container : processors.keys()) {
            if (!executionOrder.contains(container)) {
                qDebug() << "    -" << container->getName() << "(" << container->getInstanceName() << ")";
            }
        }
        // Continue anyway with partial order, but mark as invalid
        hasValidSignalOutput = false;
        return;
    }

    // Find the signal output container
    // Look for containers with "signalOut" port that have no outgoing signal connections
    // or simply the last container in execution order that has a signalOut port
    signalOutputContainer = nullptr;

    // Check containers in reverse execution order (last executed first)
    for (int i = executionOrder.size() - 1; i >= 0; i--) {
        Container *container = executionOrder[i];
        QStringList outputs = container->getOutputPorts();

        // Check if this container has a signalOut port
        if (outputs.contains("signalOut")) {
            // Check if this port has any outgoing connections
            bool hasOutgoingSignal = false;
            for (const Canvas::Connection &conn : connections) {
                if (conn.fromContainer == container && conn.fromPort == "signalOut") {
                    hasOutgoingSignal = true;
                    break;
                }
            }

            // If no outgoing signal connection, this is likely the final output
            if (!hasOutgoingSignal) {
                signalOutputContainer = container;
                hasValidSignalOutput = true;
                qDebug() << "SounitGraph: Signal output container:" << container->getName()
                         << "(" << container->getInstanceName() << ")";
                break;
            }
        }
    }

    // Debug output
    qDebug() << "SounitGraph: Topological sort complete";
    qDebug() << "  Execution order:";
    for (int i = 0; i < executionOrder.size(); i++) {
        Container *c = executionOrder[i];
        qDebug() << "    " << (i+1) << "." << c->getName() << "(" << c->getInstanceName() << ")";
    }

    if (!hasValidSignalOutput) {
        qDebug() << "SounitGraph: WARNING - No valid signal output found!";
    }
}

void SounitGraph::createProcessors()
{
    for (auto it = processors.begin(); it != processors.end(); ++it) {
        Container *container = it.key();
        ProcessorData &data = it.value();

        if (container->getName() == "Harmonic Generator") {
            data.harmonicGen = new HarmonicGenerator(sampleRate);

            int dnaSelect = static_cast<int>(container->getParameter("dnaSelect", 0.0));

            // Check if using custom DNA pattern
            if (dnaSelect == -1) {
                // Load custom DNA pattern from container
                int customDnaCount = static_cast<int>(container->getParameter("customDnaCount", 0.0));

                if (customDnaCount > 0) {
                    std::vector<double> customAmplitudes;
                    customAmplitudes.reserve(customDnaCount);

                    for (int i = 0; i < customDnaCount; i++) {
                        QString paramName = QString("customDna_%1").arg(i);
                        double amp = container->getParameter(paramName, 0.0);
                        customAmplitudes.push_back(amp);
                    }

                    qDebug() << "Loading custom DNA with" << customDnaCount << "harmonics";
                    data.harmonicGen->setCustomAmplitudes(customAmplitudes);
                } else {
                    // No custom pattern stored, fall back to rolloff-based generation
                    qDebug() << "Custom DNA selected but no pattern stored, using rolloff";
                    data.harmonicGen->setNumHarmonics(
                        static_cast<int>(container->getParameter("numHarmonics", 64.0)));
                    data.harmonicGen->setRolloffPower(container->getParameter("rolloff", 1.82));
                    data.harmonicGen->setDnaPreset(-1);
                }
            } else {
                // Using preset DNA
                data.harmonicGen->setNumHarmonics(
                    static_cast<int>(container->getParameter("numHarmonics", 64.0)));
                data.harmonicGen->setRolloffPower(container->getParameter("rolloff", 1.82));
                data.harmonicGen->setDnaPreset(dnaSelect);
            }

            // Note: purity and drift are now controlled via input ports, not stored parameters

        } else if (container->getName() == "Rolloff Processor") {
            data.rolloffProc = new RolloffProcessor();
            data.rolloffProc->setRolloffPower(container->getParameter("rolloff", 0.6));

        } else if (container->getName() == "Spectrum to Signal") {
            data.spectrumToSig = new SpectrumToSignal(sampleRate);
            data.spectrumToSig->setNormalize(container->getParameter("normalize", 1.0));

        } else if (container->getName() == "Formant Body") {
            data.formantBody = new FormantBody(sampleRate);
            data.formantBody->setF1Freq(container->getParameter("f1Freq", 500.0));
            data.formantBody->setF2Freq(container->getParameter("f2Freq", 1500.0));
            data.formantBody->setF1Q(container->getParameter("f1Q", 8.0));
            data.formantBody->setF2Q(container->getParameter("f2Q", 10.0));
            data.formantBody->setDirectMix(container->getParameter("directMix", 0.3));
            data.formantBody->setF1F2Balance(container->getParameter("f1f2Balance", 0.6));

        } else if (container->getName() == "Breath Turbulence") {
            data.breathTurb = new BreathTurbulence();
            data.breathTurb->setBlend(container->getParameter("blend", 0.10));
            // For now, use default blend curve (sqrt)

        } else if (container->getName() == "Noise Color Filter") {
            data.noiseFilter = new NoiseColorFilter(sampleRate);
            data.noiseFilter->setColor(container->getParameter("color", 2000.0));
            data.noiseFilter->setFilterQ(container->getParameter("filterQ", 1.0));

            // Set noise type
            int noiseTypeValue = static_cast<int>(container->getParameter("noiseType", 0.0));
            data.noiseFilter->setNoiseType(static_cast<NoiseColorFilter::NoiseType>(noiseTypeValue));

            // For now, use default filter type (highpass)

        } else if (container->getName() == "Physics System") {
            data.physicsSys = new PhysicsSystem();
            data.physicsSys->setMass(container->getParameter("mass", 0.5));
            data.physicsSys->setSpringK(container->getParameter("springK", 0.001));
            data.physicsSys->setDamping(container->getParameter("damping", 0.995));
            data.physicsSys->setImpulseAmount(container->getParameter("impulseAmount", 100.0));

        } else if (container->getName() == "Envelope Engine") {
            data.envelopeEng = new EnvelopeEngine();
            int envSelect = static_cast<int>(container->getParameter("envelopeSelect", 0.0));

            // If custom envelope is selected (index 5), set envelope type to Custom
            // and load the custom envelope data
            if (envSelect == 5 && container->hasCustomEnvelopeData()) {
                data.envelopeEng->setEnvelopeType(EnvelopeEngine::EnvelopeType::Custom);
                EnvelopeData customData = container->getCustomEnvelopeData();
                data.envelopeEng->setCustomEnvelope(customData.points);
            } else {
                // Standard envelope types (0-4)
                data.envelopeEng->setEnvelopeSelect(envSelect);
            }

            data.envelopeEng->setTimeScale(container->getParameter("timeScale", 1.0));
            data.envelopeEng->setValueScale(container->getParameter("valueScale", 1.0));
            data.envelopeEng->setValueOffset(container->getParameter("valueOffset", 0.0));

        } else if (container->getName() == "Drift Engine") {
            data.driftEng = new DriftEngine(sampleRate);
            data.driftEng->setAmount(container->getParameter("amount", 0.005));
            data.driftEng->setRate(container->getParameter("rate", 0.5));

            // Set drift pattern
            int patternValue = static_cast<int>(container->getParameter("driftPattern", 2.0));
            data.driftEng->setDriftPattern(static_cast<DriftEngine::DriftPattern>(patternValue));

        } else if (container->getName() == "Gate Processor") {
            data.gateProc = new GateProcessor(sampleRate);
            data.gateProc->setVelocity(container->getParameter("velocity", 1.0));
            data.gateProc->setAttackTime(container->getParameter("attackTime", 0.01));
            data.gateProc->setReleaseTime(container->getParameter("releaseTime", 0.1));
            data.gateProc->setAttackCurve(static_cast<int>(container->getParameter("attackCurve", 0.0)));
            data.gateProc->setReleaseCurve(static_cast<int>(container->getParameter("releaseCurve", 0.0)));
            data.gateProc->setVelocitySens(container->getParameter("velocitySens", 0.5));

        } else if (container->getName() == "Easing Applicator") {
            data.easingApp = new EasingApplicator();
            data.easingApp->setEasingSelect(static_cast<int>(container->getParameter("easingSelect", 0.0)));
            // For now, use default easing mode (InOut)
        }
    }
}

void SounitGraph::reset()
{
    for (auto &data : processors) {
        if (data.harmonicGen) {
            data.harmonicGen->reset();
        }
        if (data.spectrumToSig) {
            data.spectrumToSig->reset();
        }
        if (data.formantBody) {
            data.formantBody->reset();
        }
        if (data.noiseFilter) {
            data.noiseFilter->reset();
        }
        if (data.physicsSys) {
            data.physicsSys->reset();
        }
        if (data.driftEng) {
            data.driftEng->reset();
        }
        if (data.gateProc) {
            data.gateProc->reset();
            // Trigger note on when starting a new note
            data.gateProc->noteOn(1.0);
        }
        // RolloffProcessor and BreathTurbulence have no state to reset
    }
}

double SounitGraph::generateSample(double pitch, double noteProgress)
{
    if (!hasValidSignalOutput) {
        return 0.0;
    }

    // Execute containers in order
    for (Container *container : executionOrder) {
        if (processors.contains(container)) {
            executeContainer(processors[container], pitch, noteProgress);
        }
    }

    // Return final signal output
    if (signalOutputContainer && processors.contains(signalOutputContainer)) {
        return processors[signalOutputContainer].signalOut;
    }

    return 0.0;
}

void SounitGraph::executeContainer(ProcessorData &proc, double pitch, double noteProgress)
{
    Container *container = proc.container;
    if (!container) return;

    if (container->getName() == "Harmonic Generator") {
        if (proc.harmonicGen) {
            // Initialize input values with defaults
            double purityValue = 1.0;  // Default purity
            double driftValue = 0.0;   // Default drift

            // Check for connections to purity and drift ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "purity") {
                            // Get purity modulation from source container (control output 0.0-1.0)
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                purityValue = applyConnectionFunction(purityValue, sourceValue,
                                                                    conn.function, conn.weight);
                                // Clamp to valid range
                                purityValue = qBound(0.0, purityValue, 1.0);
                            }
                        } else if (conn.toPort == "drift") {
                            // Get drift modulation from source container (control output)
                            if (processors.contains(conn.fromContainer)) {
                                // Scale to drift range (0.0 to 0.1)
                                double sourceValue = processors[conn.fromContainer].controlOut * 0.1;
                                driftValue = applyConnectionFunction(driftValue, sourceValue,
                                                                   conn.function, conn.weight);
                                // Clamp to valid range
                                driftValue = qBound(0.0, driftValue, 0.1);
                            }
                        }
                    }
                }
            }

            // Apply final modulated values
            proc.harmonicGen->setPurity(purityValue);
            proc.harmonicGen->setDrift(driftValue);

            // Generate harmonic spectrum using HarmonicGenerator's pre-calculated amplitudes
            // (which include DNA preset characteristics and purity blending)
            int numHarmonics = proc.harmonicGen->getNumHarmonics();
            proc.spectrumOut.resize(numHarmonics);

            // Use the pre-calculated amplitudes from HarmonicGenerator
            // (already normalized and DNA-aware)
            for (int h = 0; h < numHarmonics; h++) {
                double amp = proc.harmonicGen->getHarmonicAmplitude(h);
                proc.spectrumOut.setAmplitude(h, amp);
            }
        }

    } else if (container->getName() == "Rolloff Processor") {
        if (proc.rolloffProc) {
            // Initialize input values with defaults
            Spectrum inputSpectrum;
            double rolloffPower = container->getParameter("rolloff", 0.6);

            // Find connections to spectrumIn and rolloff ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "spectrumIn") {
                            // Get spectrum from source container (passthrough only for spectrum data)
                            if (processors.contains(conn.fromContainer)) {
                                inputSpectrum = processors[conn.fromContainer].spectrumOut;
                            }
                        } else if (conn.toPort == "rolloff") {
                            // Get rolloff modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to rolloff range (0.1-3.0)
                                double sourceValue = 0.1 + processors[conn.fromContainer].controlOut * 2.9;
                                rolloffPower = applyConnectionFunction(rolloffPower, sourceValue,
                                                                     conn.function, conn.weight);
                                // Clamp to valid range
                                rolloffPower = qBound(0.1, rolloffPower, 3.0);
                            }
                        }
                    }
                }
            }

            // Process spectrum with rolloff curve
            proc.rolloffProc->processSpectrum(inputSpectrum, proc.spectrumOut, rolloffPower);
        }

    } else if (container->getName() == "Spectrum to Signal") {
        if (proc.spectrumToSig) {
            // Initialize input values with defaults
            Spectrum inputSpectrum;
            double effectivePitch = pitch;  // Default to global pitch

            // Find connections to spectrumIn and pitch ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "spectrumIn") {
                            // Get spectrum from source container (passthrough only for spectrum data)
                            if (processors.contains(conn.fromContainer)) {
                                inputSpectrum = processors[conn.fromContainer].spectrumOut;
                            }
                        } else if (conn.toPort == "pitch") {
                            // Get pitch modulation from source container (control output)
                            if (processors.contains(conn.fromContainer)) {
                                // Use control output as pitch multiplier (typical range 0.5-2.0)
                                double sourceValue = processors[conn.fromContainer].controlOut * pitch;
                                effectivePitch = applyConnectionFunction(effectivePitch, sourceValue,
                                                                       conn.function, conn.weight);
                                // Clamp to reasonable pitch range
                                effectivePitch = qBound(20.0, effectivePitch, 20000.0);
                            }
                        }
                    }
                }
            }

            // Generate audio sample from spectrum with modulated pitch
            proc.signalOut = proc.spectrumToSig->generateSample(inputSpectrum, effectivePitch);
        }

    } else if (container->getName() == "Formant Body") {
        if (proc.formantBody) {
            // Initialize input values with defaults
            double inputSignal = 0.0;
            double f1Freq = container->getParameter("f1Freq", 500.0);
            double f2Freq = container->getParameter("f2Freq", 1500.0);
            double f1Q = container->getParameter("f1Q", 8.0);
            double f2Q = container->getParameter("f2Q", 10.0);
            double directMix = container->getParameter("directMix", 0.3);
            double f1f2Balance = container->getParameter("f1f2Balance", 0.6);

            // Find connections to all input ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "signalIn") {
                            // Get signal from source container (passthrough only for signal data)
                            if (processors.contains(conn.fromContainer)) {
                                inputSignal = processors[conn.fromContainer].signalOut;
                            }
                        } else if (conn.toPort == "f1Freq") {
                            // Get f1Freq modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to f1Freq range (200-1000 Hz)
                                double sourceValue = 200.0 + processors[conn.fromContainer].controlOut * 800.0;
                                f1Freq = applyConnectionFunction(f1Freq, sourceValue,
                                                               conn.function, conn.weight);
                                f1Freq = qBound(200.0, f1Freq, 1000.0);
                            }
                        } else if (conn.toPort == "f2Freq") {
                            // Get f2Freq modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to f2Freq range (500-3000 Hz)
                                double sourceValue = 500.0 + processors[conn.fromContainer].controlOut * 2500.0;
                                f2Freq = applyConnectionFunction(f2Freq, sourceValue,
                                                               conn.function, conn.weight);
                                f2Freq = qBound(500.0, f2Freq, 3000.0);
                            }
                        } else if (conn.toPort == "f1Q") {
                            // Get f1Q modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to Q range (1.0-20.0)
                                double sourceValue = 1.0 + processors[conn.fromContainer].controlOut * 19.0;
                                f1Q = applyConnectionFunction(f1Q, sourceValue,
                                                            conn.function, conn.weight);
                                f1Q = qBound(1.0, f1Q, 20.0);
                            }
                        } else if (conn.toPort == "f2Q") {
                            // Get f2Q modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to Q range (1.0-20.0)
                                double sourceValue = 1.0 + processors[conn.fromContainer].controlOut * 19.0;
                                f2Q = applyConnectionFunction(f2Q, sourceValue,
                                                            conn.function, conn.weight);
                                f2Q = qBound(1.0, f2Q, 20.0);
                            }
                        } else if (conn.toPort == "directMix") {
                            // Get directMix modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                directMix = applyConnectionFunction(directMix, sourceValue,
                                                                  conn.function, conn.weight);
                                directMix = qBound(0.0, directMix, 1.0);
                            }
                        } else if (conn.toPort == "f1f2Balance") {
                            // Get f1f2Balance modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                f1f2Balance = applyConnectionFunction(f1f2Balance, sourceValue,
                                                                    conn.function, conn.weight);
                                f1f2Balance = qBound(0.0, f1f2Balance, 1.0);
                            }
                        }
                    }
                }
            }

            // Update parameters (from static config or modulated values)
            proc.formantBody->setF1Freq(f1Freq);
            proc.formantBody->setF2Freq(f2Freq);
            proc.formantBody->setF1Q(f1Q);
            proc.formantBody->setF2Q(f2Q);
            proc.formantBody->setDirectMix(directMix);
            proc.formantBody->setF1F2Balance(f1f2Balance);

            // Process the signal through formant filters
            proc.signalOut = proc.formantBody->processSample(inputSignal);
        }

    } else if (container->getName() == "Breath Turbulence") {
        if (proc.breathTurb) {
            // Initialize input values with defaults
            double voiceIn = 0.0;
            double noiseIn = 0.0;
            double blend = container->getParameter("blend", 0.10);

            // Find connections to voiceIn, noiseIn, and blend ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "voiceIn") {
                            // Get signal from source container (passthrough only for signal data)
                            if (processors.contains(conn.fromContainer)) {
                                voiceIn = processors[conn.fromContainer].signalOut;
                            }
                        } else if (conn.toPort == "noiseIn") {
                            // Get noise signal from source container (passthrough only for signal data)
                            if (processors.contains(conn.fromContainer)) {
                                noiseIn = processors[conn.fromContainer].signalOut;
                            }
                        } else if (conn.toPort == "blend") {
                            // Get blend modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                blend = applyConnectionFunction(blend, sourceValue,
                                                              conn.function, conn.weight);
                                blend = qBound(0.0, blend, 1.0);
                            }
                        }
                    }
                }
            }

            // Update blend parameter (from static config or modulated value)
            proc.breathTurb->setBlend(blend);

            // Process the blend
            proc.signalOut = proc.breathTurb->processSample(voiceIn, noiseIn);
        }

    } else if (container->getName() == "Noise Color Filter") {
        if (proc.noiseFilter) {
            // Initialize input values with defaults
            double color = container->getParameter("color", 2000.0);
            double filterQ = container->getParameter("filterQ", 1.0);
            double audioIn = 0.0;
            bool hasAudioIn = false;

            // Find connections to input ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "audioIn") {
                            // Get audio signal from source container (passthrough only for signal data)
                            if (processors.contains(conn.fromContainer)) {
                                audioIn = processors[conn.fromContainer].signalOut;
                                hasAudioIn = true;
                            }
                        } else if (conn.toPort == "color") {
                            // Get color modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to color range (100-8000 Hz)
                                double sourceValue = processors[conn.fromContainer].controlOut * 7900.0 + 100.0;
                                color = applyConnectionFunction(color, sourceValue,
                                                              conn.function, conn.weight);
                                color = qBound(100.0, color, 8000.0);
                            }
                        } else if (conn.toPort == "filterQ") {
                            // Get filterQ modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to filterQ range (0.5-10.0)
                                double sourceValue = processors[conn.fromContainer].controlOut * 9.5 + 0.5;
                                filterQ = applyConnectionFunction(filterQ, sourceValue,
                                                                conn.function, conn.weight);
                                filterQ = qBound(0.5, filterQ, 10.0);
                            }
                        }
                    }
                }
            }

            // Update parameters
            proc.noiseFilter->setColor(color);
            proc.noiseFilter->setFilterQ(filterQ);

            // Update noise type
            int noiseTypeValue = static_cast<int>(container->getParameter("noiseType", 0.0));
            proc.noiseFilter->setNoiseType(static_cast<NoiseColorFilter::NoiseType>(noiseTypeValue));

            // Process or generate noise
            if (hasAudioIn) {
                // Use external audio input
                proc.signalOut = proc.noiseFilter->processSample(audioIn);
            } else {
                // Generate internal noise
                proc.signalOut = proc.noiseFilter->generateSample();
            }
        }

    } else if (container->getName() == "Physics System") {
        if (proc.physicsSys) {
            // Initialize input values with defaults
            double targetValue = 0.0;
            double mass = container->getParameter("mass", 0.5);
            double springK = container->getParameter("springK", 0.001);
            double damping = container->getParameter("damping", 0.995);
            double impulseAmount = container->getParameter("impulseAmount", 100.0);
            double impulse = 0.0;

            // Find connections to input ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "targetValue") {
                            // Get control value from source container
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                targetValue = applyConnectionFunction(targetValue, sourceValue,
                                                                    conn.function, conn.weight);
                            }
                        } else if (conn.toPort == "mass") {
                            // Get mass modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to mass range (0.0-10.0)
                                double sourceValue = processors[conn.fromContainer].controlOut * 10.0;
                                mass = applyConnectionFunction(mass, sourceValue,
                                                             conn.function, conn.weight);
                                mass = qBound(0.0, mass, 10.0);
                            }
                        } else if (conn.toPort == "springK") {
                            // Get springK modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to springK range (0.0001-1.0)
                                double sourceValue = processors[conn.fromContainer].controlOut * 0.9999 + 0.0001;
                                springK = applyConnectionFunction(springK, sourceValue,
                                                                conn.function, conn.weight);
                                springK = qBound(0.0001, springK, 1.0);
                            }
                        } else if (conn.toPort == "damping") {
                            // Get damping modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to damping range (0.5-0.9999)
                                double sourceValue = processors[conn.fromContainer].controlOut * 0.4999 + 0.5;
                                damping = applyConnectionFunction(damping, sourceValue,
                                                                conn.function, conn.weight);
                                damping = qBound(0.5, damping, 0.9999);
                            }
                        } else if (conn.toPort == "impulseAmount") {
                            // Get impulseAmount modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to impulseAmount range (0-1000)
                                double sourceValue = processors[conn.fromContainer].controlOut * 1000.0;
                                impulseAmount = applyConnectionFunction(impulseAmount, sourceValue,
                                                                      conn.function, conn.weight);
                                impulseAmount = qBound(0.0, impulseAmount, 1000.0);
                            }
                        } else if (conn.toPort == "impulse") {
                            // Get impulse trigger from source container (check specific port)
                            if (processors.contains(conn.fromContainer)) {
                                impulse = getOutputValue(processors[conn.fromContainer], conn.fromPort);
                            }
                        }
                    }
                }
            }

            // Update physics parameters
            proc.physicsSys->setMass(mass);
            proc.physicsSys->setSpringK(springK);
            proc.physicsSys->setDamping(damping);
            proc.physicsSys->setImpulseAmount(impulseAmount);

            // Handle impulse trigger (rising edge detection)
            // Trigger when impulse crosses threshold (0.5) from below
            if (proc.prevImpulse < 0.5 && impulse >= 0.5) {
                proc.physicsSys->applyImpulse(impulseAmount);
            }
            proc.prevImpulse = impulse;

            // Process physics simulation
            proc.controlOut = proc.physicsSys->processSample(targetValue);
        }

    } else if (container->getName() == "Envelope Engine") {
        if (proc.envelopeEng) {
            // Initialize input values with defaults
            double effectiveProgress = noteProgress;
            double timeScale = container->getParameter("timeScale", 1.0);
            double valueScale = container->getParameter("valueScale", 1.0);
            double valueOffset = container->getParameter("valueOffset", 0.0);

            // Check for connections to modulatable parameters
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "timeScale") {
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to useful range (0.1-5.0)
                                double sourceValue = 0.1 + processors[conn.fromContainer].controlOut * 4.9;
                                timeScale = applyConnectionFunction(timeScale, sourceValue,
                                                                  conn.function, conn.weight);
                                timeScale = qBound(0.1, timeScale, 5.0);
                            }
                        } else if (conn.toPort == "valueScale") {
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to 0.0-2.0
                                double sourceValue = processors[conn.fromContainer].controlOut * 2.0;
                                valueScale = applyConnectionFunction(valueScale, sourceValue,
                                                                   conn.function, conn.weight);
                                valueScale = qBound(0.0, valueScale, 2.0);
                            }
                        } else if (conn.toPort == "valueOffset") {
                            if (processors.contains(conn.fromContainer)) {
                                // Scale control output (0.0-1.0) to -1.0 to 1.0
                                double sourceValue = processors[conn.fromContainer].controlOut * 2.0 - 1.0;
                                valueOffset = applyConnectionFunction(valueOffset, sourceValue,
                                                                    conn.function, conn.weight);
                                valueOffset = qBound(-1.0, valueOffset, 1.0);
                            }
                        }
                    }
                }
            }

            // Update envelope parameters
            int envSelect = static_cast<int>(container->getParameter("envelopeSelect", 0.0));

            // Handle custom envelope data if selected
            if (envSelect == 5 && container->hasCustomEnvelopeData()) {
                proc.envelopeEng->setEnvelopeType(EnvelopeEngine::EnvelopeType::Custom);
                EnvelopeData customData = container->getCustomEnvelopeData();
                proc.envelopeEng->setCustomEnvelope(customData.points);
            } else {
                // Standard envelope types
                proc.envelopeEng->setEnvelopeSelect(envSelect);
            }

            proc.envelopeEng->setTimeScale(timeScale);
            proc.envelopeEng->setValueScale(valueScale);
            proc.envelopeEng->setValueOffset(valueOffset);

            // Set envelope-specific timing parameters (for standard envelopes)
            proc.envelopeEng->setAttackTime(container->getParameter("envAttack", 0.1));
            proc.envelopeEng->setDecayTime(container->getParameter("envDecay", 0.2));
            proc.envelopeEng->setSustainLevel(container->getParameter("envSustain", 0.7));
            proc.envelopeEng->setReleaseTime(container->getParameter("envRelease", 0.2));
            proc.envelopeEng->setFadeTime(container->getParameter("envFadeTime", 0.5));

            // Process envelope with note progress (0.0 to 1.0 over note duration)
            proc.controlOut = proc.envelopeEng->process(effectiveProgress);
        }

    } else if (container->getName() == "Drift Engine") {
        if (proc.driftEng) {
            // Initialize input values with defaults
            double amount = container->getParameter("amount", 0.005);
            double rate = container->getParameter("rate", 0.5);

            // Find connections to input ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "amount") {
                            // Get amount modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to amount range (0.0-0.1)
                                double sourceValue = processors[conn.fromContainer].controlOut * 0.1;
                                amount = applyConnectionFunction(amount, sourceValue,
                                                               conn.function, conn.weight);
                                amount = qBound(0.0, amount, 0.1);
                            }
                        } else if (conn.toPort == "rate") {
                            // Get rate modulation from source container
                            if (processors.contains(conn.fromContainer)) {
                                // Scale controlOut (0-1) to rate range (0.01-10.0)
                                double sourceValue = processors[conn.fromContainer].controlOut * 9.99 + 0.01;
                                rate = applyConnectionFunction(rate, sourceValue,
                                                             conn.function, conn.weight);
                                rate = qBound(0.01, rate, 10.0);
                            }
                        }
                    }
                }
            }

            // Update drift parameters
            proc.driftEng->setAmount(amount);
            proc.driftEng->setRate(rate);

            // Update drift pattern
            int patternValue = static_cast<int>(container->getParameter("driftPattern", 2.0));
            proc.driftEng->setDriftPattern(static_cast<DriftEngine::DriftPattern>(patternValue));

            // Generate drift value (detuning multiplier around 1.0)
            proc.controlOut = proc.driftEng->generateSample();
        }

    } else if (container->getName() == "Gate Processor") {
        if (proc.gateProc) {
            // Update gate parameters (in case they changed)
            proc.gateProc->setVelocity(container->getParameter("velocity", 1.0));
            proc.gateProc->setAttackTime(container->getParameter("attackTime", 0.01));
            proc.gateProc->setReleaseTime(container->getParameter("releaseTime", 0.1));
            proc.gateProc->setAttackCurve(static_cast<int>(container->getParameter("attackCurve", 0.0)));
            proc.gateProc->setReleaseCurve(static_cast<int>(container->getParameter("releaseCurve", 0.0)));
            proc.gateProc->setVelocitySens(container->getParameter("velocitySens", 0.5));

            // Process gate state machine
            proc.gateProc->processSample();

            // Store all outputs
            proc.gateEnvelopeOut = proc.gateProc->getEnvelopeOut();
            proc.gateStateOut = static_cast<double>(proc.gateProc->getStateOut());
            proc.gateAttackTrigger = proc.gateProc->getAttackTrigger() ? 1.0 : 0.0;
            proc.gateReleaseTrigger = proc.gateProc->getReleaseTrigger() ? 1.0 : 0.0;

            // Default controlOut to envelopeOut for backward compatibility
            proc.controlOut = proc.gateEnvelopeOut;
        }

    } else if (container->getName() == "Easing Applicator") {
        if (proc.easingApp) {
            // Initialize input values with defaults
            double startValue = 0.0;
            double endValue = 1.0;
            double progress = 0.5;

            // Find connections to input ports
            Canvas *canvas = qobject_cast<Canvas*>(container->parentWidget());
            if (canvas) {
                const QVector<Canvas::Connection>& connections = canvas->getConnections();
                for (const Canvas::Connection &conn : connections) {
                    if (conn.toContainer == container) {
                        if (conn.toPort == "startValue") {
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                startValue = applyConnectionFunction(startValue, sourceValue,
                                                                   conn.function, conn.weight);
                            }
                        } else if (conn.toPort == "endValue") {
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                endValue = applyConnectionFunction(endValue, sourceValue,
                                                                 conn.function, conn.weight);
                            }
                        } else if (conn.toPort == "progress") {
                            if (processors.contains(conn.fromContainer)) {
                                double sourceValue = processors[conn.fromContainer].controlOut;
                                progress = applyConnectionFunction(progress, sourceValue,
                                                                 conn.function, conn.weight);
                                progress = qBound(0.0, progress, 1.0);
                            }
                        }
                    }
                }
            }

            // Update easing parameters (in case they changed)
            proc.easingApp->setEasingSelect(static_cast<int>(container->getParameter("easingSelect", 0.0)));

            // Process easing
            proc.controlOut = proc.easingApp->process(startValue, endValue, progress);
        }
    }
}

double SounitGraph::getInputValue(Container *container, const QString &portName, double defaultValue)
{
    // For future: traverse connections to get input values
    // For now, just return default
    return container->getParameter(portName, defaultValue);
}

double SounitGraph::getOutputValue(const ProcessorData &proc, const QString &portName) const
{
    // Gate Processor has multiple named outputs
    if (portName == "envelopeOut") {
        return proc.gateEnvelopeOut;
    } else if (portName == "stateOut") {
        return proc.gateStateOut;
    } else if (portName == "attackTrigger") {
        return proc.gateAttackTrigger;
    } else if (portName == "releaseTrigger") {
        return proc.gateReleaseTrigger;
    }

    // Default: check if it's a signal or control output
    if (portName == "signalOut" || portName.contains("signal", Qt::CaseInsensitive)) {
        return proc.signalOut;
    }

    // Default to controlOut for any other named output
    return proc.controlOut;
}

