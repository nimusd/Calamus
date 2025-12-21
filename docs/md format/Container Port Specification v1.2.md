

Calamus Container Port Specification v1.1
Container Port Specification v1.2
## Overview
This document specifies the inputs, outputs, and configuration for each container type in the
Calamus sound engine. These containers form the building blocks of sounits — connected
via the tensor architecture to create sound bodies.
## Design Principles
No Hard-Coded Limits
All numeric limits are configurable via Preferences. Reasonable defaults are provided, but
the user can extend any range. This enables unexpected discoveries — like the 1,400
harmonics that created standing waves in the room.
Implementation: A Limits/Constraints class provides min/max values. Containers query this
class rather than using magic numbers.
Everything Modulatable is an Input
Any parameter that could benefit from modulation is exposed as an Input port with a default
value. Leave it unconnected — uses default. Plug something in — it moves.
Result: Maximum flexibility. An envelope, easing curve, physics system, or any control
source can modulate any parameter.
## Modular Composition
Containers can be combined freely:
•Instantiate any container multiple times (3 HarmonicGenerators? Sure.)
•Chain containers in any order (with type compatibility)
•The signal path MUST end with a Signal output
•Skip any container by simply not including it
## Container Categories
Essential: Required for any sound
•HarmonicGenerator — creates the spectrum
•SpectrumToSignal — converts to audio (must have one to produce sound)
Shaping: Transform the sound character
•RolloffProcessor — brightness control
•FormantBody — vowel/resonance (add more instances for F3, F4, etc.)
•BreathTurbulence — voice/noise blend
•NoiseColorFilter — noise spectrum shaping
Modifiers: Affect how parameters change over time
•PhysicsSystem — mass/spring/damping (organic movement + click prevention)
•EasingApplicator — shaped transitions between values
•EnvelopeEngine — parameter evolution over note lifetime
•DriftEngine — micro-detuning for organic quality
•GateProcessor — note on/off lifecycle
Page 1 of 9

Calamus Container Port Specification v1.1
## Data Types
Signal — Audio-rate stream of samples (float, -1.0 to 1.0)
Spectrum — Array of harmonic amplitudes indexed by harmonic number
Control — Single value, can change at control rate or audio rate
Trigger — Discrete event (gate on, gate off, impulse)
## Connection Functions
When connecting an output to an input, a function determines how values combine:
•passthrough — source replaces input entirely (ignores weight)
•add — output added to input (input + source × weight)
•multiply — output multiplied with input (input × source × weight)
•subtract — output subtracted from input (input - source × weight)
•replace — weighted blend (input × (1-weight) + source × weight)
•modulate — bipolar modulation around input (input + (source - 0.5) × weight × range)
Envelope vs Easing
Both modify parameters over time, but serve different purposes:
Envelope (EnvelopeEngine):
•Arbitrary curve shape (drawn, preset, mathematical)
•Spans the note lifetime (or loops)
•Defines WHAT happens over time
•Example: 'Breath swells during note release'
Easing (EasingApplicator):
•Mathematical function (elastic, bounce, cubic, etc.)
•Spans a transition between two values
•Defines HOW movement happens
•Example: 'Pitch glides from A to B with elastic bounce'
They work together: Easing shapes the character of movement; Envelope defines the
journey.
## Reference Signal Flow
This is the typical 'human voice' configuration. The tensor architecture allows any valid
connection.
Core signal path:
•Pitch → HarmonicGenerator → RolloffProcessor → SpectrumToSignal →
- → BreathTurbulence (+ NoiseColorFilter noise input) →
- → FormantBody → Output Signal
Modifier connections:
•PhysicsSystem → FormantBody.f1Freq, FormantBody.f2Freq (smooth movement)
•PhysicsSystem → RolloffProcessor.rolloff (brightness bloom)
•DriftEngine → HarmonicGenerator.drift (organic beating)
•GateProcessor.attackTrigger → PhysicsSystem.impulse (consonant kick)
•EnvelopeEngine → BreathTurbulence.blend (breath swell on release)
Page 2 of 9

