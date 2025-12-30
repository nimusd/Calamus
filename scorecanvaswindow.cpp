#include "scorecanvaswindow.h"
#include "ui_scorecanvas.h"
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QCursor>
#include <QTimer>
#include <cmath>

ScoreCanvasWindow::ScoreCanvasWindow(AudioEngine *sharedAudioEngine, QWidget *parent)
    : QMainWindow{parent}
    , ui(new Ui::scorecanvas)
    , zoomModeActive(false)
    , currentMinHz(27.5)
    , currentMaxHz(4186.0)
    , currentPixelsPerSecond(100.0)
    , isDraggingZoom(false)
    , dragStartMinHz(27.5)
    , dragStartMaxHz(4186.0)
    , dragStartPixelsPerSecond(100.0)
    , zoomCenterHz(0.0)
    , zoomCenterTime(0.0)
    , panModeActive(false)
    , isDraggingPan(false)
    , panStartHorizontalScroll(0)
    , panStartVerticalScroll(0)
    , currentTimeMode(AbsoluteTime)
    , currentTempo(120)
    , currentTimeSigTop(4)
    , currentTimeSigBottom(4)
    , audioEngine(sharedAudioEngine)
    , playbackTimer(nullptr)
    , currentNoteIndex(0)
    , playbackStartTime(0.0)
    , playbackStartPosition(0.0)
    , isPlaying(false)
{
    qDebug() << "ScoreCanvasWindow: Starting constructor";
    ui->setupUi(this);

    // Install event filter on application to catch all key events
    qApp->installEventFilter(this);

    // Create status bar labels (according to spec: Cursor time | Cursor pitch | Tempo | Time signature)
    cursorTimeLabel = new QLabel("0:00:000", this);
    cursorTimeLabel->setMinimumWidth(100);
    cursorTimeLabel->setStyleSheet("QLabel { padding: 0 5px; }");

    cursorPitchLabel = new QLabel("-- Hz; --", this);
    cursorPitchLabel->setMinimumWidth(150);
    cursorPitchLabel->setStyleSheet("QLabel { padding: 0 5px; }");

    statusTempoLabel = new QLabel("120 BPM", this);
    statusTempoLabel->setMinimumWidth(80);
    statusTempoLabel->setStyleSheet("QLabel { padding: 0 5px; }");

    statusTimeSigLabel = new QLabel("4/4", this);
    statusTimeSigLabel->setMinimumWidth(50);
    statusTimeSigLabel->setStyleSheet("QLabel { padding: 0 5px; }");

    // Create pressure status label (for pen tablet feedback)
    pressureLabel = new QLabel("Pressure: --", this);
    pressureLabel->setMinimumWidth(120);
    pressureLabel->setStyleSheet("QLabel { padding: 0 5px; }");

    // Create navigation mode label
    navigationModeLabel = new QLabel("", this);
    navigationModeLabel->setMinimumWidth(80);
    navigationModeLabel->setStyleSheet("QLabel { padding: 0 5px; font-weight: bold; }");

    // Add to bottom toolbar (status bar)
    ui->scoreCanvasBottomToolbar->addWidget(cursorTimeLabel);
    ui->scoreCanvasBottomToolbar->addSeparator();
    ui->scoreCanvasBottomToolbar->addWidget(cursorPitchLabel);
    ui->scoreCanvasBottomToolbar->addSeparator();
    ui->scoreCanvasBottomToolbar->addWidget(statusTempoLabel);
    ui->scoreCanvasBottomToolbar->addSeparator();
    ui->scoreCanvasBottomToolbar->addWidget(statusTimeSigLabel);
    ui->scoreCanvasBottomToolbar->addSeparator();
    ui->scoreCanvasBottomToolbar->addWidget(pressureLabel);
    ui->scoreCanvasBottomToolbar->addSeparator();
    ui->scoreCanvasBottomToolbar->addWidget(navigationModeLabel);

    qDebug() << "ScoreCanvasWindow: Setting up UI components";
    setupToolbarColors();
    setupDrawingTools();
    setupCompositionSettings();
    setupTrackSelector();
    setupScoreCanvas();
    setupZoom();

    // Initialize playback timer
    playbackTimer = new QTimer(this);
    connect(playbackTimer, &QTimer::timeout, this, &ScoreCanvasWindow::onPlaybackTick);

    // Connect transport controls
    connect(ui->actionPlay, &QAction::triggered, this, &ScoreCanvasWindow::startPlayback);
    connect(ui->actionstop, &QAction::triggered, this, &ScoreCanvasWindow::stopPlayback);

    qDebug() << "ScoreCanvasWindow: Constructor complete";
}


void ScoreCanvasWindow::setupToolbarColors()
{
    // Get the tool buttons from the top toolbar
    QToolButton* drawDiscreteBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionDrawDiscreteNotes));

    QToolButton* drawContinuousBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionDrawContinuousNotes));

    QToolButton* drawPhraseBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionDrawPhrase));

    QToolButton* recordDiscreteBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionRecordDiscreteNotes));

    QToolButton* recordContinuousBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionRecordContinuousNotes));

    QToolButton* recordPhraseBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionRecordPhrase));

    QToolButton* selectBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionScoreCanvasSelect));

    QToolButton* zoomBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionScoreCanvasZoom));

    // Apply colors with checked state highlighting
    if (drawDiscreteBtn) {
        drawDiscreteBtn->setStyleSheet(
            "QToolButton { background-color: #90CAF9; }"  // Light blue
            "QToolButton:checked { background-color: #E57373; }"  // Medium red when active
        );
    }
    if (drawContinuousBtn) {
        drawContinuousBtn->setStyleSheet(
            "QToolButton { background-color: #BBDEFB; }"
            "QToolButton:checked { background-color: #E57373; }"  // Medium red when active
        );
    }
    // drawPhraseBtn removed - phrases are always selectable, no special mode needed

    if (recordDiscreteBtn) {
        recordDiscreteBtn->setStyleSheet("QToolButton { background-color: #FFCC80; }");  // Light orange
    }
    if (recordContinuousBtn) {
        recordContinuousBtn->setStyleSheet("QToolButton { background-color: #FFE0B2; }");
    }
    if (recordPhraseBtn) {
        recordPhraseBtn->setStyleSheet("QToolButton { background-color: #FFCC80; }");
    }

    if (selectBtn) {
        selectBtn->setStyleSheet(
            "QToolButton { background-color: #A5D6A7; }"  // Light green
            "QToolButton:checked { background-color: #E57373; }"  // Medium red when active
        );
    }
    if (zoomBtn) {
        zoomBtn->setStyleSheet(
            "QToolButton { background-color: #C8E6C9; }"  // Light green
            "QToolButton:checked { background-color: #E57373; }"  // Medium red when active
        );
    }
}

