#include "canvas.h"

Canvas::Canvas(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: white;");
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);  // Receive mouse moves even without button pressed
}

void Canvas::clearSelection()
{
    selectedConnectionIndex = -1;
    update();
}

QPainterPath Canvas::createConnectionPath(const QPoint &start, const QPoint &end)
{
    int dx = abs(end.x() - start.x());
    int offset = qMax(50, dx / 2);

    QPoint cp1(start.x() + offset, start.y());
    QPoint cp2(end.x() - offset, end.y());

    QPainterPath path;
    path.moveTo(start);
    path.cubicTo(cp1, cp2, end);
    return path;
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    // Deselect container
    if (selectedContainer) {
        selectedContainer->setSelected(false);
    }
    selectedContainer = nullptr;
    selectedConnectionIndex = -1;
    if (event->button() == Qt::LeftButton) {
        // Check each connection for hit
        for (int i = 0; i < connections.size(); ++i) {
            const Connection &conn = connections[i];
            QPoint start = getPortPosition(conn.fromContainer, conn.fromPort, true);
            QPoint end = getPortPosition(conn.toContainer, conn.toPort, false);

            if (!start.isNull() && !end.isNull()) {
                QPainterPath path = createConnectionPath(start, end);

                // Create a "fat" version for hit testing
                QPainterPathStroker stroker;
                stroker.setWidth(10);  // 10px click tolerance
                QPainterPath clickArea = stroker.createStroke(path);

                if (clickArea.contains(event->pos())) {
                    selectedConnectionIndex = i;
                    break;
                }
            }
        }
    }

    update();
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        if (selectedContainer) {
            // Remove all connections involving this container
            for (int i = connections.size() - 1; i >= 0; --i) {
                if (connections[i].fromContainer == selectedContainer ||
                    connections[i].toContainer == selectedContainer) {
                    connections.removeAt(i);
                }
            }

            // Delete the container widget
            selectedContainer->deleteLater();
            selectedContainer = nullptr;
            update();
        }
        else if (selectedConnectionIndex >= 0 && selectedConnectionIndex < connections.size()) {
            connections.removeAt(selectedConnectionIndex);
            selectedConnectionIndex = -1;
            update();
        }
    }
}

void Canvas::addConnection(const Connection &conn)
{
    connections.append(conn);
    update();
}

QPoint Canvas::getPortPosition(Container *container, const QString &portName, bool isOutput)
{
    for (const Container::PortInfo &port : container->getPorts()) {
        if (port.name == portName) {
            QPoint circlePos = port.circle->pos();
            QPoint containerPos = container->pos();
            return containerPos + circlePos + QPoint(12, 12);  // Center of circle
        }
    }
    return QPoint();
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < connections.size(); ++i) {
        const Connection &conn = connections[i];
        QPoint start = getPortPosition(conn.fromContainer, conn.fromPort, true);
        QPoint end = getPortPosition(conn.toContainer, conn.toPort, false);

        if (!start.isNull() && !end.isNull()) {
            QPainterPath path = createConnectionPath(start, end);

            // Different style for selected vs normal
            if (i == selectedConnectionIndex) {
                QPen pen(QColor("#0066CC"), 3);  // Blue, thicker
                painter.setPen(pen);
            } else {
                QPen pen(conn.fromContainer->getColor(), 2);  // Source container color
                painter.setPen(pen);
            }

            painter.drawPath(path);
        }
    }

    // Draw pending connection (being dragged)
    if (hasPendingConnection && pendingFromContainer) {
        QPoint start = getPortPosition(pendingFromContainer, pendingFromPort, pendingIsOutput);

        if (!start.isNull()) {
            QPainterPath path = createConnectionPath(start, pendingEndPoint);

            QPen pen(pendingFromContainer->getColor(), 2, Qt::DashLine);
            painter.setPen(pen);
            painter.drawPath(path);
        }
    }
}


void Canvas::startPendingConnection(Container *container, const QString &portName, bool isOutput)
{
    hasPendingConnection = true;
    pendingFromContainer = container;
    pendingFromPort = portName;
    pendingIsOutput = isOutput;
    pendingEndPoint = mapFromGlobal(QCursor::pos());  // Convert to local coordinates  // Initialize to current cursor
    update();
}

void Canvas::cancelPendingConnection()
{
    hasPendingConnection = false;
    pendingFromContainer = nullptr;
    pendingFromPort.clear();
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (hasPendingConnection) {
        pendingEndPoint = event->pos();
        update();
    }
}

void Canvas::selectContainer(Container *container)
{
    // Deselect previous container
    if (selectedContainer) {
        selectedContainer->setSelected(false);
    }

    // Select new container
    selectedContainer = container;
    if (selectedContainer) {
        selectedContainer->setSelected(true);
    }

    selectedConnectionIndex = -1;  // Deselect any connection
    update();
}
