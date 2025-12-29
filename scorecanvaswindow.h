#ifndef SCORECANVASWINDOW_H
#define SCORECANVASWINDOW_H

#include "ui_scorecanvas.h"
#include "trackselect.h"
#include "scorecanvas.h"
#include "timeline.h"
#include "frequencylabels.h"
#include "audioengine.h"
#include <QMainWindow>
#include <QLabel>
#include <QActionGroup>
#include <QPushButton>
#include <QSpinBox>

namespace Ui {
class ScoreCanvasWindow;
}

class ScoreCanvasWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ScoreCanvasWindow(AudioEngine *sharedAudioEngine, QWidget *parent = nullptr);
    ~ScoreCanvasWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onZoomToggled(bool checked);
    void onZoomIn();
    void onZoomOut();
    void onPressureChanged(double pressure, bool active);
    void onCursorPositionChanged(double timeMs, double pitchHz);

private:
    Ui::scorecanvas *ui;
    TrackSelector *trackSelector;
    ScoreCanvas *scoreCanvas;
    Timeline *timeline;
    FrequencyLabels *frequencyLabels;
    QScrollArea *scoreScrollArea;
    QScrollArea *trackScrollArea;
    QScrollArea *frequencyScrollArea;

    // Drawing tool action group
    QActionGroup *drawingToolGroup;

    // Status labels
    QLabel *pressureLabel;
    QLabel *cursorTimeLabel;
    QLabel *cursorPitchLabel;
    QLabel *statusTempoLabel;
    QLabel *statusTimeSigLabel;
    QLabel *navigationModeLabel;

    // Composition settings widgets (Phase 3)
    QPushButton *timeModeToggle;      // Absolute ↔ Musical
    QSpinBox *tempoSpinBox;           // BPM
    QSpinBox *timeSigNumerator;       // Top number (beats per bar)
    QSpinBox *timeSigDenominator;     // Bottom number (note value)

    // Composition state
    enum TimeMode {
        AbsoluteTime,   // Min:Sec:Ms
        MusicalTime     // Bars:Beats:Ms
    };
    TimeMode currentTimeMode;
    int currentTempo;           // BPM
    int currentTimeSigTop;      // Beats per bar
    int currentTimeSigBottom;   // Note value (4 = quarter note)

    // Audio engine (Phase 3)
    AudioEngine *audioEngine;

    // Playback state
    QTimer *playbackTimer;
    int currentNoteIndex;
    double playbackStartTime;      // Current playback position
    double playbackStartPosition;  // Where playback should always start from (set by double-click)
    bool isPlaying;

    // Zoom state
    bool zoomModeActive;
    double currentMinHz;
    double currentMaxHz;
    double currentPixelsPerSecond;  // Horizontal zoom (time axis)
    bool isDraggingZoom;
    QPoint dragStartPos;
    double dragStartMinHz;
    double dragStartMaxHz;
    double dragStartPixelsPerSecond;
    double zoomCenterHz;  // Frequency at cursor position
    double zoomCenterTime;  // Time at cursor position

    // Pan mode state
    bool panModeActive;
    bool isDraggingPan;
    QPoint panDragStartPos;
    int panStartHorizontalScroll;
    int panStartVerticalScroll;

    void setupToolbarColors();
    void setupDrawingTools();
    void setupCompositionSettings();
    void setupTrackSelector();
    void setupScoreCanvas();
    void setupZoom();
    void setZoomMode(bool active);
    void setPanMode(bool active);
    void applyZoom(double minHz, double maxHz);
    void applyHorizontalZoom(double pixelsPerSecond);
    void onTimeModeToggled();
    void onTempoChanged(int bpm);
    void onTimeSignatureChanged();

    // Audio playback (Phase 3)
    void playFirstNote();
    void startPlayback();
    void onPlaybackTick();

public:
    // Expose ScoreCanvas for sharing notes with SounitBuilder
    ScoreCanvas* getScoreCanvas() const { return scoreCanvas; }

public slots:
    void stopPlayback(bool stopAudioEngine = true);

signals:
    void playbackStarted();
};

#endif // SCORECANVASWINDOW_H
