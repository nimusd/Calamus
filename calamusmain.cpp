#include "calamusmain.h"
#include "ui_calamusmain.h"
#include "scorecanvaswindow.h"
#include "canvas.h"
#include "container.h"
#include "spectrumvisualizer.h"
#include "envelopevisualizer.h"
#include "dnaeditordialog.h"
#include "envelopelibraryDialog.h"
#include "phrasegroup.h"
#include <QCloseEvent>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

CalamusMain::CalamusMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalamusMain)
    , sounitBuilder(nullptr)    // Initialize pointers
    , scoreCanvasWindow(nullptr) // Initialize pointers
    , audioEngine(nullptr)
{
    ui->setupUi(this);

    // Initialize shared audio engine FIRST
    audioEngine = new AudioEngine();
    if (!audioEngine->initialize()) {
        qWarning("Failed to initialize audio engine - playback will not work");
    } else {
        qDebug() << "Shared AudioEngine initialized successfully";
    }

    // Create SounitBuilder with shared audio engine
    sounitBuilder = new SounitBuilder(audioEngine, this);

    // Create ScoreCanvasWindow - NOT parented to this to make it independent
    scoreCanvasWindow = new ScoreCanvasWindow(audioEngine);
    scoreCanvasWindow->hide();  // Hide initially

    // Set window attribute so it doesn't prevent app quit
    scoreCanvasWindow->setAttribute(Qt::WA_QuitOnClose, false);

    // Connect SounitBuilder to ScoreCanvas so both windows play the same notes
    sounitBuilder->setScoreCanvas(scoreCanvasWindow->getScoreCanvas());

    // Connect Canvas selection signals to inspector update slots
    Canvas *canvas = sounitBuilder->getCanvas();
    connect(canvas, &Canvas::containerSelected, this, &CalamusMain::onContainerSelected);
    connect(canvas, &Canvas::connectionSelected, this, &CalamusMain::onConnectionSelected);
    connect(canvas, &Canvas::selectionCleared, this, &CalamusMain::onSelectionCleared);

    // Connect inspector edit signals
    connect(ui->editInstanceName, &QLineEdit::editingFinished, this, &CalamusMain::onInstanceNameChanged);
    connect(ui->comboConnectionFunction, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CalamusMain::onConnectionFunctionChanged);
    connect(ui->spinConnectionWeight, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CalamusMain::onConnectionWeightChanged);
    connect(ui->buttonDisconnect, &QPushButton::clicked, this, &CalamusMain::onDisconnectClicked);

    // Initialize inspectors as empty
    onSelectionCleared();

    connect(ui->MainTab, &QTabWidget::currentChanged, this, &CalamusMain::onTabChanged);

    // Connect playback start signals to stop the other window
    connect(sounitBuilder, &SounitBuilder::playbackStarted, this, [this]() {
        if (scoreCanvasWindow) {
            scoreCanvasWindow->stopPlayback(false);  // Don't stop audio - SounitBuilder is taking over
        }
    });
    connect(scoreCanvasWindow, &ScoreCanvasWindow::playbackStarted, this, [this]() {
        if (sounitBuilder) {
            sounitBuilder->stopPlayback(false);  // Don't stop audio - ScoreCanvas is taking over
        }
    });

    // Connect Sound menu actions
    connect(ui->actionNew_Sounit, &QAction::triggered, this, &CalamusMain::onNewSounit);
    connect(ui->actionLoad_Sounit, &QAction::triggered, this, &CalamusMain::onLoadSounit);
    connect(ui->actionSave_Sounit_As, &QAction::triggered, this, &CalamusMain::onSaveSounitAs);
    connect(canvas, &Canvas::sounitNameChanged, this, &CalamusMain::onSounitNameChanged);
    connect(canvas, &Canvas::sounitCommentChanged, this, &CalamusMain::onSounitCommentChanged);

    // Connect Sounit Info UI fields
    connect(ui->editSounitName, &QLineEdit::editingFinished, this, &CalamusMain::onSounitNameEdited);
    connect(ui->editSounitComment, &QLineEdit::editingFinished, this, &CalamusMain::onSounitCommentEdited);

    // Connect phrase inspector widgets (Phase 4)
    connect(ui->editPhraseName, &QLineEdit::textChanged, this, &CalamusMain::onPhraseNameChanged);
    connect(ui->comboPhraseDynamicsEasing, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CalamusMain::onPhraseEasingChanged);
    connect(ui->btnLoadPhraseTemplate, &QPushButton::clicked, this, &CalamusMain::onLoadPhraseTemplateClicked);
    connect(ui->btnApplyPhraseTemplate, &QPushButton::clicked, this, &CalamusMain::onApplyPhraseTemplateClicked);
    connect(ui->btnCreatePhrase, &QPushButton::clicked, this, &CalamusMain::onCreatePhraseClicked);
    connect(ui->btnUngroupPhrase, &QPushButton::clicked, this, &CalamusMain::onUngroupPhraseClicked);
    connect(ui->btnSavePhraseTemplate, &QPushButton::clicked, this, &CalamusMain::onSavePhraseTemplateClicked);

    // Connect ScoreCanvas phrase selection signal (Phase 4)
    connect(scoreCanvasWindow->getScoreCanvas(), &ScoreCanvas::phraseSelectionChanged,
            this, &CalamusMain::onPhraseSelectionChanged);

    // Show initial tab content
    if(ui->MainTab->currentIndex() == 0)  {
        sounitBuilder->show();
    }
    else if (ui->MainTab->currentIndex() == 1)  {
        scoreCanvasWindow->show();
    }
}

void CalamusMain::onTabChanged(int index)
{
    if (index == 0) {  // Sound Engine tab
        if (sounitBuilder) {
            sounitBuilder->show();
        }
        if (scoreCanvasWindow) {
            scoreCanvasWindow->hide();  // Hide other window (don't close - it destroys the window!)
        }
    } else if (index == 1) {  // Composition tab
        if (sounitBuilder) {
            sounitBuilder->hide();  // Hide, don't close
        }
        if (scoreCanvasWindow) {
            scoreCanvasWindow->show();  // Show the score canvas
        }
    }
    // index == 2 (Preferences) — leave as-is
}

void CalamusMain::stopAllPlayback()
{
    if (sounitBuilder) {
        sounitBuilder->stopPlayback();
    }
    if (scoreCanvasWindow) {
        scoreCanvasWindow->stopPlayback();
    }
}