Calamus Container Port Specification v1.1
- HarmonicGenerator
Generates the fundamental harmonic spectrum — the 'bones' of the sound.
## Ports
Port NameDirectionData
## Type
DefaultDescription
pitchInputControl—Fundamental frequency in Hz
## (required)
numHarmonicsInputControl64Number of harmonics to generate
dnaSelectInputControl0DNA sequence index (preset or
custom)
purityInputControl1.0Blend: 0 = pure DNA, 1 = all
harmonics
drift
InputControl0.0Per-harmonic detuning (from
DriftEngine)
spectrumOutputSpectrum—Array of harmonic amplitudes
Configuration (Preferences)
Config OptionTypeDefaultDescription
maxHarmonicsint64Upper limit for numHarmonics (try
1400 for standing waves!)
dnaPresetslist6 presetsAll, Odd, Even, OddDominant,
## Fundamental+, Octaves
phaseModeenumrandomInitial phase: zero, random, custom
## Processing
For each harmonic h (1 to numHarmonics): amplitude[h] = dnaWeight(h) × purity + (1-purity).
Output spectrum contains amplitude per harmonic. Phase accumulators maintained
internally.
- RolloffProcessor
Applies brightness curve to spectrum — the 'magic ingredient' that prevents static sound.
## Ports
Port NameDirectionData
## Type
DefaultDescription
spectrumInInputSpectrum—Harmonic amplitudes from
HarmonicGenerator
rolloff
InputControl0.6Rolloff power (higher = darker)
spectrumOutOutputSpectrum—Modified harmonic amplitudes
Configuration (Preferences)
Config OptionTypeDefaultDescription
rolloffMinfloat0.1Minimum rolloff value
rolloffMaxfloat3.0Maximum rolloff value
rolloffCurveenum
powerShape: power, exponential, custom
## Processing
Page 3 of 9

Calamus Container Port Specification v1.1
For each harmonic h: amplitudeOut[h] = amplitudeIn[h] / pow(h, rolloff). Critical discovery:
this is what makes 64 harmonics sound different from 4 harmonics.
- SpectrumToSignal
Converts spectrum data to audio signal by summing oscillators. Essential — must have one
to produce sound.
## Ports
Port NameDirectionData
## Type
DefaultDescription
spectrumInInputSpectrum—Harmonic amplitudes
pitchInputControl—Fundamental frequency for
oscillators
normalize
InputControl1.0Auto-normalize: 0 = off, 1 = full
signalOutOutputSignal—Summed audio signal
Configuration (Preferences)
Config OptionTypeDefaultDescription
antiAliasbooltrueSkip harmonics above Nyquist
## Processing
Maintains phase accumulator per harmonic. Each sample: sum all sin(phase[h]) ×
amplitude[h], advance phases. Normalization prevents clipping when many harmonics sum.
- FormantBody
Resonant filtering creating vowel-like character — the 'throat'. Add multiple instances for F3,
F4, etc.
## Ports
Port NameDirectionData
## Type
DefaultDescription
signalInInputSignal—Audio to be filtered
f1FreqInputControl500First formant frequency (Hz)
f2FreqInputControl1500Second formant frequency (Hz)
f1QInputControl8.0First formant resonance
f2Q
InputControl10.0Second formant resonance
directMix
InputControl0.3Dry/wet: 0 = all filtered, 1 = all direct
f1f2BalanceInputControl0.6Balance: 0 = F2 only, 1 = F1 only
signalOutOutputSignal—Filtered audio
Configuration (Preferences)
Config OptionTypeDefaultDescription
f1FreqMinfloat200F1 minimum Hz
f1FreqMaxfloat1000F1 maximum Hz
f2FreqMinfloat500F2 minimum Hz
f2FreqMaxfloat3000F2 maximum Hz
qMinfloat1.0Minimum Q factor
qMaxfloat20.0Maximum Q factor
Page 4 of 9

