#include "scorecanvas.h"
#include "scorecanvascommands.h"
#include "curve.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QMouseEvent>
#include <QTabletEvent>
#include <QKeyEvent>
#include <cmath>
#include <algorithm>

// ROYGBIV color system for scale degrees (1-7)
const QColor ScoreCanvas::SCALE_COLORS[7] = {
    QColor(255, 0, 0),      // 1 (Tonic) - Red
    QColor(255, 127, 0),    // 2 - Orange
    QColor(255, 215, 0),    // 3 - Yellow (gold)
    QColor(0, 128, 0),      // 4 - Green
    QColor(0, 0, 255),      // 5 (Fifth) - Blue
    QColor(75, 0, 130),     // 6 - Indigo
    QColor(148, 0, 211)     // 7 - Violet
};

ScoreCanvas::ScoreCanvas(QWidget *parent)
    : QWidget(parent)
    , selectedPhraseIndex(-1)   // No phrase selected initially
    , isEditingPhraseCurve(false)
    , editingPhraseDotIndex(-1)
    , editingPhraseDotTimePos(0.0)
    , isDrawingPhraseCurve(false)
    , currentInputMode(DrawModeDiscrete)  // Default to discrete drawing mode
    , activeTrackIndex(0)       // Default to track 0
    , visibleMinHz(27.5)        // A0
    , visibleMaxHz(4186.0)      // C8
    , pixelsPerHz(0.1)
    , verticalScrollOffset(0)
    , pixelsPerSecond(100.0)    // Default: 100 pixels per second
    , horizontalScrollOffset(0)
    , musicalModeEnabled(false)
    , musicalTempo(120)
    , musicalTimeSigTop(4)
    , musicalTimeSigBottom(4)
    , isDrawingNote(false)
    , pendingNote(nullptr)
    , usingTablet(false)
    , penPressure(0.0)
    , pasteTargetTime(0.0)      // Initialize paste position to start
    , isDrawingLasso(false)
    , currentDragMode(NoDrag)
    , dragStartTime(0.0)
    , dragStartPitch(0.0)
    , dragStartDuration(0.0)
    , editingDotIndex(-1)
    , editingDotTimePos(0.0)
{
    setMinimumSize(400, 400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: white;");
    setMouseTracking(true);  // Enable hover cursor updates
    setFocusPolicy(Qt::StrongFocus);  // Enable keyboard input

    // Initialize undo stack
    undoStack = new QUndoStack(this);

    // Generate default C major scale
    generateScaleLines();
}

void ScoreCanvas::setFrequencyRange(double minHz, double maxHz)
{
    visibleMinHz = minHz;
    visibleMaxHz = maxHz;
    update();
    emit frequencyRangeChanged(minHz, maxHz);
}

void ScoreCanvas::setPixelsPerHz(double ratio)
{
    pixelsPerHz = ratio;
    update();
    emit zoomChanged(ratio);
}

void ScoreCanvas::setVerticalOffset(int offsetPixels)
{
    verticalScrollOffset = offsetPixels;
    update();
}

void ScoreCanvas::setPixelsPerSecond(double pps)
{
    pixelsPerSecond = pps;
    update();  // Repaint with new horizontal zoom
}

void ScoreCanvas::setScale(const QString &rootNote, const QString &scaleType)
{
    // For now, we'll keep C major
    // Future: implement different scales and roots
    generateScaleLines();
}

void ScoreCanvas::generateScaleLines()
{
    scaleLines.clear();

    // Just intonation major scale ratios
    // Scale degrees: C(1), D(2), E(3), F(4), G(5), A(6), B(7)
    const double majorScaleRatios[7] = {
        1.0,        // C (Tonic) - 1/1
        9.0/8.0,    // D - 9/8
        5.0/4.0,    // E - 5/4
        4.0/3.0,    // F - 4/3
        3.0/2.0,    // G (Fifth) - 3/2
        5.0/3.0,    // A - 5/3
        15.0/8.0    // B - 15/8
    };

    const QString noteNames[7] = {"C", "D", "E", "F", "G", "A", "B"};

    // Generate scale lines starting from BASE_FREQUENCY (25 Hz)
    // Cover octaves from 25 Hz up to 8000 Hz
    for (int octave = 0; octave <= 9; ++octave) {
        double octaveBaseFreq = BASE_FREQUENCY * std::pow(2.0, octave);

        for (int degree = 0; degree < 7; ++degree) {
            double freq = octaveBaseFreq * majorScaleRatios[degree];

            // Only add lines within a reasonable musical range
            if (freq >= 20.0 && freq <= 8000.0) {
                ScaleLine line;
                line.frequencyHz = freq;
                line.scaleDegree = degree + 1;  // 1-based
                line.color = getScaleDegreeColor(degree + 1);
                line.noteName = noteNames[degree] + QString::number(octave);
                line.isThicker = (degree == 0 || degree == 4);  // Tonic or Fifth

                scaleLines.append(line);
            }
        }
    }

    update();
}

QSize ScoreCanvas::sizeHint() const
{
    return QSize(800, 600);
}

QSize ScoreCanvas::minimumSizeHint() const
{
    return QSize(400, 400);
}

void ScoreCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background
    painter.fillRect(rect(), Qt::white);

    // Draw all scale lines
    for (const ScaleLine &line : scaleLines) {
        int y = frequencyToPixel(line.frequencyHz);

        // Only draw if visible
        if (y >= -10 && y <= height() + 10) {
            drawScaleLine(painter, line, y);
            // Hz labels are now drawn in FrequencyLabels widget
        }
    }

    // Draw bar lines (if in musical mode)
    drawBarLines(painter);

    // Draw phrase hulls (translucent boundaries)
    for (int i = 0; i < phraseGroups.size(); ++i) {
        drawPhraseHull(painter, phraseGroups[i], i == selectedPhraseIndex);
    }

    // Draw all notes
    const QVector<Note> &notes = phrase.getNotes();
    for (int i = 0; i < notes.size(); ++i) {
        bool isSelected = isNoteSelected(i);
        drawNote(painter, notes[i], isSelected);
    }

    // Draw phrase curves (overlay on notes)
    for (int i = 0; i < phraseGroups.size(); ++i) {
        drawPhraseCurve(painter, phraseGroups[i], i == selectedPhraseIndex);
    }

    // Draw pending note being drawn
    drawPendingNote(painter);

    // Draw phrase curve gesture preview
    if (isDrawingPhraseCurve && !phraseCurveGesturePoints.isEmpty()) {
        int hullHeight = phraseCurveGestureMaxY - phraseCurveGestureMinY;

        // Draw variable-thickness stroke preview
        QPainterPath gesturePath;
        QVector<QPointF> topEdge, bottomEdge;

        for (int i = 0; i < phraseCurveGesturePoints.size(); ++i) {
            int x = timeToPixel(phraseCurveGesturePoints[i].first);
            double value = phraseCurveGesturePoints[i].second;
            double pressure = phraseCurveGesturePressures[i];

            // Y position based on value
            int centerY = phraseCurveGestureMinY + static_cast<int>((1.0 - value) * hullHeight);

            // Thickness based on pressure
            double thickness = 10.0 + pressure * 30.0;

            topEdge << QPointF(x, centerY - thickness / 2.0);
            bottomEdge << QPointF(x, centerY + thickness / 2.0);
        }

        // Create preview path
        if (!topEdge.isEmpty()) {
            gesturePath.moveTo(topEdge.first());
            for (const QPointF &p : topEdge) gesturePath.lineTo(p);
            for (int i = bottomEdge.size() - 1; i >= 0; --i) {
                gesturePath.lineTo(bottomEdge[i]);
            }
            gesturePath.closeSubpath();
        }

        // Draw with semi-transparent orange
        painter.setBrush(QColor(255, 100, 50, 150));
        painter.setPen(Qt::NoPen);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawPath(gesturePath);
    }

    // Draw lasso rectangle
    drawLassoRectangle(painter);
}

void ScoreCanvas::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

int ScoreCanvas::frequencyToPixel(double hz) const
{
    // Convert Hz to pixel position using logarithmic spacing
    // Each octave = PIXELS_PER_OCTAVE (100 pixels)
    // Higher frequencies = lower Y values (top of screen)

    if (hz <= 0 || BASE_FREQUENCY <= 0) {
        return 0;
    }

    // Calculate octave number from base frequency (logarithmic)
    double octaveNumber = std::log2(hz / BASE_FREQUENCY);

    // Convert to pixels from bottom
    double pixelsFromBottom = octaveNumber * PIXELS_PER_OCTAVE;

    // Calculate visible octave range
    double minOctave = std::log2(visibleMinHz / BASE_FREQUENCY);
    double maxOctave = std::log2(visibleMaxHz / BASE_FREQUENCY);
    double visibleOctaveRange = maxOctave - minOctave;

    // Normalize position within visible range
    double normalizedPos = (octaveNumber - minOctave) / visibleOctaveRange;
    // Don't clamp - allow frequencies outside visible range to be positioned off-screen
    // The paintEvent will filter them out

    // Convert to pixel position (flip for top-down coordinate system)
    int pixel = height() - static_cast<int>(normalizedPos * height());
    return pixel - verticalScrollOffset;
}

double ScoreCanvas::pixelToFrequency(int pixel) const
{
    // Convert pixel position to Hz using logarithmic spacing
    int adjustedPixel = pixel + verticalScrollOffset;
    double normalizedPos = 1.0 - (static_cast<double>(adjustedPixel) / height());
    normalizedPos = qBound(0.0, normalizedPos, 1.0);

    // Calculate visible octave range
    double minOctave = std::log2(visibleMinHz / BASE_FREQUENCY);
    double maxOctave = std::log2(visibleMaxHz / BASE_FREQUENCY);
    double visibleOctaveRange = maxOctave - minOctave;

    // Calculate octave number at this position
    double octaveNumber = minOctave + (normalizedPos * visibleOctaveRange);

    // Convert back to Hz
    return BASE_FREQUENCY * std::pow(2.0, octaveNumber);
}

QColor ScoreCanvas::getScaleDegreeColor(int degree) const
{
    if (degree >= 1 && degree <= 7) {
        return SCALE_COLORS[degree - 1];
    }
    return Qt::black;  // Fallback
}

void ScoreCanvas::drawScaleLine(QPainter &painter, const ScaleLine &line, int y)
{
    // Set pen based on whether this is a thick line (tonic/fifth)
    QPen pen(line.color);
    pen.setWidth(line.isThicker ? THICK_LINE_WIDTH : NORMAL_LINE_WIDTH);
    painter.setPen(pen);

    // Draw line from left edge to right edge (Hz labels are in separate widget)
    int startX = 0;
    int endX = width();
    painter.drawLine(startX, y, endX, y);
}

void ScoreCanvas::drawHzLabel(QPainter &painter, double hz, int y)
{
    // Draw Hz value on the left edge
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    QString label = QString::number(hz, 'f', 1) + " Hz";
    QRect labelRect(0, y - 8, HZ_LABEL_WIDTH - 5, 16);
    painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
}

// ============================================================================
// Coordinate Conversion
// ============================================================================

double ScoreCanvas::pixelToTime(int x) const
{
    // Convert pixel X position to time in milliseconds
    double adjustedX = x + horizontalScrollOffset;
    return (adjustedX / pixelsPerSecond) * 1000.0;  // Convert to milliseconds
}

int ScoreCanvas::timeToPixel(double timeMs) const
{
    // Convert time in milliseconds to pixel X position
    double seconds = timeMs / 1000.0;
    return static_cast<int>(seconds * pixelsPerSecond) - horizontalScrollOffset;
}

double ScoreCanvas::snapToNearestScaleLine(double hz) const
{
    // Find the closest scale line frequency
    if (scaleLines.isEmpty()) return hz;

    double closestHz = scaleLines.first().frequencyHz;
    double minDistance = std::abs(hz - closestHz);

    for (const ScaleLine &line : scaleLines) {
        double distance = std::abs(hz - line.frequencyHz);
        if (distance < minDistance) {
            minDistance = distance;
            closestHz = line.frequencyHz;
        }
    }

    return closestHz;
}

void ScoreCanvas::clearNotes()
{
    phrase.clearNotes();
    update();
}

void ScoreCanvas::snapSelectedNotesToScale()
{
    // Quantize all selected continuous notes to scale degrees
    if (selectedNoteIndices.isEmpty()) {
        qDebug() << "ScoreCanvas::snapSelectedNotesToScale - No notes selected";
        return;
    }

    QVector<Note> &notes = phrase.getNotes();
    int quantizedCount = 0;

    for (int index : selectedNoteIndices) {
        if (index >= 0 && index < notes.size()) {
            Note &note = notes[index];

            // Only quantize notes that have pitch curves (continuous notes)
            if (note.hasPitchCurve()) {
                Curve quantizedCurve = quantizePitchCurveToScale(note.getPitchCurve());
                note.setPitchCurve(quantizedCurve);
                quantizedCount++;
                qDebug() << "Quantized note" << index << "- original points:"
                         << note.getPitchCurve().getPointCount()
                         << "quantized points:" << quantizedCurve.getPointCount();
            }
        }
    }

    if (quantizedCount > 0) {
        update();
        qDebug() << "ScoreCanvas::snapSelectedNotesToScale - Quantized" << quantizedCount << "notes";
    } else {
        qDebug() << "ScoreCanvas::snapSelectedNotesToScale - No continuous notes in selection";
    }
}

// ============================================================================
// Input Mode Management
// ============================================================================

void ScoreCanvas::setInputMode(InputMode mode)
{
    currentInputMode = mode;

    // Cancel any ongoing drawing when switching modes
    if (isDrawingNote) {
        isDrawingNote = false;
        usingTablet = false;
        pressurePoints.clear();
        pitchPoints.clear();
        update();
    }

    // Deselect notes when switching to drawing modes
    if (mode != SelectionMode && !selectedNoteIndices.isEmpty()) {
        deselectAll();
        update();
    }

    qDebug() << "ScoreCanvas: Input mode changed to" << mode;
}

// ============================================================================
// Musical Mode Management
// ============================================================================

void ScoreCanvas::setMusicalMode(bool enabled, int tempo, int timeSigTop, int timeSigBottom)
{
    musicalModeEnabled = enabled;
    musicalTempo = tempo;
    musicalTimeSigTop = timeSigTop;
    musicalTimeSigBottom = timeSigBottom;
    update();  // Repaint to show/hide bar lines
}

// ============================================================================
// Clipboard Operations
// ============================================================================

void ScoreCanvas::setPasteTargetTime(double timeMs)
{
    pasteTargetTime = timeMs;
}

