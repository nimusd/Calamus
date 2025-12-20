#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include "container.h"
#include <QKeyEvent>

class Canvas : public QWidget
{
    Q_OBJECT

public:
    struct Connection {
        Container *fromContainer;
        QString fromPort;
        Container *toContainer;
        QString toPort;
    };

    explicit Canvas(QWidget *parent = nullptr);

    void addConnection(const Connection &conn);
    QVector<Connection>& getConnections() { return connections; }
    void clearSelection();
    void startPendingConnection(Container *container, const QString &portName, bool isOutput);
    void cancelPendingConnection();

    // Pending connection (being dragged)
    bool hasPendingConnection = false;
    Container *pendingFromContainer = nullptr;
    QString pendingFromPort;
    bool pendingIsOutput = true;
    QPoint pendingEndPoint;

    Container *selectedContainer = nullptr;
    void selectContainer(Container *container);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QVector<Connection> connections;

    QPoint getPortPosition(Container *container, const QString &portName, bool isOutput);
    int selectedConnectionIndex = -1;
    QPainterPath createConnectionPath(const QPoint &start, const QPoint &end);
};

#endif // CANVAS_H
