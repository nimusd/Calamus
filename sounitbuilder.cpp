#include "sounitbuilder.h"
#include "ui_sounitbuilder.h"
#include "container.h"
#include "scorecanvas.h"
#include <QToolButton>
#include <QDebug>
#include <QKeyEvent>

SounitBuilder::SounitBuilder(AudioEngine *sharedAudioEngine, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SounitBuilderCanvas)
    , audioEngine(sharedAudioEngine)
    , playbackTimer(nullptr)
    , currentNoteIndex(0)
    , playbackStartTime(0.0)
    , noteDuration(0.0)
    , isPlaying(false)
    , scoreCanvas(nullptr)
{
    ui->setupUi(this);

    canvas = new Canvas(this);
    setCentralWidget(canvas);

    // Connect canvas signals
    connect(canvas, &Canvas::graphChanged, this, &SounitBuilder::rebuildGraph);

    // Style toolbar buttons by category
    QString blueStyle = "QToolButton { background-color: #3498db; color: white; }";
    QString orangeStyle = "QToolButton { background-color: #e67e22; color: white; }";
    QString greenStyle = "QToolButton { background-color: #27ae60; color: white; }";

    // Essential - Blue
    ui->toolBar->widgetForAction(ui->actionHarmonicGenerator)->setStyleSheet(blueStyle);
    ui->toolBar->widgetForAction(ui->actionSpectrum_to_Signal)->setStyleSheet(blueStyle);

    // Shaping - Orange
    ui->toolBar->widgetForAction(ui->actionRollofff_processor)->setStyleSheet(orangeStyle);
    ui->toolBar->widgetForAction(ui->actionFormant_body)->setStyleSheet(orangeStyle);
    ui->toolBar->widgetForAction(ui->actionBreath_Turbulence)->setStyleSheet(orangeStyle);
    ui->toolBar->widgetForAction(ui->actionNoise_Color_Filter)->setStyleSheet(orangeStyle);

    // Modifiers - Green
    ui->toolBar->widgetForAction(ui->actionPhysics_System)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionEasing_Applicator)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionEnvelope_Engine)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionDrift_Engine)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionGate_Processor)->setStyleSheet(greenStyle);



    // Create transport labels
    QLabel *labelPosition = new QLabel("Playback time: 0:00:00  ", this);
    QLabel *labelTempo = new QLabel("Tempo: 120 BPM   ", this);
    QLabel *labelTimeSignature = new QLabel("Time signature: 4/4", this);

    // Add to bottom toolbar
    ui->sounitBuilderBottomToolbar->addWidget(labelPosition);
    ui->sounitBuilderBottomToolbar->addWidget(labelTempo);
    ui->sounitBuilderBottomToolbar->addWidget(labelTimeSignature);

    // Essential - Blue
    connect(ui->actionHarmonicGenerator, &QAction::triggered, this, [this]() {
        onAddContainer("Harmonic Generator", QColor("#3498db"),
                       {"purity", "drift"},
                       {"spectrum"});
    });
    connect(ui->actionSpectrum_to_Signal, &QAction::triggered, this, [this]() {
        onAddContainer("Spectrum to Signal", QColor("#3498db"),
                       {"spectrumIn", "pitch", "normalize"},
                       {"signalOut"});
    });

    // Shaping - Orange
    connect(ui->actionRollofff_processor, &QAction::triggered, this, [this]() {
        onAddContainer("Rolloff Processor", QColor("#e67e22"),
                       {"spectrumIn", "rolloff"},
                       {"spectrumOut"});
    });
    connect(ui->actionFormant_body, &QAction::triggered, this, [this]() {
        onAddContainer("Formant Body", QColor("#e67e22"),
                       {"signalIn", "f1Freq", "f2Freq", "f1Q", "f2Q", "directMix", "f1f2Balance"},
                       {"signalOut"});
    });
    connect(ui->actionBreath_Turbulence, &QAction::triggered, this, [this]() {
        onAddContainer("Breath Turbulence", QColor("#e67e22"),
                       {"voiceIn", "noiseIn", "blend"},
                       {"signalOut"});
    });
    connect(ui->actionNoise_Color_Filter, &QAction::triggered, this, [this]() {
        onAddContainer("Noise Color Filter", QColor("#e67e22"),
                       {"noiseIn", "color", "filterQ"},
                       {"noiseOut"});
    });

    // Modifiers - Green
    connect(ui->actionPhysics_System, &QAction::triggered, this, [this]() {
        onAddContainer("Physics System", QColor("#27ae60"),
                       {"targetValue", "mass", "springK", "damping", "impulse", "impulseAmount"},
                       {"currentValue"});
    });
    connect(ui->actionEasing_Applicator, &QAction::triggered, this, [this]() {
        onAddContainer("Easing Applicator", QColor("#27ae60"),
                       {"startValue", "endValue", "progress", "easingSelect"},
                       {"easedValue"});
    });
    connect(ui->actionEnvelope_Engine, &QAction::triggered, this, [this]() {
        onAddContainer("Envelope Engine", QColor("#27ae60"),
                       {"timeScale", "valueScale", "valueOffset"},
                       {"envelopeValue"});
    });
    connect(ui->actionDrift_Engine, &QAction::triggered, this, [this]() {
        onAddContainer("Drift Engine", QColor("#27ae60"),
                       {"amount", "rate"},
                       {"driftOut"});
    });
    connect(ui->actionGate_Processor, &QAction::triggered, this, [this]() {
        onAddContainer("Gate Processor", QColor("#27ae60"),
                       {"velocity", "attackTime", "releaseTime", "attackCurve", "releaseCurve", "velocitySens"},
                       {"envelopeOut", "stateOut", "attackTrigger", "releaseTrigger"});
    });

    // Initialize playback timer
    playbackTimer = new QTimer(this);
    connect(playbackTimer, &QTimer::timeout, this, &SounitBuilder::onPlaybackTick);

    // Connect transport controls
    connect(ui->actionplay, &QAction::triggered, this, &SounitBuilder::onPlay);
    connect(ui->actionstop, &QAction::triggered, this, &SounitBuilder::onStop);

    // Initial graph build (empty)
    rebuildGraph();
}

