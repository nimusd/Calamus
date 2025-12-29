#include "spectrumtosignal.h"
#include <algorithm>

SpectrumToSignal::SpectrumToSignal(double sampleRate)
    : sampleRate(sampleRate)
    , normalize(1.0)
    , phases(64, 0.0)
{
}

void SpectrumToSignal::reset()
{
    std::fill(phases.begin(), phases.end(), 0.0);
}

void SpectrumToSignal::setSampleRate(double rate)
{
    sampleRate = rate;
}

double SpectrumToSignal::generateSample(const Spectrum &spectrum, double pitch)
{
    int numHarmonics = spectrum.getNumHarmonics();

    // Ensure we have enough phase accumulators
    if (static_cast<int>(phases.size()) < numHarmonics) {
        phases.resize(numHarmonics, 0.0);
    }

    double output = 0.0;
    double totalAmplitude = 0.0;
    const std::vector<double> &harmonics = spectrum.getHarmonics();

    // Sum all harmonics
    for (int h = 0; h < numHarmonics; h++) {
        double amplitude = harmonics[h];
        if (amplitude > 0.0) {
            totalAmplitude += amplitude;

            // Generate sine wave for this harmonic
            output += amplitude * std::sin(phases[h]);

            // Advance phase
            int harmonicNum = h + 1;  // h=0 is fundamental, h=1 is 2nd harmonic
            phases[h] += 2.0 * M_PI * pitch * harmonicNum / sampleRate;

            // Wrap phase to prevent overflow
            if (phases[h] > 2.0 * M_PI * 1000.0) {
                phases[h] -= 2.0 * M_PI * 1000.0;
            }
        }
    }

    // Apply normalization to prevent clipping
    if (normalize > 0.0 && totalAmplitude > 0.0) {
        output /= (totalAmplitude * (1.0 - normalize) + normalize);
    }

    return output;
}
