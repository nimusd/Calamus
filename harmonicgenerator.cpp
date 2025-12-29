    #include "harmonicgenerator.h"
#include <algorithm>
#include <QVector>

HarmonicGenerator::HarmonicGenerator(double sampleRate)
    : sampleRate(sampleRate)
    , numHarmonics(32)
    , fundamentalHz(131.0)
    , rolloffPower(1.82)  // Lab default: 91/50.0
    , dnaPreset(-1)  // Default: custom (uses rolloffPower)
    , purity(0.0)  // Default: pure DNA preset, no blending
    , drift(0.0)   // Default: no drift
    , phase(0.0)
{
    harmonicPhases.resize(64, 0.0);
    harmonicAmplitudes.resize(64, 0.0);
    driftOffsets.resize(64, 0.0);
    updateHarmonicAmplitudes();
}

void HarmonicGenerator::reset()
{
    phase = 0.0;
    std::fill(harmonicPhases.begin(), harmonicPhases.end(), 0.0);
}

void HarmonicGenerator::setNumHarmonics(int count)
{
    numHarmonics = std::clamp(count, 1, 64);
    updateHarmonicAmplitudes();
}

void HarmonicGenerator::setFundamentalHz(double hz)
{
    fundamentalHz = std::clamp(hz, 20.0, 8000.0);
}

void HarmonicGenerator::setRolloffPower(double power)
{
    rolloffPower = std::clamp(power, 0.2, 5.0);
    updateHarmonicAmplitudes();
}

void HarmonicGenerator::setSampleRate(double rate)
{
    sampleRate = rate;
}

void HarmonicGenerator::setDnaPreset(int preset)
{
    dnaPreset = preset;
    updateHarmonicAmplitudes();
}

void HarmonicGenerator::setPurity(double p)
{
    purity = std::clamp(p, 0.0, 1.0);
    updateHarmonicAmplitudes();  // Recalculate amplitudes with new purity
}

void HarmonicGenerator::setDrift(double d)
{
    drift = std::clamp(d, 0.0, 0.1);
    // Note: drift affects phase, not amplitudes, so no need to update amplitudes
}

void HarmonicGenerator::setCustomAmplitudes(const std::vector<double> &amplitudes)
{
    // Set custom harmonic amplitudes and store the pattern
    int count = std::min(static_cast<int>(amplitudes.size()), 64);

    // Store the custom DNA pattern so it can be used as base for purity blending
    customDnaPattern.resize(64, 0.0);
    for (int i = 0; i < count; i++) {
        customDnaPattern[i] = amplitudes[i];
    }

    // Update numHarmonics to match
    numHarmonics = count;

    // Set dnaPreset to -1 to indicate custom mode
    dnaPreset = -1;

    // Regenerate harmonicAmplitudes with purity blend applied
    updateHarmonicAmplitudes();
}

