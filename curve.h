#ifndef CURVE_H
#define CURVE_H

#include <QVector>
#include <QPair>

/**
 * Curve - Stores parameter values over time
 *
 * Used for dynamics, pitch modulation, and other time-varying parameters.
 * Stores (time, value) pairs and provides linear interpolation between points.
 */
class Curve
{
public:
    // Point in the curve: (normalized time 0.0-1.0, value)
    struct Point {
        double time;   // Normalized time within note (0.0 = start, 1.0 = end)
        double value;  // Parameter value

        Point(double t = 0.0, double v = 0.0) : time(t), value(v) {}
    };

    Curve();
    explicit Curve(double constantValue);  // Create constant curve

    // Point management
    void addPoint(double time, double value);
    void addPoint(const Point &point);
    void clearPoints();
    int getPointCount() const { return points.size(); }
    const QVector<Point>& getPoints() const { return points; }

    // Value query with interpolation
    double valueAt(double time) const;

    // Utility
    bool isEmpty() const { return points.isEmpty(); }
    void sortPoints();  // Ensure points are sorted by time

private:
    QVector<Point> points;
};

#endif // CURVE_H
