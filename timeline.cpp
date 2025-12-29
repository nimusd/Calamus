#include "timeline.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <cmath>

Timeline::Timeline(QWidget *parent)
    : QWidget(parent)
    , timeMode(Absolute)
    , tempo(120.0)
    , timeSigNumerator(4)
    , timeSigDenominator(4)
    , nowMarkerTime(0.0)
    , loopStartTime(-1.0)
    , loopEndTime(-1.0)
    , loopModeActive(false)
    , horizontalOffset(0)
    , pixelsPerSecond(100.0)  // Default: 100 pixels per second
    , hoveringLoopEnd(false)
    , mouseOnTimeline(false)
    , mouseCursorX(0)
{
    setMinimumHeight(60);
    setMaximumHeight(60);
    setMouseTracking(true);  // Enable hover detection
    setCursor(Qt::ArrowCursor);
}

void Timeline::setTimeMode(TimeMode mode)
{
    timeMode = mode;
    update();
}

void Timeline::setTempo(double bpm)
{
    tempo = bpm;
    update();
}

void Timeline::setTimeSignature(int numerator, int denominator)
{
    timeSigNumerator = numerator;
    timeSigDenominator = denominator;
    update();
}

void Timeline::setNowMarker(double timeMs)
{
    nowMarkerTime = timeMs;

    // If we're starting a new loop, set loop start to now marker
    if (loopModeActive && loopEndTime < 0.0) {
        loopStartTime = timeMs;
    }

    update();
}

void Timeline::setLoopModeActive(bool active)
{
    loopModeActive = active;

    if (active) {
        // Starting loop mode - set loop start to current now marker
        loopStartTime = nowMarkerTime;
        loopEndTime = -1.0;  // Clear end until user clicks
    }

    update();
}

void Timeline::setLoopEnd(double timeMs)
{
    if (loopModeActive && loopStartTime >= 0.0) {
        loopEndTime = timeMs;
        loopModeActive = false;  // Exit loop mode after setting end
        qDebug() << "Timeline: Loop set from" << loopStartTime << "ms to" << loopEndTime << "ms";
        update();
    }
}

void Timeline::clearLoop()
{
    loopStartTime = -1.0;
    loopEndTime = -1.0;
    loopModeActive = false;
    update();
}

void Timeline::setHorizontalOffset(int offset)
{
    horizontalOffset = offset;
    update();
}

void Timeline::setPixelsPerSecond(double pps)
{
    pixelsPerSecond = pps;
    update();
}

QString Timeline::formatTime(double timeMs) const
{
    if (timeMode == Absolute) {
        // Format: Min:Sec:Ms (e.g., "1:23:456")
        int totalMs = static_cast<int>(timeMs);
        int ms = totalMs % 1000;
        int totalSeconds = totalMs / 1000;
        int seconds = totalSeconds % 60;
        int minutes = totalSeconds / 60;

        return QString("%1:%2:%3")
            .arg(minutes)
            .arg(seconds, 2, 10, QChar('0'))
            .arg(ms, 3, 10, QChar('0'));
    } else {
        // Format: Bars:Beats:Ms (e.g., "4:2:240")
        double beatsPerMs = tempo / 60000.0;  // BPM to beats per millisecond
        double totalBeats = timeMs * beatsPerMs;

        int beatsPerBar = timeSigNumerator;
        int bar = static_cast<int>(totalBeats / beatsPerBar) + 1;  // 1-indexed
        int beat = static_cast<int>(std::fmod(totalBeats, beatsPerBar)) + 1;  // 1-indexed

        // Calculate milliseconds within current beat
        double beatFraction = std::fmod(totalBeats, 1.0);
        int ms = static_cast<int>(beatFraction * (60000.0 / tempo));

        return QString("%1:%2:%3")
            .arg(bar)
            .arg(beat)
            .arg(ms, 3, 10, QChar('0'));
    }
}

