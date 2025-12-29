#include "audioengine.h"
#include <iostream>
#include <cmath>

AudioEngine::AudioEngine()
    : gateOpen(false)
    , amplitude(0.0)
    , useGraph(false)
    , currentPitch(261.63)
    , noteDuration(1000.0)
    , noteStartSample(0)
    , currentSample(0)
    , useRenderBuffer(false)
    , renderPlaybackPosition(0)
    , renderCacheDirty(true)
    , sampleRate(44100)
    , initialized(false)
    , graph(nullptr)
{
}

AudioEngine::~AudioEngine()
{
    shutdown();
    delete graph;
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

    if (useGraph.load() && graph) {
        // Reset graph for new note
        graph->reset();
    } else {
        // Fallback to direct generator
        generator.setFundamentalHz(note.getPitchHz());
        generator.reset();
    }

    // Open gate
    gateOpen = true;

    std::cout << "Playing note: " << note.getPitchHz() << " Hz, duration: "
              << note.getDuration() << " ms"
              << (useGraph.load() ? " (using graph)" : " (direct)") << std::endl;
}

void AudioEngine::stopNote()
{
    gateOpen = false;
    amplitude = 0.0;  // Immediate cutoff for stop button
    useRenderBuffer.store(false);  // Stop buffer playback
}

void AudioEngine::buildGraph(Canvas *canvas)
{
    if (!canvas) {
        clearGraph();
        return;
    }

    // Lock mutex to prevent audio thread from accessing graph during rebuild
    std::lock_guard<std::mutex> lock(graphMutex);

    // Delete old graph if it exists
    delete graph;

    // Create and build graph
    graph = new SounitGraph(static_cast<double>(sampleRate));
    graph->buildFromCanvas(canvas);

    // Enable graph mode if valid
    if (graph->isValid()) {
        useGraph.store(true);
        std::cout << "AudioEngine: Graph built successfully - using graph mode" << std::endl;
    } else {
        std::cout << "AudioEngine: Graph invalid - falling back to direct mode" << std::endl;
        useGraph.store(false);
    }

    // Invalidate render cache - graph structure changed
    renderCacheDirty.store(true);
    std::cout << "AudioEngine: Render cache invalidated (graph changed)" << std::endl;
}

void AudioEngine::clearGraph()
{
    // Lock mutex to prevent audio thread from accessing graph during clear
    std::lock_guard<std::mutex> lock(graphMutex);

    delete graph;
    graph = nullptr;
    useGraph.store(false);
    renderCacheDirty.store(true);
    std::cout << "AudioEngine: Graph cleared - using direct mode" << std::endl;
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
            if (engine->useGraph.load() && engine->graph && engine->graph->isValid()) {
                // Use graph-based synthesis with current note pitch and progress
                double pitch = engine->currentPitch.load();
                sample = engine->graph->generateSample(pitch, noteProgress);
            } else {
                // Fallback to direct generator
                sample = engine->generator.generateSample();
            }

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

    // Limit to maxNotes
    int notesToRender = qMin(notes.size(), maxNotes);

    // Check if we can reuse cached render
    bool notesChanged = (cachedNotes.size() != notesToRender);
    if (!notesChanged) {
        for (int i = 0; i < notesToRender; i++) {
            if (cachedNotes[i].getPitchHz() != notes[i].getPitchHz() ||
                cachedNotes[i].getDuration() != notes[i].getDuration() ||
                cachedNotes[i].getStartTime() != notes[i].getStartTime() ||
                cachedNotes[i].getDynamics() != notes[i].getDynamics()) {
                notesChanged = true;
                break;
            }
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
    std::cout << std::endl;

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

        std::cout << "  Note " << (noteIdx + 1) << ": " << note.getPitchHz() << " Hz, "
                  << "start=" << note.getStartTime() << "ms, dur=" << note.getDuration() << "ms" << std::endl;

        // Reset synthesis for this note
        if (useGraph.load() && graph && graph->isValid()) {
            graph->reset();
        } else {
            generator.setFundamentalHz(note.getPitchHz());
            generator.reset();
        }

        double amplitude = 0.0;

        // Render this note's samples
        for (size_t i = 0; i < noteDurationSamples; i++) {
            size_t bufferPos = noteStartSample + i;
            if (bufferPos >= totalSamples) break;  // Safety check

            // Calculate note progress
            double noteProgress = static_cast<double>(i) / static_cast<double>(noteDurationSamples);

            // Update amplitude envelope (attack for first half, release for second half)
            if (noteProgress < 0.5) {
                amplitude += (1.0 - amplitude) * attackRate;
            } else {
                amplitude *= (1.0 - releaseRate);
                if (amplitude < 0.0001) amplitude = 0.0;
            }

            // Generate sample
            double sample;
            if (useGraph.load() && graph && graph->isValid()) {
                sample = graph->generateSample(note.getPitchHz(), noteProgress);
            } else {
                sample = generator.generateSample();
            }

            // Apply envelope and clamp
            sample *= amplitude;
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
