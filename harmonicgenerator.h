#ifndef HARMONICGENERATOR_H
#define HARMONICGENERATOR_H

#include <cmath>
#include <vector>

/**
 * HarmonicGenerator - Minimal additive synthesis (Phase 3: Option A)
 *
 * Generates rich harmonic content shaped by rolloff.
 * This is the "bones" from the lab - harmonics only, no formants yet.
 *
 * Parameters:
 * - numHarmonics: Number of harmonics to generate (1-64)
 * - fundamentalHz: Base frequency in Hz
 * - rolloffPower: Controls brightness (low = bright/buzzy, high = dark/mellow)
 */
class HarmonicGenerator
{
public:
    HarmonicGenerator(double sampleRate = 44100.0);

    // Generate a single audio sample
    double generateSample();

    // Reset phase (call when starting a new note)
    void reset();

    // Parameter setters
    void setNumHarmonics(int count);
    void setFundamentalHz(double hz);
    void setRolloffPower(double power);
    void setSampleRate(double rate);
    void setDnaPreset(int preset);  // 0-4 for presets, -1 for custom
    void setPurity(double purity);  // 0.0-1.0, controls harmonic purity
    void setDrift(double drift);    // 0.0-0.1, controls frequency drift amount
    void setCustomAmplitudes(const std::vector<double> &amplitudes);  // Set custom harmonic pattern

    // Parameter getters
    int getNumHarmonics() const { return numHarmonics; }
    double getFundamentalHz() const { return fundamentalHz; }
    double getRolloffPower() const { return rolloffPower; }
    int getDnaPreset() const { return dnaPreset; }

    // Get pre-calculated harmonic amplitude for a specific harmonic
    double getHarmonicAmplitude(int harmonicIndex) const {
        if (harmonicIndex >= 0 && harmonicIndex < harmonicAmplitudes.size()) {
            return harmonicAmplitudes[harmonicIndex];
        }
        return 0.0;
    }

private:
    double sampleRate;
    int numHarmonics;
    double fundamentalHz;
    double rolloffPower;
    int dnaPreset;  // -1 = custom, 0-4 = presets
    double purity;  // 0.0-1.0, controls harmonic purity
    double drift;   // 0.0-0.1, controls frequency drift

    double phase;  // Master phase accumulator
    std::vector<double> harmonicPhases;  // Phase for each harmonic
    std::vector<double> harmonicAmplitudes;  // Cached amplitudes for each harmonic
    std::vector<double> driftOffsets;  // Random drift offsets for each harmonic
    std::vector<double> customDnaPattern;  // Stored custom DNA pattern (when dnaPreset == -1)

    // Recalculate harmonic amplitudes based on DNA preset
    void updateHarmonicAmplitudes();
};

#endif // HARMONICGENERATOR_H