void CalamusMain::onContainerSelected(Container *container)
{
    if (!container) return;

    // Store current container
    currentContainer = container;

    // Show container inspector, hide connection inspector
    ui->groupContainerInspector->setVisible(true);
    ui->groupConnectionInspector->setVisible(false);

    // Enable container inspector controls
    ui->editInstanceName->setEnabled(true);

    // Populate container inspector
    ui->editInstanceName->setText(container->getInstanceName());
    ui->labelContainerType->setText(container->getName());

    // Set description
    ui->labelContainerDescription->setText(getContainerDescription(container->getName()));

    // Populate inputs list
    ui->listWidgetInputs->clear();
    for (const QString &input : container->getInputPorts()) {
        ui->listWidgetInputs->addItem(input);
    }

    // Populate outputs list
    ui->listWidgetOutputs->clear();
    for (const QString &output : container->getOutputPorts()) {
        ui->listWidgetOutputs->addItem(output);
    }

    // Clear previous config controls and populate container-specific inspector
    clearConfigInspector();

    // Create container-specific inspector controls
    if (container->getName() == "Harmonic Generator") {
        populateHarmonicGeneratorInspector();
    } else if (container->getName() == "Rolloff Processor") {
        populateRolloffProcessorInspector();
    } else if (container->getName() == "Spectrum to Signal") {
        populateSpectrumToSignalInspector();
    } else if (container->getName() == "Formant Body") {
        populateFormantBodyInspector();
    } else if (container->getName() == "Breath Turbulence") {
        populateBreathTurbulenceInspector();
    } else if (container->getName() == "Noise Color Filter") {
        populateNoiseColorFilterInspector();
    } else if (container->getName() == "Physics System") {
        populatePhysicsSystemInspector();
    } else if (container->getName() == "Envelope Engine") {
        populateEnvelopeEngineInspector();
    } else if (container->getName() == "Drift Engine") {
        populateDriftEngineInspector();
    } else if (container->getName() == "Gate Processor") {
        populateGateProcessorInspector();
    } else if (container->getName() == "Easing Applicator") {
        populateEasingApplicatorInspector();
    }
}

void CalamusMain::onConnectionSelected(int connectionIndex)
{
    Canvas *canvas = sounitBuilder->getCanvas();
    Canvas::Connection *conn = canvas->getSelectedConnection();
    if (!conn) return;

    // Hide container inspector, show connection inspector
    ui->groupContainerInspector->setVisible(false);
    ui->groupConnectionInspector->setVisible(true);

    // Enable connection inspector controls
    ui->comboConnectionFunction->setEnabled(true);
    ui->spinConnectionWeight->setEnabled(true);
    ui->buttonDisconnect->setEnabled(true);

    // Populate connection inspector
    QString fromText = QString("%1.%2").arg(conn->fromContainer->getInstanceName(), conn->fromPort);
    QString toText = QString("%1.%2").arg(conn->toContainer->getInstanceName(), conn->toPort);

    ui->labelConnectionFrom->setText(fromText);
    ui->labelConnectionTo->setText(toText);

    // Set function combo box (block signals to avoid triggering change event)
    ui->comboConnectionFunction->blockSignals(true);
    int functionIndex = ui->comboConnectionFunction->findText(conn->function);
    if (functionIndex >= 0) {
        ui->comboConnectionFunction->setCurrentIndex(functionIndex);
    }
    ui->comboConnectionFunction->blockSignals(false);

    // Set weight (block signals to avoid triggering change event)
    ui->spinConnectionWeight->blockSignals(true);
    ui->spinConnectionWeight->setValue(conn->weight);
    ui->spinConnectionWeight->blockSignals(false);
}

void CalamusMain::onSelectionCleared()
{
    // Clear current container reference
    currentContainer = nullptr;

    // Hide both inspectors when nothing is selected
    ui->groupContainerInspector->setVisible(false);
    ui->groupConnectionInspector->setVisible(false);

    // Disable connection inspector controls
    ui->comboConnectionFunction->setEnabled(false);
    ui->spinConnectionWeight->setEnabled(false);
    ui->buttonDisconnect->setEnabled(false);

    // Clear fields
    ui->editInstanceName->clear();
    ui->labelContainerType->clear();
    ui->labelContainerDescription->clear();
    ui->listWidgetInputs->clear();
    ui->listWidgetOutputs->clear();
    clearConfigInspector();
    ui->labelConnectionFrom->clear();
    ui->labelConnectionTo->clear();
}

void CalamusMain::onInstanceNameChanged()
{
    Canvas *canvas = sounitBuilder->getCanvas();
    if (canvas->selectedContainer) {
        canvas->selectedContainer->setInstanceName(ui->editInstanceName->text());
    }
}

void CalamusMain::onConnectionFunctionChanged(int index)
{
    Canvas *canvas = sounitBuilder->getCanvas();
    Canvas::Connection *conn = canvas->getSelectedConnection();
    if (conn) {
        conn->function = ui->comboConnectionFunction->currentText();
        qDebug() << "Connection function changed to:" << conn->function;

        // Trigger graph rebuild to apply new connection function
        emit canvas->graphChanged();
    }
}

void CalamusMain::onConnectionWeightChanged(double value)
{
    Canvas *canvas = sounitBuilder->getCanvas();
    Canvas::Connection *conn = canvas->getSelectedConnection();
    if (conn) {
        conn->weight = value;
        qDebug() << "Connection weight changed to:" << conn->weight;

        // Trigger graph rebuild to apply new connection weight
        emit canvas->graphChanged();
    }
}

void CalamusMain::onDisconnectClicked()
{
    Canvas *canvas = sounitBuilder->getCanvas();
    Canvas::Connection *conn = canvas->getSelectedConnection();
    if (conn) {
        // Find and remove the connection
        QVector<Canvas::Connection> &connections = canvas->getConnections();
        for (int i = 0; i < connections.size(); ++i) {
            Canvas::Connection &c = connections[i];
            if (&c == conn) {
                connections.removeAt(i);
                canvas->clearSelection();
                canvas->update();
                qDebug() << "Connection disconnected";

                // Trigger graph rebuild to apply connection removal
                emit canvas->graphChanged();
                break;
            }
        }
    }
}

void CalamusMain::onConfigParameterChanged(QListWidgetItem *item)
{
    // This method is deprecated - now using container-specific inspectors
    // Kept for compatibility but does nothing
}

// Helper method to clear all dynamic inspector controls
void CalamusMain::clearConfigInspector()
{
    // Get the layout from the scroll area contents
    QLayout *layout = ui->scrollAreaConfigContents->layout();
    if (!layout) return;

    // Delete all widgets in the layout
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Clear member variable pointers
    comboDnaSelect = nullptr;
    sliderNumHarmonics = nullptr;
    spinNumHarmonics = nullptr;
    spectrumViz = nullptr;
    comboEnvelopeSelect = nullptr;
    envelopeParamsWidget = nullptr;
    envelopeViz = nullptr;
}