// ============================================================================
// Track Management
// ============================================================================

void ScoreCanvas::setActiveTrack(int trackIndex)
{
    activeTrackIndex = trackIndex;
    qDebug() << "ScoreCanvas: Active track set to" << trackIndex;
}

void ScoreCanvas::drawBarLines(QPainter &painter)
{
    if (!musicalModeEnabled) return;

    // Calculate bar and beat durations in milliseconds
    // Two modes based on bottom number:
    // - Bottom = 0: Simple mode, bar = (60000/tempo) * top
    // - Bottom ≠ 0: Scaled mode, bar = (60000/tempo) * (top/bottom)

    double barDurationMs;
    double beatDurationMs;

    if (musicalTimeSigBottom == 0) {
        // Option A: Bottom is just a label, beat = 60000/tempo
        beatDurationMs = 60000.0 / musicalTempo;
        barDurationMs = beatDurationMs * musicalTimeSigTop;
    } else {
        // Option B: Bottom scales the beat duration
        barDurationMs = (60000.0 / musicalTempo) * (static_cast<double>(musicalTimeSigTop) / musicalTimeSigBottom);
        beatDurationMs = barDurationMs / musicalTimeSigTop;
    }

    // Calculate visible time range
    double visibleStartTime = pixelToTime(0);
    double visibleEndTime = pixelToTime(width());

    // Draw beat lines (very fine, light grey)
    painter.setPen(QPen(QColor(200, 200, 200), 1));  // Very light grey, thin

    int startBeat = static_cast<int>(visibleStartTime / beatDurationMs);
    int endBeat = static_cast<int>(visibleEndTime / beatDurationMs) + 1;

    for (int beat = startBeat; beat <= endBeat; ++beat) {
        double beatTime = beat * beatDurationMs;
        int x = timeToPixel(beatTime);

        // Only draw if within visible range
        if (x >= 0 && x <= width()) {
            painter.drawLine(x, 0, x, height());
        }
    }

    // Draw bar lines (thicker, darker grey) - these will overdraw beat lines at bar boundaries
    painter.setPen(QPen(QColor(80, 80, 80), 2));  // Dark grey, thicker

    int startBar = static_cast<int>(visibleStartTime / barDurationMs);
    int endBar = static_cast<int>(visibleEndTime / barDurationMs) + 1;

    for (int bar = startBar; bar <= endBar; ++bar) {
        double barTime = bar * barDurationMs;
        int x = timeToPixel(barTime);

        // Only draw if within visible range
        if (x >= 0 && x <= width()) {
            painter.drawLine(x, 0, x, height());
        }
    }
}

// ============================================================================
// Note Drawing
// ============================================================================

void ScoreCanvas::drawNote(QPainter &painter, const Note &note, bool isSelected)
{
    // Calculate position and size
    int x = timeToPixel(note.getStartTime());
    int width = timeToPixel(note.getEndTime()) - x;

    // Skip if note is completely off-screen
    if (x + width < 0 || x > this->width()) return;

    // Find maximum dynamics value in curve for blob height
    const Curve &dynamicsCurve = note.getDynamicsCurve();
    double maxDynamics = 0.0;
    for (int i = 0; i <= 20; ++i) {  // Sample curve at multiple points
        double t = i / 20.0;
        double value = dynamicsCurve.valueAt(t);
        if (value > maxDynamics) maxDynamics = value;
    }

    // Note blob height based on maximum dynamics in curve
    int blobHeight = static_cast<int>(20 + maxDynamics * 60);  // 20-80 pixels (increased range)

    // Generate amplitude curve for top edge and bottom edge
    // If note has pitch curve, follow it; otherwise use constant pitch
    QVector<QPointF> topCurve;
    QVector<QPointF> bottomCurve;
    int numPoints = std::max(10, width / 5);  // More points for longer notes

    for (int i = 0; i <= numPoints; ++i) {
        double t = static_cast<double>(i) / numPoints;  // 0.0 to 1.0
        int posX = x + static_cast<int>(t * width);

        // Get pitch at this point (either from pitch curve or constant)
        double pitchAtT = note.getPitchAt(t);
        int centerY = frequencyToPixel(pitchAtT);

        // Get dynamics from the note's curve at this normalized time
        double dynamics = note.getDynamicsAt(t);

        // Envelope is directly from the dynamics curve
        double envelope = dynamics;

        // Top edge (amplitude - follows pressure curve)
        int topOffset = static_cast<int>(envelope * blobHeight * 0.5);
        topCurve.append(QPointF(posX, centerY - topOffset));

        // Bottom edge (from bottom curve - independent)
        double bottomValue = note.getBottomCurveAt(t);
        int bottomOffset = static_cast<int>(bottomValue * blobHeight * 0.5);
        bottomCurve.append(QPointF(posX, centerY + bottomOffset));
    }

    // Create closed path for the blob
    QPainterPath blobPath;
    blobPath.moveTo(topCurve.first());

    // Draw top curve (left to right)
    for (int i = 1; i < topCurve.size(); ++i) {
        blobPath.lineTo(topCurve[i]);
    }

    // Draw bottom curve (right to left)
    for (int i = bottomCurve.size() - 1; i >= 0; --i) {
        blobPath.lineTo(bottomCurve[i]);
    }

    blobPath.closeSubpath();

    // Color based on selection (placeholder - will use track color later)
    QColor noteColor = QColor(100, 150, 255);  // Default blue
    if (!isSelected) {
        noteColor.setAlpha(180);  // Semi-transparent when not selected
    } else {
        noteColor.setAlpha(255);  // Full opacity when selected
    }

    // Draw the blob
    painter.setBrush(noteColor);
    painter.setPen(QPen(noteColor.darker(120), 1));
    painter.drawPath(blobPath);

    // Draw selection rectangle if selected
    if (isSelected) {
        drawSelectionRectangle(painter, note);
    }

    // Draw pitch indicator (small circle on left at starting pitch)
    double startPitch = note.getPitchAt(0.0);
    int startCenterY = frequencyToPixel(startPitch);
    painter.setBrush(noteColor.darker(140));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(x - 3, startCenterY - 3, 6, 6);
}

void ScoreCanvas::drawPendingNote(QPainter &painter)
{
    if (!isDrawingNote) return;

    // Calculate position and size for note being drawn
    int x1 = noteStartPos.x();
    int x2 = noteCurrentPos.x();
    int y = noteStartPos.y();

    int x = std::min(x1, x2);
    int width = std::abs(x2 - x1);
    int height = 20;

    // Draw semi-transparent preview
    painter.setBrush(QColor(100, 150, 255, 100));  // More transparent
    painter.setPen(QPen(QColor(50, 100, 200), 2, Qt::DashLine));
    painter.drawRoundedRect(x, y - height/2, width, height, 5, 5);
}

void ScoreCanvas::drawLassoRectangle(QPainter &painter)
{
    if (!isDrawingLasso) return;

    // Calculate lasso rectangle bounds
    int x = std::min(lassoStartPos.x(), lassoCurrentPos.x());
    int y = std::min(lassoStartPos.y(), lassoCurrentPos.y());
    int width = std::abs(lassoCurrentPos.x() - lassoStartPos.x());
    int height = std::abs(lassoCurrentPos.y() - lassoStartPos.y());

    // Draw selection rectangle with dashed border
    painter.setBrush(QColor(100, 150, 255, 50));  // Semi-transparent blue fill
    painter.setPen(QPen(QColor(50, 100, 200), 2, Qt::DashLine));
    painter.drawRect(x, y, width, height);
}

// ============================================================================
// Mouse Interaction
// ============================================================================

void ScoreCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // FIRST: Check if clicking on phrase (phrases have HIGHEST priority, work in ALL modes)
        int phraseIdx = findPhraseAtPosition(event->pos());
        if (phraseIdx >= 0) {
            selectPhrase(phraseIdx);

            PhraseGroup *phrasePtr = &phraseGroups[phraseIdx];

            // Check if clicking on hull resize handles (highest priority)
            DragMode hullResizeMode = detectPhraseHullResizeHandle(event->pos(), *phrasePtr);
            if (hullResizeMode != NoDrag) {
                currentDragMode = hullResizeMode;
                dragStartPos = event->pos();
                dragStartPhraseVerticalPadding = phrasePtr->getVerticalPadding();
                update();
                return;
            }

            // Check if clicking on phrase curve dot
            int dotIdx = findPhraseCurveDotAtPosition(event->pos(), *phrasePtr);
            if (dotIdx >= 0) {
                isEditingPhraseCurve = true;
                editingPhraseDotIndex = dotIdx;
                dragStartPhraseCurve = phrasePtr->getDynamicsCurve();
                dragStartPos = event->pos();
                editingPhraseDotTimePos = phrasePtr->getDynamicsCurve().getPoints()[dotIdx].time;
                update();
                return;
            }

            // Just selected the phrase, don't continue to other actions
            update();
            return;
        }

        // SECOND: Mode-specific behavior (only if NOT clicking on a phrase)
        if (currentInputMode == SelectionMode) {
            const QVector<Note> &notes = phrase.getNotes();

            // FIRST: Check if clicking on handles/dots of selected note (these are outside note rect)
            if (selectedNoteIndices.size() == 1) {
                int selectedIndex = selectedNoteIndices.first();
                if (selectedIndex < notes.size()) {
                    const Note &selectedNote = notes[selectedIndex];
                    DragMode detectedMode = detectDragMode(event->pos(), selectedNote);

                    if (detectedMode != NoDrag) {
                        // Clicking on a handle/dot - start drag operation
                        currentDragMode = detectedMode;
                        dragStartPos = event->pos();
                        dragStartTime = selectedNote.getStartTime();
                        dragStartPitch = selectedNote.getPitchHz();
                        dragStartDuration = selectedNote.getDuration();

                        // Store original curves for editing
                        if (currentDragMode == EditingTopCurve) {
                            dragStartCurve = selectedNote.getDynamicsCurve();
                        } else if (currentDragMode == EditingBottomCurve) {
                            dragStartCurve = selectedNote.getBottomCurve();
                        } else if (currentDragMode == DraggingNote && selectedNote.hasPitchCurve()) {
                            // Save pitch curve for continuous notes
                            dragStartCurve = selectedNote.getPitchCurve();
                        }

                        return;  // Don't proceed with other selection logic
                    }
                }
            }

            // SECOND: Check if clicking on a note body
            int clickedNoteIndex = findNoteAtPosition(event->pos());

            if (!selectedNoteIndices.isEmpty() && selectedNoteIndices.contains(clickedNoteIndex)) {
                // Clicking on body of a selected note - prepare for drag
                if (selectedNoteIndices.size() == 1) {
                    // Already handled above with detectDragMode
                    return;
                } else {
                    // Multi-selection - only allow dragging the group (no resize/curve edit)
                    currentDragMode = DraggingNote;
                    dragStartPos = event->pos();

                    // Store original states for all selected notes
                    multiDragStartTimes.clear();
                    multiDragStartPitches.clear();
                    multiDragStartCurves.clear();

                    for (int index : selectedNoteIndices) {
                        if (index >= 0 && index < notes.size()) {
                            const Note &note = notes[index];
                            multiDragStartTimes.append(qMakePair(index, note.getStartTime()));
                            multiDragStartPitches.append(qMakePair(index, note.getPitchHz()));
                            if (note.hasPitchCurve()) {
                                multiDragStartCurves.append(qMakePair(index, note.getPitchCurve()));
                            }
                        }
                    }

                    return;  // Don't start selecting another note
                }
            }

            // THIRD: Selecting a different note or empty space
            if (clickedNoteIndex >= 0) {
                // Clicking on a note - select it
                // If Ctrl is held, add to selection; otherwise replace selection
                bool addToSelection = (event->modifiers() & Qt::ControlModifier);
                selectNote(clickedNoteIndex, addToSelection);
                update();
            } else {
                // Clicked empty space - start lasso selection
                deselectAll();
                isDrawingLasso = true;
                lassoStartPos = event->pos();
                lassoCurrentPos = event->pos();
                update();
            }
        } else {
            // In drawing modes (discrete or continuous), start drawing a new note
            isDrawingNote = true;
            noteStartPos = event->pos();
            noteCurrentPos = event->pos();

            // In continuous mode, start tracking pitch curve
            if (currentInputMode == DrawModeContinuous) {
                pitchPoints.clear();
                double startTime = pixelToTime(event->pos().x());
                double startPitch = pixelToFrequency(event->pos().y());
                pitchPoints.append(qMakePair(startTime, startPitch));
            }

            update();
        }
    }

    // Right-click: Snap continuous note to scale
    if (event->button() == Qt::RightButton) {
        int clickedNoteIndex = findNoteAtPosition(event->pos());

        if (clickedNoteIndex >= 0) {
            QVector<Note> &notes = phrase.getNotes();
            Note &note = notes[clickedNoteIndex];

            // Only quantize notes that have pitch curves (continuous notes)
            if (note.hasPitchCurve()) {
                Curve quantizedCurve = quantizePitchCurveToScale(note.getPitchCurve());
                note.setPitchCurve(quantizedCurve);
                update();
                qDebug() << "Quantized continuous note to scale - original points:"
                         << note.getPitchCurve().getPointCount()
                         << "quantized points:" << quantizedCurve.getPointCount();
            }
        }
    }
}

