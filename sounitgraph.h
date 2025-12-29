#ifndef SOUNITGRAPH_H
#define SOUNITGRAPH_H

#include "container.h"
#include "canvas.h"
#include "harmonicgenerator.h"
#include "spectrumtosignal.h"
#include "rolloffprocessor.h"
#include "formantbody.h"
#include "breathturbulence.h"
#include "noisecolorfilter.h"
#include "physicssystem.h"
#include "envelopeengine.h"
#include "driftengine.h"
#include "gateprocessor.h"
#include "easingapplicator.h"
#include "spectrum.h"
#include <QMap>
#include <QVector>

/**
 * SounitGraph - Executes a graph of connected containers
 *
 * Manages processor instances for each container and executes them
 * in topological order to generate audio.
 */
class SounitGraph
{
public:
    SounitGraph(double sampleRate = 44100.0);

    // Build the graph from canvas containers and connections
    void buildFromCanvas(Canvas *canvas);

    // Generate a single audio sample
    // pitch = fundamental frequency in Hz
    // noteProgress = 0.0 to 1.0, represents position within note duration
    double generateSample(double pitch, double noteProgress = 0.5);

    // Reset all processors (call when starting new note)
    void reset();

    // Check if graph is valid and can produce audio
    bool isValid() const { return hasValidSignalOutput; }

private:
    struct ProcessorData {
        Container *container = nullptr;

        // Processor instances (only one will be used per container)
        // Using raw pointers for QMap compatibility
        HarmonicGenerator *harmonicGen = nullptr;
        RolloffProcessor *rolloffProc = nullptr;
        SpectrumToSignal *spectrumToSig = nullptr;
        FormantBody *formantBody = nullptr;
        BreathTurbulence *breathTurb = nullptr;
        NoiseColorFilter *noiseFilter = nullptr;
        PhysicsSystem *physicsSys = nullptr;
        EnvelopeEngine *envelopeEng = nullptr;
        DriftEngine *driftEng = nullptr;
        GateProcessor *gateProc = nullptr;
        EasingApplicator *easingApp = nullptr;

        // Data storage for this container's outputs
        Spectrum spectrumOut;
        double signalOut = 0.0;
        double controlOut = 0.0;

        // Gate Processor specific outputs
        double gateEnvelopeOut = 0.0;
        double gateStateOut = 0.0;
        double gateAttackTrigger = 0.0;
        double gateReleaseTrigger = 0.0;

        // State tracking for Physics System impulse trigger
        double prevImpulse = 0.0;

        // Destructor to clean up processors
        ~ProcessorData() {
            delete harmonicGen;
            delete rolloffProc;
            delete spectrumToSig;
            delete formantBody;
            delete breathTurb;
            delete noiseFilter;
            delete physicsSys;
            delete envelopeEng;
            delete driftEng;
            delete gateProc;
            delete easingApp;
        }
    };

    double sampleRate;
    QMap<Container*, ProcessorData> processors;
    QVector<Container*> executionOrder;  // Topological order
    Container *signalOutputContainer = nullptr;  // Final output
    bool hasValidSignalOutput = false;

    void computeExecutionOrder(Canvas *canvas);
    void createProcessors();
    void executeContainer(ProcessorData &proc, double pitch, double noteProgress);
    double getInputValue(Container *container, const QString &portName, double defaultValue);
    double getOutputValue(const ProcessorData &proc, const QString &portName) const;
};

#endif // SOUNITGRAPH_H