// Get description text for each container type
QString CalamusMain::getContainerDescription(const QString &containerType)
{
    if (containerType == "Harmonic Generator") {
        return "Generates harmonic spectrum using additive synthesis. "
               "Creates rich, controllable timbres from fundamental frequency and harmonic series.";
    } else if (containerType == "Spectrum to Signal") {
        return "Converts harmonic spectrum to audio signal using additive synthesis.";
    } else if (containerType == "Rolloff Processor") {
        return "Applies rolloff curve to attenuate higher harmonics, shaping spectral brightness.";
    } else if (containerType == "Formant Body") {
        return "Resonant filter pair simulating vocal tract formants for vocal-like timbres.";
    } else if (containerType == "Breath Turbulence") {
        return "Blends voice signal with noise for breathiness and air texture.";
    } else if (containerType == "Noise Color Filter") {
        return "Generates and filters colored noise with adjustable spectral characteristics.";
    } else if (containerType == "Physics System") {
        return "Spring-mass-damper system for natural, physics-based parameter evolution.";
    } else if (containerType == "Easing Applicator") {
        return "Applies easing curves to interpolate smoothly between parameter values.";
    } else if (containerType == "Envelope Engine") {
        return "Multi-stage envelope generator for shaping parameter evolution over note lifetime.";
    } else if (containerType == "Drift Engine") {
        return "Generates subtle random variations for organic, living sound textures.";
    } else if (containerType == "Gate Processor") {
        return "Note gate with velocity-sensitive attack/release envelopes.";
    }
    return "";
}

// Create Harmonic Generator-specific inspector controls
void CalamusMain::populateHarmonicGeneratorInspector()
{
    if (!currentContainer) return;

    // Get the existing layout from the scroll area contents
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) {
        qDebug() << "Warning: scrollAreaConfigContents has no layout!";
        return;
    }

    // Create a widget to hold the form layout
    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    // DNA Select dropdown
    QLabel *labelDna = new QLabel("DNA Select:");
    comboDnaSelect = new QComboBox();
    comboDnaSelect->addItem("Vocal (Bright)", 0);
    comboDnaSelect->addItem("Vocal (Warm)", 1);
    comboDnaSelect->addItem("Brass (Trumpet-like)", 2);
    comboDnaSelect->addItem("Reed (Clarinet-like)", 3);
    comboDnaSelect->addItem("String (Violin-like)", 4);
    comboDnaSelect->addItem("Custom...", -1);

    // Set current value (block signals to prevent triggering dialog on initial load)
    int dnaValue = static_cast<int>(currentContainer->getParameter("dnaSelect", 0.0));
    int index = comboDnaSelect->findData(dnaValue);
    if (index >= 0) {
        comboDnaSelect->blockSignals(true);
        comboDnaSelect->setCurrentIndex(index);
        comboDnaSelect->blockSignals(false);
    }

    // Use 'activated' instead of 'currentIndexChanged' so clicking Custom again opens the dialog
    connect(comboDnaSelect, QOverload<int>::of(&QComboBox::activated),
            this, &CalamusMain::onDnaSelectChanged);
    formLayout->addRow(labelDna, comboDnaSelect);

    // numHarmonics slider + spinbox
    QLabel *labelNumHarmonics = new QLabel("Num Harmonics:");
    QHBoxLayout *layoutNumHarmonics = new QHBoxLayout();
    sliderNumHarmonics = new QSlider(Qt::Horizontal);
    sliderNumHarmonics->setRange(1, 128);
    sliderNumHarmonics->setValue(static_cast<int>(currentContainer->getParameter("numHarmonics", 64.0)));
    spinNumHarmonics = new QSpinBox();
    spinNumHarmonics->setRange(1, 128);
    spinNumHarmonics->setValue(sliderNumHarmonics->value());
    layoutNumHarmonics->addWidget(sliderNumHarmonics);
    layoutNumHarmonics->addWidget(spinNumHarmonics);

    connect(sliderNumHarmonics, &QSlider::valueChanged, spinNumHarmonics, &QSpinBox::setValue);
    connect(spinNumHarmonics, QOverload<int>::of(&QSpinBox::valueChanged),
            sliderNumHarmonics, &QSlider::setValue);
    connect(spinNumHarmonics, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CalamusMain::onNumHarmonicsChanged);
    formLayout->addRow(labelNumHarmonics, layoutNumHarmonics);

    // Add the form widget to the main layout
    mainLayout->addWidget(formWidget);

    // Add spectrum visualizer
    QLabel *labelSpectrum = new QLabel("Spectrum Preview:");
    labelSpectrum->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    mainLayout->addWidget(labelSpectrum);

    spectrumViz = new SpectrumVisualizer();
    int numHarmonics = static_cast<int>(currentContainer->getParameter("numHarmonics", 64.0));

    // Check if this is a custom DNA pattern (dnaValue == -1)
    if (dnaValue == -1) {
        // Load custom DNA pattern from container
        int customDnaCount = static_cast<int>(currentContainer->getParameter("customDnaCount", 0.0));
        if (customDnaCount > 0) {
            QVector<double> customAmplitudes;
            for (int i = 0; i < customDnaCount; i++) {
                QString paramName = QString("customDna_%1").arg(i);
                double amp = currentContainer->getParameter(paramName, 0.0);
                customAmplitudes.append(amp);
            }
            spectrumViz->setSpectrum(customAmplitudes);
            qDebug() << "Loaded custom DNA pattern with" << customDnaCount << "harmonics for display";
        } else {
            // No custom pattern stored, show default rolloff
            spectrumViz->setDnaPreset(-1, numHarmonics, 0.0);
        }
    } else {
        // Using preset DNA
        spectrumViz->setDnaPreset(dnaValue, numHarmonics, 0.0);
    }

    mainLayout->addWidget(spectrumViz);

    mainLayout->addStretch();  // Push controls to top

    // CRITICAL: Initialize any unset parameters to their UI defaults
    // This ensures newly created containers have proper default values
    bool needsRebuild = false;
    if (currentContainer->getParameter("dnaSelect", -999.0) == -999.0) {
        currentContainer->setParameter("dnaSelect", 0.0);  // Default to Vocal (Bright)
        needsRebuild = true;
    }
    if (currentContainer->getParameter("numHarmonics", -999.0) == -999.0) {
        currentContainer->setParameter("numHarmonics", 64.0);
        needsRebuild = true;
    }

    // Rebuild graph to apply defaults if any were set
    if (needsRebuild) {
        sounitBuilder->rebuildGraph();
        qDebug() << "Applied default parameters to new Harmonic Generator";
    }
}

// Helper to update spectrum visualization
void CalamusMain::updateSpectrumVisualization()
{
    if (!spectrumViz || !currentContainer) return;

    int numHarmonics = static_cast<int>(currentContainer->getParameter("numHarmonics", 64.0));
    int dnaValue = static_cast<int>(currentContainer->getParameter("dnaSelect", 0.0));

    // If custom DNA, load and resize the custom pattern
    if (dnaValue == -1) {
        int customDnaCount = static_cast<int>(currentContainer->getParameter("customDnaCount", 0.0));
        if (customDnaCount > 0) {
            QVector<double> customAmplitudes;
            for (int i = 0; i < customDnaCount; i++) {
                QString paramName = QString("customDna_%1").arg(i);
                double amp = currentContainer->getParameter(paramName, 0.0);
                customAmplitudes.append(amp);
            }

            // Resize the pattern to match new numHarmonics
            if (customAmplitudes.size() < numHarmonics) {
                // Add zeros at the end
                customAmplitudes.resize(numHarmonics, 0.0);
            } else if (customAmplitudes.size() > numHarmonics) {
                // Truncate
                customAmplitudes.resize(numHarmonics);
            }

            spectrumViz->setSpectrum(customAmplitudes);
        } else {
            // No custom pattern stored, use rolloff
            spectrumViz->setDnaPreset(-1, numHarmonics, 0.0);
        }
    } else {
        // Using preset DNA
        spectrumViz->setDnaPreset(dnaValue, numHarmonics, 0.0);
    }
}

