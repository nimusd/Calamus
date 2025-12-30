#include "phrasegroup.h"
#include <QJsonArray>
#include <algorithm>

PhraseGroup::PhraseGroup()
    : id(QUuid::createUuid().toString())
    , name("New Phrase")
    , color(QColor(100, 150, 250))  // Default blue
    , useEasing(false)
    , easingType("Linear")
    , verticalPadding(50)  // Default 50 pixels padding
{
}

PhraseGroup::PhraseGroup(const QString &name)
    : id(QUuid::createUuid().toString())
    , name(name)
    , color(QColor(100, 150, 250))  // Default blue
    , useEasing(false)
    , easingType("Linear")
    , verticalPadding(50)  // Default 50 pixels padding
{
}

void PhraseGroup::addNoteIndex(int index)
{
    if (!noteIndices.contains(index)) {
        noteIndices.append(index);
    }
}

void PhraseGroup::removeNoteIndex(int index)
{
    noteIndices.removeAll(index);
}

void PhraseGroup::clearNoteIndices()
{
    noteIndices.clear();
}

bool PhraseGroup::containsNote(int index) const
{
    return noteIndices.contains(index);
}

void PhraseGroup::updateBounds(const QVector<QPointF> &noteCorners)
{
    // For MVP: Simple bounding box
    // Future enhancement: Implement proper convex hull algorithm
    if (noteCorners.isEmpty()) {
        hullPoints.clear();
        return;
    }

    // Find min/max x and y
    double minX = noteCorners.first().x();
    double maxX = minX;
    double minY = noteCorners.first().y();
    double maxY = minY;

    for (const QPointF &corner : noteCorners) {
        minX = std::min(minX, corner.x());
        maxX = std::max(maxX, corner.x());
        minY = std::min(minY, corner.y());
        maxY = std::max(maxY, corner.y());
    }

    // Add padding
    const double padding = 10.0;
    minX -= padding;
    maxX += padding;
    minY -= padding;
    maxY += padding;

    // Create bounding rectangle
    hullPoints.clear();
    hullPoints.append(QPointF(minX, minY));  // Top-left
    hullPoints.append(QPointF(maxX, minY));  // Top-right
    hullPoints.append(QPointF(maxX, maxY));  // Bottom-right
    hullPoints.append(QPointF(minX, maxY));  // Bottom-left
}

QJsonObject PhraseGroup::toJson() const
{
    QJsonObject json;
    json["id"] = id;
    json["name"] = name;
    json["color"] = color.name();
    json["useEasing"] = useEasing;
    json["easingType"] = easingType;
    json["verticalPadding"] = verticalPadding;

    // Serialize dynamics curve with pressure data
    QJsonArray dynamicsPoints;
    for (const Curve::Point &pt : dynamicsCurve.getPoints()) {
        QJsonObject pointObj;
        pointObj["time"] = pt.time;
        pointObj["value"] = pt.value;
        pointObj["pressure"] = pt.pressure;
        dynamicsPoints.append(pointObj);
    }
    json["dynamicsCurve"] = dynamicsPoints;

    // Serialize vibrato curve
    QJsonArray vibratoPoints;
    for (const Curve::Point &pt : vibratoCurve.getPoints()) {
        QJsonObject pointObj;
        pointObj["time"] = pt.time;
        pointObj["value"] = pt.value;
        pointObj["pressure"] = pt.pressure;
        vibratoPoints.append(pointObj);
    }
    json["vibratoCurve"] = vibratoPoints;

    // Serialize rhythmic curve
    QJsonArray rhythmicPoints;
    for (const Curve::Point &pt : rhythmicCurve.getPoints()) {
        QJsonObject pointObj;
        pointObj["time"] = pt.time;
        pointObj["value"] = pt.value;
        pointObj["pressure"] = pt.pressure;
        rhythmicPoints.append(pointObj);
    }
    json["rhythmicCurve"] = rhythmicPoints;

    return json;
}

PhraseGroup PhraseGroup::fromJson(const QJsonObject &json)
{
    PhraseGroup phrase;
    phrase.id = json["id"].toString();
    phrase.name = json["name"].toString();
    phrase.color = QColor(json["color"].toString());
    phrase.useEasing = json["useEasing"].toBool();
    phrase.easingType = json["easingType"].toString();
    phrase.verticalPadding = json["verticalPadding"].toInt(50);  // Default to 50 if not present

    // Deserialize dynamics curve
    QJsonArray dynamicsPoints = json["dynamicsCurve"].toArray();
    phrase.dynamicsCurve.clearPoints();
    for (const QJsonValue &val : dynamicsPoints) {
        QJsonObject pointObj = val.toObject();
        phrase.dynamicsCurve.addPoint(
            pointObj["time"].toDouble(),
            pointObj["value"].toDouble(),
            pointObj["pressure"].toDouble()
        );
    }

    // Deserialize vibrato curve
    QJsonArray vibratoPoints = json["vibratoCurve"].toArray();
    phrase.vibratoCurve.clearPoints();
    for (const QJsonValue &val : vibratoPoints) {
        QJsonObject pointObj = val.toObject();
        phrase.vibratoCurve.addPoint(
            pointObj["time"].toDouble(),
            pointObj["value"].toDouble(),
            pointObj["pressure"].toDouble()
        );
    }

    // Deserialize rhythmic curve
    QJsonArray rhythmicPoints = json["rhythmicCurve"].toArray();
    phrase.rhythmicCurve.clearPoints();
    for (const QJsonValue &val : rhythmicPoints) {
        QJsonObject pointObj = val.toObject();
        phrase.rhythmicCurve.addPoint(
            pointObj["time"].toDouble(),
            pointObj["value"].toDouble(),
            pointObj["pressure"].toDouble()
        );
    }

    return phrase;
}