void ScoreCanvas::mouseMoveEvent(QMouseEvent *event)
{
    // Emit cursor position for status bar (always update, regardless of what else is happening)
    double cursorTime = pixelToTime(event->pos().x());
    double cursorPitch = pixelToFrequency(event->pos().y());
    emit cursorPositionChanged(cursorTime, cursorPitch);

    // Handle phrase hull resizing
    if ((currentDragMode == ResizingPhraseHullTop || currentDragMode == ResizingPhraseHullBottom) && selectedPhraseIndex >= 0) {
        PhraseGroup *phrasePtr = &phraseGroups[selectedPhraseIndex];
        int deltaY = event->pos().y() - dragStartPos.y();

        // Both handles adjust padding symmetrically
        // Dragging either handle AWAY from center = increase padding (taller hull)
        // Dragging either handle TOWARD center = decrease padding (shorter hull)
        int newPadding;
        if (currentDragMode == ResizingPhraseHullTop) {
            // Dragging top edge DOWN (positive deltaY) = less padding (shorter)
            // Dragging top edge UP (negative deltaY) = more padding (taller)
            newPadding = dragStartPhraseVerticalPadding - deltaY;
        } else {
            // Dragging bottom edge DOWN (positive deltaY) = more padding (taller)
            // Dragging bottom edge UP (negative deltaY) = less padding (shorter)
            newPadding = dragStartPhraseVerticalPadding + deltaY;
        }

        newPadding = std::max(10, std::min(200, newPadding));  // Clamp 10-200 pixels
        phrasePtr->setVerticalPadding(newPadding);

        update();
        return;
    }

    // Handle phrase curve dot editing
    if (isEditingPhraseCurve && selectedPhraseIndex >= 0) {
        PhraseGroup *phrasePtr = &phraseGroups[selectedPhraseIndex];
        Curve &curve = phrasePtr->getDynamicsCurve();

        const QVector<Curve::Point> &points = dragStartPhraseCurve.getPoints();

        if (editingPhraseDotIndex >= 0 && editingPhraseDotIndex < points.size()) {
            // Get phrase time bounds for horizontal movement
            const QVector<Note> &notes = phrase.getNotes();
            double phraseStart = 1e9, phraseEnd = 0;
            for (int noteIdx : phrasePtr->getNoteIndices()) {
                if (noteIdx >= 0 && noteIdx < notes.size()) {
                    const Note &note = notes[noteIdx];
                    phraseStart = std::min(phraseStart, note.getStartTime());
                    phraseEnd = std::max(phraseEnd, note.getEndTime());
                }
            }
            double phraseDuration = phraseEnd - phraseStart;

            // Get phrase pitch bounds for vertical movement
            double minPitch = 1e9, maxPitch = 0;
            for (int noteIdx : phrasePtr->getNoteIndices()) {
                if (noteIdx >= 0 && noteIdx < notes.size()) {
                    const Note &note = notes[noteIdx];
                    minPitch = std::min(minPitch, note.getPitchHz());
                    maxPitch = std::max(maxPitch, note.getPitchHz());
                }
            }
            int padding = phrasePtr->getVerticalPadding();
            int minY = frequencyToPixel(maxPitch) - padding;
            int maxY = frequencyToPixel(minPitch) + padding;
            int hullHeight = maxY - minY;

            // Calculate new time based on horizontal movement
            double currentTime = pixelToTime(event->pos().x());
            double newNormalizedTime = (currentTime - phraseStart) / phraseDuration;
            newNormalizedTime = qBound(0.0, newNormalizedTime, 1.0);

            // Calculate new value based on vertical position
            double newValue = 1.0 - (static_cast<double>(event->pos().y() - minY) / hullHeight);
            newValue = qBound(0.0, newValue, 1.0);

            // Rebuild curve with updated control point
            curve.clearPoints();
            for (int i = 0; i < points.size(); ++i) {
                if (i == editingPhraseDotIndex) {
                    curve.addPoint(newNormalizedTime, newValue, points[i].pressure);
                } else {
                    curve.addPoint(points[i].time, points[i].value, points[i].pressure);
                }
            }

            update();
        }
        return;
    }

    // Handle dragging/resizing selected notes
    if (currentDragMode != NoDrag && !selectedNoteIndices.isEmpty()) {
        QVector<Note> &notes = phrase.getNotes();
        QPoint delta = event->pos() - dragStartPos;

        if (selectedNoteIndices.size() == 1) {
            // Single selection - full editing capabilities
            int selectedIndex = selectedNoteIndices.first();
            if (selectedIndex >= notes.size()) return;

            Note &note = notes[selectedIndex];

            switch (currentDragMode) {
            case DraggingNote: {
            // Move note in time and pitch
            double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);
            double newStartTime = dragStartTime + timeDelta;
            if (newStartTime < 0.0) newStartTime = 0.0;  // Don't go negative

            note.setStartTime(newStartTime);

            // Handle pitch change based on whether note has pitch curve
            if (note.hasPitchCurve()) {
                // Continuous note with pitch curve: shift entire curve by pixel delta
                Curve shiftedPitchCurve;
                const Curve& originalCurve = dragStartCurve;  // Use curve saved at drag start
                for (const auto& point : originalCurve.getPoints()) {
                    double t = point.time;
                    double originalPitch = point.value;
                    // Convert to pixel, apply delta, convert back
                    int originalPixelY = frequencyToPixel(originalPitch);
                    int newPixelY = originalPixelY + delta.y();
                    double newPitch = pixelToFrequency(newPixelY);
                    shiftedPitchCurve.addPoint(t, newPitch);
                }
                note.setPitchCurve(shiftedPitchCurve);
            } else {
                // Discrete note: snap to scale lines
                double rawPitch = pixelToFrequency(dragStartPos.y() + delta.y());
                double snappedPitch = snapToNearestScaleLine(rawPitch);
                note.setPitchHz(snappedPitch);
            }
            break;
        }

        case ResizingLeft: {
            // Resize from left (change start time and duration)
            double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);
            double newStartTime = dragStartTime + timeDelta;
            double newDuration = dragStartDuration - timeDelta;

            // Constrain minimum duration
            if (newDuration < 100.0) {  // Minimum 100ms
                newDuration = 100.0;
                newStartTime = dragStartTime + dragStartDuration - 100.0;
            }
            if (newStartTime < 0.0) newStartTime = 0.0;

            note.setStartTime(newStartTime);
            note.setDuration(newDuration);
            break;
        }

        case ResizingRight: {
            // Resize from right (change duration only)
            double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);
            double newDuration = dragStartDuration + timeDelta;

            // Constrain minimum duration
            if (newDuration < 100.0) newDuration = 100.0;

            note.setDuration(newDuration);
            break;
        }

        case EditingTopCurve: {
            // Edit dynamics curve by dragging dot vertically
            // Calculate dynamics change based on vertical drag
            // Use a fixed scale so dragging feels consistent
            double dynamicsChange = -delta.y() / 50.0;  // 50 pixels = 1.0 change

            // Get original value at this point from when drag started
            double originalValue = dragStartCurve.valueAt(editingDotTimePos);
            double newValue = qBound(0.0, originalValue + dynamicsChange, 1.0);

            // Get current curve
            Curve &dynamicsCurve = note.getDynamicsCurve();
            dynamicsCurve.clearPoints();

            // Rebuild curve with adaptive number of control points
            int numDots = calculateCurveDotCount(note);
            for (int i = 0; i <= numDots + 1; ++i) {
                double t = i / (numDots + 1.0);
                double value;

                if (i == editingDotIndex + 1) {
                    // This is the dot being edited - use new value
                    value = newValue;
                } else {
                    // Keep original value from dragStartCurve
                    value = dragStartCurve.valueAt(t);
                }

                dynamicsCurve.addPoint(t, value);
            }

            dynamicsCurve.sortPoints();
            break;
        }

        case EditingBottomCurve: {
            // Edit bottom curve by dragging dot vertically
            double curveChange = delta.y() / 50.0;  // Positive because down = more for bottom

            double originalValue = dragStartCurve.valueAt(editingDotTimePos);
            double newValue = qBound(0.0, originalValue + curveChange, 1.0);

            // Get current bottom curve
            Curve &bottomCurve = note.getBottomCurve();
            bottomCurve.clearPoints();

            // Rebuild curve with adaptive number of control points
            int numDots = calculateCurveDotCount(note);
            for (int i = 0; i <= numDots + 1; ++i) {
                double t = i / (numDots + 1.0);
                double value;

                if (i == editingDotIndex + 1) {
                    value = newValue;
                } else {
                    value = dragStartCurve.valueAt(t);
                }

                bottomCurve.addPoint(t, value);
            }

            bottomCurve.sortPoints();
            break;
        }

            case NoDrag:
                break;
            }
        } else {
            // Multi-selection - only allow dragging the group
            if (currentDragMode == DraggingNote) {
                // Calculate time delta from original position
                double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);

                // Apply time deltas to all selected notes
                for (int i = 0; i < multiDragStartTimes.size(); ++i) {
                    int index = multiDragStartTimes[i].first;
                    double originalTime = multiDragStartTimes[i].second;

                    if (index >= 0 && index < notes.size()) {
                        Note &note = notes[index];

                        // Apply time movement from original position
                        double newStartTime = originalTime + timeDelta;
                        if (newStartTime < 0.0) newStartTime = 0.0;
                        note.setStartTime(newStartTime);
                    }
                }

                // Apply pitch movement - each note moves by same PIXEL delta
                for (int i = 0; i < multiDragStartPitches.size(); ++i) {
                    int index = multiDragStartPitches[i].first;
                    double originalPitch = multiDragStartPitches[i].second;

                    if (index >= 0 && index < notes.size()) {
                        Note &note = notes[index];

                        if (!note.hasPitchCurve()) {
                            // Convert original pitch to pixel Y
                            int originalPixelY = frequencyToPixel(originalPitch);
                            // Apply pixel delta
                            int newPixelY = originalPixelY + delta.y();
                            // Convert back to frequency
                            double newPitch = pixelToFrequency(newPixelY);
                            note.setPitchHz(newPitch);
                        }
                    }
                }

                // Apply pitch curve shifts - each curve point moves by same PIXEL delta
                for (int i = 0; i < multiDragStartCurves.size(); ++i) {
                    int index = multiDragStartCurves[i].first;
                    const Curve &originalCurve = multiDragStartCurves[i].second;

                    if (index >= 0 && index < notes.size()) {
                        Note &note = notes[index];

                        // Shift pitch curve by pixel delta
                        Curve newPitchCurve;
                        for (const auto& point : originalCurve.getPoints()) {
                            // Convert original pitch to pixel Y
                            int originalPixelY = frequencyToPixel(point.value);
                            // Apply pixel delta
                            int newPixelY = originalPixelY + delta.y();
                            // Convert back to frequency
                            double newPitch = pixelToFrequency(newPixelY);
                            newPitchCurve.addPoint(point.time, newPitch);
                        }
                        note.setPitchCurve(newPitchCurve);
                    }
                }
            }
        }

        update();
        return;
    }

    // Update cursor based on hover position over selected note (only for single selection)
    if (selectedNoteIndices.size() == 1 && currentDragMode == NoDrag && !isDrawingNote) {
        const QVector<Note> &notes = phrase.getNotes();
        int selectedIndex = selectedNoteIndices.first();
        if (selectedIndex < notes.size()) {
            const Note &selectedNote = notes[selectedIndex];
            DragMode hoverMode = detectDragMode(event->pos(), selectedNote);

            switch (hoverMode) {
            case ResizingLeft:
            case ResizingRight:
                setCursor(Qt::SizeHorCursor);  // Horizontal resize cursor
                break;
            case EditingTopCurve:
            case EditingBottomCurve:
                setCursor(Qt::SizeVerCursor);  // Vertical resize cursor for curve editing
                break;
            case DraggingNote:
                setCursor(Qt::SizeAllCursor);  // Move cursor
                break;
            case NoDrag:
                setCursor(Qt::ArrowCursor);  // Default cursor
                break;
            }
        }
    } else if (!isDrawingNote) {
        setCursor(Qt::ArrowCursor);  // Default cursor when not over note
    }

    // Handle lasso selection dragging
    if (isDrawingLasso) {
        lassoCurrentPos = event->pos();
        update();
        return;
    }

    // Handle drawing new note
    if (isDrawingNote) {
        noteCurrentPos = event->pos();

        // In continuous mode, track pitch changes
        if (currentInputMode == DrawModeContinuous) {
            double currentTime = pixelToTime(event->pos().x());
            double currentPitch = pixelToFrequency(event->pos().y());
            pitchPoints.append(qMakePair(currentTime, currentPitch));
        }

        update();
    }
}

void ScoreCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    // End phrase hull resizing
    if (event->button() == Qt::LeftButton && (currentDragMode == ResizingPhraseHullTop || currentDragMode == ResizingPhraseHullBottom)) {
        currentDragMode = NoDrag;
        update();
        qDebug() << "Phrase hull resize complete - new padding:" << phraseGroups[selectedPhraseIndex].getVerticalPadding();
        return;
    }

    // End phrase curve editing
    if (event->button() == Qt::LeftButton && isEditingPhraseCurve && selectedPhraseIndex >= 0) {
        PhraseGroup *phrase = &phraseGroups[selectedPhraseIndex];
        Curve newCurve = phrase->getDynamicsCurve();

        // Create undo command
        undoStack->push(new EditPhraseCurveCommand(
            this, selectedPhraseIndex,
            EditPhraseCurveCommand::DynamicsCurve,
            dragStartPhraseCurve, newCurve
        ));

        isEditingPhraseCurve = false;
        editingPhraseDotIndex = -1;
        update();
        return;
    }

    // End drag/resize operation
    if (event->button() == Qt::LeftButton && currentDragMode != NoDrag) {
        if (selectedNoteIndices.size() == 1) {
            // Single selection - full editing capabilities
            int selectedIndex = selectedNoteIndices.first();
            if (selectedIndex >= 0 && selectedIndex < phrase.getNotes().size()) {
                const Note &note = phrase.getNotes()[selectedIndex];

                switch (currentDragMode) {
                case DraggingNote: {
                    // Push move command with old and new states
                    double newStartTime = note.getStartTime();
                    double newPitch = note.getPitchHz();
                    Curve newPitchCurve;
                    bool hasCurve = note.hasPitchCurve();
                    if (hasCurve) {
                        newPitchCurve = note.getPitchCurve();
                    }

                    undoStack->push(new MoveNoteCommand(&phrase, selectedIndex,
                                                        dragStartTime, dragStartPitch,
                                                        newStartTime, newPitch,
                                                        dragStartCurve, newPitchCurve,
                                                        hasCurve, this));
                    break;
                }

                case ResizingLeft:
                case ResizingRight: {
                    // Push resize command
                    double newStartTime = note.getStartTime();
                    double newDuration = note.getDuration();

                    undoStack->push(new ResizeNoteCommand(&phrase, selectedIndex,
                                                          dragStartTime, dragStartDuration,
                                                          newStartTime, newDuration,
                                                          this));
                    break;
                }

                case EditingTopCurve: {
                    // Push curve edit command for dynamics
                    Curve newCurve = note.getDynamicsCurve();
                    undoStack->push(new EditCurveCommand(&phrase, selectedIndex,
                                                         EditCurveCommand::DynamicsCurve,
                                                         dragStartCurve, newCurve,
                                                         this));
                    break;
                }

                case EditingBottomCurve: {
                    // Push curve edit command for bottom curve
                    Curve newCurve = note.getBottomCurve();
                    undoStack->push(new EditCurveCommand(&phrase, selectedIndex,
                                                         EditCurveCommand::BottomCurve,
                                                         dragStartCurve, newCurve,
                                                         this));
                    break;
                }

                case NoDrag:
                    break;
                }
            }
        } else if (selectedNoteIndices.size() > 1 && currentDragMode == DraggingNote) {
            // Multi-selection drag - create batch move command
            QVector<MoveMultipleNotesCommand::NoteState> oldStates;
            QVector<MoveMultipleNotesCommand::NoteState> newStates;

            const QVector<Note> &notes = phrase.getNotes();

            // Build old states from stored drag start data
            for (int i = 0; i < multiDragStartTimes.size(); ++i) {
                int index = multiDragStartTimes[i].first;
                MoveMultipleNotesCommand::NoteState state;
                state.index = index;
                state.startTime = multiDragStartTimes[i].second;

                // Find matching pitch
                for (const auto &pair : multiDragStartPitches) {
                    if (pair.first == index) {
                        state.pitch = pair.second;
                        break;
                    }
                }

                // Find matching curve if it exists
                state.hasPitchCurve = false;
                for (const auto &pair : multiDragStartCurves) {
                    if (pair.first == index) {
                        state.pitchCurve = pair.second;
                        state.hasPitchCurve = true;
                        break;
                    }
                }

                oldStates.append(state);
            }

            // Build new states from current note positions
            for (int index : selectedNoteIndices) {
                if (index >= 0 && index < notes.size()) {
                    const Note &note = notes[index];
                    MoveMultipleNotesCommand::NoteState state;
                    state.index = index;
                    state.startTime = note.getStartTime();
                    state.pitch = note.getPitchHz();
                    state.hasPitchCurve = note.hasPitchCurve();
                    if (state.hasPitchCurve) {
                        state.pitchCurve = note.getPitchCurve();
                    }
                    newStates.append(state);
                }
            }

            undoStack->push(new MoveMultipleNotesCommand(&phrase, selectedNoteIndices,
                                                         oldStates, newStates, this));
        }

        currentDragMode = NoDrag;
        update();
        return;
    }

    // Complete lasso selection
    if (event->button() == Qt::LeftButton && isDrawingLasso) {
        isDrawingLasso = false;

        // Calculate lasso rectangle
        int x = std::min(lassoStartPos.x(), lassoCurrentPos.x());
        int y = std::min(lassoStartPos.y(), lassoCurrentPos.y());
        int width = std::abs(lassoCurrentPos.x() - lassoStartPos.x());
        int height = std::abs(lassoCurrentPos.y() - lassoStartPos.y());
        QRect lassoRect(x, y, width, height);

        // Find all notes within the lasso rectangle
        QVector<int> notesInRect = findNotesInRectangle(lassoRect);
        selectNotes(notesInRect);

        update();
        return;
    }

    if (event->button() == Qt::LeftButton && isDrawingNote) {
        isDrawingNote = false;

        // Calculate note parameters
        double startTime = pixelToTime(std::min(noteStartPos.x(), noteCurrentPos.x()));
        double endTime = pixelToTime(std::max(noteStartPos.x(), noteCurrentPos.x()));
        double duration = endTime - startTime;

        // Minimum duration threshold (avoid tiny notes from clicks)
        if (duration < 50.0) {  // Minimum 50ms
            // This was a click, not a drag
            // In drawing modes, create a default duration note
            // (Selection is handled in mousePressEvent for Selection mode)
            duration = 500.0;   // Default to 500ms
            endTime = startTime + duration;
        }

        // Deselect when creating a new note
        deselectAll();

        // Get pitch from vertical position
        double rawPitch = pixelToFrequency(noteStartPos.y());
        double finalPitch;

        // Apply pitch snapping based on input mode
        if (currentInputMode == DrawModeDiscrete) {
            // Discrete mode: snap to nearest scale line
            finalPitch = snapToNearestScaleLine(rawPitch);
        } else {
            // Continuous mode: use literal pitch (no snapping)
            finalPitch = rawPitch;
        }

        // Create note with default dynamics first
        Note newNote(startTime, duration, finalPitch, 0.7);

        // Set the note to use the active track
        newNote.setTrackIndex(activeTrackIndex);

        // Create a default bottom curve with some variation (for visual interest)
        Curve defaultBottomCurve;
        defaultBottomCurve.addPoint(0.0, 0.5);
        defaultBottomCurve.addPoint(0.25, 0.6);
        defaultBottomCurve.addPoint(0.5, 0.65);
        defaultBottomCurve.addPoint(0.75, 0.6);
        defaultBottomCurve.addPoint(1.0, 0.55);
        newNote.setBottomCurve(defaultBottomCurve);

        // If we have captured pressure points from tablet, create a dynamics curve
        if (usingTablet && pressurePoints.size() >= 2) {
            Curve dynamicsCurve;

            // Convert absolute time points to normalized time (0.0-1.0 within the note)
            for (const auto &point : pressurePoints) {
                double absoluteTime = point.first;
                double pressure = point.second;

                // Normalize time within the note duration
                double normalizedTime = (absoluteTime - startTime) / duration;
                normalizedTime = qBound(0.0, normalizedTime, 1.0);

                dynamicsCurve.addPoint(normalizedTime, pressure);
            }

            // Sort points by time to ensure proper interpolation
            dynamicsCurve.sortPoints();

            // Set the curve on the note
            newNote.setDynamicsCurve(dynamicsCurve);
        }

        // If in continuous mode and we have pitch points, create a pitch curve for glissando
        if (currentInputMode == DrawModeContinuous && pitchPoints.size() >= 2) {
            Curve pitchCurve;

            // Convert absolute time points to normalized time (0.0-1.0 within the note)
            for (const auto &point : pitchPoints) {
                double absoluteTime = point.first;
                double pitchHz = point.second;

                // Normalize time within the note duration
                double normalizedTime = (absoluteTime - startTime) / duration;
                normalizedTime = qBound(0.0, normalizedTime, 1.0);

                pitchCurve.addPoint(normalizedTime, pitchHz);
            }

            // Sort points by time to ensure proper interpolation
            pitchCurve.sortPoints();

            // Set the pitch curve on the note
            newNote.setPitchCurve(pitchCurve);
        }

        // If in continuous mode and we have pressure points, create a dynamics curve
        if (currentInputMode == DrawModeContinuous && pressurePoints.size() >= 2) {
            Curve dynamicsCurve;

            // Convert absolute time/pressure points to normalized time (0.0-1.0 within the note)
            for (const auto &point : pressurePoints) {
                double absoluteTime = point.first;
                double pressure = point.second;

                // Normalize time within the note duration
                double normalizedTime = (absoluteTime - startTime) / duration;
                normalizedTime = qBound(0.0, normalizedTime, 1.0);

                // Use pressure as dynamics (already in 0.0-1.0 range)
                dynamicsCurve.addPoint(normalizedTime, pressure);
            }

            // Sort points by time to ensure proper interpolation
            dynamicsCurve.sortPoints();

            // Set the dynamics curve on the note
            newNote.setDynamicsCurve(dynamicsCurve);
        }

        // Use undo command to add note
        undoStack->push(new AddNoteCommand(&phrase, newNote, this));

        // Reset state for next input
        usingTablet = false;
        pressurePoints.clear();
        pitchPoints.clear();

        update();
    }
}

// ============================================================================
// Selection Rectangle
// ============================================================================