void HarmonicGenerator::updateHarmonicAmplitudes()
{
    // First, generate the base DNA pattern
    QVector<double> basePattern(numHarmonics, 0.0);
    double totalAmp = 0.0;

    switch (dnaPreset) {
    case 0: // Vocal (Bright)
        for (int h = 0; h < numHarmonics; h++) {
            int harmonicNum = h + 1;
            double amp = 1.0 / std::pow(harmonicNum, 1.2);
            // Boost formant regions
            if ((harmonicNum >= 3 && harmonicNum <= 5) || (harmonicNum >= 8 && harmonicNum <= 12)) {
                amp *= 1.4;
            }
            basePattern[h] = amp;
            totalAmp += amp;
        }
        break;

    case 1: // Vocal (Warm)
        for (int h = 0; h < numHarmonics; h++) {
            int harmonicNum = h + 1;
            double amp = 1.0 / std::pow(harmonicNum, 1.6);
            // Boost low formant region
            if (harmonicNum >= 2 && harmonicNum <= 4) {
                amp *= 1.5;
            }
            basePattern[h] = amp;
            totalAmp += amp;
        }
        break;

    case 2: // Brass (Trumpet-like)
        for (int h = 0; h < numHarmonics; h++) {
            int harmonicNum = h + 1;
            double amp = 1.0 / std::pow(harmonicNum, 1.5);
            // Emphasize odd harmonics
            if (harmonicNum % 2 == 1) {
                amp *= 1.3;
            }
            basePattern[h] = amp;
            totalAmp += amp;
        }
        break;

    case 3: // Reed (Clarinet-like)
        for (int h = 0; h < numHarmonics; h++) {
            int harmonicNum = h + 1;
            if (harmonicNum % 2 == 1) {
                // Odd harmonics - strong
                double amp = 1.0 / std::pow(harmonicNum, 1.4);
                basePattern[h] = amp;
                totalAmp += amp;
            } else {
                // Even harmonics - very weak
                double amp = 0.1 / std::pow(harmonicNum, 2.0);
                basePattern[h] = amp;
                totalAmp += amp;
            }
        }
        break;

    case 4: // String (Violin-like)
        for (int h = 0; h < numHarmonics; h++) {
            int harmonicNum = h + 1;
            double amp = 1.0 / std::pow(harmonicNum, 1.3);
            // Emphasis on harmonics 5-10
            if (harmonicNum >= 5 && harmonicNum <= 10) {
                amp *= 1.2;
            }
            basePattern[h] = amp;
            totalAmp += amp;
        }
        break;

    default: // Custom (-1) - use stored custom DNA pattern if available
        if (!customDnaPattern.empty() && dnaPreset == -1) {
            // Use the stored custom DNA pattern as the base
            for (int h = 0; h < numHarmonics; h++) {
                basePattern[h] = customDnaPattern[h];
                totalAmp += basePattern[h];
            }
        } else {
            // Fallback to rolloffPower-based generation
            for (int h = 0; h < numHarmonics; h++) {
                int harmonicNum = h + 1;
                double amp = 1.0 / std::pow(harmonicNum, rolloffPower);
                basePattern[h] = amp;
                totalAmp += amp;
            }
        }
        break;
    }

    // Normalize base pattern
    if (totalAmp > 0.0) {
        for (int h = 0; h < numHarmonics; h++) {
            basePattern[h] /= totalAmp;
        }
    }

    // Apply purity blend: mix DNA pattern with flat spectrum
    // purity 0.0 = pure DNA pattern
    // purity 1.0 = all harmonics equal (flat with gentle rolloff)
    totalAmp = 0.0;
    for (int h = 0; h < numHarmonics; h++) {
        int harmonicNum = h + 1;
        double dnaAmp = basePattern[h];
        double flatAmp = 1.0 / harmonicNum;  // Flat spectrum with gentle rolloff
        harmonicAmplitudes[h] = dnaAmp * (1.0 - purity) + flatAmp * purity;
        totalAmp += harmonicAmplitudes[h];
    }

    // Final normalization
    if (totalAmp > 0.0) {
        for (int h = 0; h < numHarmonics; h++) {
            harmonicAmplitudes[h] /= totalAmp;
        }
    }
}

double HarmonicGenerator::generateSample()
{
    // Generate harmonics using pre-calculated amplitudes (includes purity blend)
    double voiceSample = 0.0;

    for (int h = 0; h < numHarmonics; h++)
    {
        int harmonicNum = h + 1;

        // Use pre-calculated amplitude (already normalized and purity-blended)
        double amp = harmonicAmplitudes[h];

        // Calculate phase for this harmonic with drift
        double harmonicPhase = phase * harmonicNum;

        // Apply drift: slowly varying frequency offsets for organic sound
        if (drift > 0.0) {
            // Update drift offset very slowly using low-frequency oscillation
            driftOffsets[h] += drift * 0.0001 * std::sin(phase * 0.023 + h * 1.3);
            // Clamp drift offset
            driftOffsets[h] = std::clamp(driftOffsets[h], -drift, drift);
            // Apply drift to phase
            harmonicPhase += driftOffsets[h] * 2.0 * M_PI;
        }

        // Generate sine wave
        voiceSample += amp * std::sin(harmonicPhase);
    }

    // Advance phase
    phase += 2.0 * M_PI * fundamentalHz / sampleRate;

    // Wrap phase to prevent overflow
    if (phase > 2.0 * M_PI * 1000.0)
        phase -= 2.0 * M_PI * 1000.0;

    return voiceSample;
}
