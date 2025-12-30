#ifndef NOTE_H
#define NOTE_H

#include "curve.h"
#include <QString>
#include <QUuid>

/**
 * Note - The compositional atom
 *
 * What you see on the score canvas. Stores parameter curves captured from pen input.
 */
class Note
{
public:
    Note();
    Note(double startTime, double duration, double pitchHz, double dynamics = 0.5);

    // Getters
    QString getId() const { return id; }
    double getStartTime() const { return startTime; }
    double getDuration() const { return duration; }
    double getPitchHz() const { return pitchHz; }
    double getEndTime() const { return startTime + duration; }
    int getTrackIndex() const { return trackIndex; }

    // Pitch - supports both simple value and curve (for glissando/portamento)
    double getPitchAt(double normalizedTime) const;  // Query pitch curve at specific time
    const Curve& getPitchCurve() const { return pitchCurve; }
    Curve& getPitchCurve() { return pitchCurve; }
    bool hasPitchCurve() const { return pitchCurve.getPoints().size() > 0; }

    // Dynamics - supports both simple value and curve
    double getDynamics() const;  // Returns average dynamics
    double getDynamicsAt(double normalizedTime) const;  // Query curve at specific time
    const Curve& getDynamicsCurve() const { return dynamicsCurve; }
    Curve& getDynamicsCurve() { return dynamicsCurve; }

    // Bottom curve (spectrum/timbre for now, will be configurable later)
    double getBottomCurveAt(double normalizedTime) const;
    const Curve& getBottomCurve() const { return bottomCurve; }
    Curve& getBottomCurve() { return bottomCurve; }

    // Setters
    void setStartTime(double time) { startTime = time; }
    void setDuration(double dur) { duration = dur; }
    void setPitchHz(double pitch) { pitchHz = pitch; }
    void setDynamics(double dyn);  // Sets constant dynamics
    void setDynamicsCurve(const Curve &curve) { dynamicsCurve = curve; }
    void setBottomCurve(const Curve &curve) { bottomCurve = curve; }
    void setPitchCurve(const Curve &curve) { pitchCurve = curve; }
    void setTrackIndex(int index) { trackIndex = index; }

private:
    QString id;           // Unique identifier
    double startTime;     // Start time in milliseconds
    double duration;      // Duration in milliseconds
    double pitchHz;       // Base pitch in Hz (used when no pitch curve)
    int trackIndex;       // Which track/sounit this note uses (default 0)
    Curve pitchCurve;     // Pitch curve over time (Hz values for glissando/portamento)
    Curve dynamicsCurve;  // Dynamics curve over time (0.0-1.0)
    Curve bottomCurve;    // Bottom edge curve (spectrum/timbre placeholder, configurable later)
};

#endif // NOTE_H
