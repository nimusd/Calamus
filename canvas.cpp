#include "canvas.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include "envelopelibraryDialog.h"

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
    if (selectedContainer) {
        selectedContainer->setSelected(false);
        selectedContainer = nullptr;
    }
    emit selectionCleared();
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

    bool somethingSelected = false;

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
                    emit connectionSelected(i);
                    somethingSelected = true;
                    break;
                }
            }
        }
    }

    if (!somethingSelected) {
        emit selectionCleared();
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
            // Remove from parent first to prevent findChildren() from finding it during graph rebuild
            selectedContainer->setParent(nullptr);
            selectedContainer->deleteLater();
            selectedContainer = nullptr;
            update();

            // Notify that graph structure changed
            emit graphChanged();
        }
        else if (selectedConnectionIndex >= 0 && selectedConnectionIndex < connections.size()) {
            connections.removeAt(selectedConnectionIndex);
            selectedConnectionIndex = -1;
            update();

            // Notify that graph structure changed
            emit graphChanged();
        }
        event->accept();
        return;
    }

    // Pass unhandled key events to parent widget (SounitBuilder)
    QWidget::keyPressEvent(event);
}

void Canvas::addConnection(const Connection &conn)
{
    connections.append(conn);
    update();

    // Notify that graph structure changed
    emit graphChanged();
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
        emit containerSelected(container);
    }

    selectedConnectionIndex = -1;  // Deselect any connection
    update();
}

void Canvas::setSounitName(const QString &name)
{
    sounitName = name;
    emit sounitNameChanged(name);
}

void Canvas::setSounitComment(const QString &comment)
{
    sounitComment = comment;
    emit sounitCommentChanged(comment);
}

QJsonObject Canvas::serializeContainer(const Container *container) const
{
    QJsonObject json;

    // Basic info
    json["type"] = container->getName();
    json["instanceName"] = container->getInstanceName();
    json["color"] = container->getColor().name();

    // Position
    QJsonObject pos;
    pos["x"] = container->pos().x();
    pos["y"] = container->pos().y();
    json["position"] = pos;

    // Parameters
    QJsonObject params;
    QMap<QString, double> containerParams = container->getParameters();
    for (auto it = containerParams.begin(); it != containerParams.end(); ++it) {
        // Skip customDna_N parameters - they'll be stored separately
        if (!it.key().startsWith("customDna_") && it.key() != "customDnaCount") {
            params[it.key()] = it.value();
        }
    }
    json["parameters"] = params;

    // Custom DNA (only if using custom mode)
    if (containerParams.value("dnaSelect", 0.0) == -1.0) {
        int count = static_cast<int>(containerParams.value("customDnaCount", 0.0));
        if (count > 0) {
            QJsonObject customDna;
            customDna["count"] = count;
            QJsonArray amps;
            for (int i = 0; i < count; i++) {
                QString key = QString("customDna_%1").arg(i);
                amps.append(containerParams.value(key, 0.0));
            }
            customDna["amplitudes"] = amps;
            json["customDna"] = customDna;
        }
    }

    // Custom Envelope (only if present)
    if (container->hasCustomEnvelopeData()) {
        EnvelopeData envData = container->getCustomEnvelopeData();
        QJsonObject customEnv;
        customEnv["name"] = envData.name;
        customEnv["loopMode"] = envData.loopMode;

        QJsonArray points;
        for (const EnvelopePoint &pt : envData.points) {
            QJsonObject ptObj;
            ptObj["time"] = pt.time;
            ptObj["value"] = pt.value;
            ptObj["curveType"] = pt.curveType;
            points.append(ptObj);
        }
        customEnv["points"] = points;
        json["customEnvelope"] = customEnv;
    }

    return json;
}

QJsonObject Canvas::serializeConnection(const Connection &conn) const
{
    QJsonObject json;
    json["fromContainer"] = conn.fromContainer->getInstanceName();
    json["fromPort"] = conn.fromPort;
    json["toContainer"] = conn.toContainer->getInstanceName();
    json["toPort"] = conn.toPort;
    json["function"] = conn.function;
    json["weight"] = conn.weight;
    return json;
}