double Timeline::pixelToTime(int x) const
{
    // Convert pixel position to time in milliseconds
    double adjustedX = x + horizontalOffset;
    return (adjustedX / pixelsPerSecond) * 1000.0;  // Convert to ms
}

int Timeline::timeToPixel(double timeMs) const
{
    // Convert time in milliseconds to pixel position
    double seconds = timeMs / 1000.0;
    return static_cast<int>(seconds * pixelsPerSecond) - horizontalOffset;
}

void Timeline::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fill background
    painter.fillRect(rect(), QColor("#f0f0f0"));

    // Draw loop region first (background layer)
    if (loopStartTime >= 0.0) {
        drawLoopRegion(painter);
    }

    // Draw tick marks and labels
    drawTickMarks(painter);

    // Draw now marker (playback position)
    drawNowMarker(painter);

    // Draw mouse cursor position indicator
    if (mouseOnTimeline) {
        // Draw thin vertical line at cursor position
        painter.setPen(QPen(QColor(100, 100, 255, 180), 1, Qt::DashLine));  // Semi-transparent blue dashed line
        painter.drawLine(mouseCursorX, 0, mouseCursorX, height());

        // Draw time label at cursor position
        double cursorTime = pixelToTime(mouseCursorX);
        QString timeLabel = formatTime(cursorTime);

        // Draw label background
        QFontMetrics fm(painter.font());
        int labelWidth = fm.horizontalAdvance(timeLabel) + 8;
        int labelHeight = fm.height() + 4;
        int labelX = mouseCursorX - labelWidth / 2;
        int labelY = 2;

        // Keep label within bounds
        if (labelX < 2) labelX = 2;
        if (labelX + labelWidth > width() - 2) labelX = width() - labelWidth - 2;

        painter.fillRect(labelX, labelY, labelWidth, labelHeight, QColor(100, 100, 255, 200));
        painter.setPen(Qt::white);
        painter.drawText(labelX + 4, labelY + fm.ascent() + 2, timeLabel);
    }

    // Draw border
    painter.setPen(QColor("#ccc"));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

void Timeline::drawTickMarks(QPainter &painter)
{
    painter.setPen(QColor("#666"));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    // Determine tick interval based on zoom level and time mode
    double intervalSeconds;
    if (timeMode == Absolute) {
        // For absolute mode, use 1, 5, 10, 30, 60 second intervals
        if (pixelsPerSecond > 50) {
            intervalSeconds = 1.0;  // 1 second
        } else if (pixelsPerSecond > 10) {
            intervalSeconds = 5.0;  // 5 seconds
        } else if (pixelsPerSecond > 5) {
            intervalSeconds = 10.0;  // 10 seconds
        } else if (pixelsPerSecond > 2) {
            intervalSeconds = 30.0;  // 30 seconds
        } else {
            intervalSeconds = 60.0;  // 1 minute
        }
    } else {
        // For musical mode, use bar intervals
        double secondsPerBeat = 60.0 / tempo;
        double secondsPerBar = secondsPerBeat * timeSigNumerator;
        intervalSeconds = secondsPerBar;
    }

    // Draw ticks across visible area
    int visibleWidth = width();
    double startTime = pixelToTime(0);
    double endTime = pixelToTime(visibleWidth);

    // Find first tick position
    double intervalMs = intervalSeconds * 1000.0;
    double firstTickTime = std::floor(startTime / intervalMs) * intervalMs;

    for (double tickTime = firstTickTime; tickTime <= endTime; tickTime += intervalMs) {
        int x = timeToPixel(tickTime);

        if (x >= 0 && x <= visibleWidth) {
            // Draw tick mark
            painter.drawLine(x, height() - 15, x, height() - 5);

            // Draw time label
            QString label = formatTime(tickTime);
            QRect textRect(x - 40, height() - 35, 80, 20);
            painter.drawText(textRect, Qt::AlignCenter, label);
        }
    }
}