// Harmonic Generator parameter change slots
void CalamusMain::onDnaSelectChanged(int index)
{
    if (!currentContainer) return;

    int dnaValue = comboDnaSelect->itemData(index).toInt();

    if (dnaValue == -1) {
        // Custom option selected - open DNA editor dialog
        int numHarmonics = static_cast<int>(currentContainer->getParameter("numHarmonics", 64.0));
        DnaEditorDialog dialog(numHarmonics, this);

        // If there's an existing custom pattern, load it into the dialog
        int customDnaCount = static_cast<int>(currentContainer->getParameter("customDnaCount", 0.0));
        if (customDnaCount > 0) {
            QVector<double> existingPattern;
            for (int i = 0; i < customDnaCount; i++) {
                QString paramName = QString("customDna_%1").arg(i);
                double amp = currentContainer->getParameter(paramName, 0.0);
                existingPattern.append(amp);
            }
            // Load the existing pattern into the dialog
            dialog.setCustomPattern(existingPattern);
            qDebug() << "Loaded existing custom DNA pattern with" << customDnaCount << "harmonics into editor";
        }

        if (dialog.exec() == QDialog::Accepted) {
            // User clicked OK - get the custom DNA pattern
            QVector<double> customDna = dialog.getCustomDnaPattern();
            qDebug() << "Custom DNA created with" << customDna.size() << "harmonics";

            // Update spectrum visualization with custom pattern
            if (spectrumViz) {
                spectrumViz->setSpectrum(customDna);
            }

            // Batch parameter updates to avoid multiple graph rebuilds
            currentContainer->beginParameterUpdate();

            // Store dnaSelect = -1 to indicate custom mode
            currentContainer->setParameter("dnaSelect", -1.0);

            // Store custom DNA pattern - using individual parameters for each harmonic
            // (In the future, we could add QMap<QString, QString> to Container for JSON storage)
            for (int i = 0; i < customDna.size(); i++) {
                QString paramName = QString("customDna_%1").arg(i);
                currentContainer->setParameter(paramName, customDna[i]);
            }
            // Store the count
            currentContainer->setParameter("customDnaCount", static_cast<double>(customDna.size()));

            // End batch update - this triggers a single graph rebuild
            currentContainer->endParameterUpdate();
            qDebug() << "DNA Select changed to: Custom (-1) with" << customDna.size() << "harmonics stored";

            // CRITICAL: Update the spectrum visualizer in the config section to show the new pattern
            if (spectrumViz) {
                spectrumViz->setSpectrum(customDna);
            }
        } else {
            // User cancelled - revert to previous selection
            // Find the current dnaSelect value and set combo box back to it
            int currentDna = static_cast<int>(currentContainer->getParameter("dnaSelect", 0.0));
            int comboIndex = comboDnaSelect->findData(currentDna);
            if (comboIndex >= 0) {
                comboDnaSelect->blockSignals(true);
                comboDnaSelect->setCurrentIndex(comboIndex);
                comboDnaSelect->blockSignals(false);
            }
        }
        return;
    }

    currentContainer->setParameter("dnaSelect", static_cast<double>(dnaValue));
    updateSpectrumVisualization();  // Update spectrum preview
    sounitBuilder->rebuildGraph();  // Rebuild graph to apply changes
    qDebug() << "DNA Select changed to:" << dnaValue;
}

void CalamusMain::onNumHarmonicsChanged(int value)
{
    if (!currentContainer) return;

    // If custom DNA is loaded, resize the custom pattern
    int dnaValue = static_cast<int>(currentContainer->getParameter("dnaSelect", 0.0));
    if (dnaValue == -1) {
        int customDnaCount = static_cast<int>(currentContainer->getParameter("customDnaCount", 0.0));
        if (customDnaCount > 0) {
            QVector<double> customAmplitudes;
            for (int i = 0; i < customDnaCount; i++) {
                QString paramName = QString("customDna_%1").arg(i);
                double amp = currentContainer->getParameter(paramName, 0.0);
                customAmplitudes.append(amp);
            }

            // Resize the pattern
            if (customAmplitudes.size() < value) {
                // Add zeros at the end
                customAmplitudes.resize(value, 0.0);
            } else if (customAmplitudes.size() > value) {
                // Truncate
                customAmplitudes.resize(value);
            }

            // Batch parameter updates to avoid multiple graph rebuilds
            currentContainer->beginParameterUpdate();

            // Store the resized pattern back
            for (int i = 0; i < customAmplitudes.size(); i++) {
                QString paramName = QString("customDna_%1").arg(i);
                currentContainer->setParameter(paramName, customAmplitudes[i]);
            }
            currentContainer->setParameter("customDnaCount", static_cast<double>(customAmplitudes.size()));

            // End batch update
            currentContainer->endParameterUpdate();

            qDebug() << "Resized custom DNA pattern to" << value << "harmonics";
        }
    }

    currentContainer->setParameter("numHarmonics", static_cast<double>(value));
    updateSpectrumVisualization();
    qDebug() << "Num Harmonics changed to:" << value;
}

// Helper function to add a parameter slider to the inspector
void CalamusMain::addParameterSlider(QFormLayout *layout, const QString &label,
                                     const QString &paramName, double minVal, double maxVal,
                                     double defaultVal, double step, int decimals)
{
    if (!currentContainer) return;

    QHBoxLayout *paramLayout = new QHBoxLayout();
    QSlider *slider = new QSlider(Qt::Horizontal);
    QDoubleSpinBox *spinBox = new QDoubleSpinBox();

    // Calculate slider range (map double range to int)
    int sliderMin = static_cast<int>(minVal / step);
    int sliderMax = static_cast<int>(maxVal / step);
    int sliderVal = static_cast<int>(currentContainer->getParameter(paramName, defaultVal) / step);

    slider->setRange(sliderMin, sliderMax);
    slider->setValue(sliderVal);

    spinBox->setRange(minVal, maxVal);
    spinBox->setSingleStep(step);
    spinBox->setDecimals(decimals);
    spinBox->setValue(currentContainer->getParameter(paramName, defaultVal));

    paramLayout->addWidget(slider);
    paramLayout->addWidget(spinBox);

    // Connect slider and spinbox
    connect(slider, &QSlider::valueChanged, this, [this, spinBox, slider, paramName, step](int value) {
        double doubleVal = value * step;
        spinBox->blockSignals(true);
        spinBox->setValue(doubleVal);
        spinBox->blockSignals(false);
        if (currentContainer) {
            currentContainer->setParameter(paramName, doubleVal);
            sounitBuilder->rebuildGraph();
        }
    });

    connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this, slider, paramName, step](double value) {
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(value / step));
        slider->blockSignals(false);
        if (currentContainer) {
            currentContainer->setParameter(paramName, value);
            sounitBuilder->rebuildGraph();
        }
    });

    layout->addRow(label + ":", paramLayout);
}