bool Canvas::saveToJson(const QString &filePath, const QString &sounitName) const
{
    QJsonObject root;

    // Sounit metadata
    QJsonObject sounitMeta;
    sounitMeta["name"] = sounitName;
    sounitMeta["version"] = "1.0";
    sounitMeta["created"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    sounitMeta["comment"] = sounitComment;
    root["sounit"] = sounitMeta;

    // Serialize containers
    QJsonArray containersArray;
    QList<Container*> containers = findChildren<Container*>();
    for (const Container *container : containers) {
        containersArray.append(serializeContainer(container));
    }
    root["containers"] = containersArray;

    // Serialize connections
    QJsonArray connectionsArray;
    for (const Connection &conn : connections) {
        connectionsArray.append(serializeConnection(conn));
    }
    root["connections"] = connectionsArray;

    // Write to file
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    qDebug() << "Sounit saved to:" << filePath;
    return true;
}

// Static helper to map container type to port lists
static void getPortsForContainerType(const QString &type, QStringList &inputs, QStringList &outputs)
{
    if (type == "Harmonic Generator") {
        inputs = {"purity", "drift"};
        outputs = {"spectrum"};
    } else if (type == "Spectrum to Signal") {
        inputs = {"spectrumIn", "pitch", "normalize"};
        outputs = {"signalOut"};
    } else if (type == "Rolloff Processor") {
        inputs = {"spectrumIn", "rolloff"};
        outputs = {"spectrumOut"};
    } else if (type == "Formant Body") {
        inputs = {"signalIn", "f1Freq", "f2Freq", "f1Q", "f2Q", "directMix", "f1f2Balance"};
        outputs = {"signalOut"};
    } else if (type == "Breath Turbulence") {
        inputs = {"voiceIn", "noiseIn", "blend"};
        outputs = {"signalOut"};
    } else if (type == "Noise Color Filter") {
        inputs = {"audioIn", "color", "filterQ"};
        outputs = {"noiseOut"};
    } else if (type == "Physics System") {
        inputs = {"targetValue", "mass", "springK", "damping", "impulse", "impulseAmount"};
        outputs = {"currentValue"};
    } else if (type == "Easing Applicator") {
        inputs = {"startValue", "endValue", "progress", "easingSelect"};
        outputs = {"easedValue"};
    } else if (type == "Envelope Engine") {
        inputs = {"timeScale", "valueScale", "valueOffset"};
        outputs = {"envelopeValue"};
    } else if (type == "Drift Engine") {
        inputs = {"amount", "rate"};
        outputs = {"driftOut"};
    } else if (type == "Gate Processor") {
        inputs = {"velocity", "attackTime", "releaseTime", "attackCurve", "releaseCurve", "velocitySens"};
        outputs = {"envelopeOut", "stateOut", "attackTrigger", "releaseTrigger"};
    } else {
        qWarning() << "Unknown container type:" << type;
    }
}

Container* Canvas::deserializeContainer(const QJsonObject &json, QWidget *parent)
{
    QString type = json["type"].toString();
    QString instanceName = json["instanceName"].toString();
    QColor color(json["color"].toString());

    // Get position
    QJsonObject posObj = json["position"].toObject();
    QPoint position(posObj["x"].toInt(), posObj["y"].toInt());

    // Determine inputs/outputs based on type
    QStringList inputs, outputs;
    getPortsForContainerType(type, inputs, outputs);

    if (inputs.isEmpty() && outputs.isEmpty()) {
        qWarning() << "Cannot deserialize unknown container type:" << type;
        return nullptr;
    }

    // Create container
    Container *container = new Container(parent, type, color, inputs, outputs);
    container->setInstanceName(instanceName);
    container->move(position);

    // Restore parameters
    QJsonObject params = json["parameters"].toObject();
    for (auto it = params.begin(); it != params.end(); ++it) {
        container->setParameter(it.key(), it.value().toDouble());
    }

    // Restore custom DNA if present
    if (json.contains("customDna")) {
        QJsonObject customDna = json["customDna"].toObject();
        int count = customDna["count"].toInt();
        QJsonArray amps = customDna["amplitudes"].toArray();

        container->setParameter("customDnaCount", static_cast<double>(count));
        container->setParameter("dnaSelect", -1.0);  // Mark as custom
        for (int i = 0; i < count && i < amps.size(); i++) {
            QString key = QString("customDna_%1").arg(i);
            container->setParameter(key, amps[i].toDouble());
        }
    }

    // Restore custom envelope if present
    if (json.contains("customEnvelope")) {
        QJsonObject envObj = json["customEnvelope"].toObject();
        EnvelopeData envData;
        envData.name = envObj["name"].toString();
        envData.loopMode = envObj["loopMode"].toInt();
        envData.isFactory = false;

        QJsonArray pointsArray = envObj["points"].toArray();
        for (const QJsonValue &ptVal : pointsArray) {
            QJsonObject ptObj = ptVal.toObject();
            EnvelopePoint pt;
            pt.time = ptObj["time"].toDouble();
            pt.value = ptObj["value"].toDouble();
            pt.curveType = ptObj["curveType"].toInt();
            envData.points.append(pt);
        }

        container->setCustomEnvelopeData(envData);
    }

    return container;
}

bool Canvas::loadFromJson(const QString &filePath, QString &outSounitName)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON format";
        return false;
    }

    QJsonObject root = doc.object();

    // Read sounit metadata
    QJsonObject sounitMeta = root["sounit"].toObject();
    outSounitName = sounitMeta["name"].toString("Untitled Sounit");
    setSounitName(outSounitName);

    // Load comment
    QString comment = sounitMeta["comment"].toString("");
    setSounitComment(comment);

    // Clear existing containers and connections
    QList<Container*> existingContainers = findChildren<Container*>();
    for (Container *c : existingContainers) {
        c->deleteLater();
    }
    connections.clear();
    selectedConnectionIndex = -1;
    selectedContainer = nullptr;

    // Deserialize containers
    QMap<QString, Container*> containerMap;  // instanceName -> Container*
    QJsonArray containersArray = root["containers"].toArray();

    for (const QJsonValue &val : containersArray) {
        QJsonObject containerJson = val.toObject();
        Container *container = deserializeContainer(containerJson, this);

        if (!container) {
            qWarning() << "Failed to deserialize container";
            continue;
        }

        // Check for duplicate instance names and auto-rename if needed
        QString originalName = container->getInstanceName();
        if (containerMap.contains(originalName)) {
            qWarning() << "Duplicate instance name detected:" << originalName;
            QString baseName = originalName;
            int suffix = 1;
            QString newName;
            do {
                newName = QString("%1-%2").arg(baseName).arg(suffix++);
            } while (containerMap.contains(newName));
            container->setInstanceName(newName);
            qDebug() << "Renamed to:" << newName;
        }

        // Show container
        container->show();

        // Store in map for connection reconstruction
        containerMap[container->getInstanceName()] = container;
    }

    // Deserialize connections (after all containers exist)
    QJsonArray connectionsArray = root["connections"].toArray();
    int skippedConnections = 0;

    for (const QJsonValue &val : connectionsArray) {
        QJsonObject connJson = val.toObject();

        QString fromName = connJson["fromContainer"].toString();
        QString toName = connJson["toContainer"].toString();

        if (!containerMap.contains(fromName) || !containerMap.contains(toName)) {
            qWarning() << "Connection references missing container:" << fromName << "->" << toName;
            skippedConnections++;
            continue;
        }

        Connection conn;
        conn.fromContainer = containerMap[fromName];
        conn.fromPort = connJson["fromPort"].toString();
        conn.toContainer = containerMap[toName];
        conn.toPort = connJson["toPort"].toString();
        conn.function = connJson["function"].toString("passthrough");
        conn.weight = connJson["weight"].toDouble(1.0);

        // Validate ports exist
        if (!conn.fromContainer->getOutputPorts().contains(conn.fromPort)) {
            qWarning() << "Invalid output port:" << conn.fromPort << "on" << fromName;
            skippedConnections++;
            continue;
        }
        if (!conn.toContainer->getInputPorts().contains(conn.toPort)) {
            qWarning() << "Invalid input port:" << conn.toPort << "on" << toName;
            skippedConnections++;
            continue;
        }

        connections.append(conn);
    }

    update();
    emit graphChanged();

    if (skippedConnections > 0) {
        qWarning() << "Skipped" << skippedConnections << "invalid connections during load";
    }

    qDebug() << "Sounit loaded from:" << filePath;
    qDebug() << "  Containers:" << containerMap.size();
    qDebug() << "  Connections:" << connections.size();

    return true;
}