Calamus Container Port Specification v1.1
## Processing
Two parallel resonant bandpass filters (biquad). Output = directMix × input + (1-directMix) ×
(F1 × f1f2Balance + F2 × (1-f1f2Balance)).
Click prevention: Route f1Freq and f2Freq through PhysicsSystem before connecting. The
smoothing prevents coefficient discontinuities that cause audio glitches.
- BreathTurbulence
Blends harmonic content with noise — adds the 'breath' that makes sound human.
## Ports
Port NameDirectionData
## Type
DefaultDescription
voiceInInputSignal—Harmonic/voiced signal
noiseInInputSignal—Noise signal (from NoiseColorFilter)
blendInputControl0.10Mix: 0 = pure voice, 1 = pure noise
signalOutOutputSignal—Blended output
Configuration (Preferences)
Config OptionTypeDefaultDescription
blendCurveenumsqrtMapping: linear, sqrt, log
## Processing
Output = voiceIn × (1 - blend) + noiseIn × blend.
Critical discovery: Just 5-15% noise transforms electronic sound into something with lungs,
throat, and body.
- NoiseColorFilter
Shapes noise spectrum — from dark breath to bright fricatives.
## Ports
Port NameDirectionData
## Type
DefaultDescription
noiseInInputSignal(internal)External noise or use internal
generator
colorInputControl2000Filter cutoff frequency (Hz)
filterQInputControl1.0Filter resonance
noiseOut
OutputSignal—Colored noise
Configuration (Preferences)
Config OptionTypeDefaultDescription
useInternalbooltrueGenerate white noise internally
filterTypeenumhighpassFilter: lowpass, highpass, bandpass
colorMinfloat100Minimum cutoff Hz
colorMaxfloat12000Maximum cutoff Hz
## Processing
Page 5 of 9

Calamus Container Port Specification v1.1
Low color (100 Hz) = dark, breathy rumble. Mid (2000 Hz) = 'SH' territory. High (8000 Hz) =
'S' territory, hissy.
- DriftEngine
Adds micro-detuning to harmonics — subtle beating that creates organic quality.
## Ports
Port NameDirectionData
## Type
DefaultDescription
amountInputControl0.005Drift intensity (0 = none)
rateInputControl0.5How quickly drift wanders (Hz)
driftOutOutputControl—Per-harmonic detuning multipliers
Configuration (Preferences)
Config OptionTypeDefaultDescription
amountMaxfloat0.1Maximum drift (10%)
rateMaxfloat10.0Maximum drift rate Hz
driftPatternenum
perlinPattern: random, sine, perlin
perHarmonicbooltrueEach harmonic drifts independently
## Processing
Slow-moving LFOs per harmonic. Output: array of multipliers near 1.0. Creates subtle
beating between partials — the sound 'breathes' even on sustained notes.
- PhysicsSystem
Applies mass/spring/damping to parameter changes — the core 'organic without random'
mechanism.
## Ports
Port NameDirectionData
## Type
DefaultDescription
targetValueInputControl—Where the parameter wants to go
massInputControl0.5Resistance to change (0 = instant)
springKInputControl0.001Spring constant (pull toward target)
dampingInputControl0.995Energy loss per sample
impulseInputTrigger—Velocity kick (for attacks)
impulseAmountInputControl100Strength of impulse
currentValueOutputControl—Smoothed value with physics
Configuration (Preferences)
Config OptionTypeDefaultDescription
massMaxfloat1.0Maximum mass
velocityLimitfloat1000Clamp to prevent instability
## Processing
Per sample: force = (target - current) × spring; velocity += force; velocity *= damping; current
+= velocity. On impulse: velocity += impulseAmount.
Page 6 of 9

Calamus Container Port Specification v1.1
Dual purpose: Expression (organic movement) AND stability (prevents clicks from abrupt
parameter changes).
- EasingApplicator
Shapes parameter transitions using easing functions — prescribed rather than emergent
movement.
## Ports
Port NameDirectionData
## Type
DefaultDescription
startValueInputControl0.0Beginning of transition
endValueInputControl1.0End of transition
progressInputControl—Position in transition (0-1)
easingSelectInputControl0Which easing function
easedValueOutputControl—Result after easing
Configuration (Preferences)
Config OptionTypeDefaultDescription
easingLibraryrefbuilt-inAvailable easing functions
defaultEasingenumlinearDefault when easingSelect not
connected
defaultModeenuminOut
Default: in, out, inOut
## Processing
t_eased = easingFunction(progress); output = startValue + (endValue - startValue) × t_eased.
Use cases: Pitch glide between notes, formant sweeps for consonants, brightness bloom on
attack.
- GateProcessor
Handles note on/off lifecycle — the birth and death of sound.
## Ports
Port NameDirectionData
## Type
DefaultDescription
—Assumed active for lifetime of the
note
velocityInputControl1.0Note velocity
attackTimeInputControl0.01Attack duration (seconds)
releaseTimeInputControl0.1Release duration (seconds)
attackCurveInputControl0Easing function for attack
releaseCurveInputControl0Easing function for release
velocitySensInputControl0.5How much velocity affects output
envelopeOutOutputControl—Current envelope value (0-1)
stateOutOutputControl—State: 0=off, 1=attack, 2=sustain,
## 3=release
attackTriggerOutputTrigger—Fires once at note start
releaseTriggerOutputTrigger—Fires once at note end
Page 7 of 9