void Timeline::drawLoopRegion(QPainter &painter)
{
    if (loopEndTime >= 0.0) {
        // Loop is set - draw blue vertical line at loop end with triangle
        int endX = timeToPixel(loopEndTime);

        // Only draw if visible
        if (endX >= 0 && endX <= width()) {
            // Save painter state
            painter.save();

            // Reset everything
            painter.setBrush(Qt::NoBrush);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            // Draw JUST a vertical line
            QPen linePen(QColor(100, 150, 255), 3);
            linePen.setStyle(Qt::SolidLine);
            painter.setPen(linePen);
            painter.drawLine(QPoint(endX, 0), QPoint(endX, height()));

            // Draw triangle marker at top
            QPolygon triangle = getLoopEndTriangle(endX);
            painter.setBrush(QColor(100, 150, 255));
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(triangle);

            // Store triangle position for hit testing
            loopEndTrianglePos = QPoint(endX, 10);

            // Restore painter state
            painter.restore();
        }
    } else if (loopModeActive) {
        // Loop mode active but no end yet - just show hint text
        painter.save();
        painter.setPen(QColor("#666"));
        QFont font = painter.font();
        font.setItalic(true);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, "Click to set loop end");
        painter.restore();
    }
}

void Timeline::drawNowMarker(QPainter &painter)
{
    int x = timeToPixel(nowMarkerTime);

    // Draw medium thick red line
    painter.setPen(QPen(Qt::red, 3));
    painter.drawLine(x, 0, x, height());
}

QPolygon Timeline::getLoopEndTriangle(int x) const
{
    // Inverted triangle pointing down at the end moment
    QPolygon triangle;
    triangle << QPoint(x - 8, 5)   // Left point
             << QPoint(x + 8, 5)   // Right point
             << QPoint(x, 15);     // Bottom point (pointing down)
    return triangle;
}

bool Timeline::isPointInLoopEndTriangle(const QPoint &pos) const
{
    if (loopEndTime < 0.0) return false;

    int endX = timeToPixel(loopEndTime);
    QPolygon triangle = getLoopEndTriangle(endX);
    return triangle.containsPoint(pos, Qt::OddEvenFill);
}

void Timeline::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Set now marker to clicked position
        double clickedTime = pixelToTime(event->pos().x());
        if (clickedTime < 0.0) clickedTime = 0.0;

        setNowMarker(clickedTime);
        emit nowMarkerChanged(clickedTime);
    }
}

void Timeline::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Check if clicking loop end triangle for selection
        if (isPointInLoopEndTriangle(event->pos())) {
            // Triangle is now selected for deletion (user can press Delete key)
            // For now, just highlight it
            hoveringLoopEnd = true;
            update();
            return;
        }

        // If in loop mode, clicking sets the loop end
        if (loopModeActive && loopStartTime >= 0.0) {
            double clickedTime = pixelToTime(event->pos().x());
            if (clickedTime < 0.0) clickedTime = 0.0;

            // Only set loop end if it's after loop start
            if (clickedTime > loopStartTime) {
                setLoopEnd(clickedTime);
                emit loopEndSet(clickedTime);
            }
        }
    }
}

void Timeline::mouseMoveEvent(QMouseEvent *event)
{
    // Update mouse cursor position
    mouseOnTimeline = true;
    mouseCursorX = event->pos().x();

    // Check if hovering over loop end triangle
    bool wasHovering = hoveringLoopEnd;
    hoveringLoopEnd = isPointInLoopEndTriangle(event->pos());

    // Update cursor
    if (hoveringLoopEnd) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    // Redraw (for cursor indicator and/or hover state change)
    update();
}

void Timeline::leaveEvent(QEvent *event)
{
    // Hide cursor position indicator
    mouseOnTimeline = false;

    if (hoveringLoopEnd) {
        hoveringLoopEnd = false;
        setCursor(Qt::ArrowCursor);
    }

    update();
    QWidget::leaveEvent(event);
}
