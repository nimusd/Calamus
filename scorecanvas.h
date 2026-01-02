#ifndef SCORECANVAS_H
#define SCORECANVAS_H

#include <QWidget>
#include <QVector>
#include <QColor>
#include <QString>
#include <QUndoStack>
#include "phrase.h"
#include "note.h"
#include "phrasegroup.h"

class ScoreCanvas : public QWidget
{
    Q_OBJECT

public:
    // Input modes for drawing tools
    enum InputMode {
        SelectionMode,          // Select notes (no drawing)
        DrawModeDiscrete,       // Draw notes snapped to scale lines
        DrawModeContinuous      // Draw notes with literal pitch (no snapping)
    };

    struct ScaleLine {
        double frequencyHz;
        int scaleDegree;        // 1-7 for major scale
        QColor color;
        QString noteName;
        bool isThicker;         // Tonic and Fifth are thicker
    };

    explicit ScoreCanvas(QWidget *parent = nullptr);

    // Synchronization with track selector
    void setFrequencyRange(double minHz, double maxHz);
    void setPixelsPerHz(double ratio);
    void setVerticalOffset(int offsetPixels);

    // Horizontal zoom (time axis)
    void setPixelsPerSecond(double pps);

    // Scale configuration
    void setScale(const QString &rootNote, const QString &scaleType = "Major");
    void generateScaleLines();

    // Coordinate conversion (needed for zoom at cursor)
    double pixelToFrequency(int pixel) const;

    // Coordinate conversion for note input
    double pixelToTime(int x) const;
    int timeToPixel(double timeMs) const;
    double snapToNearestScaleLine(double hz) const;

    // Note management
    Phrase& getPhrase() { return phrase; }
    const Phrase& getPhrase() const { return phrase; }
    void clearNotes();

    // Undo/Redo
    QUndoStack* getUndoStack() { return undoStack; }

    // Input mode management
    void setInputMode(InputMode mode);
    InputMode getInputMode() const { return currentInputMode; }

    // Track management
    void setActiveTrack(int trackIndex);
    int getActiveTrack() const { return activeTrackIndex; }

    // Musical mode settings (for bar lines)
    void setMusicalMode(bool enabled, int tempo = 120, int timeSigTop = 4, int timeSigBottom = 4);

    // Clipboard operations
    void setPasteTargetTime(double timeMs);

    // Note editing operations
    void snapSelectedNotesToScale();  // Quantize selected continuous notes to scale degrees

    // Phrase management
    void createPhraseFromSelection(const QString &name = "New Phrase");
    void ungroupPhrase(int phraseIndex);
    int findPhraseAtPosition(const QPoint &pos) const;
    void selectPhrase(int index);
    void deselectPhrase();
    int getSelectedPhraseIndex() const { return selectedPhraseIndex; }
    PhraseGroup* getSelectedPhrase();
    const PhraseGroup* getSelectedPhrase() const;
    QVector<PhraseGroup>& getPhraseGroups() { return phraseGroups; }
    const QVector<PhraseGroup>& getPhraseGroups() const { return phraseGroups; }
    void applyPhraseTemplate(const PhraseGroup &templatePhrase);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void zoomChanged(double pixelsPerHz);
    void frequencyRangeChanged(double minHz, double maxHz);
    void pressureChanged(double pressure, bool active);  // Emits pressure updates during drawing
    void cursorPositionChanged(double timeMs, double pitchHz);  // Emits cursor position for status bar
    void phraseSelectionChanged();  // Emitted when phrase selection changes

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void tabletEvent(QTabletEvent *event) override;

private:
    QVector<ScaleLine> scaleLines;
    Phrase phrase;  // Notes storage
    QUndoStack *undoStack;  // Undo/Redo stack

    // Phrase groups
    QVector<PhraseGroup> phraseGroups;  // All phrase groups
    int selectedPhraseIndex;            // -1 if no phrase selected

    // Phrase curve editing state
    bool isEditingPhraseCurve;
    int editingPhraseDotIndex;
    double editingPhraseDotTimePos;
    Curve dragStartPhraseCurve;

    // Phrase curve drawing state (gesture capture)
    bool isDrawingPhraseCurve;
    QVector<QPair<double, double>> phraseCurveGesturePoints;  // (time in ms, normalized value 0-1)
    QVector<double> phraseCurveGesturePressures;  // Pressure for each point
    int phraseCurveGestureMinY, phraseCurveGestureMaxY;  // Hull bounds for normalization

    // Input mode state
    InputMode currentInputMode;

    // Track state
    int activeTrackIndex;   // Which track new notes will use (default 0)

    // Frequency mapping (must match TrackSelector)
    double visibleMinHz;
    double visibleMaxHz;
    double pixelsPerHz;
    int verticalScrollOffset;

    // Time axis mapping
    double pixelsPerSecond;    // Horizontal zoom (pixels per second of time)
    int horizontalScrollOffset;  // Horizontal scroll position