void ScoreCanvasWindow::setupDrawingTools()
{
    // Create action group for mutually exclusive drawing tools
    drawingToolGroup = new QActionGroup(this);
    drawingToolGroup->setExclusive(true);

    // Make actions checkable and add to group
    ui->actionDrawDiscreteNotes->setCheckable(true);
    ui->actionDrawContinuousNotes->setCheckable(true);
    ui->actionScoreCanvasSelect->setCheckable(true);

    drawingToolGroup->addAction(ui->actionDrawDiscreteNotes);
    drawingToolGroup->addAction(ui->actionDrawContinuousNotes);
    drawingToolGroup->addAction(ui->actionScoreCanvasSelect);

    // Set discrete drawing as default (will be connected to ScoreCanvas after it's created)
    ui->actionDrawDiscreteNotes->setChecked(true);

    // Add keyboard shortcuts (application-level, work regardless of focus)
    ui->actionScoreCanvasSelect->setShortcut(QKeySequence(Qt::Key_S));
    ui->actionScoreCanvasSelect->setShortcutContext(Qt::ApplicationShortcut);

    ui->actionDrawDiscreteNotes->setShortcut(QKeySequence(Qt::Key_D));
    ui->actionDrawDiscreteNotes->setShortcutContext(Qt::ApplicationShortcut);

    ui->actionDrawContinuousNotes->setShortcut(QKeySequence(Qt::Key_C));
    ui->actionDrawContinuousNotes->setShortcutContext(Qt::ApplicationShortcut);

    // Note: actionDrawPhrase removed - phrases are always selectable by clicking their hull
    // No special mode needed

    qDebug() << "ScoreCanvasWindow: Drawing tool group configured with shortcuts";
}

void ScoreCanvasWindow::setupCompositionSettings()
{
    // Add separator before composition settings
    ui->toolBar->addSeparator();

    // Time Mode Toggle Button (Absolute ↔ Musical)
    timeModeToggle = new QPushButton("Absolute", this);
    timeModeToggle->setCheckable(true);
    timeModeToggle->setMinimumWidth(80);
    timeModeToggle->setFocusPolicy(Qt::NoFocus);  // Don't steal keyboard focus
    timeModeToggle->setStyleSheet(
        "QPushButton {"
        "    background-color: #B0BEC5;"  // Blue-grey
        "    padding: 5px 10px;"
        "    border: 1px solid #78909C;"
        "    border-radius: 3px;"
        "}"
        "QPushButton:checked {"
        "    background-color: #66BB6A;"  // Green when Musical mode
        "}"
    );
    ui->toolBar->addWidget(timeModeToggle);
    connect(timeModeToggle, &QPushButton::clicked, this, &ScoreCanvasWindow::onTimeModeToggled);

    ui->toolBar->addSeparator();

    // Tempo Label and SpinBox
    QLabel *tempoLabel = new QLabel(" Tempo: ", this);
    ui->toolBar->addWidget(tempoLabel);

    tempoSpinBox = new QSpinBox(this);
    tempoSpinBox->setRange(20, 300);  // Reasonable BPM range
    tempoSpinBox->setValue(currentTempo);
    tempoSpinBox->setSuffix(" BPM");
    tempoSpinBox->setMinimumWidth(90);
    tempoSpinBox->setFocusPolicy(Qt::StrongFocus);  // Only gets focus when clicked
    ui->toolBar->addWidget(tempoSpinBox);
    connect(tempoSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ScoreCanvasWindow::onTempoChanged);

    ui->toolBar->addSeparator();

    // Time Signature Label and SpinBoxes
    QLabel *timeSigLabel = new QLabel(" Time Sig: ", this);
    ui->toolBar->addWidget(timeSigLabel);

    timeSigNumerator = new QSpinBox(this);
    timeSigNumerator->setRange(1, 99);  // Up to 99 for complex rhythms
    timeSigNumerator->setValue(currentTimeSigTop);
    timeSigNumerator->setMinimumWidth(60);
    timeSigNumerator->setMaximumWidth(60);
    timeSigNumerator->setFocusPolicy(Qt::StrongFocus);  // Only gets focus when clicked
    ui->toolBar->addWidget(timeSigNumerator);

    QLabel *slashLabel = new QLabel(" / ", this);
    ui->toolBar->addWidget(slashLabel);

    timeSigDenominator = new QSpinBox(this);
    // 0 = simple mode (beat = tempo), 1-99 = scaled mode
    timeSigDenominator->setRange(0, 99);
    timeSigDenominator->setValue(currentTimeSigBottom);
    timeSigDenominator->setMinimumWidth(60);
    timeSigDenominator->setMaximumWidth(60);
    timeSigDenominator->setFocusPolicy(Qt::StrongFocus);  // Only gets focus when clicked
    ui->toolBar->addWidget(timeSigDenominator);

    connect(timeSigNumerator, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ScoreCanvasWindow::onTimeSignatureChanged);
    connect(timeSigDenominator, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ScoreCanvasWindow::onTimeSignatureChanged);

    qDebug() << "ScoreCanvasWindow: Composition settings configured";
}