void ScoreCanvas::drawSelectionRectangle(QPainter &painter, const Note &note)
{
    // Calculate bounding rectangle for the note
    int x = timeToPixel(note.getStartTime());
    int width = timeToPixel(note.getEndTime()) - x;

    // Find maximum dynamics value in curve for blob height (same as drawNote)
    const Curve &dynamicsCurve = note.getDynamicsCurve();
    double maxDynamics = 0.0;
    for (int i = 0; i <= 20; ++i) {
        double t = i / 20.0;
        double value = dynamicsCurve.valueAt(t);
        if (value > maxDynamics) maxDynamics = value;
    }

    int blobHeight = static_cast<int>(20 + maxDynamics * 60);  // Match drawNote calculation

    // Find min and max pitch across the note (for glissando notes)
    double minPitch = note.getPitchAt(0.0);
    double maxPitch = minPitch;
    for (int i = 0; i <= 20; ++i) {
        double t = i / 20.0;
        double pitch = note.getPitchAt(t);
        if (pitch < minPitch) minPitch = pitch;
        if (pitch > maxPitch) maxPitch = pitch;
    }

    // Calculate vertical bounds
    int topY = frequencyToPixel(maxPitch) - blobHeight/2 - 5;
    int bottomY = frequencyToPixel(minPitch) + blobHeight/2 + 5;
    int height = bottomY - topY;

    // Draw selection rectangle (gray outline)
    QRect selectionRect(x - 5, topY, width + 10, height);
    painter.setPen(QPen(QColor(128, 128, 128), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(selectionRect);

    // Draw manipulation dots (round circles)
    painter.setBrush(QColor(200, 200, 200));
    painter.setPen(QPen(QColor(100, 100, 100), 1));

    int dotSize = 8;

    // Calculate adaptive number of dots based on note width
    int numDots = calculateCurveDotCount(note);
    int numSegments = numDots + 1;  // e.g., 4 dots = 5 segments

    // Top edge dots (adaptive count for shaping amplitude curve)
    for (int i = 1; i <= numDots; ++i) {
        int dotX = selectionRect.left() + (selectionRect.width() * i / numSegments);
        painter.drawEllipse(dotX - dotSize/2, selectionRect.top() - dotSize/2, dotSize, dotSize);
    }

    // Bottom edge dots (adaptive count for shaping spectrum curve)
    for (int i = 1; i <= numDots; ++i) {
        int dotX = selectionRect.left() + (selectionRect.width() * i / numSegments);
        painter.drawEllipse(dotX - dotSize/2, selectionRect.bottom() - dotSize/2, dotSize, dotSize);
    }

    // Left edge dots (3 dots - middle is square for resize)
    int leftX = selectionRect.left();
    for (int i = 1; i <= 3; ++i) {
        int dotY = selectionRect.top() + (selectionRect.height() * i / 4);
        if (i == 2) {
            // Middle dot is square (resize handle)
            painter.setBrush(QColor(255, 200, 100));  // Orange for resize
            painter.drawRect(leftX - dotSize/2, dotY - dotSize/2, dotSize, dotSize);
            painter.setBrush(QColor(200, 200, 200));  // Reset to gray
        } else {
            painter.drawEllipse(leftX - dotSize/2, dotY - dotSize/2, dotSize, dotSize);
        }
    }

    // Right edge dots (3 dots - middle is square for resize)
    int rightX = selectionRect.right();
    for (int i = 1; i <= 3; ++i) {
        int dotY = selectionRect.top() + (selectionRect.height() * i / 4);
        if (i == 2) {
            // Middle dot is square (resize handle)
            painter.setBrush(QColor(255, 200, 100));  // Orange for resize
            painter.drawRect(rightX - dotSize/2, dotY - dotSize/2, dotSize, dotSize);
            painter.setBrush(QColor(200, 200, 200));  // Reset to gray
        } else {
            painter.drawEllipse(rightX - dotSize/2, dotY - dotSize/2, dotSize, dotSize);
        }
    }
}

// ============================================================================
// Selection Helpers
// ============================================================================

int ScoreCanvas::findNoteAtPosition(const QPoint &pos) const
{
    const QVector<Note> &notes = phrase.getNotes();

    // Search in reverse order (topmost/newest notes first)
    for (int i = notes.size() - 1; i >= 0; --i) {
        const Note &note = notes[i];

        int x = timeToPixel(note.getStartTime());
        int width = timeToPixel(note.getEndTime()) - x;

        // Find maximum dynamics value in curve for blob height
        const Curve &dynamicsCurve = note.getDynamicsCurve();
        double maxDynamics = 0.0;
        for (int j = 0; j <= 20; ++j) {
            double t = j / 20.0;
            double value = dynamicsCurve.valueAt(t);
            if (value > maxDynamics) maxDynamics = value;
        }

        int blobHeight = static_cast<int>(20 + maxDynamics * 60);

        // Find min and max pitch across the note (for glissando notes)
        double minPitch = note.getPitchAt(0.0);
        double maxPitch = minPitch;
        for (int j = 0; j <= 20; ++j) {
            double t = j / 20.0;
            double pitch = note.getPitchAt(t);
            if (pitch < minPitch) minPitch = pitch;
            if (pitch > maxPitch) maxPitch = pitch;
        }

        // Calculate vertical bounds
        int topY = frequencyToPixel(maxPitch) - blobHeight/2;
        int bottomY = frequencyToPixel(minPitch) + blobHeight/2;
        int height = bottomY - topY;

        // Simple bounding box hit test
        QRect noteRect(x, topY, width, height);
        if (noteRect.contains(pos)) {
            return i;
        }
    }

    return -1;  // No note found
}

QVector<int> ScoreCanvas::findNotesInRectangle(const QRect &rect) const
{
    QVector<int> foundIndices;
    const QVector<Note> &notes = phrase.getNotes();

    for (int i = 0; i < notes.size(); ++i) {
        const Note &note = notes[i];
        QRect noteRect = getNoteRect(note);

        // Check if note rectangle intersects with selection rectangle
        if (noteRect.intersects(rect)) {
            foundIndices.append(i);
        }
    }

    return foundIndices;
}

void ScoreCanvas::selectNote(int index, bool addToSelection)
{
    const QVector<Note> &notes = phrase.getNotes();
    if (index >= 0 && index < notes.size()) {
        if (!addToSelection) {
            selectedNoteIndices.clear();
        }
        if (!selectedNoteIndices.contains(index)) {
            selectedNoteIndices.append(index);
        }
    }
}

void ScoreCanvas::selectNotes(const QVector<int> &indices)
{
    selectedNoteIndices = indices;
}

void ScoreCanvas::deselectAll()
{
    selectedNoteIndices.clear();
}

bool ScoreCanvas::isNoteSelected(int index) const
{
    return selectedNoteIndices.contains(index);
}

// ============================================================================
// Drag Helpers
// ============================================================================

QRect ScoreCanvas::getNoteRect(const Note &note) const
{
    int x = timeToPixel(note.getStartTime());
    int width = timeToPixel(note.getEndTime()) - x;

    // Find maximum dynamics value in curve for blob height
    const Curve &dynamicsCurve = note.getDynamicsCurve();
    double maxDynamics = 0.0;
    for (int i = 0; i <= 20; ++i) {
        double t = i / 20.0;
        double value = dynamicsCurve.valueAt(t);
        if (value > maxDynamics) maxDynamics = value;
    }

    int blobHeight = static_cast<int>(20 + maxDynamics * 60);

    // Find min and max pitch across the note (for glissando notes)
    double minPitch = note.getPitchAt(0.0);
    double maxPitch = minPitch;
    for (int i = 0; i <= 20; ++i) {
        double t = i / 20.0;
        double pitch = note.getPitchAt(t);
        if (pitch < minPitch) minPitch = pitch;
        if (pitch > maxPitch) maxPitch = pitch;
    }

    // Calculate vertical bounds
    int topY = frequencyToPixel(maxPitch) - blobHeight/2 - 5;
    int bottomY = frequencyToPixel(minPitch) + blobHeight/2 + 5;
    int height = bottomY - topY;

    return QRect(x - 5, topY, width + 10, height);
}

QRect ScoreCanvas::getLeftResizeHandle(const Note &note) const
{
    QRect noteRect = getNoteRect(note);
    int handleSize = 16;
    // Center the handle vertically on the left edge
    return QRect(noteRect.left() - handleSize/2,
                 noteRect.center().y() - handleSize/2,
                 handleSize, handleSize);
}

QRect ScoreCanvas::getRightResizeHandle(const Note &note) const
{
    QRect noteRect = getNoteRect(note);
    int handleSize = 16;
    // Center the handle vertically on the right edge
    return QRect(noteRect.right() - handleSize/2,
                 noteRect.center().y() - handleSize/2,
                 handleSize, handleSize);
}

int ScoreCanvas::calculateCurveDotCount(const Note &note) const
{
    int width = timeToPixel(note.getEndTime()) - timeToPixel(note.getStartTime());

    // Adaptive dot count based on note width
    // One dot every ~75 pixels, minimum 4, maximum 12
    int dotCount = std::max(4, std::min(12, width / 75));

    return dotCount;
}

Curve ScoreCanvas::quantizePitchCurveToScale(const Curve &pitchCurve) const
{
    // Quantize a continuous pitch curve to snap to scale degrees
    // Uses hysteresis: only switches to a new pitch when crossing halfway between scale degrees

    if (pitchCurve.isEmpty() || scaleLines.isEmpty()) {
        return pitchCurve;
    }

    Curve quantizedCurve;

    // Sample the curve at many points to detect all transitions
    const int sampleCount = 500;  // High resolution for accurate transition detection
    double currentScalePitch = 0.0;  // Track the current snapped pitch
    double previousTime = 0.0;

    for (int i = 0; i < sampleCount; ++i) {
        double t = i / static_cast<double>(sampleCount - 1);  // Normalized time 0.0-1.0
        double rawPitch = pitchCurve.valueAt(t);

        if (i == 0) {
            // Initialize to nearest scale line
            currentScalePitch = snapToNearestScaleLine(rawPitch);
            quantizedCurve.addPoint(0.0, currentScalePitch);
            previousTime = 0.0;
            continue;
        }

        // Find the two nearest scale lines to the current snapped pitch
        double lowerScalePitch = 0.0;
        double upperScalePitch = 0.0;
        bool foundLower = false;
        bool foundUpper = false;

        for (const ScaleLine &line : scaleLines) {
            if (line.frequencyHz < currentScalePitch) {
                if (!foundLower || line.frequencyHz > lowerScalePitch) {
                    lowerScalePitch = line.frequencyHz;
                    foundLower = true;
                }
            } else if (line.frequencyHz > currentScalePitch) {
                if (!foundUpper || line.frequencyHz < upperScalePitch) {
                    upperScalePitch = line.frequencyHz;
                    foundUpper = true;
                }
            }
        }

        // Calculate threshold for switching (halfway point between current and adjacent scale lines)
        bool shouldSwitch = false;
        double newScalePitch = currentScalePitch;

        if (rawPitch > currentScalePitch && foundUpper) {
            // Moving upward - check if we've crossed halfway to upper scale line
            double threshold = (currentScalePitch + upperScalePitch) / 2.0;
            if (rawPitch > threshold) {
                shouldSwitch = true;
                newScalePitch = upperScalePitch;
            }
        } else if (rawPitch < currentScalePitch && foundLower) {
            // Moving downward - check if we've crossed halfway to lower scale line
            double threshold = (currentScalePitch + lowerScalePitch) / 2.0;
            if (rawPitch < threshold) {
                shouldSwitch = true;
                newScalePitch = lowerScalePitch;
            }
        }

        // When switching to a new pitch, create a clean step transition
        if (shouldSwitch) {
            // Add a point just before the transition at the old pitch (creates horizontal line)
            double transitionTime = (previousTime + t) / 2.0;  // Midpoint for clean transition
            quantizedCurve.addPoint(transitionTime - 0.001, currentScalePitch);
            // Add a point at the transition at the new pitch (creates vertical step)
            quantizedCurve.addPoint(transitionTime, newScalePitch);

            currentScalePitch = newScalePitch;
        }

        previousTime = t;
    }

    // Ensure curve ends at 1.0 with the final pitch
    quantizedCurve.addPoint(1.0, currentScalePitch);

    qDebug() << "Quantized curve points:" << quantizedCurve.getPointCount();

    return quantizedCurve;
}

ScoreCanvas::DragMode ScoreCanvas::detectDragMode(const QPoint &pos, const Note &note) const
{
    QRect noteRect = getNoteRect(note);
    int dotSize = 8;
    int clickTolerance = 12;  // Larger tolerance for easier clicking

    int numDots = calculateCurveDotCount(note);
    int numSegments = numDots + 1;  // e.g., 4 dots = 5 segments

    // Check top edge dots (for editing dynamics curve)
    int topY = noteRect.top();
    for (int i = 1; i <= numDots; ++i) {
        int dotX = noteRect.left() + (noteRect.width() * i / numSegments);
        QRect dotRect(dotX - clickTolerance/2, topY - clickTolerance/2,
                     clickTolerance, clickTolerance);
        if (dotRect.contains(pos)) {
            // Store which dot was clicked (will be used in press event)
            const_cast<ScoreCanvas*>(this)->editingDotIndex = i - 1;  // 0-based index
            const_cast<ScoreCanvas*>(this)->editingDotTimePos = static_cast<double>(i) / numSegments;  // Normalized time
            return EditingTopCurve;
        }
    }

    // Check bottom edge dots (for editing bottom curve)
    int bottomY = noteRect.bottom();
    for (int i = 1; i <= numDots; ++i) {
        int dotX = noteRect.left() + (noteRect.width() * i / numSegments);
        QRect dotRect(dotX - clickTolerance/2, bottomY - clickTolerance/2,
                     clickTolerance, clickTolerance);
        if (dotRect.contains(pos)) {
            // Store which dot was clicked
            const_cast<ScoreCanvas*>(this)->editingDotIndex = i - 1;  // 0-based index
            const_cast<ScoreCanvas*>(this)->editingDotTimePos = static_cast<double>(i) / numSegments;  // Normalized time
            return EditingBottomCurve;
        }
    }

    // Check resize handles (higher priority than note body)
    if (getLeftResizeHandle(note).contains(pos)) {
        return ResizingLeft;
    }
    if (getRightResizeHandle(note).contains(pos)) {
        return ResizingRight;
    }

    // Check if clicking on note body
    if (noteRect.contains(pos)) {
        return DraggingNote;
    }

    return NoDrag;
}

// ============================================================================
// Pen Tablet Input
// ============================================================================

void ScoreCanvas::tabletEvent(QTabletEvent *event)
{
    // Mark that we're using tablet input
    usingTablet = true;

    // Capture pen pressure (0.0-1.0)
    penPressure = event->pressure();

    // Get position
    QPointF pos = event->position();

    // Check if using eraser - delete notes on contact
    if (event->pointerType() == QPointingDevice::PointerType::Eraser) {
        if (event->type() == QEvent::TabletPress || event->type() == QEvent::TabletMove) {
            int noteIndex = findNoteAtPosition(pos.toPoint());
            if (noteIndex >= 0) {
                // Use undo command to delete the note under the eraser
                undoStack->push(new DeleteNoteCommand(&phrase, noteIndex, this));

                // Deselect if the deleted note was selected
                if (selectedNoteIndices.contains(noteIndex)) {
                    selectedNoteIndices.removeAll(noteIndex);
                }

                // Adjust selection indices if a note before them was deleted
                for (int &idx : selectedNoteIndices) {
                    if (idx > noteIndex) {
                        idx--;
                    }
                }

                update();
            }
        }
        event->accept();
        return;  // Don't process as normal pen input
    }

    switch (event->type()) {
    case QEvent::TabletPress:
        // FIRST: Check if pressing on ANY phrase (phrases have HIGHEST priority, work in ALL modes)
        {
            int phraseIdx = findPhraseAtPosition(pos.toPoint());
            if (phraseIdx >= 0) {
                selectPhrase(phraseIdx);
                PhraseGroup *phrasePtr = &phraseGroups[phraseIdx];

                // Check if clicking on hull resize handles (highest priority)
                DragMode hullResizeMode = detectPhraseHullResizeHandle(pos.toPoint(), *phrasePtr);
                if (hullResizeMode != NoDrag) {
                    currentDragMode = hullResizeMode;
                    dragStartPos = pos.toPoint();
                    dragStartPhraseVerticalPadding = phrasePtr->getVerticalPadding();
                    event->accept();
                    return;
                }

                // Check if clicking on existing curve dot
                int dotIdx = findPhraseCurveDotAtPosition(pos.toPoint(), *phrasePtr);
                if (dotIdx >= 0) {
                    isEditingPhraseCurve = true;
                    editingPhraseDotIndex = dotIdx;
                    dragStartPhraseCurve = phrasePtr->getDynamicsCurve();
                    dragStartPos = pos.toPoint();
                    editingPhraseDotTimePos = phrasePtr->getDynamicsCurve().getPoints()[dotIdx].time;
                    event->accept();
                    return;
                }

                // Not clicking on handle or dot - start drawing gesture curve!
                isDrawingPhraseCurve = true;
                    phraseCurveGesturePoints.clear();
                    phraseCurveGesturePressures.clear();

                    // Calculate phrase hull bounds for normalization
                    const QVector<Note> &notes = phrase.getNotes();
                    double minPitch = 1e9, maxPitch = 0;
                    for (int noteIdx : phrasePtr->getNoteIndices()) {
                        if (noteIdx >= 0 && noteIdx < notes.size()) {
                            const Note &note = notes[noteIdx];
                            minPitch = std::min(minPitch, note.getPitchHz());
                            maxPitch = std::max(maxPitch, note.getPitchHz());
                        }
                    }
                    int padding = phrasePtr->getVerticalPadding();
                    phraseCurveGestureMinY = frequencyToPixel(maxPitch) - padding;
                    phraseCurveGestureMaxY = frequencyToPixel(minPitch) + padding;
                    int hullHeight = phraseCurveGestureMaxY - phraseCurveGestureMinY;

                    // Capture first point
                    double time = pixelToTime(pos.x());
                    // Normalize Y position within hull bounds (value=1.0 at top, 0.0 at bottom)
                    double normalizedValue = 1.0 - (static_cast<double>(pos.y() - phraseCurveGestureMinY) / hullHeight);
                    normalizedValue = std::max(0.0, std::min(1.0, normalizedValue));

                    phraseCurveGesturePoints.append(qMakePair(time, normalizedValue));
                    phraseCurveGesturePressures.append(penPressure);

                    emit pressureChanged(penPressure, true);
                    event->accept();
                    return;
                }
            }

        // SECOND: Mode-specific behavior (only if NOT clicking on a phrase)
        if (currentInputMode == SelectionMode) {
            const QVector<Note> &notes = phrase.getNotes();

            // FIRST: Check if clicking on handles/dots of selected note (these are outside note rect)
            if (selectedNoteIndices.size() == 1) {
                int selectedIndex = selectedNoteIndices.first();
                if (selectedIndex < notes.size()) {
                    const Note &selectedNote = notes[selectedIndex];
                    DragMode detectedMode = detectDragMode(pos.toPoint(), selectedNote);

                    if (detectedMode != NoDrag) {
                        // Clicking on a handle/dot - start drag operation
                        currentDragMode = detectedMode;
                        dragStartPos = pos.toPoint();
                        dragStartTime = selectedNote.getStartTime();
                        dragStartPitch = selectedNote.getPitchHz();
                        dragStartDuration = selectedNote.getDuration();

                        // Store original curves for editing
                        if (currentDragMode == EditingTopCurve) {
                            dragStartCurve = selectedNote.getDynamicsCurve();
                        } else if (currentDragMode == EditingBottomCurve) {
                            dragStartCurve = selectedNote.getBottomCurve();
                        } else if (currentDragMode == DraggingNote && selectedNote.hasPitchCurve()) {
                            // Save pitch curve for continuous notes
                            dragStartCurve = selectedNote.getPitchCurve();
                        }

                        event->accept();
                        return;  // Don't proceed with other selection logic
                    }
                }
            }

            // SECOND: Check if clicking on a note body
            int clickedNoteIndex = findNoteAtPosition(pos.toPoint());

            if (!selectedNoteIndices.isEmpty() && selectedNoteIndices.contains(clickedNoteIndex)) {
                // Clicking on body of a selected note - prepare for drag
                if (selectedNoteIndices.size() == 1) {
                    // Already handled above with detectDragMode
                    event->accept();
                    return;
                } else {
                    // Multi-selection - only allow dragging the group (no resize/curve edit)
                    currentDragMode = DraggingNote;
                    dragStartPos = pos.toPoint();

                    // Store original states for all selected notes
                    multiDragStartTimes.clear();
                    multiDragStartPitches.clear();
                    multiDragStartCurves.clear();

                    for (int index : selectedNoteIndices) {
                        if (index >= 0 && index < notes.size()) {
                            const Note &note = notes[index];
                            multiDragStartTimes.append(qMakePair(index, note.getStartTime()));
                            multiDragStartPitches.append(qMakePair(index, note.getPitchHz()));
                            if (note.hasPitchCurve()) {
                                multiDragStartCurves.append(qMakePair(index, note.getPitchCurve()));
                            }
                        }
                    }

                    event->accept();
                    return;  // Don't start selecting another note
                }
            }

            // THIRD: Selecting a different note or empty space
            if (clickedNoteIndex >= 0) {
                selectNote(clickedNoteIndex);
                setFocus();  // Ensure widget has keyboard focus for Delete key
                update();
            } else {
                // Tapped empty space - start lasso selection
                deselectAll();
                isDrawingLasso = true;
                lassoStartPos = pos.toPoint();
                lassoCurrentPos = pos.toPoint();
                update();
            }
        } else if (!isDrawingNote) {
            // In drawing modes (discrete or continuous), start drawing note
            isDrawingNote = true;
            noteStartPos = pos.toPoint();
            noteCurrentPos = pos.toPoint();

            // Start capturing pressure curve
            pressurePoints.clear();
            double startTime = pixelToTime(pos.x());
            pressurePoints.append(qMakePair(startTime, penPressure));

            // In continuous mode, start tracking pitch curve
            if (currentInputMode == DrawModeContinuous) {
                pitchPoints.clear();
                double startPitch = pixelToFrequency(pos.y());
                pitchPoints.append(qMakePair(startTime, startPitch));
            }

            // Emit pressure update
            emit pressureChanged(penPressure, true);

            update();
        }
        event->accept();
        break;

    case QEvent::TabletMove:
        // Handle phrase hull resizing
        if ((currentDragMode == ResizingPhraseHullTop || currentDragMode == ResizingPhraseHullBottom) && selectedPhraseIndex >= 0) {
            PhraseGroup *phrasePtr = &phraseGroups[selectedPhraseIndex];
            int deltaY = pos.y() - dragStartPos.y();

            int newPadding;
            if (currentDragMode == ResizingPhraseHullTop) {
                newPadding = dragStartPhraseVerticalPadding - deltaY;
            } else {
                newPadding = dragStartPhraseVerticalPadding + deltaY;
            }

            newPadding = std::max(10, std::min(200, newPadding));
            phrasePtr->setVerticalPadding(newPadding);

            update();
            event->accept();
            break;
        }

        // Handle phrase curve dot editing
        if (isEditingPhraseCurve && selectedPhraseIndex >= 0) {
            PhraseGroup *phrasePtr = &phraseGroups[selectedPhraseIndex];
            Curve &curve = phrasePtr->getDynamicsCurve();

            const QVector<Curve::Point> &points = dragStartPhraseCurve.getPoints();

            if (editingPhraseDotIndex >= 0 && editingPhraseDotIndex < points.size()) {
                // Get phrase time bounds
                const QVector<Note> &notes = phrase.getNotes();
                double phraseStart = 1e9, phraseEnd = 0;
                for (int noteIdx : phrasePtr->getNoteIndices()) {
                    if (noteIdx >= 0 && noteIdx < notes.size()) {
                        const Note &note = notes[noteIdx];
                        phraseStart = std::min(phraseStart, note.getStartTime());
                        phraseEnd = std::max(phraseEnd, note.getEndTime());
                    }
                }
                double phraseDuration = phraseEnd - phraseStart;

                // Get phrase pitch bounds
                double minPitch = 1e9, maxPitch = 0;
                for (int noteIdx : phrasePtr->getNoteIndices()) {
                    if (noteIdx >= 0 && noteIdx < notes.size()) {
                        const Note &note = notes[noteIdx];
                        minPitch = std::min(minPitch, note.getPitchHz());
                        maxPitch = std::max(maxPitch, note.getPitchHz());
                    }
                }
                int padding = phrasePtr->getVerticalPadding();
                int minY = frequencyToPixel(maxPitch) - padding;
                int maxY = frequencyToPixel(minPitch) + padding;
                int hullHeight = maxY - minY;

                // Calculate new position
                double currentTime = pixelToTime(pos.x());
                double newNormalizedTime = (currentTime - phraseStart) / phraseDuration;
                newNormalizedTime = qBound(0.0, newNormalizedTime, 1.0);

                double newValue = 1.0 - (static_cast<double>(pos.y() - minY) / hullHeight);
                newValue = qBound(0.0, newValue, 1.0);

                // Rebuild curve
                curve.clearPoints();
                for (int i = 0; i < points.size(); ++i) {
                    if (i == editingPhraseDotIndex) {
                        curve.addPoint(newNormalizedTime, newValue, points[i].pressure);
                    } else {
                        curve.addPoint(points[i].time, points[i].value, points[i].pressure);
                    }
                }

                update();
            }
            event->accept();
            break;
        }

        // Handle phrase curve gesture drawing
        if (isDrawingPhraseCurve) {
            double time = pixelToTime(pos.x());
            int hullHeight = phraseCurveGestureMaxY - phraseCurveGestureMinY;
            double normalizedValue = 1.0 - (static_cast<double>(pos.y() - phraseCurveGestureMinY) / hullHeight);
            normalizedValue = std::max(0.0, std::min(1.0, normalizedValue));

            phraseCurveGesturePoints.append(qMakePair(time, normalizedValue));
            phraseCurveGesturePressures.append(penPressure);

            emit pressureChanged(penPressure, true);
            update();
            event->accept();
            break;
        }

        // Handle dragging/resizing selected notes
        if (currentDragMode != NoDrag && !selectedNoteIndices.isEmpty()) {
            QVector<Note> &notes = phrase.getNotes();
            QPoint delta = pos.toPoint() - dragStartPos;

            if (selectedNoteIndices.size() == 1) {
                // Single selection - full editing capabilities
                int selectedIndex = selectedNoteIndices.first();
                if (selectedIndex >= notes.size()) {
                    event->accept();
                    return;
                }

                Note &note = notes[selectedIndex];

                switch (currentDragMode) {
                case DraggingNote: {
                // Move note in time and pitch
                double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);
                double newStartTime = dragStartTime + timeDelta;
                if (newStartTime < 0.0) newStartTime = 0.0;

                note.setStartTime(newStartTime);

                // Handle pitch change based on whether note has pitch curve
                if (note.hasPitchCurve()) {
                    // Continuous note with pitch curve: shift entire curve by pixel delta
                    Curve shiftedPitchCurve;
                    const Curve& originalCurve = dragStartCurve;  // Use curve saved at drag start
                    for (const auto& point : originalCurve.getPoints()) {
                        double t = point.time;
                        double originalPitch = point.value;
                        // Convert to pixel, apply delta, convert back
                        int originalPixelY = frequencyToPixel(originalPitch);
                        int newPixelY = originalPixelY + delta.y();
                        double newPitch = pixelToFrequency(newPixelY);
                        shiftedPitchCurve.addPoint(t, newPitch);
                    }
                    note.setPitchCurve(shiftedPitchCurve);
                } else {
                    // Discrete note: snap to scale lines
                    double rawPitch = pixelToFrequency(dragStartPos.y() + delta.y());
                    double snappedPitch = snapToNearestScaleLine(rawPitch);
                    note.setPitchHz(snappedPitch);
                }
                break;
            }

            case ResizingLeft: {
                double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);
                double newStartTime = dragStartTime + timeDelta;
                double newDuration = dragStartDuration - timeDelta;

                if (newDuration < 100.0) {
                    newDuration = 100.0;
                    newStartTime = dragStartTime + dragStartDuration - 100.0;
                }
                if (newStartTime < 0.0) newStartTime = 0.0;

                note.setStartTime(newStartTime);
                note.setDuration(newDuration);
                break;
            }

            case ResizingRight: {
                double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);
                double newDuration = dragStartDuration + timeDelta;

                if (newDuration < 100.0) newDuration = 100.0;

                note.setDuration(newDuration);
                break;
            }

            case EditingTopCurve: {
                // Edit dynamics curve by dragging dot vertically
                double dynamicsChange = -delta.y() / 50.0;  // 50 pixels = 1.0 change

                double originalValue = dragStartCurve.valueAt(editingDotTimePos);
                double newValue = qBound(0.0, originalValue + dynamicsChange, 1.0);

                Curve &dynamicsCurve = note.getDynamicsCurve();
                dynamicsCurve.clearPoints();

                // Rebuild curve with adaptive number of control points
                int numDots = calculateCurveDotCount(note);
                for (int i = 0; i <= numDots + 1; ++i) {
                    double t = i / (numDots + 1.0);
                    double value;

                    if (i == editingDotIndex + 1) {
                        value = newValue;
                    } else {
                        value = dragStartCurve.valueAt(t);
                    }

                    dynamicsCurve.addPoint(t, value);
                }

                dynamicsCurve.sortPoints();
                break;
            }

            case EditingBottomCurve: {
                // Edit bottom curve by dragging dot vertically
                double curveChange = delta.y() / 50.0;  // Positive because down = more for bottom

                double originalValue = dragStartCurve.valueAt(editingDotTimePos);
                double newValue = qBound(0.0, originalValue + curveChange, 1.0);

                Curve &bottomCurve = note.getBottomCurve();
                bottomCurve.clearPoints();

                // Rebuild curve with adaptive number of control points
                int numDots = calculateCurveDotCount(note);
                for (int i = 0; i <= numDots + 1; ++i) {
                    double t = i / (numDots + 1.0);
                    double value;

                    if (i == editingDotIndex + 1) {
                        value = newValue;
                    } else {
                        value = dragStartCurve.valueAt(t);
                    }

                    bottomCurve.addPoint(t, value);
                }

                bottomCurve.sortPoints();
                break;
            }

                case NoDrag:
                    break;
                }
            } else {
                // Multi-selection - only allow dragging the group
                if (currentDragMode == DraggingNote) {
                    // Calculate time delta from original position
                    double timeDelta = pixelToTime(delta.x()) - pixelToTime(0);

                    // Apply time deltas to all selected notes
                    for (int i = 0; i < multiDragStartTimes.size(); ++i) {
                        int index = multiDragStartTimes[i].first;
                        double originalTime = multiDragStartTimes[i].second;

                        if (index >= 0 && index < notes.size()) {
                            Note &note = notes[index];

                            // Apply time movement from original position
                            double newStartTime = originalTime + timeDelta;
                            if (newStartTime < 0.0) newStartTime = 0.0;
                            note.setStartTime(newStartTime);
                        }
                    }

                    // Apply pitch movement - each note moves by same PIXEL delta
                    for (int i = 0; i < multiDragStartPitches.size(); ++i) {
                        int index = multiDragStartPitches[i].first;
                        double originalPitch = multiDragStartPitches[i].second;

                        if (index >= 0 && index < notes.size()) {
                            Note &note = notes[index];

                            if (!note.hasPitchCurve()) {
                                // Convert original pitch to pixel Y
                                int originalPixelY = frequencyToPixel(originalPitch);
                                // Apply pixel delta
                                int newPixelY = originalPixelY + delta.y();
                                // Convert back to frequency
                                double newPitch = pixelToFrequency(newPixelY);
                                note.setPitchHz(newPitch);
                            }
                        }
                    }

                    // Apply pitch curve shifts - each curve point moves by same PIXEL delta
                    for (int i = 0; i < multiDragStartCurves.size(); ++i) {
                        int index = multiDragStartCurves[i].first;
                        const Curve &originalCurve = multiDragStartCurves[i].second;

                        if (index >= 0 && index < notes.size()) {
                            Note &note = notes[index];

                            // Shift pitch curve by pixel delta
                            Curve newPitchCurve;
                            for (const auto& point : originalCurve.getPoints()) {
                                // Convert original pitch to pixel Y
                                int originalPixelY = frequencyToPixel(point.value);
                                // Apply pixel delta
                                int newPixelY = originalPixelY + delta.y();
                                // Convert back to frequency
                                double newPitch = pixelToFrequency(newPixelY);
                                newPitchCurve.addPoint(point.time, newPitch);
                            }
                            note.setPitchCurve(newPitchCurve);
                        }
                    }
                }
            }

            update();
            event->accept();
            return;
        }

        // Handle lasso selection dragging
        if (isDrawingLasso) {
            lassoCurrentPos = pos.toPoint();
            update();
            event->accept();
            return;
        }

        // Update note drawing position
        if (isDrawingNote) {
            noteCurrentPos = pos.toPoint();

            // Capture pressure point at this position
            double currentTime = pixelToTime(pos.x());
            pressurePoints.append(qMakePair(currentTime, penPressure));

            // In continuous mode, track pitch changes
            if (currentInputMode == DrawModeContinuous) {
                double currentPitch = pixelToFrequency(pos.y());
                pitchPoints.append(qMakePair(currentTime, currentPitch));
            }

            // Emit pressure update
            emit pressureChanged(penPressure, true);

            update();
        }
        event->accept();
        break;

    case QEvent::TabletRelease:
        // End phrase hull resizing
        if (currentDragMode == ResizingPhraseHullTop || currentDragMode == ResizingPhraseHullBottom) {
            currentDragMode = NoDrag;
            update();
            qDebug() << "Phrase hull resize complete (tablet) - new padding:" << phraseGroups[selectedPhraseIndex].getVerticalPadding();
            event->accept();
            break;
        }

        // End phrase curve dot editing
        if (isEditingPhraseCurve && selectedPhraseIndex >= 0) {
            PhraseGroup *phrasePtr = &phraseGroups[selectedPhraseIndex];
            Curve newCurve = phrasePtr->getDynamicsCurve();

            // Create undo command
            undoStack->push(new EditPhraseCurveCommand(
                this, selectedPhraseIndex,
                EditPhraseCurveCommand::DynamicsCurve,
                dragStartPhraseCurve, newCurve
            ));

            isEditingPhraseCurve = false;
            editingPhraseDotIndex = -1;
            update();
            event->accept();
            break;
        }

        // End phrase curve gesture drawing
        if (isDrawingPhraseCurve) {
            isDrawingPhraseCurve = false;

            if (selectedPhraseIndex >= 0 && !phraseCurveGesturePoints.isEmpty()) {
                PhraseGroup *phrasePtr = &phraseGroups[selectedPhraseIndex];

                // Convert gesture points to curve points
                // Calculate time range of the phrase
                const QVector<int> &noteIndices = phrasePtr->getNoteIndices();
                double minTime = 1e9, maxTime = 0.0;
                for (int idx : noteIndices) {
                    if (idx >= 0 && idx < phrase.getNotes().size()) {
                        const Note &note = phrase.getNotes()[idx];
                        double noteStart = note.getStartTime();
                        double noteEnd = noteStart + note.getDuration();
                        minTime = std::min(minTime, noteStart);
                        maxTime = std::max(maxTime, noteEnd);
                    }
                }

                // Downsample captured points to a reasonable number for editing
                // Use simple uniform sampling to get ~8-12 control points
                int numCapturedPoints = static_cast<int>(phraseCurveGesturePoints.size());
                int targetPoints = std::min(12, std::max(5, numCapturedPoints / 20));
                int sampleInterval = std::max(1, numCapturedPoints / targetPoints);

                Curve newCurve;
                for (int i = 0; i < numCapturedPoints; i += sampleInterval) {
                    double time = phraseCurveGesturePoints[i].first;
                    double value = phraseCurveGesturePoints[i].second;
                    double pressure = phraseCurveGesturePressures[i];

                    // Normalize time to 0-1 range relative to phrase duration
                    double normalizedTime = (time - minTime) / (maxTime - minTime);
                    normalizedTime = std::max(0.0, std::min(1.0, normalizedTime));

                    newCurve.addPoint(normalizedTime, value, pressure);
                }

                // Always include the last point
                if (numCapturedPoints > 1) {
                    int lastIdx = numCapturedPoints - 1;
                    double time = phraseCurveGesturePoints[lastIdx].first;
                    double value = phraseCurveGesturePoints[lastIdx].second;
                    double pressure = phraseCurveGesturePressures[lastIdx];
                    double normalizedTime = (time - minTime) / (maxTime - minTime);
                    normalizedTime = std::max(0.0, std::min(1.0, normalizedTime));
                    newCurve.addPoint(normalizedTime, value, pressure);
                }

                // Set the curve on the phrase
                phrasePtr->setDynamicsCurve(newCurve);

                qDebug() << "Created phrase curve with" << newCurve.getPoints().size() << "points";

                emit pressureChanged(0.0, false);
                update();
            }

            event->accept();
            break;
        }

        // End drag/resize operation
        if (currentDragMode != NoDrag) {
            if (selectedNoteIndices.size() == 1) {
                // Single selection - full editing capabilities
                int selectedIndex = selectedNoteIndices.first();
                if (selectedIndex >= 0 && selectedIndex < phrase.getNotes().size()) {
                    const Note &note = phrase.getNotes()[selectedIndex];

                    switch (currentDragMode) {
                    case DraggingNote: {
                        // Push move command with old and new states
                        double newStartTime = note.getStartTime();
                        double newPitch = note.getPitchHz();
                        Curve newPitchCurve;
                        bool hasCurve = note.hasPitchCurve();
                        if (hasCurve) {
                            newPitchCurve = note.getPitchCurve();
                        }

                        undoStack->push(new MoveNoteCommand(&phrase, selectedIndex,
                                                            dragStartTime, dragStartPitch,
                                                            newStartTime, newPitch,
                                                            dragStartCurve, newPitchCurve,
                                                            hasCurve, this));
                        break;
                    }

                    case ResizingLeft:
                    case ResizingRight: {
                        // Push resize command
                        double newStartTime = note.getStartTime();
                        double newDuration = note.getDuration();

                        undoStack->push(new ResizeNoteCommand(&phrase, selectedIndex,
                                                              dragStartTime, dragStartDuration,
                                                              newStartTime, newDuration,
                                                              this));
                        break;
                    }

                    case EditingTopCurve: {
                        // Push curve edit command for dynamics
                        Curve newCurve = note.getDynamicsCurve();
                        undoStack->push(new EditCurveCommand(&phrase, selectedIndex,
                                                             EditCurveCommand::DynamicsCurve,
                                                             dragStartCurve, newCurve,
                                                             this));
                        break;
                    }

                    case EditingBottomCurve: {
                        // Push curve edit command for bottom curve
                        Curve newCurve = note.getBottomCurve();
                        undoStack->push(new EditCurveCommand(&phrase, selectedIndex,
                                                             EditCurveCommand::BottomCurve,
                                                             dragStartCurve, newCurve,
                                                             this));
                        break;
                    }

                    case NoDrag:
                        break;
                    }
                }
            } else if (selectedNoteIndices.size() > 1 && currentDragMode == DraggingNote) {
                // Multi-selection drag - create batch move command
                QVector<MoveMultipleNotesCommand::NoteState> oldStates;
                QVector<MoveMultipleNotesCommand::NoteState> newStates;

                const QVector<Note> &notes = phrase.getNotes();

                // Build old states from stored drag start data
                for (int i = 0; i < multiDragStartTimes.size(); ++i) {
                    int index = multiDragStartTimes[i].first;
                    MoveMultipleNotesCommand::NoteState state;
                    state.index = index;
                    state.startTime = multiDragStartTimes[i].second;

                    // Find matching pitch
                    for (const auto &pair : multiDragStartPitches) {
                        if (pair.first == index) {
                            state.pitch = pair.second;
                            break;
                        }
                    }

                    // Find matching curve if it exists
                    state.hasPitchCurve = false;
                    for (const auto &pair : multiDragStartCurves) {
                        if (pair.first == index) {
                            state.pitchCurve = pair.second;
                            state.hasPitchCurve = true;
                            break;
                        }
                    }

                    oldStates.append(state);
                }

                // Build new states from current note positions
                for (int index : selectedNoteIndices) {
                    if (index >= 0 && index < notes.size()) {
                        const Note &note = notes[index];
                        MoveMultipleNotesCommand::NoteState state;
                        state.index = index;
                        state.startTime = note.getStartTime();
                        state.pitch = note.getPitchHz();
                        state.hasPitchCurve = note.hasPitchCurve();
                        if (state.hasPitchCurve) {
                            state.pitchCurve = note.getPitchCurve();
                        }
                        newStates.append(state);
                    }
                }

                undoStack->push(new MoveMultipleNotesCommand(&phrase, selectedNoteIndices,
                                                             oldStates, newStates, this));
            }

            currentDragMode = NoDrag;
            update();
            event->accept();
            return;
        }

        // Complete lasso selection
        if (isDrawingLasso) {
            isDrawingLasso = false;

            // Calculate lasso rectangle
            int x = std::min(lassoStartPos.x(), lassoCurrentPos.x());
            int y = std::min(lassoStartPos.y(), lassoCurrentPos.y());
            int width = std::abs(lassoCurrentPos.x() - lassoStartPos.x());
            int height = std::abs(lassoCurrentPos.y() - lassoStartPos.y());
            QRect lassoRect(x, y, width, height);

            // Find all notes within the lasso rectangle
            QVector<int> notesInRect = findNotesInRectangle(lassoRect);
            selectNotes(notesInRect);

            update();
            event->accept();
            return;
        }

        // Capture final pressure point and handle note creation or selection
        if (isDrawingNote) {
            double endTime = pixelToTime(pos.x());
            pressurePoints.append(qMakePair(endTime, penPressure));

            // Create the note directly here (don't wait for mouseReleaseEvent)
            isDrawingNote = false;

            // Calculate note parameters
            double startTime = pixelToTime(std::min(noteStartPos.x(), noteCurrentPos.x()));
            double endTimeCalc = pixelToTime(std::max(noteStartPos.x(), noteCurrentPos.x()));
            double duration = endTimeCalc - startTime;

            // Check for minimum duration - if too short, use default
            if (duration < 50.0) {
                // This was a click/tap - create a default duration note
                duration = 500.0;  // Default to 500ms
                endTimeCalc = startTime + duration;
            }

            // Create a new note (we're in drawing mode)
            // Get pitch from vertical position
            double rawPitch = pixelToFrequency(noteStartPos.y());
            double finalPitch;

            // Apply pitch snapping based on input mode
            if (currentInputMode == DrawModeDiscrete) {
                // Discrete mode: snap to nearest scale line
                finalPitch = snapToNearestScaleLine(rawPitch);
            } else {
                // Continuous mode: use literal pitch (no snapping)
                finalPitch = rawPitch;
            }

            // Deselect when creating a new note
            deselectAll();

            // Create note with default dynamics first
            Note newNote(startTime, duration, finalPitch, 0.7);

            // Set the note to use the active track
            newNote.setTrackIndex(activeTrackIndex);

            // Create a default bottom curve with some variation (for visual interest)
            Curve defaultBottomCurve;
            defaultBottomCurve.addPoint(0.0, 0.5);
            defaultBottomCurve.addPoint(0.25, 0.6);
            defaultBottomCurve.addPoint(0.5, 0.65);
            defaultBottomCurve.addPoint(0.75, 0.6);
            defaultBottomCurve.addPoint(1.0, 0.55);
            newNote.setBottomCurve(defaultBottomCurve);

            // Create dynamics curve from captured pressure points
            if (pressurePoints.size() >= 2) {
                Curve dynamicsCurve;

                // Convert absolute time points to normalized time (0.0-1.0 within the note)
                for (const auto &point : pressurePoints) {
                    double absoluteTime = point.first;
                    double pressure = point.second;

                    // Normalize time within the note duration
                    double normalizedTime = (absoluteTime - startTime) / duration;
                    normalizedTime = qBound(0.0, normalizedTime, 1.0);

                    dynamicsCurve.addPoint(normalizedTime, pressure);
                }

                // Sort points by time to ensure proper interpolation
                dynamicsCurve.sortPoints();

                // Set the curve on the note
                newNote.setDynamicsCurve(dynamicsCurve);
            }

            // If in continuous mode and we have pitch points, create a pitch curve for glissando
            if (currentInputMode == DrawModeContinuous && pitchPoints.size() >= 2) {
                Curve pitchCurve;

                // Convert absolute time points to normalized time (0.0-1.0 within the note)
                for (const auto &point : pitchPoints) {
                    double absoluteTime = point.first;
                    double pitchHz = point.second;

                    // Normalize time within the note duration
                    double normalizedTime = (absoluteTime - startTime) / duration;
                    normalizedTime = qBound(0.0, normalizedTime, 1.0);

                    pitchCurve.addPoint(normalizedTime, pitchHz);
                }

                // Sort points by time to ensure proper interpolation
                pitchCurve.sortPoints();

                // Set the pitch curve on the note
                newNote.setPitchCurve(pitchCurve);
            }

            // Use undo command to add note
            undoStack->push(new AddNoteCommand(&phrase, newNote, this));

            // Reset state for next input
            usingTablet = false;
            pressurePoints.clear();
            pitchPoints.clear();

            // Signal that pressure monitoring is inactive
            emit pressureChanged(0.0, false);

            update();
        }
        event->accept();
        break;

    default:
        event->ignore();
        break;
    }
}

