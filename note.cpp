#include "note.h"

Note::Note()
    : id(QUuid::createUuid().toString())
    , startTime(0.0)
    , duration(1000.0)  // Default 1 second
    , pitchHz(440.0)    // Default A4
    , trackIndex(0)       // Default to track 0
    , dynamicsCurve(0.5)  // Default medium dynamics (constant curve)
    , bottomCurve(0.6)    // Default bottom curve value (spectrum placeholder)
{
}

Note::Note(double startTime, double duration, double pitchHz, double dynamics)
    : id(QUuid::createUuid().toString())
    , startTime(startTime)
    , duration(duration)
    , pitchHz(pitchHz)
    , trackIndex(0)           // Default to track 0
    , dynamicsCurve(dynamics)  // Create constant dynamics curve
    , bottomCurve(0.6)         // Default bottom curve value
{
}

double Note::getDynamics() const
{
    // Return average dynamics across the curve
    if (dynamicsCurve.isEmpty()) {
        return 0.5;  // Default
    }

    // Sample at several points and average
    const int numSamples = 10;
    double sum = 0.0;
    for (int i = 0; i < numSamples; ++i) {
        double t = static_cast<double>(i) / (numSamples - 1);
        sum += dynamicsCurve.valueAt(t);
    }
    return sum / numSamples;
}

double Note::getDynamicsAt(double normalizedTime) const
{
    return dynamicsCurve.valueAt(normalizedTime);
}

double Note::getPitchAt(double normalizedTime) const
{
    // If pitch curve exists, use it; otherwise return constant pitch
    if (pitchCurve.getPoints().size() > 0) {
        return pitchCurve.valueAt(normalizedTime);
    }
    return pitchHz;  // Constant pitch
}

void Note::setDynamics(double dyn)
{
    // Set constant dynamics (flat curve)
    dynamicsCurve.clearPoints();
    dynamicsCurve.addPoint(0.0, dyn);
    dynamicsCurve.addPoint(1.0, dyn);
}

double Note::getBottomCurveAt(double normalizedTime) const
{
    return bottomCurve.valueAt(normalizedTime);
}