void ScoreCanvasWindow::setupTrackSelector()
{
    // Note: old UI widgets will be deleted in setupScoreCanvas()

    // Create TrackSelector
    trackSelector = new TrackSelector();
    trackSelector->setMinimumWidth(180);
    trackSelector->setMaximumWidth(250);
    trackSelector->setMinimumHeight(2000);  // Match ScoreCanvas height for scrolling

    // Set frequency range (typical musical range: A0 to C8)
    trackSelector->setFrequencyRange(currentMinHz, currentMaxHz);

    // Add some test tracks to visualize
    // Bass track (E1 to E3)
    trackSelector->addTrack("Bass", QColor("#E74C3C"), 41.2, 164.8);

    // Tenor track (C3 to C5)
    trackSelector->addTrack("Tenor", QColor("#3498DB"), 130.8, 523.3);

    // Alto track (F3 to F5)
    trackSelector->addTrack("Alto", QColor("#9B59B6"), 174.6, 698.5);

    // Soprano track (C4 to C6)
    trackSelector->addTrack("Soprano", QColor("#F39C12"), 261.6, 1046.5);
}

void ScoreCanvasWindow::setupScoreCanvas()
{
    // Create ScoreCanvas widget
    scoreCanvas = new ScoreCanvas();

    // Set minimum size for ScoreCanvas to enable scrolling
    // Width: large enough for horizontal time scrolling (e.g., 10000 pixels for long compositions)
    // Height: will be calculated based on visible octave range
    scoreCanvas->setMinimumSize(10000, 2000);

    // Set the same frequency range as TrackSelector for synchronization
    scoreCanvas->setFrequencyRange(currentMinHz, currentMaxHz);

    // Set initial horizontal zoom (pixels per second)
    scoreCanvas->setPixelsPerSecond(currentPixelsPerSecond);

    // Create scroll area for ScoreCanvas
    scoreScrollArea = new QScrollArea(ui->centralwidget);
    scoreScrollArea->setWidget(scoreCanvas);
    scoreScrollArea->setWidgetResizable(false);  // Don't auto-resize widget
    scoreScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scoreScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Apply light gold styling to scroll bars
    scoreScrollArea->setStyleSheet(
        "QScrollBar:vertical {"
        "    background: rgb(255, 235, 205);"  // Light gold
        "    width: 15px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: rgb(200, 180, 150);"  // Darker gold for handle
        "    min-height: 20px;"
        "    border-radius: 3px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    background: rgb(255, 235, 205);"
        "    height: 0px;"
        "}"
        "QScrollBar:horizontal {"
        "    background: rgb(255, 235, 205);"  // Light gold
        "    height: 15px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "    background: rgb(200, 180, 150);"  // Darker gold for handle
        "    min-width: 20px;"
        "    border-radius: 3px;"
        "}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "    background: rgb(255, 235, 205);"
        "    width: 0px;"
        "}"
    );

    // Create scroll area for TrackSelector
    trackScrollArea = new QScrollArea(ui->centralwidget);
    trackScrollArea->setWidget(trackSelector);
    trackScrollArea->setWidgetResizable(false);
    trackScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    trackScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // Hidden, synced with score
    trackScrollArea->setMinimumWidth(180);
    trackScrollArea->setMaximumWidth(250);

    // Setup layouts for proper resizing
    // DELETE the old fixed-geometry widgets from the .ui file
    // (They conflict with the layout system)
    if (ui->horizontalLayoutWidget) {
        delete ui->horizontalLayoutWidget;
        ui->horizontalLayoutWidget = nullptr;
    }
    if (ui->scrollAreatracks) {
        delete ui->scrollAreatracks;
        ui->scrollAreatracks = nullptr;
    }

    // Create main vertical layout for the central widget
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create FrequencyLabels widget
    frequencyLabels = new FrequencyLabels(ui->centralwidget);
    frequencyLabels->setFrequencyRange(currentMinHz, currentMaxHz);
    frequencyLabels->setMinimumHeight(2000);

    // Create scroll area for FrequencyLabels
    frequencyScrollArea = new QScrollArea(ui->centralwidget);
    frequencyScrollArea->setWidget(frequencyLabels);
    frequencyScrollArea->setWidgetResizable(false);
    frequencyScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frequencyScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frequencyScrollArea->setMinimumWidth(60);
    frequencyScrollArea->setMaximumWidth(60);

    // Create Timeline widget
    timeline = new Timeline(ui->centralwidget);
    timeline->setPixelsPerSecond(100.0);  // Initial zoom level

    // Connect timeline signals
    connect(timeline, &Timeline::nowMarkerChanged, this, [this](double timeMs) {
        qDebug() << "ScoreCanvasWindow: Playback start position set to" << timeMs << "ms";
        // Double-clicking timeline sets the START POSITION (where playback will always return to)
        playbackStartPosition = timeMs;
        // Also update paste target time in ScoreCanvas
        scoreCanvas->setPasteTargetTime(timeMs);
    });

    // Initialize paste target time from timeline's current now marker
    scoreCanvas->setPasteTargetTime(timeline->getNowMarker());

    // Create horizontal layout for timeline with left spacing
    QHBoxLayout *timelineLayout = new QHBoxLayout();
    timelineLayout->setContentsMargins(0, 0, 0, 0);
    timelineLayout->setSpacing(0);

    // Add spacers to align timeline with score canvas
    // Spacer for track selector width
    QWidget *trackSpacer = new QWidget();
    trackSpacer->setMinimumWidth(180);
    trackSpacer->setMaximumWidth(250);
    trackSpacer->setStyleSheet("background-color: #f0f0f0;");

    // Spacer for frequency labels width
    QWidget *freqSpacer = new QWidget();
    freqSpacer->setMinimumWidth(60);
    freqSpacer->setMaximumWidth(60);
    freqSpacer->setStyleSheet("background-color: #f0f0f0;");

    timelineLayout->addWidget(trackSpacer);
    timelineLayout->addWidget(freqSpacer);
    timelineLayout->addWidget(timeline, 1);  // Timeline takes remaining space

    // Create horizontal layout for track selector + frequency labels + score canvas scroll areas
    QHBoxLayout *scoreLayout = new QHBoxLayout();
    scoreLayout->setContentsMargins(0, 0, 0, 0);
    scoreLayout->setSpacing(0);
    scoreLayout->addWidget(trackScrollArea);
    scoreLayout->addWidget(frequencyScrollArea);
    scoreLayout->addWidget(scoreScrollArea, 1);  // Stretch factor 1 = expands

    // Add layouts to main layout
    mainLayout->addLayout(timelineLayout);
    mainLayout->addLayout(scoreLayout, 1);  // Stretch factor 1 = expands vertically

    // Synchronize vertical scrolling between TrackSelector, FrequencyLabels, and ScoreCanvas
    connect(scoreScrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            [this](int value) {
                // Update scroll positions
                trackScrollArea->verticalScrollBar()->setValue(value);
                frequencyScrollArea->verticalScrollBar()->setValue(value);
                // Update vertical offset for all three widgets
                trackSelector->setVerticalOffset(value);
                frequencyLabels->setVerticalOffset(value);
                scoreCanvas->setVerticalOffset(value);
            });

    // Synchronize canvas height for pixel calculations
    // Both widgets must use the same height for frequencyToPixel() to align correctly
    // Use the ScoreCanvas actual height (set to minimum 2000 in setupScoreCanvas)
    QTimer::singleShot(0, [this]() {
        // Delay to ensure widgets are fully initialized
        frequencyLabels->setCanvasHeight(scoreCanvas->height());
    });

    // Synchronize horizontal scrolling between Timeline and ScoreCanvas
    connect(scoreScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged,
            [this](int value) {
                // Update timeline horizontal offset
                timeline->setHorizontalOffset(value);
            });

    // Install event filter on scoreCanvas for zoom
    scoreCanvas->installEventFilter(this);

    // Connect pressure signal to status display
    connect(scoreCanvas, &ScoreCanvas::pressureChanged, this, &ScoreCanvasWindow::onPressureChanged);

    // Connect cursor position signal to status bar
    connect(scoreCanvas, &ScoreCanvas::cursorPositionChanged, this, &ScoreCanvasWindow::onCursorPositionChanged);

    // Connect drawing tool actions to ScoreCanvas input mode
    connect(ui->actionDrawDiscreteNotes, &QAction::triggered, this, [this]() {
        scoreCanvas->setInputMode(ScoreCanvas::DrawModeDiscrete);
        qDebug() << "ScoreCanvasWindow: Switched to Discrete Drawing mode";
    });

    connect(ui->actionDrawContinuousNotes, &QAction::triggered, this, [this]() {
        scoreCanvas->setInputMode(ScoreCanvas::DrawModeContinuous);
        qDebug() << "ScoreCanvasWindow: Switched to Continuous Drawing mode";
    });

    connect(ui->actionScoreCanvasSelect, &QAction::triggered, this, [this]() {
        scoreCanvas->setInputMode(ScoreCanvas::SelectionMode);
        qDebug() << "ScoreCanvasWindow: Switched to Selection mode";
    });
}