// ============================================================================
// Keyboard Input
// ============================================================================

void ScoreCanvas::keyPressEvent(QKeyEvent *event)
{
    // Delete key removes selected notes
    if (event->key() == Qt::Key_Delete && !selectedNoteIndices.isEmpty()) {
        // Use batch delete command for all selected notes
        undoStack->push(new DeleteMultipleNotesCommand(&phrase, selectedNoteIndices, this));

        deselectAll();
        update();
        event->accept();
        return;
    }

    // Ctrl+Z for undo
    if (event->matches(QKeySequence::Undo)) {
        undoStack->undo();
        event->accept();
        return;
    }

    // Ctrl+Y or Ctrl+Shift+Z for redo
    if (event->matches(QKeySequence::Redo)) {
        undoStack->redo();
        event->accept();
        return;
    }

    // Ctrl+C for copy
    if (event->matches(QKeySequence::Copy)) {
        if (!selectedNoteIndices.isEmpty()) {
            // Copy selected notes to clipboard
            clipboard.clear();
            const QVector<Note> &notes = phrase.getNotes();
            for (int index : selectedNoteIndices) {
                if (index >= 0 && index < notes.size()) {
                    clipboard.append(notes[index]);
                }
            }
            qDebug() << "Copied" << clipboard.size() << "notes to clipboard";
        }
        event->accept();
        return;
    }

    // Ctrl+X for cut
    if (event->matches(QKeySequence::Cut)) {
        if (!selectedNoteIndices.isEmpty()) {
            // Copy selected notes to clipboard
            clipboard.clear();
            const QVector<Note> &notes = phrase.getNotes();
            for (int index : selectedNoteIndices) {
                if (index >= 0 && index < notes.size()) {
                    clipboard.append(notes[index]);
                }
            }
            qDebug() << "Cut" << clipboard.size() << "notes to clipboard";

            // Delete selected notes (using existing delete command)
            undoStack->push(new DeleteMultipleNotesCommand(&phrase, selectedNoteIndices, this));
            deselectAll();
            update();
        }
        event->accept();
        return;
    }

    // Ctrl+V for paste
    if (event->matches(QKeySequence::Paste)) {
        if (!clipboard.isEmpty()) {
            // Paste notes at pasteTargetTime
            PasteNotesCommand *pasteCmd = new PasteNotesCommand(&phrase, clipboard, pasteTargetTime, this);
            undoStack->push(pasteCmd);

            // Select the pasted notes
            selectNotes(pasteCmd->getPastedIndices());
            update();
            qDebug() << "Pasted" << clipboard.size() << "notes at time" << pasteTargetTime;
        }
        event->accept();
        return;
    }

    // Let parent window handle other keys (including shortcuts)
    event->ignore();
}