// Rolloff Processor Inspector
void CalamusMain::populateRolloffProcessorInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    addParameterSlider(formLayout, "Rolloff Power", "rolloff", 0.2, 5.0, 0.6, 0.01, 2);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Spectrum to Signal Inspector
void CalamusMain::populateSpectrumToSignalInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    addParameterSlider(formLayout, "Normalize", "normalize", 0.0, 1.0, 1.0, 0.01, 2);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Formant Body Inspector
void CalamusMain::populateFormantBodyInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    addParameterSlider(formLayout, "F1 Frequency (Hz)", "f1Freq", 200.0, 1000.0, 500.0, 1.0, 0);
    addParameterSlider(formLayout, "F2 Frequency (Hz)", "f2Freq", 800.0, 3000.0, 1500.0, 1.0, 0);
    addParameterSlider(formLayout, "F1 Q", "f1Q", 1.0, 20.0, 8.0, 0.1, 1);
    addParameterSlider(formLayout, "F2 Q", "f2Q", 1.0, 20.0, 10.0, 0.1, 1);
    addParameterSlider(formLayout, "Direct Mix", "directMix", 0.0, 1.0, 0.3, 0.01, 2);
    addParameterSlider(formLayout, "F1/F2 Balance", "f1f2Balance", 0.0, 1.0, 0.6, 0.01, 2);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Breath Turbulence Inspector
void CalamusMain::populateBreathTurbulenceInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    addParameterSlider(formLayout, "Blend", "blend", 0.0, 1.0, 0.10, 0.01, 2);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Noise Color Filter Inspector
void CalamusMain::populateNoiseColorFilterInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    // Noise Type dropdown
    QComboBox *comboNoiseType = new QComboBox();
    comboNoiseType->addItem("White Noise", 0);
    comboNoiseType->addItem("Pink Noise", 1);
    comboNoiseType->addItem("Brown Noise", 2);
    int noiseType = static_cast<int>(currentContainer->getParameter("noiseType", 0.0));
    comboNoiseType->setCurrentIndex(noiseType);
    connect(comboNoiseType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (currentContainer) {
            currentContainer->setParameter("noiseType", static_cast<double>(index));
            sounitBuilder->rebuildGraph();
        }
    });
    formLayout->addRow("Noise Type:", comboNoiseType);

    // Add note about noise type
    QLabel *noteLabel = new QLabel("(Only used when audioIn not connected)");
    noteLabel->setStyleSheet("color: gray; font-size: 9pt;");
    formLayout->addRow("", noteLabel);

    addParameterSlider(formLayout, "Color (Hz)", "color", 100.0, 8000.0, 2000.0, 10.0, 0);
    addParameterSlider(formLayout, "Filter Q", "filterQ", 0.5, 10.0, 1.0, 0.1, 1);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Physics System Inspector
void CalamusMain::populatePhysicsSystemInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    addParameterSlider(formLayout, "Mass", "mass", 0.0, 10.0, 0.5, 0.1, 1);
    addParameterSlider(formLayout, "Spring K", "springK", 0.0001, 1.0, 0.001, 0.001, 3);
    addParameterSlider(formLayout, "Damping", "damping", 0.5, 0.9999, 0.995, 0.001, 3);
    addParameterSlider(formLayout, "Impulse Amount", "impulseAmount", 0.0, 1000.0, 100.0, 1.0, 0);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Envelope Engine Inspector
void CalamusMain::populateEnvelopeEngineInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    // Envelope Select dropdown
    comboEnvelopeSelect = new QComboBox();
    comboEnvelopeSelect->addItem("Linear", 0);
    comboEnvelopeSelect->addItem("Attack-Decay", 1);
    comboEnvelopeSelect->addItem("ADSR", 2);
    comboEnvelopeSelect->addItem("Fade In", 3);
    comboEnvelopeSelect->addItem("Fade Out", 4);
    comboEnvelopeSelect->addItem("Custom...", 5);
    int envValue = static_cast<int>(currentContainer->getParameter("envelopeSelect", 0.0));
    comboEnvelopeSelect->setCurrentIndex(envValue);

    // Lambda to open custom envelope dialog
    auto openCustomEnvelopeDialog = [this]() {
        if (!currentContainer) return;

        // Open Envelope Library dialog
        EnvelopeLibraryDialog dialog(this);

        // If already using custom envelope, load it into the dialog
        if (currentContainer->hasCustomEnvelopeData()) {
            dialog.setEnvelope(currentContainer->getCustomEnvelopeData());
        }

        if (dialog.exec() == QDialog::Accepted) {
            EnvelopeData selectedEnvelope = dialog.getSelectedEnvelope();
            // Store custom envelope data in container
            currentContainer->setParameter("envelopeSelect", 5.0);
            currentContainer->setCustomEnvelopeData(selectedEnvelope);
            // Update the UI and visualizer
            updateEnvelopeParameters(5);
            // Rebuild graph to apply the custom envelope
            sounitBuilder->rebuildGraph();
        } else {
            // User cancelled - revert to previous selection if not already custom
            int previousValue = static_cast<int>(currentContainer->getParameter("envelopeSelect", 0.0));
            if (previousValue != 5) {
                comboEnvelopeSelect->blockSignals(true);
                comboEnvelopeSelect->setCurrentIndex(previousValue);
                comboEnvelopeSelect->blockSignals(false);
            }
        }
    };

    // Use activated signal to handle clicks even when already selected
    connect(comboEnvelopeSelect, QOverload<int>::of(&QComboBox::activated),
            this, [this, openCustomEnvelopeDialog](int index) {
        if (!currentContainer) return;

        if (index == 5) {
            openCustomEnvelopeDialog();
        } else {
            // Standard envelope type
            currentContainer->setParameter("envelopeSelect", static_cast<double>(index));
            updateEnvelopeParameters(index);
            sounitBuilder->rebuildGraph();
        }
    });
    formLayout->addRow("Envelope Shape:", comboEnvelopeSelect);

    mainLayout->addWidget(formWidget);

    // Create container for contextual parameters
    envelopeParamsWidget = new QWidget();
    mainLayout->addWidget(envelopeParamsWidget);

    // Envelope preview visualizer
    QLabel *labelPreview = new QLabel("Envelope Preview:");
    labelPreview->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(labelPreview);

    envelopeViz = new EnvelopeVisualizer();
    mainLayout->addWidget(envelopeViz);

    // Populate parameters based on current envelope type
    updateEnvelopeParameters(envValue);

    mainLayout->addStretch();
}