void ScoreCanvasWindow::setupZoom()
{
    // Connect zoom toolbar action
    connect(ui->actionScoreCanvasZoom, &QAction::toggled, this, &ScoreCanvasWindow::onZoomToggled);

    // Make the zoom action checkable
    ui->actionScoreCanvasZoom->setCheckable(true);

    // Set keyboard shortcut to 'z' key (application-level)
    ui->actionScoreCanvasZoom->setShortcut(QKeySequence(Qt::Key_Z));
    ui->actionScoreCanvasZoom->setShortcutContext(Qt::ApplicationShortcut);
}

void ScoreCanvasWindow::setZoomMode(bool active)
{
    zoomModeActive = active;

    if (active) {
        // Disable pan mode if it's active
        if (panModeActive) {
            setPanMode(false);
        }

        // Set magnifier cursor
        scoreCanvas->setCursor(Qt::CrossCursor);  // Qt doesn't have magnifier, use cross
        trackSelector->setCursor(Qt::CrossCursor);

        // Update status bar
        navigationModeLabel->setText("ZOOM (z)");
        navigationModeLabel->setStyleSheet("QLabel { padding: 0 5px; font-weight: bold; color: #E57373; }");  // Red
    } else {
        // Restore normal cursor
        scoreCanvas->setCursor(Qt::ArrowCursor);
        trackSelector->setCursor(Qt::ArrowCursor);

        // Clear status bar
        navigationModeLabel->setText("");
        navigationModeLabel->setStyleSheet("QLabel { padding: 0 5px; font-weight: bold; }");
    }

    // Update toolbar button state
    ui->actionScoreCanvasZoom->setChecked(active);
}

void ScoreCanvasWindow::setPanMode(bool active)
{
    panModeActive = active;

    if (active) {
        // Disable zoom mode if it's active
        if (zoomModeActive) {
            setZoomMode(false);
        }

        // Set hand cursor for panning
        scoreCanvas->setCursor(Qt::OpenHandCursor);
        trackSelector->setCursor(Qt::OpenHandCursor);

        // Update status bar
        navigationModeLabel->setText("PAN (Ctrl+Space)");
        navigationModeLabel->setStyleSheet("QLabel { padding: 0 5px; font-weight: bold; color: #66BB6A; }");  // Green
    } else {
        // Restore normal cursor
        scoreCanvas->setCursor(Qt::ArrowCursor);
        trackSelector->setCursor(Qt::ArrowCursor);

        // Clear status bar
        navigationModeLabel->setText("");
        navigationModeLabel->setStyleSheet("QLabel { padding: 0 5px; font-weight: bold; }");
    }
}

void ScoreCanvasWindow::applyZoom(double minHz, double maxHz)
{
    // Constrain to reasonable limits
    if (minHz < 20.0) minHz = 20.0;
    if (maxHz > 8000.0) maxHz = 8000.0;
    if (maxHz <= minHz) return;  // Invalid range

    currentMinHz = minHz;
    currentMaxHz = maxHz;

    // Apply to all three widgets synchronously
    scoreCanvas->setFrequencyRange(minHz, maxHz);
    trackSelector->setFrequencyRange(minHz, maxHz);
    frequencyLabels->setFrequencyRange(minHz, maxHz);
}

