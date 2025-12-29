#include "rolloffprocessor.h"
#include <cmath>

RolloffProcessor::RolloffProcessor()
    : rolloffPower(0.6)
{
}

void RolloffProcessor::processSpectrum(const Spectrum &input, Spectrum &output, double rolloffPower)
{
    int numHarmonics = input.getNumHarmonics();
    output.resize(numHarmonics);

    // Apply rolloff curve: amplitudeOut[h] = amplitudeIn[h] / pow(h, rolloff)
    for (int h = 0; h < numHarmonics; h++) {
        int harmonicNum = h + 1;  // h=0 is fundamental (harmonic #1)
        double inputAmp = input.getAmplitude(h);
        double outputAmp = inputAmp / std::pow(harmonicNum, rolloffPower);
        output.setAmplitude(h, outputAmp);
    }
}