// ============================================================================
// Phrase Rendering
// ============================================================================

void ScoreCanvas::drawPhraseHull(QPainter &painter, const PhraseGroup &phrase, bool isSelected)
{
    if (phrase.getNoteCount() == 0) return;

    const QVector<Note> &notes = this->phrase.getNotes();

    // Get phrase time and pitch bounds
    double phraseStart = 1e9, phraseEnd = 0;
    double minPitch = 1e9, maxPitch = 0;

    for (int noteIdx : phrase.getNoteIndices()) {
        if (noteIdx < 0 || noteIdx >= notes.size()) continue;
        const Note &note = notes[noteIdx];
        phraseStart = std::min(phraseStart, note.getStartTime());
        phraseEnd = std::max(phraseEnd, note.getEndTime());
        minPitch = std::min(minPitch, note.getPitchHz());
        maxPitch = std::max(maxPitch, note.getPitchHz());
    }

    if (phraseStart >= phraseEnd) return;

    // Calculate hull bounds using custom vertical padding
    int padding = phrase.getVerticalPadding();
    int minY = frequencyToPixel(maxPitch) - padding;  // Higher pitch = lower Y
    int maxY = frequencyToPixel(minPitch) + padding;  // Lower pitch = higher Y
    int minX = timeToPixel(phraseStart);
    int maxX = timeToPixel(phraseEnd);

    // Add horizontal padding
    const double horizontalPadding = 10.0;
    minX -= horizontalPadding;
    maxX += horizontalPadding;

    // Create hull polygon
    QPolygonF hull;
    hull << QPointF(minX, minY) << QPointF(maxX, minY)
         << QPointF(maxX, maxY) << QPointF(minX, maxY);

    // Draw translucent fill
    QColor hullColor = phrase.getColor();
    hullColor.setAlpha(isSelected ? 77 : 38);  // 0.3 : 0.15 opacity
    painter.setBrush(hullColor);

    // Draw border - MUCH thicker when selected for visibility
    QPen borderPen;
    if (isSelected) {
        borderPen = QPen(hullColor.darker(120), 4, Qt::SolidLine);  // Thick solid line
    } else {
        borderPen = QPen(hullColor.darker(150), 2, Qt::DashLine);   // Thin dashed line
    }
    painter.setPen(borderPen);
    painter.drawPolygon(hull);

    // Draw phrase name label if selected
    if (isSelected) {
        QFont labelFont = painter.font();
        labelFont.setPointSize(10);
        labelFont.setBold(true);
        painter.setFont(labelFont);

        // Draw text with background
        QString label = QString("📝 %1").arg(phrase.getName());
        QRectF textRect = painter.boundingRect(QRectF(minX, minY - 25, 200, 20), Qt::AlignLeft, label);

        // Draw semi-transparent background for text
        painter.setBrush(QColor(255, 255, 255, 220));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(textRect.adjusted(-3, -2, 3, 2), 3, 3);

        // Draw text
        painter.setPen(QColor(50, 50, 50));
        painter.drawText(textRect, Qt::AlignLeft, label);
    }

    // Draw resize handles on top and bottom edges if selected
    if (isSelected) {
        int centerX = (minX + maxX) / 2;
        int handleWidth = 60;
        int handleHeight = 8;

        // Top edge handle
        QRectF topHandle(centerX - handleWidth/2, minY - handleHeight/2, handleWidth, handleHeight);
        painter.setBrush(QColor(255, 255, 255, 200));
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.drawRoundedRect(topHandle, 3, 3);

        // Bottom edge handle
        QRectF bottomHandle(centerX - handleWidth/2, maxY - handleHeight/2, handleWidth, handleHeight);
        painter.setBrush(QColor(255, 255, 255, 200));
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.drawRoundedRect(bottomHandle, 3, 3);
    }
}

