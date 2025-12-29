#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include <QJsonObject>
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
        QString function = "passthrough";  // Connection function
        double weight = 1.0;               // Connection weight
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

    // Get selected connection (nullptr if none selected)
    Connection* getSelectedConnection() {
        if (selectedConnectionIndex >= 0 && selectedConnectionIndex < connections.size()) {
            return &connections[selectedConnectionIndex];
        }
        return nullptr;
    }

    // Sounit save/load
    bool saveToJson(const QString &filePath, const QString &sounitName) const;
    bool loadFromJson(const QString &filePath, QString &outSounitName);
    QString getSounitName() const { return sounitName; }
    void setSounitName(const QString &name);
    QString getSounitComment() const { return sounitComment; }
    void setSounitComment(const QString &comment);

signals:
    void containerSelected(Container *container);
    void connectionSelected(int connectionIndex);
    void selectionCleared();
    void graphChanged();  // Emitted when containers/connections change
    void sounitNameChanged(const QString &name);
    void sounitCommentChanged(const QString &comment);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QVector<Connection> connections;
    QString sounitName = "Untitled Sounit";
    QString sounitComment;

    QPoint getPortPosition(Container *container, const QString &portName, bool isOutput);
    int selectedConnectionIndex = -1;
    QPainterPath createConnectionPath(const QPoint &start, const QPoint &end);

    // Serialization helpers
    QJsonObject serializeContainer(const Container *container) const;
    Container* deserializeContainer(const QJsonObject &json, QWidget *parent);
    QJsonObject serializeConnection(const Connection &conn) const;
};

#endif // CANVAS_H
