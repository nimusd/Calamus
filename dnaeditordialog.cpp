#include "dnaeditordialog.h"
#include "spectrumvisualizer.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>
#include <cmath>

DnaEditorDialog::DnaEditorDialog(int numHarmonics, QWidget *parent)
    : QDialog(parent)
    , numHarmonics(numHarmonics)
{
    setWindowTitle("DNA Editor");
    setMinimumSize(500, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title
    QLabel *title = new QLabel("Edit Harmonic Distribution Pattern");
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleFont.setBold(true);
    title->setFont(titleFont);
    mainLayout->addWidget(title);

    // Parameters form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    // Sequence dropdown
    comboSequence = new QComboBox();
    comboSequence->addItem("All Harmonics", 0);
    comboSequence->addItem("Odd Only (Clarinet)", 1);
    comboSequence->addItem("Even Only (Hollow)", 2);
    comboSequence->addItem("Odd Dominant", 3);
    comboSequence->addItem("Fundamental+ (Near-sine)", 4);
    comboSequence->addItem("Octaves Only (Organ)", 5);
    comboSequence->addItem("Fibonacci Sequence", 6);
    comboSequence->addItem("Prime Numbers", 7);
    comboSequence->addItem("Triangular Numbers", 8);
    comboSequence->addItem("Perfect Squares", 9);
    comboSequence->addItem("Bell Curve (Gaussian)", 10);
    comboSequence->addItem("Inverse (Bright)", 11);
    comboSequence->addItem("Custom", -1);  // Interactive editing
    formLayout->addRow("Sequence:", comboSequence);

    mainLayout->addLayout(formLayout);

    // Spectrum preview
    QLabel *previewLabel = new QLabel("Spectrum Editor:");
    previewLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(previewLabel);

    QLabel *helpText = new QLabel("Drag bar tops to adjust • Double-click to restore • Delete key to zero");
    helpText->setStyleSheet("color: gray; font-size: 9pt; margin-bottom: 5px;");
    mainLayout->addWidget(helpText);

    spectrumPreview = new SpectrumVisualizer();
    spectrumPreview->setEditable(true);  // Enable editing in custom DNA dialog

    // Connect visualizer changes to update our internal pattern
    connect(spectrumPreview, &SpectrumVisualizer::spectrumChanged,
            this, [this](const QVector<double> &spectrum) {
        customPattern = spectrum;
    });

    mainLayout->addWidget(spectrumPreview);

    // Save/Load buttons
    QHBoxLayout *fileButtonLayout = new QHBoxLayout();
    QPushButton *btnSave = new QPushButton("Save DNA...");
    QPushButton *btnLoad = new QPushButton("Load DNA...");
    btnSave->setToolTip("Save custom DNA pattern to .json file");
    btnLoad->setToolTip("Load custom DNA pattern from .json file");
    fileButtonLayout->addWidget(btnSave);
    fileButtonLayout->addWidget(btnLoad);
    fileButtonLayout->addStretch();
    mainLayout->addLayout(fileButtonLayout);

    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(comboSequence, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DnaEditorDialog::onSequenceChanged);
    connect(btnSave, &QPushButton::clicked, this, &DnaEditorDialog::onSaveCustomDna);
    connect(btnLoad, &QPushButton::clicked, this, &DnaEditorDialog::onLoadCustomDna);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Initialize with default pattern (only if not set by setCustomPattern later)
    customPattern.resize(numHarmonics);
    updatePattern();
}

void DnaEditorDialog::setCustomPattern(const QVector<double> &pattern)
{
    // Set the custom pattern directly
    customPattern = pattern;

    // Update the spectrum visualizer
    if (spectrumPreview) {
        spectrumPreview->setSpectrum(customPattern);
    }

    // Switch combo box to "Custom" mode to indicate this is a custom pattern
    int customIndex = comboSequence->findData(-1);
    if (customIndex >= 0) {
        comboSequence->blockSignals(true);
        comboSequence->setCurrentIndex(customIndex);
        comboSequence->blockSignals(false);
    }
}

DnaEditorDialog::~DnaEditorDialog()
{
}

void DnaEditorDialog::onSequenceChanged(int index)
{
    updatePattern();
}

void DnaEditorDialog::onSaveCustomDna()
{
    // Get current pattern (either from editor or generated from sequence/purity)
    QVector<double> patternToSave = customPattern;

    // If empty, use what's currently displayed in the visualizer
    if (patternToSave.isEmpty() && spectrumPreview) {
        patternToSave = spectrumPreview->getSpectrum();
    }

    if (patternToSave.isEmpty()) {
        QMessageBox::warning(this, "No Pattern", "No custom DNA pattern to save.");
        return;
    }

    // Open file dialog for save location
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Custom DNA Pattern",
        "",  // Default directory
        "Custom DNA Files (*.dna.json);;JSON Files (*.json);;All Files (*)"
    );

    if (fileName.isEmpty()) {
        return;  // User cancelled
    }

    // Ensure .dna.json extension
    if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
        fileName += ".dna.json";
    }

    // Create JSON object
    QJsonObject jsonObj;
    jsonObj["name"] = "Custom DNA Pattern";
    jsonObj["description"] = "Custom harmonic distribution created in Calamus DNA Editor";
    jsonObj["version"] = "1.0";
    jsonObj["numHarmonics"] = patternToSave.size();

    // Store amplitudes array
    QJsonArray amplitudesArray;
    for (double amp : patternToSave) {
        amplitudesArray.append(amp);
    }
    jsonObj["amplitudes"] = amplitudesArray;

    // Write to file
    QJsonDocument doc(jsonObj);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Save Error",
            "Could not open file for writing:\n" + fileName);
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, "Saved",
        "Custom DNA pattern saved successfully to:\n" + fileName);
}

