#ifndef CALAMUSMAIN_H
#define CALAMUSMAIN_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "sounitbuilder.h"
#include "scorecanvaswindow.h"
#include "audioengine.h"

// Forward declarations for Qt widgets
class QComboBox;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QFormLayout;
class SpectrumVisualizer;
class EnvelopeVisualizer;
class PhraseGroup;

QT_BEGIN_NAMESPACE
namespace Ui {
class CalamusMain;
}
QT_END_NAMESPACE

class CalamusMain : public QMainWindow
{
    Q_OBJECT

public:
    CalamusMain(QWidget *parent = nullptr);
    ~CalamusMain();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::CalamusMain *ui;

private:
    SounitBuilder *sounitBuilder;
    ScoreCanvasWindow *scoreCanvasWindow;
    AudioEngine *audioEngine;  // Shared audio engine for both windows

    // Inspector state
    Container *currentContainer = nullptr;

    // Sounit file management
    QString currentSounitFilePath;

    // Dynamic inspector controls (created based on container type)
    QComboBox *comboDnaSelect = nullptr;
    QSlider *sliderNumHarmonics = nullptr;
    QSpinBox *spinNumHarmonics = nullptr;
    SpectrumVisualizer *spectrumViz = nullptr;

    // Envelope Engine inspector controls
    QComboBox *comboEnvelopeSelect = nullptr;
    QWidget *envelopeParamsWidget = nullptr;  // Container for contextual parameters
    EnvelopeVisualizer *envelopeViz = nullptr;

    // Phrase inspector widgets (Phase 4)
    QLineEdit *editPhraseName = nullptr;
    QComboBox *comboPhraseDynamicsEasing = nullptr;
    QComboBox *comboPhraseTemplates = nullptr;
    QPushButton *btnLoadPhraseTemplate = nullptr;
    QPushButton *btnApplyPhraseTemplate = nullptr;
    QPushButton *btnCreatePhrase = nullptr;
    QPushButton *btnUngroupPhrase = nullptr;
    QPushButton *btnSavePhraseTemplate = nullptr;

    // Phrase library
    QVector<PhraseGroup> savedPhraseTemplates;

    // Helper methods
    void clearConfigInspector();
    void populateHarmonicGeneratorInspector();
    void populateRolloffProcessorInspector();
    void populateSpectrumToSignalInspector();
    void populateFormantBodyInspector();
    void populateBreathTurbulenceInspector();
    void populateNoiseColorFilterInspector();
    void populatePhysicsSystemInspector();
    void populateEnvelopeEngineInspector();
    void populateDriftEngineInspector();
    void populateGateProcessorInspector();
    void populateEasingApplicatorInspector();
    void updateSpectrumVisualization();
    void updateEnvelopeParameters(int envelopeType);  // Update contextual envelope params
    void updateEnvelopePreview();  // Update envelope preview visualization
    QString getContainerDescription(const QString &containerType);

    // Helper to add a simple parameter slider
    void addParameterSlider(QFormLayout *layout, const QString &label,
                           const QString &paramName, double minVal, double maxVal,
                           double defaultVal, double step = 0.01, int decimals = 2);

public slots:
    void stopAllPlayback();  // Stop playback in both windows

private slots:
    void onTabChanged(int index);

    // Sounit file menu slots
    void onNewSounit();
    void onLoadSounit();
    void onSaveSounitAs();
    void onSounitNameChanged(const QString &name);
    void onSounitCommentChanged(const QString &comment);
    void onSounitNameEdited();
    void onSounitCommentEdited();

    // Inspector update slots
    void onContainerSelected(Container *container);
    void onConnectionSelected(int connectionIndex);
    void onSelectionCleared();

    // Inspector edit slots
    void onInstanceNameChanged();
    void onConnectionFunctionChanged(int index);
    void onConnectionWeightChanged(double value);
    void onDisconnectClicked();
    void onConfigParameterChanged(QListWidgetItem *item);

    // Harmonic Generator inspector slots
    void onDnaSelectChanged(int index);
    void onNumHarmonicsChanged(int value);

    // Phrase inspector slots (Phase 4)
    void onPhraseSelectionChanged();
    void onPhraseNameChanged(const QString &name);
    void onPhraseEasingChanged(int index);
    void onLoadPhraseTemplateClicked();
    void onApplyPhraseTemplateClicked();
    void onCreatePhraseClicked();
    void onUngroupPhraseClicked();
    void onSavePhraseTemplateClicked();
};

#endif // CALAMUSMAIN_H
