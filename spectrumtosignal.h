#ifndef SPECTRUMTOSIGNAL_H
#define SPECTRUMTOSIGNAL_H

#include "spectrum.h"
#include <vector>
#include <cmath>

/**
 * SpectrumToSignal - Converts spectrum to audio signal
 *
 * Essential container - must have one to produce sound.
 * Sums oscillators for each harmonic to create audio output.
 */
class SpectrumToSignal
{
public:
    SpectrumToSignal(double sampleRate = 44100.0);

    // Generate a single audio sample from spectrum
    double generateSample(const Spectrum &spectrum, double pitch);

    // Reset phase accumulators (call when starting new note)
    void reset();

    // Parameter setters
    void setSampleRate(double rate);
    void setNormalize(double normalize) { this->normalize = normalize; }

private:
    double sampleRate;
    double normalize;  // 0 = off, 1 = full auto-normalize

    std::vector<double> phases;  // Phase accumulator per harmonic
};

#endif // SPECTRUMTOSIGNAL_H