void SounitBuilder::rebuildGraph()
{
    if (audioEngine && canvas) {
        qDebug() << "SounitBuilder: Rebuilding graph from canvas";
        audioEngine->buildGraph(canvas);
    }
}

void SounitBuilder::setScoreCanvas(ScoreCanvas *canvas)
{
    scoreCanvas = canvas;
    qDebug() << "SounitBuilder: Connected to ScoreCanvas - will play drawn notes";
}

void SounitBuilder::onAddContainer(const QString &name, const QColor &color,
                                   const QStringList &inputs, const QStringList &outputs)
{
    Container *newContainer = new Container(canvas, name, color, inputs, outputs);
    newContainer->move(100, 100);

    // Set default parameters based on container type (from Container Port Specification v1.2)
    if (name == "Harmonic Generator") {
        newContainer->setParameter("numHarmonics", 64.0);
        newContainer->setParameter("dnaSelect", 0.0);
        newContainer->setParameter("purity", 0.0);  // 0.0 = pure DNA preset, no blending
        newContainer->setParameter("drift", 0.0);
    } else if (name == "Spectrum to Signal") {
        newContainer->setParameter("normalize", 1.0);
    } else if (name == "Rolloff Processor") {
        newContainer->setParameter("rolloff", 0.6);
    } else if (name == "Formant Body") {
        newContainer->setParameter("f1Freq", 500.0);
        newContainer->setParameter("f2Freq", 1500.0);
        newContainer->setParameter("f1Q", 8.0);
        newContainer->setParameter("f2Q", 10.0);
        newContainer->setParameter("directMix", 0.3);
        newContainer->setParameter("f1f2Balance", 0.6);
    } else if (name == "Breath Turbulence") {
        newContainer->setParameter("blend", 0.10);
    } else if (name == "Noise Color Filter") {
        newContainer->setParameter("color", 2000.0);
        newContainer->setParameter("filterQ", 1.0);
    } else if (name == "Physics System") {
        newContainer->setParameter("mass", 0.5);
        newContainer->setParameter("springK", 0.001);
        newContainer->setParameter("damping", 0.995);
        newContainer->setParameter("impulseAmount", 100.0);
    } else if (name == "Easing Applicator") {
        newContainer->setParameter("startValue", 0.0);
        newContainer->setParameter("endValue", 1.0);
        newContainer->setParameter("easingSelect", 0.0);
    } else if (name == "Envelope Engine") {
        newContainer->setParameter("envelopeSelect", 0.0);
        newContainer->setParameter("timeScale", 1.0);
        newContainer->setParameter("valueScale", 1.0);
        newContainer->setParameter("valueOffset", 0.0);
    } else if (name == "Drift Engine") {
        newContainer->setParameter("amount", 0.005);
        newContainer->setParameter("rate", 0.5);
    } else if (name == "Gate Processor") {
        newContainer->setParameter("velocity", 1.0);
        newContainer->setParameter("attackTime", 0.01);
        newContainer->setParameter("releaseTime", 0.1);
        newContainer->setParameter("attackCurve", 0.0);
        newContainer->setParameter("releaseCurve", 0.0);
        newContainer->setParameter("velocitySens", 0.5);
    }

    newContainer->show();

    connect(newContainer, &Container::portClicked, this, &SounitBuilder::onPortClicked);
    connect(newContainer, &Container::clicked, canvas, &Canvas::selectContainer);
    connect(newContainer, &Container::moved, canvas, QOverload<>::of(&QWidget::update));

    containers.append(newContainer);

    // Rebuild graph when container is added
    rebuildGraph();
}