void ScoreCanvasWindow::applyHorizontalZoom(double pixelsPerSecond)
{
    // Constrain to reasonable limits (10 to 1000 pixels per second)
    if (pixelsPerSecond < 10.0) pixelsPerSecond = 10.0;
    if (pixelsPerSecond > 1000.0) pixelsPerSecond = 1000.0;

    currentPixelsPerSecond = pixelsPerSecond;

    // Apply to both ScoreCanvas and Timeline
    scoreCanvas->setPixelsPerSecond(pixelsPerSecond);
    timeline->setPixelsPerSecond(pixelsPerSecond);
}

void ScoreCanvasWindow::onZoomToggled(bool checked)
{
    setZoomMode(checked);
}

void ScoreCanvasWindow::onZoomIn()
{
    // Zoom in: reduce frequency range by 20%
    double center = std::sqrt(currentMinHz * currentMaxHz);  // Geometric mean
    double range = std::log2(currentMaxHz / currentMinHz);
    double newRange = range * 0.8;  // 20% reduction

    double newMinHz = center / std::pow(2.0, newRange / 2.0);
    double newMaxHz = center * std::pow(2.0, newRange / 2.0);

    applyZoom(newMinHz, newMaxHz);
}

void ScoreCanvasWindow::onZoomOut()
{
    // Zoom out: increase frequency range by 25%
    double center = std::sqrt(currentMinHz * currentMaxHz);  // Geometric mean
    double range = std::log2(currentMaxHz / currentMinHz);
    double newRange = range * 1.25;  // 25% increase

    double newMinHz = center / std::pow(2.0, newRange / 2.0);
    double newMaxHz = center * std::pow(2.0, newRange / 2.0);

    applyZoom(newMinHz, newMaxHz);
}

void ScoreCanvasWindow::onPressureChanged(double pressure, bool active)
{
    if (active) {
        // Display pressure as percentage (0-100%)
        int pressurePercent = static_cast<int>(pressure * 100);
        pressureLabel->setText(QString("Pressure: %1%").arg(pressurePercent));
    } else {
        // Inactive - show placeholder
        pressureLabel->setText("Pressure: --");
    }
}

void ScoreCanvasWindow::onCursorPositionChanged(double timeMs, double pitchHz)
{
    // Format time based on current time mode
    QString timeStr;
    if (currentTimeMode == AbsoluteTime) {
        // Absolute: Min:Sec:Ms
        int totalMs = static_cast<int>(timeMs);
        int minutes = totalMs / 60000;
        int seconds = (totalMs % 60000) / 1000;
        int milliseconds = totalMs % 1000;
        timeStr = QString("%1:%2:%3")
                      .arg(minutes)
                      .arg(seconds, 2, 10, QChar('0'))
                      .arg(milliseconds, 3, 10, QChar('0'));
    } else {
        // Musical: Bars:Beats:Ms
        // Calculate bar duration based on bottom number
        double barDurationMs;
        double beatDurationMs;

        if (currentTimeSigBottom == 0) {
            // Option A: Bottom is just a label
            beatDurationMs = 60000.0 / currentTempo;
            barDurationMs = beatDurationMs * currentTimeSigTop;
        } else {
            // Option B: Bottom scales beat duration
            barDurationMs = (60000.0 / currentTempo) * (static_cast<double>(currentTimeSigTop) / currentTimeSigBottom);
            beatDurationMs = barDurationMs / currentTimeSigTop;
        }

        int bar = static_cast<int>(timeMs / barDurationMs) + 1;  // 1-based
        double timeInBar = std::fmod(timeMs, barDurationMs);
        int beat = static_cast<int>(timeInBar / beatDurationMs) + 1;  // 1-based
        int ms = static_cast<int>(std::fmod(timeInBar, beatDurationMs));

        timeStr = QString("%1:%2:%3")
                      .arg(bar)
                      .arg(beat)
                      .arg(ms, 3, 10, QChar('0'));
    }
    cursorTimeLabel->setText(timeStr);

    // Format pitch: Hz; Scale degree (no western note names)
    if (pitchHz > 0) {
        // Find closest scale line to determine scale degree
        // Access the scale lines from ScoreCanvas
        QString scaleDegreeStr = "--";
        double minDistance = 1000000.0;  // Large initial value

        // Get scale lines from ScoreCanvas to find closest degree
        const auto& scaleLines = scoreCanvas->getPhrase();  // We need access to scale lines
        // For now, use a simple calculation based on just intonation C major
        // TODO: This should query ScoreCanvas for actual scale lines

        // Just intonation major scale ratios starting from BASE_FREQUENCY (25 Hz)
        const double BASE_FREQ = 25.0;
        const double majorScaleRatios[7] = {1.0, 9.0/8.0, 5.0/4.0, 4.0/3.0, 3.0/2.0, 5.0/3.0, 15.0/8.0};

        // Find which scale degree is closest
        int closestDegree = 0;
        for (int octave = 0; octave <= 9; ++octave) {
            double octaveBase = BASE_FREQ * std::pow(2.0, octave);
            for (int degree = 0; degree < 7; ++degree) {
                double scaleFreq = octaveBase * majorScaleRatios[degree];
                double distance = std::abs(pitchHz - scaleFreq);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestDegree = degree + 1;  // 1-based
                }
            }
        }

        // Only show scale degree if within reasonable tolerance (within ~50 cents)
        if (minDistance < pitchHz * 0.03) {  // ~3% tolerance
            scaleDegreeStr = QString::number(closestDegree);
        }

        QString pitchStr = QString("%1 Hz; %2")
                               .arg(pitchHz, 0, 'f', 1)
                               .arg(scaleDegreeStr);
        cursorPitchLabel->setText(pitchStr);
    } else {
        cursorPitchLabel->setText("-- Hz; --");
    }

    // Update tempo and time signature in status bar
    statusTempoLabel->setText(QString("%1 BPM").arg(currentTempo));
    statusTimeSigLabel->setText(QString("%1/%2").arg(currentTimeSigTop).arg(currentTimeSigBottom));
}

