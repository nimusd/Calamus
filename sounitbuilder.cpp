#include "sounitbuilder.h"
#include "ui_sounitbuilder.h"
#include "container.h"
#include <QToolButton>
#include <QDebug>

SounitBuilder::SounitBuilder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SounitBuilderCanvas)
{
    ui->setupUi(this);

    canvas = new Canvas(this);
    setCentralWidget(canvas);

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
                       {"pitch", "numHarmonics", "dnaSelect", "purity", "drift"},
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
                       {"noteProgress", "envelopeSelect", "timeScale", "valueScale", "valueOffset"},
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

}

void SounitBuilder::onAddContainer(const QString &name, const QColor &color,
                                   const QStringList &inputs, const QStringList &outputs)
{
    Container *newContainer = new Container(canvas, name, color, inputs, outputs);
    newContainer->move(100, 100);
    newContainer->show();

    connect(newContainer, &Container::portClicked, this, &SounitBuilder::onPortClicked);
    connect(newContainer, &Container::clicked, canvas, &Canvas::selectContainer);
    connect(newContainer, &Container::moved, canvas, QOverload<>::of(&QWidget::update));

    containers.append(newContainer);
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
        }

        canvas->cancelPendingConnection();
        connectionStartContainer = nullptr;
    }
}

SounitBuilder::~SounitBuilder()
{
    delete ui;
}