// Drift Engine Inspector
void CalamusMain::populateDriftEngineInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    // Drift Pattern dropdown
    QComboBox *comboDriftPattern = new QComboBox();
    comboDriftPattern->addItem("Sine", 0);
    comboDriftPattern->addItem("Random", 1);
    comboDriftPattern->addItem("Perlin (Smooth)", 2);
    int driftPattern = static_cast<int>(currentContainer->getParameter("driftPattern", 2.0));
    comboDriftPattern->setCurrentIndex(driftPattern);
    connect(comboDriftPattern, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (currentContainer) {
            currentContainer->setParameter("driftPattern", static_cast<double>(index));
            sounitBuilder->rebuildGraph();
        }
    });
    formLayout->addRow("Pattern:", comboDriftPattern);

    addParameterSlider(formLayout, "Amount", "amount", 0.0, 0.1, 0.005, 0.001, 3);
    addParameterSlider(formLayout, "Rate (Hz)", "rate", 0.01, 10.0, 0.5, 0.01, 2);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Gate Processor Inspector
void CalamusMain::populateGateProcessorInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    addParameterSlider(formLayout, "Velocity", "velocity", 0.0, 1.0, 1.0, 0.01, 2);
    addParameterSlider(formLayout, "Attack Time (s)", "attackTime", 0.001, 1.0, 0.01, 0.001, 3);
    addParameterSlider(formLayout, "Release Time (s)", "releaseTime", 0.001, 2.0, 0.1, 0.001, 3);
    addParameterSlider(formLayout, "Velocity Sens", "velocitySens", 0.0, 1.0, 0.5, 0.01, 2);

    // Attack/Release Curve dropdowns
    QComboBox *comboAttack = new QComboBox();
    comboAttack->addItem("Linear", 0);
    comboAttack->addItem("Exponential", 1);
    comboAttack->addItem("Logarithmic", 2);
    int attackCurve = static_cast<int>(currentContainer->getParameter("attackCurve", 0.0));
    comboAttack->setCurrentIndex(attackCurve);
    connect(comboAttack, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (currentContainer) {
            currentContainer->setParameter("attackCurve", static_cast<double>(index));
            sounitBuilder->rebuildGraph();
        }
    });
    formLayout->addRow("Attack Curve:", comboAttack);

    QComboBox *comboRelease = new QComboBox();
    comboRelease->addItem("Linear", 0);
    comboRelease->addItem("Exponential", 1);
    comboRelease->addItem("Logarithmic", 2);
    int releaseCurve = static_cast<int>(currentContainer->getParameter("releaseCurve", 0.0));
    comboRelease->setCurrentIndex(releaseCurve);
    connect(comboRelease, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (currentContainer) {
            currentContainer->setParameter("releaseCurve", static_cast<double>(index));
            sounitBuilder->rebuildGraph();
        }
    });
    formLayout->addRow("Release Curve:", comboRelease);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Easing Applicator Inspector
void CalamusMain::populateEasingApplicatorInspector()
{
    if (!currentContainer) return;
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaConfigContents->layout());
    if (!mainLayout) return;

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    // Easing Select dropdown
    QComboBox *comboEasing = new QComboBox();
    comboEasing->addItem("Linear", 0);
    comboEasing->addItem("Quad In", 1);
    comboEasing->addItem("Quad Out", 2);
    comboEasing->addItem("Quad InOut", 3);
    comboEasing->addItem("Cubic In", 4);
    comboEasing->addItem("Cubic Out", 5);
    comboEasing->addItem("Cubic InOut", 6);
    comboEasing->addItem("Sine In", 7);
    comboEasing->addItem("Sine Out", 8);
    comboEasing->addItem("Sine InOut", 9);
    comboEasing->addItem("Elastic", 10);
    comboEasing->addItem("Bounce", 11);
    comboEasing->addItem("Back", 12);
    int easingValue = static_cast<int>(currentContainer->getParameter("easingSelect", 0.0));
    comboEasing->setCurrentIndex(easingValue);
    connect(comboEasing, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (currentContainer) {
            currentContainer->setParameter("easingSelect", static_cast<double>(index));
            sounitBuilder->rebuildGraph();
        }
    });
    formLayout->addRow("Easing Function:", comboEasing);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
}

// Update envelope parameters based on envelope type (contextual sliders)
void CalamusMain::updateEnvelopeParameters(int envelopeType)
{
    if (!currentContainer || !envelopeParamsWidget || !envelopeViz) return;

    // Hide widget to prevent visual artifacts during rebuild
    envelopeParamsWidget->setVisible(false);

    // Clear existing parameters
    if (envelopeParamsWidget->layout()) {
        QLayout *oldLayout = envelopeParamsWidget->layout();
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->setVisible(false);
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }

    // Create new layout for parameters
    QFormLayout *paramLayout = new QFormLayout(envelopeParamsWidget);
    paramLayout->setSpacing(10);
    paramLayout->setContentsMargins(0, 0, 0, 0);

    // Helper to create slider+spinbox and connect to preview
    auto addEnvelopeParam = [this, paramLayout](const QString &label, const QString &paramName,
                                                 double minVal, double maxVal, double defaultVal,
                                                 double step, int decimals) {
        if (!currentContainer) return;

        QHBoxLayout *layout = new QHBoxLayout();
        QSlider *slider = new QSlider(Qt::Horizontal);
        QDoubleSpinBox *spinBox = new QDoubleSpinBox();

        int sliderMin = static_cast<int>(minVal / step);
        int sliderMax = static_cast<int>(maxVal / step);
        int sliderVal = static_cast<int>(currentContainer->getParameter(paramName, defaultVal) / step);

        slider->setRange(sliderMin, sliderMax);
        slider->setValue(sliderVal);

        spinBox->setRange(minVal, maxVal);
        spinBox->setSingleStep(step);
        spinBox->setDecimals(decimals);
        spinBox->setValue(currentContainer->getParameter(paramName, defaultVal));

        layout->addWidget(slider);
        layout->addWidget(spinBox);

        // Connect slider and spinbox
        connect(slider, &QSlider::valueChanged, this, [this, spinBox, slider, paramName, step](int value) {
            double doubleVal = value * step;
            spinBox->blockSignals(true);
            spinBox->setValue(doubleVal);
            spinBox->blockSignals(false);
            if (currentContainer) {
                currentContainer->setParameter(paramName, doubleVal);
                updateEnvelopePreview();
                sounitBuilder->rebuildGraph();
            }
        });

        connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this, slider, paramName, step](double value) {
            slider->blockSignals(true);
            slider->setValue(static_cast<int>(value / step));
            slider->blockSignals(false);
            if (currentContainer) {
                currentContainer->setParameter(paramName, value);
                updateEnvelopePreview();
                sounitBuilder->rebuildGraph();
            }
        });

        paramLayout->addRow(label + ":", layout);
    };

    // Add parameters based on envelope type
    switch (envelopeType) {
    case 0: // Linear - no parameters
        break;

    case 1: // Attack-Decay
        addEnvelopeParam("Attack Time (s)", "envAttack", 0.001, 2.0, 0.1, 0.001, 3);
        addEnvelopeParam("Decay Time (s)", "envDecay", 0.001, 2.0, 0.2, 0.001, 3);
        break;

    case 2: // ADSR
        addEnvelopeParam("Attack Time (s)", "envAttack", 0.001, 2.0, 0.1, 0.001, 3);
        addEnvelopeParam("Decay Time (s)", "envDecay", 0.001, 2.0, 0.1, 0.001, 3);
        addEnvelopeParam("Sustain Level", "envSustain", 0.0, 1.0, 0.7, 0.01, 2);
        addEnvelopeParam("Release Time (s)", "envRelease", 0.001, 3.0, 0.2, 0.001, 3);
        break;

    case 3: // Fade In
        addEnvelopeParam("Fade Time", "envFadeTime", 0.0, 1.0, 0.5, 0.01, 2);
        break;

    case 4: // Fade Out
        addEnvelopeParam("Fade Time", "envFadeTime", 0.0, 1.0, 0.5, 0.01, 2);
        break;

    case 5: // Custom - no parameters (configured in library dialog)
        // Display custom envelope in visualizer
        if (currentContainer->hasCustomEnvelopeData() && envelopeViz) {
            EnvelopeData customData = currentContainer->getCustomEnvelopeData();
            envelopeViz->setCustomEnvelope(customData.points);
        }
        break;
    }

    // Show widget again after rebuild
    envelopeParamsWidget->setVisible(true);

    // Always show envelope preview
    if (envelopeViz) {
        envelopeViz->setVisible(true);
    }

    // Update preview for non-custom envelopes
    if (envelopeType != 5) {
        updateEnvelopePreview();
    }
}