void ScoreCanvasWindow::keyPressEvent(QKeyEvent *event)
{
    // Ctrl+Space toggles pan mode
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::ControlModifier) {
        setPanMode(!panModeActive);
        event->accept();
        return;
    }

    // Spacebar alone toggles playback (start/stop)
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::NoModifier) {
        if (isPlaying) {
            stopPlayback();
        } else {
            startPlayback();
        }
        event->accept();
        return;
    }

    // L key enters loop mode
    if (event->key() == Qt::Key_L && event->modifiers() == Qt::NoModifier) {
        timeline->setLoopModeActive(true);
        event->accept();
        return;
    }

    // Delete key clears loop (when loop end is selected)
    if (event->key() == Qt::Key_Delete && event->modifiers() == Qt::NoModifier) {
        if (timeline->hasLoop()) {
            timeline->clearLoop();
            event->accept();
            return;
        }
    }

    // Let the base class handle other keys (including action shortcuts)
    QMainWindow::keyPressEvent(event);
}

void ScoreCanvasWindow::keyReleaseEvent(QKeyEvent *event)
{
    QMainWindow::keyReleaseEvent(event);
}

bool ScoreCanvasWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Handle key presses globally for shortcuts
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // Only handle if this window is active
        if (isActiveWindow()) {
            // Let keyPressEvent handle it
            keyPressEvent(keyEvent);
            if (keyEvent->isAccepted()) {
                return true;
            }
        }
    }

    if (obj == scoreCanvas) {
        // Handle tablet press to start zoom drag
        if (event->type() == QEvent::TabletPress && zoomModeActive) {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);
            isDraggingZoom = true;
            dragStartPos = tabletEvent->position().toPoint();
            dragStartMinHz = currentMinHz;
            dragStartMaxHz = currentMaxHz;
            dragStartPixelsPerSecond = currentPixelsPerSecond;

            // Calculate frequency at cursor position for zoom center (vertical)
            int y = tabletEvent->position().y();
            zoomCenterHz = scoreCanvas->pixelToFrequency(y);

            // Calculate time at cursor position for zoom center (horizontal)
            int x = tabletEvent->position().x();
            zoomCenterTime = scoreCanvas->pixelToTime(x);

            tabletEvent->accept();
            return true;  // Event handled
        }

        // Handle tablet move for drag zoom
        if (event->type() == QEvent::TabletMove && zoomModeActive && isDraggingZoom) {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);

            // Calculate vertical drag distance (for frequency zoom)
            int deltaY = dragStartPos.y() - tabletEvent->position().y();

            // Calculate horizontal drag distance (for time zoom)
            int deltaX = dragStartPos.x() - tabletEvent->position().x();

            // VERTICAL ZOOM (Frequency)
            double verticalZoomFactor = std::pow(2.0, deltaY / 100.0);
            double currentFreqRange = std::log2(dragStartMaxHz / dragStartMinHz);
            double newFreqRange = currentFreqRange / verticalZoomFactor;
            double newMinHz = zoomCenterHz / std::pow(2.0, newFreqRange / 2.0);
            double newMaxHz = zoomCenterHz * std::pow(2.0, newFreqRange / 2.0);
            applyZoom(newMinHz, newMaxHz);

            // HORIZONTAL ZOOM (Time)
            double horizontalZoomFactor = std::pow(2.0, deltaX / 100.0);
            double newPixelsPerSecond = dragStartPixelsPerSecond * horizontalZoomFactor;
            applyHorizontalZoom(newPixelsPerSecond);

            tabletEvent->accept();
            return true;  // Event handled
        }

        // Handle tablet release to end zoom drag
        if (event->type() == QEvent::TabletRelease && zoomModeActive && isDraggingZoom) {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);
            isDraggingZoom = false;
            tabletEvent->accept();
            return true;  // Event handled
        }

        // Handle tablet press to start pan drag
        if (event->type() == QEvent::TabletPress && panModeActive) {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);
            isDraggingPan = true;
            panDragStartPos = tabletEvent->globalPosition().toPoint();
            panStartHorizontalScroll = scoreScrollArea->horizontalScrollBar()->value();
            panStartVerticalScroll = scoreScrollArea->verticalScrollBar()->value();

            // Change cursor to closed hand while dragging
            scoreCanvas->setCursor(Qt::ClosedHandCursor);

            tabletEvent->accept();
            return true;  // Event handled
        }

        // Handle tablet move for panning
        if (event->type() == QEvent::TabletMove && panModeActive && isDraggingPan) {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);

            // Calculate delta from drag start position
            QPoint currentPos = tabletEvent->globalPosition().toPoint();
            QPoint delta = currentPos - panDragStartPos;

            // Apply inverse delta to scroll bars for horizontal
            int newHorizontalScroll = panStartHorizontalScroll - delta.x();
            scoreScrollArea->horizontalScrollBar()->setValue(newHorizontalScroll);

            // For vertical panning, bypass scroll bar limits
            int newVerticalOffset = panStartVerticalScroll - delta.y();

            // Update the vertical offset directly for all three widgets
            trackSelector->setVerticalOffset(newVerticalOffset);
            frequencyLabels->setVerticalOffset(newVerticalOffset);
            scoreCanvas->setVerticalOffset(newVerticalOffset);

            // Keep scroll bar in sync when within normal range
            if (newVerticalOffset >= scoreScrollArea->verticalScrollBar()->minimum() &&
                newVerticalOffset <= scoreScrollArea->verticalScrollBar()->maximum()) {
                scoreScrollArea->verticalScrollBar()->setValue(newVerticalOffset);
            }

            tabletEvent->accept();
            return true;  // Event handled
        }

        // Handle tablet release to end pan drag
        if (event->type() == QEvent::TabletRelease && panModeActive && isDraggingPan) {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);
            isDraggingPan = false;

            // Restore open hand cursor
            scoreCanvas->setCursor(Qt::OpenHandCursor);

            tabletEvent->accept();
            return true;  // Event handled
        }

        // Handle mouse press to start pan drag
        if (event->type() == QEvent::MouseButtonPress && panModeActive) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDraggingPan = true;
                panDragStartPos = mouseEvent->globalPosition().toPoint();
                panStartHorizontalScroll = scoreScrollArea->horizontalScrollBar()->value();
                panStartVerticalScroll = scoreScrollArea->verticalScrollBar()->value();

                // Change cursor to closed hand while dragging
                scoreCanvas->setCursor(Qt::ClosedHandCursor);

                return true;  // Event handled
            }
        }

        // Handle mouse move for panning
        if (event->type() == QEvent::MouseMove && panModeActive && isDraggingPan) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            // Calculate delta from drag start position
            QPoint currentPos = mouseEvent->globalPosition().toPoint();
            QPoint delta = currentPos - panDragStartPos;

            // Apply inverse delta to scroll bars for horizontal (still constrained)
            int newHorizontalScroll = panStartHorizontalScroll - delta.x();
            scoreScrollArea->horizontalScrollBar()->setValue(newHorizontalScroll);

            // For vertical panning, bypass scroll bar limits and directly set offset
            // This allows access to off-screen octaves at top and bottom
            int newVerticalOffset = panStartVerticalScroll - delta.y();

            // Allow unlimited vertical panning (no clamping)
            // Update the vertical offset directly for all three widgets
            trackSelector->setVerticalOffset(newVerticalOffset);
            frequencyLabels->setVerticalOffset(newVerticalOffset);
            scoreCanvas->setVerticalOffset(newVerticalOffset);

            // Also update scroll bar position (but don't clamp the offset to scroll bar range)
            // This keeps scroll bar in sync when within normal range
            if (newVerticalOffset >= scoreScrollArea->verticalScrollBar()->minimum() &&
                newVerticalOffset <= scoreScrollArea->verticalScrollBar()->maximum()) {
                scoreScrollArea->verticalScrollBar()->setValue(newVerticalOffset);
            }

            return true;  // Event handled
        }

        // Handle mouse release to end pan drag
        if (event->type() == QEvent::MouseButtonRelease && panModeActive && isDraggingPan) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDraggingPan = false;

                // Restore open hand cursor
                scoreCanvas->setCursor(Qt::OpenHandCursor);

                return true;  // Event handled
            }
        }

        // Handle mouse press to start drag zoom
        if (event->type() == QEvent::MouseButtonPress && zoomModeActive) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDraggingZoom = true;
                dragStartPos = mouseEvent->pos();
                dragStartMinHz = currentMinHz;
                dragStartMaxHz = currentMaxHz;
                dragStartPixelsPerSecond = currentPixelsPerSecond;

                // Calculate frequency at cursor position for zoom center (vertical)
                int y = mouseEvent->pos().y();
                zoomCenterHz = scoreCanvas->pixelToFrequency(y);

                // Calculate time at cursor position for zoom center (horizontal)
                int x = mouseEvent->pos().x();
                zoomCenterTime = scoreCanvas->pixelToTime(x);

                return true;  // Event handled
            }
        }

        // Handle mouse move for drag zoom
        if (event->type() == QEvent::MouseMove && zoomModeActive && isDraggingZoom) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            // Calculate vertical drag distance (for frequency zoom)
            int deltaY = dragStartPos.y() - mouseEvent->pos().y();

            // Calculate horizontal drag distance (for time zoom)
            int deltaX = dragStartPos.x() - mouseEvent->pos().x();

            // VERTICAL ZOOM (Frequency)
            // Drag up = zoom in, drag down = zoom out
            // 100 pixels = 2x zoom
            double verticalZoomFactor = std::pow(2.0, deltaY / 100.0);

            // Calculate new frequency range centered at cursor
            double currentFreqRange = std::log2(dragStartMaxHz / dragStartMinHz);
            double newFreqRange = currentFreqRange / verticalZoomFactor;

            // Apply vertical zoom centered at cursor position
            double newMinHz = zoomCenterHz / std::pow(2.0, newFreqRange / 2.0);
            double newMaxHz = zoomCenterHz * std::pow(2.0, newFreqRange / 2.0);

            applyZoom(newMinHz, newMaxHz);

            // HORIZONTAL ZOOM (Time)
            // Drag left = zoom in, drag right = zoom out
            // 100 pixels = 2x zoom
            double horizontalZoomFactor = std::pow(2.0, deltaX / 100.0);

            // Calculate new pixels per second
            double newPixelsPerSecond = dragStartPixelsPerSecond * horizontalZoomFactor;

            applyHorizontalZoom(newPixelsPerSecond);

            return true;  // Event handled
        }

        // Handle mouse release to end drag zoom
        if (event->type() == QEvent::MouseButtonRelease && zoomModeActive && isDraggingZoom) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDraggingZoom = false;
                return true;  // Event handled
            }
        }

        // Handle mouse wheel zoom when zoom mode is active (both axes)
        if (event->type() == QEvent::Wheel && zoomModeActive) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);

            // Zoom in/out based on wheel direction (both vertical and horizontal)
            if (wheelEvent->angleDelta().y() > 0) {
                onZoomIn();
                // Also zoom in horizontally
                applyHorizontalZoom(currentPixelsPerSecond * 1.25);
            } else {
                onZoomOut();
                // Also zoom out horizontally
                applyHorizontalZoom(currentPixelsPerSecond * 0.8);
            }

            return true;  // Event handled
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void ScoreCanvasWindow::playFirstNote()
{
    if (!audioEngine) return;

    // Get the first note from the canvas phrase
    const QVector<Note>& notes = scoreCanvas->getPhrase().getNotes();

    if (notes.isEmpty()) {
        qDebug() << "No notes to play";
        return;
    }

    // Play the first note
    audioEngine->playNote(notes.first());
}

