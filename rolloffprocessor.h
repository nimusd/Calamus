#ifndef ROLLOFFPROCESSOR_H
#define ROLLOFFPROCESSOR_H

#include "spectrum.h"

/**
 * RolloffProcessor - Applies brightness curve to spectrum
 *
 * The "magic ingredient" that prevents static sound.
 * Higher rolloff = darker sound (more attenuation of high harmonics)
 */
class RolloffProcessor
{
public:
    RolloffProcessor();

    // Process a spectrum with the rolloff curve
    void processSpectrum(const Spectrum &input, Spectrum &output, double rolloffPower);

    // Parameter setters
    void setRolloffPower(double rolloff) { this->rolloffPower = rolloff; }
    double getRolloffPower() const { return rolloffPower; }

private:
    double rolloffPower;  // Default rolloff value
};

#endif // ROLLOFFPROCESSOR_H
