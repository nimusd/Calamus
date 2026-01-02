#include "audioengine.h"
#include <iostream>
#include <cmath>

AudioEngine::AudioEngine()
    : gateOpen(false)
    , amplitude(0.0)
    , currentPitch(261.63)
    , noteDuration(1000.0)
    , noteStartSample(0)
    , currentSample(0)
    , segmentDurationMs(1000.0)  // 1 second segments by default
    , useRenderBuffer(false)
    , renderPlaybackPosition(0)
    , renderPlaybackSegmentIndex(0)
    , renderCacheDirty(true)
    , sampleRate(48000)
    , initialized(false)
{
    // trackGraphs initialized as empty QMap
    // renderSegments initialized as empty vector
}

AudioEngine::~AudioEngine()
{
    shutdown();
    clearAllGraphs();
}

bool AudioEngine::initialize(unsigned int sampleRate, unsigned int bufferFrames)
{
    if (initialized) {
        std::cerr << "AudioEngine already initialized" << std::endl;
        return false;
    }

    this->sampleRate = sampleRate;
    generator.setSampleRate(static_cast<double>(sampleRate));

    // Try to open the default audio device
    if (audioDevice.getDeviceCount() < 1) {
        std::cerr << "No audio devices found!" << std::endl;
        return false;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = audioDevice.getDefaultOutputDevice();
    parameters.nChannels = 2;  // Stereo (most devices expect this)
    parameters.firstChannel = 0;

    RtAudioFormat format = RTAUDIO_FLOAT32;

    try {
        audioDevice.openStream(&parameters, nullptr, format, sampleRate,
                              &bufferFrames, &audioCallback, this);
        audioDevice.startStream();
        initialized = true;
        std::cout << "AudioEngine initialized: " << sampleRate << " Hz, "
                  << bufferFrames << " frames" << std::endl;
        return true;
    }
    catch (RtAudioErrorType &error) {
        std::cerr << "RTAudio error: " << audioDevice.getErrorText() << std::endl;
        return false;
    }
}

void AudioEngine::shutdown()
{
    if (!initialized) return;

    try {
        if (audioDevice.isStreamRunning())
            audioDevice.stopStream();
        if (audioDevice.isStreamOpen())
            audioDevice.closeStream();
    }
    catch (RtAudioErrorType &error) {
        std::cerr << "Error closing stream: " << audioDevice.getErrorText() << std::endl;
    }

    initialized = false;
    std::cout << "AudioEngine shut down" << std::endl;
}

bool AudioEngine::isRunning() const
{
    return initialized && audioDevice.isStreamRunning();
}

void AudioEngine::playNote(const Note& note)
{
    // Store current pitch and note duration
    currentPitch.store(note.getPitchHz());
    noteDuration.store(note.getDuration());

    // Record the start sample for timing
    noteStartSample.store(currentSample.load());

    // Check if note's track has a graph
    int trackIndex = note.getTrackIndex();
    bool hasValidGraph = hasGraph(trackIndex);

    if (hasValidGraph) {
        // Reset graph for new note
        std::lock_guard<std::mutex> lock(graphMutex);
        if (trackGraphs.contains(trackIndex)) {
            trackGraphs[trackIndex]->reset();
        }
    } else {
        // Fallback to direct generator
        generator.setFundamentalHz(note.getPitchHz());
        generator.reset();
    }

    // Open gate
    gateOpen = true;

    std::cout << "Playing note: " << note.getPitchHz() << " Hz, duration: "
              << note.getDuration() << " ms (track " << trackIndex << ")"
              << (hasValidGraph ? " (using graph)" : " (direct)") << std::endl;
}

void AudioEngine::stopNote()
{
    gateOpen = false;
    amplitude = 0.0;  // Immediate cutoff for stop button
    useRenderBuffer.store(false);  // Stop buffer playback
}

bool AudioEngine::buildGraph(Canvas *canvas, int trackIndex)
{
    if (!canvas) {
        clearGraph(trackIndex);
        return false;
    }

    // Lock mutex to prevent audio thread from accessing graph during rebuild
    std::lock_guard<std::mutex> lock(graphMutex);

    // Delete old graph for this track if it exists
    if (trackGraphs.contains(trackIndex)) {
        delete trackGraphs[trackIndex];
        trackGraphs.remove(trackIndex);
    }

    // Create and build graph
    SounitGraph *newGraph = new SounitGraph(static_cast<double>(sampleRate));
    newGraph->buildFromCanvas(canvas);

    // Check if valid
    bool isValid = newGraph->isValid();
    if (isValid) {
        trackGraphs[trackIndex] = newGraph;
        std::cout << "AudioEngine: Graph built successfully for track " << trackIndex
                  << " - using graph mode" << std::endl;
    } else {
        std::cout << "AudioEngine: Graph invalid for track " << trackIndex
                  << " - falling back to direct mode" << std::endl;
        delete newGraph;
    }

    // Invalidate render cache - graph structure changed
    renderCacheDirty.store(true);
    std::cout << "AudioEngine: Render cache invalidated (graph changed)" << std::endl;

    return isValid;
}

void AudioEngine::clearGraph(int trackIndex)
{
    // Lock mutex to prevent audio thread from accessing graph during clear
    std::lock_guard<std::mutex> lock(graphMutex);

    if (trackGraphs.contains(trackIndex)) {
        delete trackGraphs[trackIndex];
        trackGraphs.remove(trackIndex);
        renderCacheDirty.store(true);
        std::cout << "AudioEngine: Graph cleared for track " << trackIndex
                  << " - using direct mode" << std::endl;
    }
}

void AudioEngine::clearAllGraphs()
{
    // Lock mutex to prevent audio thread from accessing graphs during clear
    std::lock_guard<std::mutex> lock(graphMutex);

    for (SounitGraph *graph : trackGraphs) {
        delete graph;
    }
    trackGraphs.clear();
    renderCacheDirty.store(true);
    std::cout << "AudioEngine: All graphs cleared - using direct mode" << std::endl;
}

bool AudioEngine::hasGraph(int trackIndex) const
{
    return trackGraphs.contains(trackIndex) && trackGraphs[trackIndex] != nullptr
           && trackGraphs[trackIndex]->isValid();
}

int AudioEngine::audioCallback(void *outputBuffer, void *inputBuffer,
                               unsigned int nFrames, double streamTime,
                               RtAudioStreamStatus status, void *userData)
{
    AudioEngine *engine = static_cast<AudioEngine*>(userData);
    float *buffer = static_cast<float*>(outputBuffer);

    // Check if we're in buffer playback mode
    if (engine->useRenderBuffer.load()) {
        // Lock render buffer mutex
        std::lock_guard<std::mutex> lock(engine->renderBufferMutex);

        for (unsigned int i = 0; i < nFrames; i++) {
            size_t pos = engine->renderPlaybackPosition.load();
            float sample = 0.0f;

            // Read from render buffer if position is valid
            if (pos < engine->renderBuffer.size()) {
                sample = engine->renderBuffer[pos];
                engine->renderPlaybackPosition.fetch_add(1);
            } else {
                // Reached end of buffer, stop playback
                engine->useRenderBuffer.store(false);
            }

            // Output to both stereo channels
            buffer[i * 2] = sample;      // Left channel
            buffer[i * 2 + 1] = sample;  // Right channel
        }
    } else {
        // Live synthesis mode
        // Simple envelope: fast attack, slow release
        const double attackRate = 0.01;   // Rise quickly
        const double releaseRate = 0.001; // Fall slowly

        // Lock mutex ONCE per buffer, not per sample
        std::lock_guard<std::mutex> lock(engine->graphMutex);

        for (unsigned int i = 0; i < nFrames; i++)
        {
            // Calculate note progress (0.0 to 1.0 over note duration)
            uint64_t currentSmp = engine->currentSample.load();
            uint64_t startSmp = engine->noteStartSample.load();
            double duration = engine->noteDuration.load();

            // Convert duration from milliseconds to samples
            double durationSamples = (duration / 1000.0) * engine->sampleRate;

            // Calculate progress
            double noteProgress = 0.5;  // Default fallback
            if (durationSamples > 0.0) {
                double elapsedSamples = static_cast<double>(currentSmp - startSmp);
                noteProgress = elapsedSamples / durationSamples;
                // Clamp to [0.0, 1.0]
                noteProgress = std::clamp(noteProgress, 0.0, 1.0);
            }

            // Update amplitude envelope
            if (engine->gateOpen.load()) {
                double currentAmp = engine->amplitude.load();
                currentAmp += (1.0 - currentAmp) * attackRate;
                engine->amplitude.store(currentAmp);
            } else {
                double currentAmp = engine->amplitude.load();
                currentAmp *= (1.0 - releaseRate);
                if (currentAmp < 0.0001) currentAmp = 0.0;
                engine->amplitude.store(currentAmp);
            }

            // Generate sample
            double sample;
            // Note: For live synthesis, we would need to track which track is active
            // For now, fallback to direct generator for live playback
            // Pre-rendered playback (the main use case) is handled in renderNotes()
            sample = engine->generator.generateSample();

            // Apply envelope
            sample *= engine->amplitude.load();

            // Output to both stereo channels (clamp to prevent clipping)
            float outputSample = static_cast<float>(std::clamp(sample * 0.3, -1.0, 1.0));
            buffer[i * 2] = outputSample;      // Left channel
            buffer[i * 2 + 1] = outputSample;  // Right channel

            // Increment sample counter
            engine->currentSample.fetch_add(1);
        }
    }

    return 0;
}

void AudioEngine::renderNotes(const QVector<Note>& notes, int maxNotes)
{
    if (notes.isEmpty()) {
        std::cout << "AudioEngine: No notes to render" << std::endl;
        return;
    }

    // Limit to maxNotes (-1 means render all notes)
    int notesToRender = (maxNotes < 0) ? notes.size() : qMin(notes.size(), maxNotes);

    // Check if we can reuse cached render
    bool notesChanged = (cachedNotes.size() != notesToRender);
    if (!notesChanged) {
        for (int i = 0; i < notesToRender; i++) {
            // Check basic properties
            if (cachedNotes[i].getPitchHz() != notes[i].getPitchHz() ||
                cachedNotes[i].getDuration() != notes[i].getDuration() ||
                cachedNotes[i].getStartTime() != notes[i].getStartTime() ||
                cachedNotes[i].getDynamics() != notes[i].getDynamics()) {
                notesChanged = true;
                break;
            }

            // Check if pitch curves differ (for continuous notes)
            if (cachedNotes[i].getPitchCurve().getPoints().size() != notes[i].getPitchCurve().getPoints().size()) {
                notesChanged = true;
                break;
            }

            // Check if dynamics curves differ (for continuous notes)
            if (cachedNotes[i].getDynamicsCurve().getPoints().size() != notes[i].getDynamicsCurve().getPoints().size()) {
                notesChanged = true;
                break;
            }

            // TODO: Could add deeper curve comparison (point values) if needed
        }
    }

    if (!renderCacheDirty.load() && !notesChanged && !renderBuffer.empty()) {
        std::cout << "AudioEngine: Using cached render (no changes detected)" << std::endl;
        return;
    }

    std::cout << "AudioEngine: Rendering " << notesToRender << " note(s)";
    if (renderCacheDirty.load()) {
        std::cout << " (graph changed)";
    }
    if (notesChanged) {
        std::cout << " (notes changed)";
    }

    // Show which synthesis mode we're using
    std::cout << " [Loaded graphs for tracks:";
    bool hasAnyGraph = false;
    for (auto it = trackGraphs.constBegin(); it != trackGraphs.constEnd(); ++it) {
        if (it.value() && it.value()->isValid()) {
            std::cout << " " << it.key();
            hasAnyGraph = true;
        }
    }
    if (!hasAnyGraph) {
        std::cout << " NONE - using fallback generator";
    }
    std::cout << "]" << std::endl;

    // Find the total duration (last note's end time)
    double totalDurationMs = 0.0;
    for (int i = 0; i < notesToRender; i++) {
        double noteEndTime = notes[i].getStartTime() + notes[i].getDuration();
        if (noteEndTime > totalDurationMs) {
            totalDurationMs = noteEndTime;
        }
    }

    // Calculate total samples needed
    double totalDurationSeconds = totalDurationMs / 1000.0;
    size_t totalSamples = static_cast<size_t>(totalDurationSeconds * sampleRate);

    std::cout << "AudioEngine: Total duration: " << totalDurationMs << " ms ("
              << totalSamples << " samples)" << std::endl;

    // Lock both mutexes during rendering
    std::lock_guard<std::mutex> graphLock(graphMutex);
    std::lock_guard<std::mutex> renderLock(renderBufferMutex);

    // Allocate render buffer
    renderBuffer.clear();
    renderBuffer.resize(totalSamples, 0.0f);  // Initialize with silence

    // Envelope parameters
    const double attackRate = 0.01;
    const double releaseRate = 0.001;

    // Render each note
    for (int noteIdx = 0; noteIdx < notesToRender; noteIdx++) {
        const Note& note = notes[noteIdx];

        // Calculate sample positions for this note
        size_t noteStartSample = static_cast<size_t>((note.getStartTime() / 1000.0) * sampleRate);
        size_t noteDurationSamples = static_cast<size_t>((note.getDuration() / 1000.0) * sampleRate);

        // Get note's track and check if it has a graph
        int noteTrackIndex = note.getTrackIndex();
        bool noteHasGraph = trackGraphs.contains(noteTrackIndex)
                            && trackGraphs[noteTrackIndex]
                            && trackGraphs[noteTrackIndex]->isValid();

        std::cout << "  Note " << (noteIdx + 1) << ": track=" << noteTrackIndex
                  << ", " << note.getPitchHz() << " Hz, "
                  << "start=" << note.getStartTime() << "ms, dur=" << note.getDuration() << "ms"
                  << ", avgDyn=" << note.getDynamics();

        // Show if note has curves (continuous note)
        if (note.hasPitchCurve()) {
            std::cout << " [CONTINUOUS PITCH: " << note.getPitchCurve().getPoints().size() << " points]";
        }
        int dynPoints = note.getDynamicsCurve().getPoints().size();
        if (dynPoints > 2) {  // More than 2 points = varying dynamics
            std::cout << " [CONTINUOUS DYNAMICS: " << dynPoints << " points]";
        } else if (dynPoints == 2) {
            std::cout << " [DISCRETE DYNAMICS: " << dynPoints << " points]";
        } else {
            std::cout << " [WARNING: " << dynPoints << " dynamics points!]";
        }
        std::cout << (noteHasGraph ? " [GRAPH]" : " [FALLBACK]") << std::endl;

        // Reset synthesis for this note
        if (noteHasGraph) {
            trackGraphs[noteTrackIndex]->reset();
        } else {
            // For fallback generator, set initial pitch (will be updated per-sample for continuous notes)
            generator.setFundamentalHz(note.getPitchHz());
            generator.reset();
        }

        double amplitude = 0.0;

        // Render this note's samples
        for (size_t i = 0; i < noteDurationSamples; i++) {
            size_t bufferPos = noteStartSample + i;
            if (bufferPos >= totalSamples) break;  // Safety check

            // Calculate note progress (0.0 to 1.0)
            // Use noteDurationSamples - 1 as denominator to ensure we reach 1.0 at the last sample
            double noteProgress;
            if (noteDurationSamples > 1) {
                noteProgress = static_cast<double>(i) / static_cast<double>(noteDurationSamples - 1);
            } else {
                noteProgress = 0.0;  // Single sample note
            }

            // Sample pitch curve at current time (supports continuous pitch variation)
            double currentPitch = note.getPitchAt(noteProgress);

            // Sample dynamics curve at current time (supports continuous dynamics variation)
            double currentDynamics = note.getDynamicsAt(noteProgress);

            // Update amplitude envelope (ADSR)
            // Attack: first 5% of note
            // Sustain: middle 85% of note
            // Release: last 10% of note
            if (noteProgress < 0.05) {
                // Attack phase - ramp up quickly
                amplitude += (1.0 - amplitude) * attackRate;
            } else if (noteProgress < 0.90) {
                // Sustain phase - maintain full amplitude
                amplitude = 1.0;
            } else {
                // Release phase - fade out in last 10%
                amplitude *= (1.0 - releaseRate);
                if (amplitude < 0.0001) amplitude = 0.0;
            }

            // Generate sample using note's track graph
            double sample;
            if (noteHasGraph) {
                // Use continuous pitch for graph-based synthesis
                sample = trackGraphs[noteTrackIndex]->generateSample(currentPitch, noteProgress);
            } else {
                // Update fallback generator pitch for continuous notes
                generator.setFundamentalHz(currentPitch);
                sample = generator.generateSample();
            }

            // Apply envelope, dynamics curve, and clamp
            sample *= amplitude * currentDynamics;
            float outputSample = static_cast<float>(std::clamp(sample * 0.3, -1.0, 1.0));

            // Mix into buffer (for now just overwrite, but could add overlapping notes later)
            renderBuffer[bufferPos] = outputSample;
        }
    }

    // Cache the rendered notes and mark cache as clean
    cachedNotes.clear();
    for (int i = 0; i < notesToRender; i++) {
        cachedNotes.append(notes[i]);
    }
    renderCacheDirty.store(false);

    std::cout << "AudioEngine: Rendered " << renderBuffer.size() << " samples (cached)" << std::endl;
}

void AudioEngine::playRenderedBuffer()
{
    if (renderBuffer.empty()) {
        std::cout << "AudioEngine: No rendered buffer to play" << std::endl;
        return;
    }

    // Reset playback position and enable buffer playback mode
    renderPlaybackPosition.store(0);
    useRenderBuffer.store(true);

    std::cout << "AudioEngine: Playing rendered buffer (" << renderBuffer.size()
              << " samples, " << (renderBuffer.size() / static_cast<double>(sampleRate))
              << " seconds)" << std::endl;
}