void ScoreCanvasWindow::startPlayback()
{
    if (!audioEngine) return;

    // Get all notes from the canvas phrase
    const QVector<Note>& notes = scoreCanvas->getPhrase().getNotes();

    if (notes.isEmpty()) {
        qDebug() << "ScoreCanvas: No notes to play";
        return;
    }

    // Stop any currently playing audio first
    if (isPlaying) {
        stopPlayback();
    }

    // Make sure audio is stopped from any other source
    audioEngine->stopNote();

    // Always start playback from the START POSITION (set by double-clicking timeline)
    // This returns to the same position every time, like a "playback anchor"
    playbackStartTime = playbackStartPosition;
    currentNoteIndex = 0;

    // Move the now marker to the start position
    timeline->setNowMarker(playbackStartTime);

    // Find the first note that starts at or after the playback position
    for (int i = 0; i < notes.size(); ++i) {
        if (notes[i].getStartTime() >= playbackStartTime) {
            currentNoteIndex = i;
            break;
        }
    }

    isPlaying = true;

    // Start the playback timer (tick every 10ms for smooth timing)
    playbackTimer->start(10);

    qDebug() << "ScoreCanvas: Starting playback from" << playbackStartTime << "ms with" << notes.size() << "notes";

    // Emit signal to stop other windows
    emit playbackStarted();
}

