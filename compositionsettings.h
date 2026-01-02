#ifndef COMPOSITIONSETTINGS_H
#define COMPOSITIONSETTINGS_H

#include <QString>
#include <QJsonObject>

/**
 * CompositionSettings - Global composition metadata
 *
 * Stores tempo, time signature, length, and other composition-level settings.
 * Persisted to .calamus file format via JSON serialization.
 */
struct CompositionSettings
{
    // Identity
    QString compositionName;

    // Time settings
    enum TimeMode { Absolute, Musical };
    TimeMode timeMode;

    // Musical settings
    int tempo;              // BPM (20-300)
    int timeSigTop;         // Numerator (1-99)
    int timeSigBottom;      // Denominator (0-99, 0=simple mode)

    // Length
    double lengthMs;        // Length in milliseconds (for Absolute mode)
    int lengthBars;         // Length in bars (for Musical mode)

    // Constructors
    CompositionSettings();  // Default values

    // Factory method for default settings
    static CompositionSettings defaults();

    // Conversion utilities
    double calculateBarDuration() const;
    void syncLengthFromBars();  // Update lengthMs from lengthBars
    void syncLengthFromMs();    // Update lengthBars from lengthMs

    // Serialization
    QJsonObject toJson() const;
    static CompositionSettings fromJson(const QJsonObject &json);

    // Validation
    bool isValid() const;
};

#endif // COMPOSITIONSETTINGS_H