Calamus Container Port Specification v1.1
Configuration (Preferences)
Config OptionTypeDefaultDescription
attackTimeMinfloat0.001Minimum attack (1ms)
attackTimeMaxfloat5.0Maximum attack
releaseTimeMinfloat0.01Minimum release
releaseTimeMaxfloat10.0Maximum release
## Processing
State machine: OFF → ATTACK → SUSTAIN → RELEASE → OFF. EnvelopeOut follows
attack/release curves. AttackTrigger routes to PhysicsSystem.impulse for 'Ba Ba' consonant
effect.
- EnvelopeEngine
Applies arbitrary envelope shapes to parameters over note lifetime.
## Ports
Port NameDirectionData
## Type
DefaultDescription
—Assumed active for lifetime of the
note
noteProgressInputControl—Position in note (0-1)
envelopeSelectInputControl0Which envelope curve
timeScaleInputControl1.0Stretch/compress duration
valueScaleInputControl1.0Multiply output
valueOffsetInputControl0.0Add to output
envelopeValueOutputControl—Current envelope value
Configuration (Preferences)
Config OptionTypeDefaultDescription
envelopeLibraryrefbuilt-inAvailable envelope curves
loopModeenumnoneBehavior at end: none, loop,
pingpong
## Processing
On trigger: reset position. Each sample: read envelope at position × timeScale, apply
valueScale and valueOffset. Multiple instances can run in parallel for different parameters.
Use case: Ben Webster's breathy release — breath blend increases as note fades.
Brightness that evolves over sustained notes. Vibrato that develops over time.
## Typical Sounit Configurations
Minimal (Essential Only)
HarmonicGenerator → RolloffProcessor → SpectrumToSignal → [Output]
Just harmonics and brightness. No breath, no formants, no physics.
## Voice Character
Page 8 of 9

Calamus Container Port Specification v1.1
HarmonicGenerator → RolloffProcessor → SpectrumToSignal → BreathTurbulence →
FormantBody → [Output]
With: NoiseColorFilter → BreathTurbulence.noiseIn
Adds throat resonance and breath. Sounds human-ish.
'Ba Ba' Configuration
Voice Character setup, plus:
•PhysicsSystem(A) → FormantBody.f1Freq
•PhysicsSystem(B) → FormantBody.f2Freq
•GateProcessor.attackTrigger → PhysicsSystem(A).impulse
•GateProcessor.attackTrigger → PhysicsSystem(B).impulse
The consonant 'B' emerges from formant trajectories kicked by attack impulse.
## Full Expression
All containers, with:
•PhysicsSystem on every modulatable frequency/level parameter
•DriftEngine → HarmonicGenerator.drift
•EasingApplicator on pitch for glides
•EnvelopeEngine on breath blend for release swell
•Multiple FormantBody instances for F1, F2, F3, F4
## Summary
## 11 Containers:
•2 Essential: HarmonicGenerator, SpectrumToSignal
•4 Shaping: RolloffProcessor, FormantBody, BreathTurbulence, NoiseColorFilter
•5 Modifiers: PhysicsSystem, EasingApplicator, EnvelopeEngine, DriftEngine,
GateProcessor
## Key Principles:
•No hard-coded limits — all configurable via Preferences
•Everything modulatable is an input with a default
•Multiple instances of any container allowed
•Must end with Signal output
•PhysicsSystem provides both expression AND stability
## Version 1.1 — December 2025
Page 9 of 9