void ScoreCanvasWindow::onPlaybackTick()
{
    if (!audioEngine || !isPlaying) return;

    const QVector<Note>& notes = scoreCanvas->getPhrase().getNotes();

    // Check if we've hit the loop end - if so, jump back to loop start
    if (timeline->hasLoop()) {
        double loopEnd = timeline->getLoopEnd();
        if (playbackStartTime >= loopEnd) {
            // Jump back to loop start
            playbackStartTime = timeline->getLoopStart();

            // Reset note index to find notes at loop start
            currentNoteIndex = 0;
            for (int i = 0; i < notes.size(); ++i) {
                if (notes[i].getStartTime() >= playbackStartTime) {
                    currentNoteIndex = i;
                    break;
                }
            }

            qDebug() << "ScoreCanvas: Looping back to" << playbackStartTime << "ms";
        }
    }

    // Update timeline now marker (always advance, even if no notes)
    timeline->setNowMarker(playbackStartTime);

    // Check if it's time to trigger the next note (only if there are notes)
    if (!notes.isEmpty() && currentNoteIndex < notes.size()) {
        const Note& currentNote = notes[currentNoteIndex];

        if (playbackStartTime >= currentNote.getStartTime()) {
            // Trigger this note
            audioEngine->playNote(currentNote);
            qDebug() << "Playing note" << currentNoteIndex + 1 << "of" << notes.size()
                     << "- Pitch:" << currentNote.getPitchHz() << "Hz"
                     << "Duration:" << currentNote.getDuration() << "ms";

            // Move to next note
            currentNoteIndex++;
        }
    }

    // Always advance playback time (10ms per tick)
    // Playback continues indefinitely until user manually stops
    playbackStartTime += 10.0;
}

void ScoreCanvasWindow::stopPlayback(bool stopAudioEngine)
{
    if (!audioEngine) return;

    // Stop the timer first
    if (playbackTimer && playbackTimer->isActive()) {
        playbackTimer->stop();
    }

    // Reset playback state BEFORE stopping note (prevents race condition)
    isPlaying = false;
    currentNoteIndex = 0;
    // DON'T reset playbackStartTime - leave it where playback stopped
    // DON'T reset timeline now marker - leave it at current position so user can see where it stopped

    // Only stop audio engine if requested (don't stop when another window is taking over)
    if (stopAudioEngine) {
        audioEngine->stopNote();
    }

    qDebug() << "ScoreCanvas: Playback stopped" << (stopAudioEngine ? "(audio stopped)" : "(audio continues)");
}

void ScoreCanvasWindow::onTimeModeToggled()
{
    // Toggle between Absolute and Musical time modes
    if (currentTimeMode == AbsoluteTime) {
        currentTimeMode = MusicalTime;
        timeModeToggle->setText("Musical");
        timeModeToggle->setChecked(true);
        qDebug() << "ScoreCanvasWindow: Switched to Musical time mode";
    } else {
        currentTimeMode = AbsoluteTime;
        timeModeToggle->setText("Absolute");
        timeModeToggle->setChecked(false);
        qDebug() << "ScoreCanvasWindow: Switched to Absolute time mode";
    }

    // Update timeline display
    timeline->setTimeMode(currentTimeMode == MusicalTime ? Timeline::Musical : Timeline::Absolute);
    timeline->setTempo(currentTempo);
    timeline->setTimeSignature(currentTimeSigTop, currentTimeSigBottom);

    // Update ScoreCanvas musical mode to show/hide bar lines
    scoreCanvas->setMusicalMode(currentTimeMode == MusicalTime, currentTempo, currentTimeSigTop, currentTimeSigBottom);
}

void ScoreCanvasWindow::onTempoChanged(int bpm)
{
    currentTempo = bpm;
    qDebug() << "ScoreCanvasWindow: Tempo changed to" << bpm << "BPM";

    // Update timeline
    timeline->setTempo(currentTempo);

    // If in musical mode, update bar line spacing
    if (currentTimeMode == MusicalTime) {
        scoreCanvas->setMusicalMode(true, currentTempo, currentTimeSigTop, currentTimeSigBottom);
    }
}

void ScoreCanvasWindow::onTimeSignatureChanged()
{
    currentTimeSigTop = timeSigNumerator->value();
    currentTimeSigBottom = timeSigDenominator->value();
    qDebug() << "ScoreCanvasWindow: Time signature changed to"
             << currentTimeSigTop << "/" << currentTimeSigBottom;

    // Update timeline
    timeline->setTimeSignature(currentTimeSigTop, currentTimeSigBottom);

    // If in musical mode, update bar line spacing
    if (currentTimeMode == MusicalTime) {
        scoreCanvas->setMusicalMode(true, currentTempo, currentTimeSigTop, currentTimeSigBottom);
    }
}

ScoreCanvasWindow::~ScoreCanvasWindow()
{
    // Remove event filter
    qApp->removeEventFilter(this);

    // Don't delete audioEngine - it's owned by CalamusMain
    delete ui;
}