void SounitBuilder::onPortClicked(Container *container, const QString &portName, bool isOutput, QPoint globalPos)
{
    if (!connectionStartContainer) {
        // First click — start dragging
        connectionStartContainer = container;
        connectionStartPort = portName;
        connectionStartIsOutput = isOutput;

        canvas->startPendingConnection(container, portName, isOutput);
    } else {
        // Second click — complete or cancel
        if (connectionStartIsOutput != isOutput) {
            Canvas::Connection conn;
            if (connectionStartIsOutput) {
                conn.fromContainer = connectionStartContainer;
                conn.fromPort = connectionStartPort;
                conn.toContainer = container;
                conn.toPort = portName;
            } else {
                conn.fromContainer = container;
                conn.fromPort = portName;
                conn.toContainer = connectionStartContainer;
                conn.toPort = connectionStartPort;
            }
            canvas->addConnection(conn);

            qDebug() << "Connection added:" << conn.fromContainer->getName() << conn.fromPort
                     << "->" << conn.toContainer->getName() << conn.toPort;

            // Rebuild graph when connection is added
            rebuildGraph();
        }

        canvas->cancelPendingConnection();
        connectionStartContainer = nullptr;
    }
}

void SounitBuilder::onPlay()
{
    startPlayback();
}

void SounitBuilder::onStop()
{
    stopPlayback();
}