    // Musical mode state (for bar lines)
    bool musicalModeEnabled;
    int musicalTempo;          // BPM
    int musicalTimeSigTop;     // Beats per bar
    int musicalTimeSigBottom;  // Note value (4 = quarter note)

    // Note input interaction state
    bool isDrawingNote;
    QPoint noteStartPos;
    QPoint noteCurrentPos;
    Note* pendingNote;

    // Pen tablet state
    bool usingTablet;        // true if current input is from pen tablet
    double penPressure;      // Current pen pressure (0.0-1.0)
    QVector<QPair<double, double>> pressurePoints;  // (time in ms, pressure) pairs during stroke
    QVector<QPair<double, double>> pitchPoints;     // (time in ms, pitch Hz) pairs during stroke

    // Selection state
    QVector<int> selectedNoteIndices;  // Empty if no selection

    // Clipboard state
    QVector<Note> clipboard;           // Copied notes
    double pasteTargetTime;            // Where to paste (set from now marker)

    // Lasso selection state
    bool isDrawingLasso;
    QPoint lassoStartPos;
    QPoint lassoCurrentPos;

    // Drag state
    enum DragMode {
        NoDrag,
        DraggingNote,          // Dragging note body (move time/pitch)
        ResizingLeft,          // Dragging left resize handle
        ResizingRight,         // Dragging right resize handle
        EditingTopCurve,       // Dragging top edge dot to edit dynamics curve
        EditingBottomCurve,    // Dragging bottom edge dot to edit bottom curve
        ResizingPhraseHullTop,    // Dragging phrase hull top edge
        ResizingPhraseHullBottom  // Dragging phrase hull bottom edge
    };
    DragMode currentDragMode;
    int dragStartPhraseVerticalPadding;  // For phrase hull resize
    QPoint dragStartPos;
    double dragStartTime;
    double dragStartPitch;
    double dragStartDuration;
    int editingDotIndex;       // Which dot is being edited (-1 if none)
    double editingDotTimePos;  // Normalized time position of the dot being edited
    Curve dragStartCurve;      // Original curve state when drag started
    QVector<QPair<int, double>> multiDragStartTimes;  // Original times for multi-selection drag
    QVector<QPair<int, double>> multiDragStartPitches;  // Original pitches for multi-selection drag
    QVector<QPair<int, Curve>> multiDragStartCurves;  // Original curves for multi-selection drag

    // Visual constants
    static constexpr int HZ_LABEL_WIDTH = 60;
    static constexpr int NORMAL_LINE_WIDTH = 1;
    static constexpr int THICK_LINE_WIDTH = 2;
    static constexpr double BASE_FREQUENCY = 25.0;      // Hz - base frequency for just intonation
    static constexpr int PIXELS_PER_OCTAVE = 100;       // Fixed vertical size for each octave

    // ROYGBIV color system for scale degrees
    static const QColor SCALE_COLORS[7];

    // Helper methods
    int frequencyToPixel(double hz) const;
    QColor getScaleDegreeColor(int degree) const;
    void drawScaleLine(QPainter &painter, const ScaleLine &line, int y);
    void drawHzLabel(QPainter &painter, double hz, int y);
    void drawNote(QPainter &painter, const Note &note, bool isSelected);
    void drawPendingNote(QPainter &painter);
    void drawSelectionRectangle(QPainter &painter, const Note &note);
    void drawLassoRectangle(QPainter &painter);
    void drawBarLines(QPainter &painter);

    // Phrase rendering methods
    void drawPhraseHull(QPainter &painter, const PhraseGroup &phrase, bool isSelected);
    void drawPhraseCurve(QPainter &painter, const PhraseGroup &phrase, bool isSelected);
    void drawPhraseCurveDots(QPainter &painter, const PhraseGroup &phrase, double phraseStart, double phraseDuration, int minY, int maxY);
    double getAveragePitchAtTime(const PhraseGroup &phrase, double time) const;
    int findPhraseCurveDotAtPosition(const QPoint &pos, const PhraseGroup &phrase) const;
    DragMode detectPhraseHullResizeHandle(const QPoint &pos, const PhraseGroup &phrase) const;

    // Selection helpers
    int findNoteAtPosition(const QPoint &pos) const;
    QVector<int> findNotesInRectangle(const QRect &rect) const;
    void selectNote(int index, bool addToSelection = false);
    void selectNotes(const QVector<int> &indices);
    void deselectAll();
    bool isNoteSelected(int index) const;

    // Drag helpers
    DragMode detectDragMode(const QPoint &pos, const Note &note) const;
    QRect getNoteRect(const Note &note) const;
    int calculateCurveDotCount(const Note &note) const;  // Adaptive dot count based on note width
    QRect getLeftResizeHandle(const Note &note) const;
    QRect getRightResizeHandle(const Note &note) const;

    // Pitch curve quantization
    Curve quantizePitchCurveToScale(const Curve &pitchCurve) const;
};

#endif // SCORECANVAS_H