void ScoreCanvas::drawPhraseCurve(QPainter &painter, const PhraseGroup &phrase, bool isSelected)
{
    if (phrase.getDynamicsCurve().isEmpty()) return;
    if (phrase.getNoteCount() == 0) return;

    const QVector<Note> &notes = this->phrase.getNotes();

    // Get phrase time and pitch bounds
    double phraseStart = 1e9, phraseEnd = 0;
    double minPitch = 1e9, maxPitch = 0;

    for (int noteIdx : phrase.getNoteIndices()) {
        if (noteIdx < 0 || noteIdx >= notes.size()) continue;
        const Note &note = notes[noteIdx];
        phraseStart = std::min(phraseStart, note.getStartTime());
        phraseEnd = std::max(phraseEnd, note.getEndTime());
        minPitch = std::min(minPitch, note.getPitchHz());
        maxPitch = std::max(maxPitch, note.getPitchHz());
    }

    double phraseDuration = phraseEnd - phraseStart;
    if (phraseDuration < 0.001) return;

    // Calculate vertical bounds of phrase hull (with custom padding)
    int padding = phrase.getVerticalPadding();
    int minY = frequencyToPixel(maxPitch) - padding;  // Higher pitch = lower Y
    int maxY = frequencyToPixel(minPitch) + padding;  // Lower pitch = higher Y
    int hullHeight = maxY - minY;

    // Sample curve and create variable-width path
    const Curve &dynamicsCurve = phrase.getDynamicsCurve();
    int numSamples = std::max(50, static_cast<int>(phraseDuration / 5.0));

    QVector<QPointF> topEdge, bottomEdge;

    for (int i = 0; i <= numSamples; ++i) {
        double t = i / static_cast<double>(numSamples);
        double absoluteTime = phraseStart + t * phraseDuration;
        int x = timeToPixel(absoluteTime);

        // Get curve value and pressure at this point
        double value = dynamicsCurve.valueAt(t);
        double pressure = dynamicsCurve.pressureAt(t);

        // Y position based on curve value (0-1) within phrase hull bounds
        // value=1.0 → top of hull (minY), value=0.0 → bottom of hull (maxY)
        int centerY = minY + static_cast<int>((1.0 - value) * hullHeight);

        // Line thickness based on pressure (10-40 pixels for visibility)
        double thickness = 10.0 + pressure * 30.0;

        topEdge << QPointF(x, centerY - thickness / 2.0);
        bottomEdge << QPointF(x, centerY + thickness / 2.0);
    }

    // Create closed path
    QPainterPath curvePath;
    if (!topEdge.isEmpty()) {
        curvePath.moveTo(topEdge.first());
        for (const QPointF &p : topEdge) curvePath.lineTo(p);
        for (int i = bottomEdge.size() - 1; i >= 0; --i) {
            curvePath.lineTo(bottomEdge[i]);
        }
        curvePath.closeSubpath();
    }

    // Draw curve with prominent orange/red color
    QColor curveColor = QColor(255, 100, 50);  // Orange-red
    curveColor.setAlpha(isSelected ? 220 : 180);
    painter.setBrush(curveColor);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPath(curvePath);

    // Draw editable dots if selected
    if (isSelected) {
        drawPhraseCurveDots(painter, phrase, phraseStart, phraseDuration, minY, maxY);
    }
}

void ScoreCanvas::drawPhraseCurveDots(QPainter &painter, const PhraseGroup &phrase,
                                      double phraseStart, double phraseDuration, int minY, int maxY)
{
    const Curve &curve = phrase.getDynamicsCurve();
    const QVector<Curve::Point> &points = curve.getPoints();

    int hullHeight = maxY - minY;

    for (int i = 0; i < points.size(); ++i) {
        const Curve::Point &pt = points[i];
        double absoluteTime = phraseStart + pt.time * phraseDuration;
        int x = timeToPixel(absoluteTime);

        // Y position based on curve value (0-1) within phrase hull bounds
        int y = minY + static_cast<int>((1.0 - pt.value) * hullHeight);

        // Draw dot
        QRectF dotRect(x - 5, y - 5, 10, 10);
        painter.setBrush(Qt::white);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(dotRect);

        // Highlight if being edited
        if (isEditingPhraseCurve && editingPhraseDotIndex == i) {
            painter.setBrush(QColor(255, 200, 0));
            painter.drawEllipse(dotRect);
        }
    }
}

double ScoreCanvas::getAveragePitchAtTime(const PhraseGroup &phrase, double time) const
{
    const QVector<Note> &notes = this->phrase.getNotes();
    double sumPitch = 0;
    int count = 0;

    for (int noteIdx : phrase.getNoteIndices()) {
        if (noteIdx < 0 || noteIdx >= notes.size()) continue;
        const Note &note = notes[noteIdx];

        if (time >= note.getStartTime() && time <= note.getEndTime()) {
            double t = (time - note.getStartTime()) / note.getDuration();
            sumPitch += note.getPitchAt(t);
            count++;
        }
    }

    return count > 0 ? sumPitch / count : 440.0;  // Default to A4
}

ScoreCanvas::DragMode ScoreCanvas::detectPhraseHullResizeHandle(const QPoint &pos, const PhraseGroup &phrase) const
{
    const QVector<Note> &notes = this->phrase.getNotes();

    // Get phrase bounds
    double phraseStart = 1e9, phraseEnd = 0;
    double minPitch = 1e9, maxPitch = 0;

    for (int noteIdx : phrase.getNoteIndices()) {
        if (noteIdx < 0 || noteIdx >= notes.size()) continue;
        const Note &note = notes[noteIdx];
        phraseStart = std::min(phraseStart, note.getStartTime());
        phraseEnd = std::max(phraseEnd, note.getEndTime());
        minPitch = std::min(minPitch, note.getPitchHz());
        maxPitch = std::max(maxPitch, note.getPitchHz());
    }

    if (phraseStart >= phraseEnd) return NoDrag;

    int padding = phrase.getVerticalPadding();
    int minY = frequencyToPixel(maxPitch) - padding;
    int maxY = frequencyToPixel(minPitch) + padding;
    int minX = timeToPixel(phraseStart);
    int maxX = timeToPixel(phraseEnd);

    int centerX = (minX + maxX) / 2;
    int handleWidth = 60;
    int handleHeight = 8;

    // Check top handle
    QRectF topHandle(centerX - handleWidth/2, minY - handleHeight/2, handleWidth, handleHeight);
    if (topHandle.contains(pos)) {
        return ResizingPhraseHullTop;
    }

    // Check bottom handle
    QRectF bottomHandle(centerX - handleWidth/2, maxY - handleHeight/2, handleWidth, handleHeight);
    if (bottomHandle.contains(pos)) {
        return ResizingPhraseHullBottom;
    }

    return NoDrag;
}

int ScoreCanvas::findPhraseCurveDotAtPosition(const QPoint &pos, const PhraseGroup &phrase) const
{
    const Curve &curve = phrase.getDynamicsCurve();
    const QVector<Curve::Point> &points = curve.getPoints();
    const QVector<Note> &notes = this->phrase.getNotes();

    // Get phrase time and pitch bounds
    double phraseStart = 1e9, phraseEnd = 0;
    double minPitch = 1e9, maxPitch = 0;

    for (int noteIdx : phrase.getNoteIndices()) {
        if (noteIdx < 0 || noteIdx >= notes.size()) continue;
        const Note &note = notes[noteIdx];
        phraseStart = std::min(phraseStart, note.getStartTime());
        phraseEnd = std::max(phraseEnd, note.getEndTime());
        minPitch = std::min(minPitch, note.getPitchHz());
        maxPitch = std::max(maxPitch, note.getPitchHz());
    }

    double phraseDuration = phraseEnd - phraseStart;
    if (phraseDuration < 0.001) return -1;

    // Calculate vertical bounds of phrase hull (same as drawPhraseCurve)
    int padding = phrase.getVerticalPadding();
    int minY = frequencyToPixel(maxPitch) - padding;
    int maxY = frequencyToPixel(minPitch) + padding;
    int hullHeight = maxY - minY;

    const int tolerance = 12;  // Click tolerance in pixels

    for (int i = 0; i < points.size(); ++i) {
        const Curve::Point &pt = points[i];
        double absoluteTime = phraseStart + pt.time * phraseDuration;
        int x = timeToPixel(absoluteTime);

        // Y position based on curve value (0-1) within phrase hull bounds
        int y = minY + static_cast<int>((1.0 - pt.value) * hullHeight);

        QRectF dotRect(x - tolerance, y - tolerance, tolerance * 2, tolerance * 2);
        if (dotRect.contains(pos)) {
            return i;
        }
    }

    return -1;
}

// ============================================================================
// Phrase Management
// ============================================================================

void ScoreCanvas::selectPhrase(int index)
{
    if (index < 0 || index >= phraseGroups.size()) {
        selectedPhraseIndex = -1;
        emit phraseSelectionChanged();
        update();
        return;
    }

    // Deselect all notes when selecting a phrase
    deselectAll();

    selectedPhraseIndex = index;
    emit phraseSelectionChanged();
    update();
}

void ScoreCanvas::deselectPhrase()
{
    selectedPhraseIndex = -1;
    emit phraseSelectionChanged();
    update();
}

PhraseGroup* ScoreCanvas::getSelectedPhrase()
{
    if (selectedPhraseIndex >= 0 && selectedPhraseIndex < phraseGroups.size()) {
        return &phraseGroups[selectedPhraseIndex];
    }
    return nullptr;
}

const PhraseGroup* ScoreCanvas::getSelectedPhrase() const
{
    if (selectedPhraseIndex >= 0 && selectedPhraseIndex < phraseGroups.size()) {
        return &phraseGroups[selectedPhraseIndex];
    }
    return nullptr;
}

int ScoreCanvas::findPhraseAtPosition(const QPoint &pos) const
{
    const QVector<Note> &notes = phrase.getNotes();

    // Check phrases in reverse order (top layer first)
    for (int i = phraseGroups.size() - 1; i >= 0; --i) {
        const PhraseGroup &pg = phraseGroups[i];

        // Get phrase bounds
        double minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9;

        for (int noteIdx : pg.getNoteIndices()) {
            if (noteIdx < 0 || noteIdx >= notes.size()) continue;
            const Note &note = notes[noteIdx];

            int x = timeToPixel(note.getStartTime());
            int width = timeToPixel(note.getEndTime()) - x;
            double pitchHz = note.getPitchHz();
            int y = frequencyToPixel(pitchHz);

            minX = std::min(minX, static_cast<double>(x));
            maxX = std::max(maxX, static_cast<double>(x + width));
            minY = std::min(minY, static_cast<double>(y - 50));
            maxY = std::max(maxY, static_cast<double>(y + 50));
        }

        // Check if click is within phrase bounds
        if (pos.x() >= minX && pos.x() <= maxX &&
            pos.y() >= minY && pos.y() <= maxY) {
            return i;
        }
    }

    return -1;
}

void ScoreCanvas::createPhraseFromSelection(const QString &name)
{
    if (selectedNoteIndices.isEmpty()) {
        qWarning() << "No notes selected for phrase grouping";
        return;
    }

    // Create command
    undoStack->push(new CreatePhraseGroupCommand(this, selectedNoteIndices, name));

    // Select the newly created phrase
    selectPhrase(phraseGroups.size() - 1);
}

void ScoreCanvas::ungroupPhrase(int phraseIndex)
{
    if (phraseIndex < 0 || phraseIndex >= phraseGroups.size()) return;

    undoStack->push(new DeletePhraseGroupCommand(this, phraseIndex));
}

void ScoreCanvas::applyPhraseTemplate(const PhraseGroup &templatePhrase)
{
    if (selectedNoteIndices.isEmpty()) {
        qWarning() << "No notes selected to apply phrase template";
        return;
    }

    // Create new phrase with template curves
    PhraseGroup newPhrase(templatePhrase.getName() + " (applied)");
    newPhrase.setDynamicsCurve(templatePhrase.getDynamicsCurve());
    newPhrase.setVibratoCurve(templatePhrase.getVibratoCurve());
    newPhrase.setRhythmicCurve(templatePhrase.getRhythmicCurve());
    newPhrase.setUseEasing(templatePhrase.hasEasing());
    newPhrase.setEasingType(templatePhrase.getEasingType());
    newPhrase.setColor(templatePhrase.getColor());

    for (int idx : selectedNoteIndices) {
        newPhrase.addNoteIndex(idx);
    }

    undoStack->push(new CreatePhraseGroupCommand(this, selectedNoteIndices, newPhrase.getName()));

    // Override the created phrase's curves with template data
    phraseGroups.last().setDynamicsCurve(newPhrase.getDynamicsCurve());
    phraseGroups.last().setVibratoCurve(newPhrase.getVibratoCurve());
    phraseGroups.last().setRhythmicCurve(newPhrase.getRhythmicCurve());
    phraseGroups.last().setUseEasing(newPhrase.hasEasing());
    phraseGroups.last().setEasingType(newPhrase.getEasingType());
    phraseGroups.last().setColor(newPhrase.getColor());

    update();
}