// Update envelope preview visualization
void CalamusMain::updateEnvelopePreview()
{
    if (!currentContainer || !envelopeViz) return;

    int envelopeType = static_cast<int>(currentContainer->getParameter("envelopeSelect", 0.0));
    double attack = currentContainer->getParameter("envAttack", 0.1);
    double decay = currentContainer->getParameter("envDecay", 0.2);
    double sustain = currentContainer->getParameter("envSustain", 0.7);
    double release = currentContainer->getParameter("envRelease", 0.2);
    double fadeTime = currentContainer->getParameter("envFadeTime", 0.5);

    envelopeViz->setEnvelope(envelopeType, attack, decay, sustain, release, fadeTime);
}

void CalamusMain::closeEvent(QCloseEvent *event)
{
    // Close ScoreCanvasWindow when main window closes
    if (scoreCanvasWindow) {
        scoreCanvasWindow->close();
    }

    // Accept the close event
    QMainWindow::closeEvent(event);
}

CalamusMain::~CalamusMain()
{
    // Clean up in reverse order
    delete scoreCanvasWindow;
    delete sounitBuilder;

    // Shutdown and delete audio engine last
    if (audioEngine) {
        audioEngine->shutdown();
        delete audioEngine;
    }

    delete ui;
}

void CalamusMain::onNewSounit()
{
    Canvas *canvas = sounitBuilder->getCanvas();

    // Clear all containers
    QList<Container*> containers = canvas->findChildren<Container*>();
    for (Container *c : containers) {
        c->deleteLater();
    }

    // Clear connections
    canvas->getConnections().clear();
    canvas->clearSelection();
    canvas->update();

    // Reset sounit name and comment
    canvas->setSounitName("Untitled Sounit");
    canvas->setSounitComment("");
    currentSounitFilePath.clear();

    // Rebuild empty graph
    sounitBuilder->rebuildGraph();

    qDebug() << "New Sounit created";
}

void CalamusMain::onLoadSounit()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load Sounit",
        "",
        "Sounit Files (*.sounit);;JSON Files (*.json);;All Files (*)"
    );

    if (fileName.isEmpty()) {
        return;  // User cancelled
    }

    Canvas *canvas = sounitBuilder->getCanvas();
    QString sounitName;

    if (!canvas->loadFromJson(fileName, sounitName)) {
        QMessageBox::critical(this, "Load Error",
            "Failed to load sounit from:\n" + fileName);
        return;
    }

    // Reconnect container signals after load
    QList<Container*> containers = canvas->findChildren<Container*>();
    for (Container *container : containers) {
        sounitBuilder->connectContainerSignals(container);
    }

    currentSounitFilePath = fileName;

    // Rebuild graph with loaded containers/connections
    sounitBuilder->rebuildGraph();

    QMessageBox::information(this, "Loaded",
        QString("Sounit '%1' loaded successfully").arg(sounitName));

    qDebug() << "Sounit loaded from:" << fileName;
}

void CalamusMain::onSaveSounitAs()
{
    // Prompt for sounit name first
    Canvas *canvas = sounitBuilder->getCanvas();
    QString currentName = canvas->getSounitName();

    bool ok;
    QString sounitName = QInputDialog::getText(
        this,
        "Save Sounit As",
        "Enter sounit name:",
        QLineEdit::Normal,
        currentName,
        &ok
    );

    if (!ok || sounitName.isEmpty()) {
        return;  // User cancelled
    }

    // Update canvas sounit name
    canvas->setSounitName(sounitName);

    // Open file dialog
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Sounit As",
        sounitName + ".sounit",  // Suggest filename based on sounit name
        "Sounit Files (*.sounit);;JSON Files (*.json);;All Files (*)"
    );

    if (fileName.isEmpty()) {
        return;  // User cancelled
    }

    // Ensure .sounit extension
    if (!fileName.endsWith(".sounit", Qt::CaseInsensitive) &&
        !fileName.endsWith(".json", Qt::CaseInsensitive)) {
        fileName += ".sounit";
    }

    if (!canvas->saveToJson(fileName, sounitName)) {
        QMessageBox::critical(this, "Save Error",
            "Failed to save sounit to:\n" + fileName);
        return;
    }

    currentSounitFilePath = fileName;

    QMessageBox::information(this, "Saved",
        QString("Sounit '%1' saved successfully").arg(sounitName));

    qDebug() << "Sounit saved to:" << fileName;
}

void CalamusMain::onSounitNameChanged(const QString &name)
{
    // Update window title to show sounit name
    setWindowTitle(QString("Calamus - %1").arg(name));

    // Update the UI field (block signals to avoid recursive updates)
    ui->editSounitName->blockSignals(true);
    ui->editSounitName->setText(name);
    ui->editSounitName->blockSignals(false);

    // Update track 0 in score canvas with sounit info
    if (scoreCanvasWindow) {
        // Use a distinctive color for the sounit track (deep blue)
        QColor sounitColor(0, 102, 204);  // Deep blue
        scoreCanvasWindow->updateSounitTrack(name, sounitColor);
    }
}

void CalamusMain::onSounitCommentChanged(const QString &comment)
{
    // Update the UI field (block signals to avoid recursive updates)
    ui->editSounitComment->blockSignals(true);
    ui->editSounitComment->setText(comment);
    ui->editSounitComment->blockSignals(false);
}

void CalamusMain::onSounitNameEdited()
{
    Canvas *canvas = sounitBuilder->getCanvas();
    canvas->setSounitName(ui->editSounitName->text());
}