void DnaEditorDialog::onLoadCustomDna()
{
    // Open file dialog for load location
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load Custom DNA Pattern",
        "",  // Default directory
        "Custom DNA Files (*.dna.json);;JSON Files (*.json);;All Files (*)"
    );

    if (fileName.isEmpty()) {
        return;  // User cancelled
    }

    // Read file
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Load Error",
            "Could not open file for reading:\n" + fileName);
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    // Parse JSON
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::critical(this, "Load Error",
            "Invalid JSON format in file:\n" + fileName);
        return;
    }

    QJsonObject jsonObj = doc.object();

    // Validate required fields
    if (!jsonObj.contains("amplitudes") || !jsonObj["amplitudes"].isArray()) {
        QMessageBox::critical(this, "Load Error",
            "Missing or invalid 'amplitudes' field in JSON file.");
        return;
    }

    // Extract amplitudes
    QJsonArray amplitudesArray = jsonObj["amplitudes"].toArray();
    QVector<double> loadedPattern;

    for (const QJsonValue &val : amplitudesArray) {
        if (val.isDouble()) {
            loadedPattern.append(val.toDouble());
        } else {
            QMessageBox::warning(this, "Load Warning",
                "Some amplitude values were invalid and skipped.");
        }
    }

    if (loadedPattern.isEmpty()) {
        QMessageBox::critical(this, "Load Error",
            "No valid amplitude data found in file.");
        return;
    }

    // Load the pattern
    setCustomPattern(loadedPattern);

    QString name = jsonObj.value("name").toString("Custom DNA Pattern");
    QMessageBox::information(this, "Loaded",
        "Successfully loaded: " + name + "\n" +
        QString::number(loadedPattern.size()) + " harmonics");
}

void DnaEditorDialog::updatePattern()
{
    int sequenceType = comboSequence->currentData().toInt();
    generateSequencePattern(sequenceType);

    // Update spectrum preview
    spectrumPreview->setSpectrum(customPattern);
}

