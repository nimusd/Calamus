#ifndef PHRASEGROUP_H
#define PHRASEGROUP_H

#include "curve.h"
#include <QString>
#include <QVector>
#include <QColor>
#include <QUuid>
#include <QPointF>
#include <QJsonObject>

/**
 * PhraseGroup - Musical phrase encompassing multiple notes
 *
 * Contains:
 * - References to grouped notes (indices into ScoreCanvas phrase)
 * - Phrase-level parameter curves (dynamics, vibrato, rhythmic)
 * - Visual metadata (name, color, hull boundary)
 * - Optional physics/easing configuration
 */
class PhraseGroup
{
public:
    PhraseGroup();
    explicit PhraseGroup(const QString &name);

    // Identity
    QString getId() const { return id; }
    QString getName() const { return name; }
    void setName(const QString &n) { name = n; }
    QColor getColor() const { return color; }
    void setColor(const QColor &c) { color = c; }

    // Note membership
    void addNoteIndex(int index);
    void removeNoteIndex(int index);
    void clearNoteIndices();
    const QVector<int>& getNoteIndices() const { return noteIndices; }
    bool containsNote(int index) const;
    int getNoteCount() const { return noteIndices.size(); }

    // Parameter curves
    Curve& getDynamicsCurve() { return dynamicsCurve; }
    const Curve& getDynamicsCurve() const { return dynamicsCurve; }
    void setDynamicsCurve(const Curve &curve) { dynamicsCurve = curve; }

    Curve& getVibratoCurve() { return vibratoCurve; }
    const Curve& getVibratoCurve() const { return vibratoCurve; }
    void setVibratoCurve(const Curve &curve) { vibratoCurve = curve; }

    Curve& getRhythmicCurve() { return rhythmicCurve; }  // Accelerando/ritardando
    const Curve& getRhythmicCurve() const { return rhythmicCurve; }
    void setRhythmicCurve(const Curve &curve) { rhythmicCurve = curve; }

    // Easing configuration
    bool hasEasing() const { return useEasing; }
    void setUseEasing(bool use) { useEasing = use; }
    QString getEasingType() const { return easingType; }
    void setEasingType(const QString &type) { easingType = type; }

    // Hull vertical bounds control
    int getVerticalPadding() const { return verticalPadding; }
    void setVerticalPadding(int padding) { verticalPadding = padding; }

    // Serialization
    QJsonObject toJson() const;
    static PhraseGroup fromJson(const QJsonObject &json);

    // Visual bounds (calculated from notes)
    void updateBounds(const QVector<QPointF> &noteCorners);
    const QVector<QPointF>& getHullPoints() const { return hullPoints; }

private:
    QString id;                    // Unique identifier
    QString name;                  // User-visible name
    QColor color;                  // Phrase hull color
    QVector<int> noteIndices;      // Indices of notes in this phrase

    // Parameter curves (drawn over phrase, affect all notes)
    Curve dynamicsCurve;           // Master dynamics envelope
    Curve vibratoCurve;            // Vibrato depth over time
    Curve rhythmicCurve;           // Tempo modulation (accelerando/ritardando)

    // Easing configuration
    bool useEasing;                // Apply easing to curves?
    QString easingType;            // "QuadInOut", "Elastic", etc.

    // Visual hull
    QVector<QPointF> hullPoints;   // Convex/concave hull boundary
    int verticalPadding;           // Vertical padding for hull (pixels), default 50
};

#endif // PHRASEGROUP_H