void CalamusMain::onSounitCommentEdited()
{
    Canvas *canvas = sounitBuilder->getCanvas();
    canvas->setSounitComment(ui->editSounitComment->text());
}

// ============================================================================
// Phrase Inspector Slots (Phase 4)
// ============================================================================

void CalamusMain::onPhraseSelectionChanged()
{
    ScoreCanvas *canvas = scoreCanvasWindow->getScoreCanvas();
    if (!canvas) return;

    const PhraseGroup *selectedPhrase = canvas->getSelectedPhrase();

    if (selectedPhrase) {
        // Populate inspector with phrase data
        ui->editPhraseName->blockSignals(true);
        ui->editPhraseName->setText(selectedPhrase->getName());
        ui->editPhraseName->blockSignals(false);

        // Set easing type
        ui->comboPhraseDynamicsEasing->blockSignals(true);
        if (selectedPhrase->hasEasing()) {
            QString easingType = selectedPhrase->getEasingType();
            int index = ui->comboPhraseDynamicsEasing->findText(easingType);
            if (index >= 0) {
                ui->comboPhraseDynamicsEasing->setCurrentIndex(index);
            }
        } else {
            ui->comboPhraseDynamicsEasing->setCurrentIndex(0);  // "None"
        }
        ui->comboPhraseDynamicsEasing->blockSignals(false);

        // Enable controls
        ui->editPhraseName->setEnabled(true);
        ui->comboPhraseDynamicsEasing->setEnabled(true);
        ui->btnUngroupPhrase->setEnabled(true);
        ui->btnSavePhraseTemplate->setEnabled(true);
        ui->btnApplyPhraseTemplate->setEnabled(false);  // Disabled when phrase selected

        // Switch to Phrase tab
        ui->tabInspector->setCurrentWidget(ui->tabPhraseInspector);
    } else {
        // No phrase selected - clear inspector
        ui->editPhraseName->clear();
        ui->editPhraseName->setEnabled(false);
        ui->comboPhraseDynamicsEasing->setCurrentIndex(0);
        ui->comboPhraseDynamicsEasing->setEnabled(false);
        ui->btnUngroupPhrase->setEnabled(false);
        ui->btnSavePhraseTemplate->setEnabled(false);
        ui->btnApplyPhraseTemplate->setEnabled(true);  // Enable if templates available
    }
}

void CalamusMain::onPhraseNameChanged(const QString &name)
{
    ScoreCanvas *canvas = scoreCanvasWindow->getScoreCanvas();
    if (!canvas) return;

    PhraseGroup *selectedPhrase = canvas->getSelectedPhrase();
    if (selectedPhrase) {
        selectedPhrase->setName(name);
        canvas->update();  // Redraw to show new name
    }
}

void CalamusMain::onPhraseEasingChanged(int index)
{
    ScoreCanvas *canvas = scoreCanvasWindow->getScoreCanvas();
    if (!canvas) return;

    PhraseGroup *selectedPhrase = canvas->getSelectedPhrase();
    if (!selectedPhrase) return;

    if (index == 0) {
        // "None" selected
        selectedPhrase->setUseEasing(false);
    } else {
        // Easing type selected
        QString easingType = ui->comboPhraseDynamicsEasing->currentText();
        selectedPhrase->setUseEasing(true);
        selectedPhrase->setEasingType(easingType);
    }

    canvas->update();  // Redraw
}

void CalamusMain::onLoadPhraseTemplateClicked()
{
    // Open file dialog to load .cphrase JSON file
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Load Phrase Template",
        QString(),  // Default directory
        "Calamus Phrase (*.cphrase);;All Files (*)"
    );

    if (filename.isEmpty()) return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Could not open file for reading.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        QMessageBox::warning(this, "Error", "Invalid phrase template file format.");
        return;
    }

    PhraseGroup loadedPhrase = PhraseGroup::fromJson(doc.object());

    // Add to saved templates
    savedPhraseTemplates.append(loadedPhrase);

    // Update combo box
    ui->comboPhraseTemplates->blockSignals(true);
    if (ui->comboPhraseTemplates->count() == 1 &&
        ui->comboPhraseTemplates->itemText(0) == "(No templates)") {
        ui->comboPhraseTemplates->clear();
    }
    ui->comboPhraseTemplates->addItem(loadedPhrase.getName());
    ui->comboPhraseTemplates->setCurrentIndex(ui->comboPhraseTemplates->count() - 1);
    ui->comboPhraseTemplates->blockSignals(false);

    qDebug() << "Loaded phrase template:" << loadedPhrase.getName();
}

void CalamusMain::onApplyPhraseTemplateClicked()
{
    ScoreCanvas *canvas = scoreCanvasWindow->getScoreCanvas();
    if (!canvas) return;

    // Get selected template
    int templateIndex = ui->comboPhraseTemplates->currentIndex();
    if (templateIndex < 0 || templateIndex >= savedPhraseTemplates.size()) {
        QMessageBox::warning(this, "Error", "No template selected.");
        return;
    }

    const PhraseGroup &templatePhrase = savedPhraseTemplates[templateIndex];

    // Apply template to current selection
    canvas->applyPhraseTemplate(templatePhrase);

    qDebug() << "Applied phrase template:" << templatePhrase.getName();
}

void CalamusMain::onCreatePhraseClicked()
{
    ScoreCanvas *canvas = scoreCanvasWindow->getScoreCanvas();
    if (!canvas) return;

    // Ask user for phrase name
    bool ok;
    QString name = QInputDialog::getText(
        this,
        "Create Phrase",
        "Phrase name:",
        QLineEdit::Normal,
        "New Phrase",
        &ok
    );

    if (ok && !name.isEmpty()) {
        canvas->createPhraseFromSelection(name);
        qDebug() << "Created phrase:" << name;
    }
}

void CalamusMain::onUngroupPhraseClicked()
{
    ScoreCanvas *canvas = scoreCanvasWindow->getScoreCanvas();
    if (!canvas) return;

    int selectedIndex = canvas->getSelectedPhraseIndex();
    if (selectedIndex >= 0) {
        canvas->ungroupPhrase(selectedIndex);
        qDebug() << "Ungrouped phrase at index" << selectedIndex;
    }
}

void CalamusMain::onSavePhraseTemplateClicked()
{
    ScoreCanvas *canvas = scoreCanvasWindow->getScoreCanvas();
    if (!canvas) return;

    const PhraseGroup *selectedPhrase = canvas->getSelectedPhrase();
    if (!selectedPhrase) {
        QMessageBox::warning(this, "Error", "No phrase selected.");
        return;
    }

    // Open file dialog to save .cphrase JSON file
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Save Phrase Template",
        selectedPhrase->getName() + ".cphrase",
        "Calamus Phrase (*.cphrase);;All Files (*)"
    );

    if (filename.isEmpty()) return;

    QJsonObject json = selectedPhrase->toJson();
    QJsonDocument doc(json);

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Could not open file for writing.");
        return;
    }

    file.write(doc.toJson());
    file.close();

    qDebug() << "Saved phrase template:" << selectedPhrase->getName() << "to" << filename;
}