void DnaEditorDialog::generateSequencePattern(int sequenceType)
{
    // Generate pattern according to sequence type
    customPattern.resize(numHarmonics);
    customPattern.fill(0.0);
    double totalAmp = 0.0;

    switch (sequenceType) {
    case 0: // All Harmonics
        for (int i = 0; i < numHarmonics; i++) {
            customPattern[i] = 1.0 / std::pow(i + 1, 1.5);  // Standard rolloff
            totalAmp += customPattern[i];
        }
        break;

    case 1: // Odd Only (Clarinet)
        for (int i = 0; i < numHarmonics; i++) {
            if ((i + 1) % 2 == 1) {  // Odd harmonics
                customPattern[i] = 1.0 / std::pow(i + 1, 1.4);
            } else {
                customPattern[i] = 0.0;
            }
            totalAmp += customPattern[i];
        }
        break;

    case 2: // Even Only (Hollow)
        for (int i = 0; i < numHarmonics; i++) {
            if ((i + 1) % 2 == 0) {  // Even harmonics
                customPattern[i] = 1.0 / std::pow(i + 1, 1.5);
            } else {
                customPattern[i] = 0.0;
            }
            totalAmp += customPattern[i];
        }
        break;

    case 3: // Odd Dominant
        for (int i = 0; i < numHarmonics; i++) {
            if ((i + 1) % 2 == 1) {  // Odd harmonics
                customPattern[i] = 1.0 / std::pow(i + 1, 1.3);
            } else {  // Even harmonics (much weaker)
                customPattern[i] = 0.2 / std::pow(i + 1, 2.0);
            }
            totalAmp += customPattern[i];
        }
        break;

    case 4: // Fundamental+ (Near-sine)
        customPattern[0] = 1.0;  // Fundamental
        totalAmp = 1.0;
        for (int i = 1; i < std::min(8, numHarmonics); i++) {
            customPattern[i] = 0.05 / (i + 1);  // Very weak upper harmonics
            totalAmp += customPattern[i];
        }
        break;

    case 5: // Octaves Only (Organ)
        for (int i = 0; i < numHarmonics; i++) {
            int harmonic = i + 1;
            // Check if harmonic is a power of 2 (1, 2, 4, 8, 16, 32, 64...)
            if ((harmonic & (harmonic - 1)) == 0) {
                customPattern[i] = 1.0 / harmonic;
            } else {
                customPattern[i] = 0.0;
            }
            totalAmp += customPattern[i];
        }
        break;

    case 6: // Fibonacci Sequence
        {
            // Generate Fibonacci sequence
            int fib1 = 1, fib2 = 1;
            for (int i = 0; i < numHarmonics; i++) {
                int harmonic = i + 1;
                // Check if current harmonic number is in Fibonacci sequence
                if (harmonic == fib1) {
                    customPattern[i] = 1.0 / std::pow(harmonic, 1.2);
                    // Advance to next Fibonacci number
                    int next = fib1 + fib2;
                    fib2 = fib1;
                    fib1 = next;
                } else {
                    customPattern[i] = 0.0;
                }
                totalAmp += customPattern[i];
            }
        }
        break;

    case 7: // Prime Numbers
        {
            auto isPrime = [](int n) {
                if (n <= 1) return false;
                if (n <= 3) return true;
                if (n % 2 == 0 || n % 3 == 0) return false;
                for (int i = 5; i * i <= n; i += 6) {
                    if (n % i == 0 || n % (i + 2) == 0) return false;
                }
                return true;
            };

            for (int i = 0; i < numHarmonics; i++) {
                int harmonic = i + 1;
                if (isPrime(harmonic)) {
                    customPattern[i] = 1.0 / std::pow(harmonic, 1.3);
                } else {
                    customPattern[i] = 0.0;
                }
                totalAmp += customPattern[i];
            }
        }
        break;

    case 8: // Triangular Numbers
        {
            // Triangular numbers: T(n) = n(n+1)/2 → 1, 3, 6, 10, 15, 21, 28, 36, 45, 55...
            int n = 1;
            int triangular = 1;
            for (int i = 0; i < numHarmonics; i++) {
                int harmonic = i + 1;
                if (harmonic == triangular) {
                    customPattern[i] = 1.0 / std::pow(harmonic, 1.2);
                    // Next triangular number
                    n++;
                    triangular = (n * (n + 1)) / 2;
                } else {
                    customPattern[i] = 0.0;
                }
                totalAmp += customPattern[i];
            }
        }
        break;

    case 9: // Perfect Squares
        {
            int n = 1;
            for (int i = 0; i < numHarmonics; i++) {
                int harmonic = i + 1;
                if (harmonic == n * n) {
                    customPattern[i] = 1.0 / std::pow(harmonic, 1.1);
                    n++;
                } else {
                    customPattern[i] = 0.0;
                }
                totalAmp += customPattern[i];
            }
        }
        break;

    case 10: // Bell Curve (Gaussian)
        {
            // Peak at middle harmonics, fade both directions
            double center = numHarmonics / 3.0;  // Peak around 1/3 point
            double width = numHarmonics / 6.0;   // Standard deviation
            for (int i = 0; i < numHarmonics; i++) {
                double distance = (i - center) / width;
                // Gaussian curve: e^(-(x²/2))
                double gaussian = std::exp(-(distance * distance) / 2.0);
                customPattern[i] = gaussian / std::pow(i + 1, 0.8);  // Gentle rolloff
                totalAmp += customPattern[i];
            }
        }
        break;

    case 11: // Inverse (Bright/Subharmonic)
        {
            // Amplitude increases with harmonic number (opposite of normal)
            for (int i = 0; i < numHarmonics; i++) {
                int harmonic = i + 1;
                // Inverse rolloff: higher harmonics are louder
                customPattern[i] = std::pow(harmonic, 0.5) / numHarmonics;
                totalAmp += customPattern[i];
            }
        }
        break;

    case -1: // Custom (for interactive editing)
        // Keep current pattern
        return;

    default:
        // Default to all harmonics
        for (int i = 0; i < numHarmonics; i++) {
            customPattern[i] = 1.0 / std::pow(i + 1, 1.5);
            totalAmp += customPattern[i];
        }
        break;
    }

    // Normalize
    if (totalAmp > 0.0) {
        for (int i = 0; i < numHarmonics; i++) {
            customPattern[i] /= totalAmp;
        }
    }
}