void SounitBuilder::startPlayback()
{
    if (!audioEngine || !scoreCanvas) return;

    // Get all notes from the ScoreCanvas phrase (shared with ScoreCanvasWindow)
    const QVector<Note>& notes = scoreCanvas->getPhrase().getNotes();

    if (notes.isEmpty()) {
        qDebug() << "SounitBuilder: No notes to play";
        return;
    }

    // Stop any currently playing audio first
    if (isPlaying) {
        stopPlayback();
    }

    // Make sure audio is stopped from any other source
    audioEngine->stopNote();

    // PRE-RENDER: Render up to 10 notes into a single continuous buffer
    qDebug() << "SounitBuilder: Pre-rendering notes...";
    audioEngine->renderNotes(notes, 10);  // Render up to 10 notes

    // Play the rendered buffer
    audioEngine->playRenderedBuffer();

    // Calculate total playback duration from all notes
    double totalDuration = 0.0;
    int maxNotes = qMin(notes.size(), 10);
    for (int i = 0; i < maxNotes; i++) {
        double noteEndTime = notes[i].getStartTime() + notes[i].getDuration();
        if (noteEndTime > totalDuration) {
            totalDuration = noteEndTime;
        }
    }

    // Start playback from the beginning
    currentNoteIndex = 0;
    playbackStartTime = 0.0;
    isPlaying = true;
    noteDuration = totalDuration;  // Store total duration for timer

    // Start the playback timer (tick every 10ms for smooth timing)
    playbackTimer->start(10);

    qDebug() << "SounitBuilder: Starting playback of" << maxNotes << "note(s) (rendered mode, total duration:" << totalDuration << "ms)";

    // Emit signal to stop other windows
    emit playbackStarted();
}

void SounitBuilder::onPlaybackTick()
{
    if (!audioEngine || !isPlaying) return;

    // With pre-rendered buffer playback, we don't need to trigger individual notes
    // The audio callback automatically plays through the entire rendered buffer
    // This timer just checks if playback has finished

    // Check if we've exceeded the total duration
    if (playbackStartTime >= noteDuration) {
        stopPlayback();
        return;
    }

    // Advance playback time (10ms per tick)
    playbackStartTime += 10.0;
}

void SounitBuilder::stopPlayback(bool stopAudioEngine)
{
    if (!audioEngine) return;

    // Stop the timer first
    if (playbackTimer && playbackTimer->isActive()) {
        playbackTimer->stop();
    }

    // Reset playback state BEFORE stopping note (prevents race condition)
    isPlaying = false;
    currentNoteIndex = 0;
    playbackStartTime = 0.0;
    noteDuration = 0.0;

    // Only stop audio engine if requested (don't stop when another window is taking over)
    if (stopAudioEngine) {
        audioEngine->stopNote();
    }

    qDebug() << "SounitBuilder: Playback stopped" << (stopAudioEngine ? "(audio stopped)" : "(audio continues)");
}

void SounitBuilder::keyPressEvent(QKeyEvent *event)
{
    // Spacebar toggles playback (start/stop)
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::NoModifier) {
        if (isPlaying) {
            stopPlayback();
        } else {
            startPlayback();
        }
        event->accept();
        return;
    }

    // T key plays test tone at 262 Hz (middle C)
    if (event->key() == Qt::Key_T && event->modifiers() == Qt::NoModifier) {
        if (audioEngine) {
            // Create a test note: Middle C (261.63 Hz), 1 second duration
            Note testNote(0.0, 1000.0, 261.63, 0.7);
            QVector<Note> testNotes;
            testNotes.append(testNote);
            qDebug() << "SounitBuilder: Rendering test tone: 261.63 Hz (Middle C)";
            audioEngine->renderNotes(testNotes, 1);
            audioEngine->playRenderedBuffer();
        }
        event->accept();
        return;
    }

    // S key stops playback
    if (event->key() == Qt::Key_S && event->modifiers() == Qt::NoModifier) {
        stopPlayback();
        event->accept();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void SounitBuilder::connectContainerSignals(Container *container)
{
    connect(container, &Container::portClicked, this, &SounitBuilder::onPortClicked);
    connect(container, &Container::clicked, canvas, &Canvas::selectContainer);
    connect(container, &Container::moved, canvas, QOverload<>::of(&QWidget::update));
}

SounitBuilder::~SounitBuilder()
{
    delete ui;
}
