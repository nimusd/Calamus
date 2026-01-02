#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "harmonicgenerator.h"
#include "sounitgraph.h"
#include "note.h"
#include <RtAudio.h>
#include <memory>
#include <atomic>
#include <mutex>
#include <cstdint>
#include <vector>
#include <QVector>

/**
 * AudioEngine - RTAudio wrapper for real-time synthesis (Phase 3)
 *
 * Manages audio stream and connects HarmonicGenerator to output.
 * Currently plays one note at a time (single-voice).
 *
 * Future: Will handle multiple voices, pre-rendered buffers, etc.
 */
class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();

    // Audio stream control
    bool initialize(unsigned int sampleRate = 44100, unsigned int bufferFrames = 256);
    void shutdown();
    bool isRunning() const;

    // Note playback (simple gate on/off for now)
    void playNote(const Note& note);
    void stopNote();

    // Pre-rendering (render notes to buffer, then play from buffer)
    void renderNotes(const QVector<Note>& notes, int maxNotes = -1);  // -1 = all notes
    void playRenderedBuffer();

    // Graph-based synthesis
    bool buildGraph(class Canvas *canvas);  // Returns true if graph is valid, false if invalid
    void clearGraph();

    // Parameter access
    HarmonicGenerator& getGenerator() { return generator; }

private:
    // RTAudio callback (must be static)
    static int audioCallback(void *outputBuffer, void *inputBuffer,
                            unsigned int nFrames, double streamTime,
                            RtAudioStreamStatus status, void *userData);

    RtAudio audioDevice;
    HarmonicGenerator generator;  // Fallback for direct playback
    SounitGraph *graph;  // Graph-based synthesis

    std::atomic<bool> gateOpen;  // Simple gate for now
    std::atomic<double> amplitude;  // Envelope state
    std::atomic<bool> useGraph;  // true = use graph, false = use generator
    std::atomic<double> currentPitch;  // Current note pitch in Hz

    // Note timing for envelope engine
    std::atomic<double> noteDuration;  // Note duration in milliseconds
    std::atomic<uint64_t> noteStartSample;  // Sample number when note started
    std::atomic<uint64_t> currentSample;  // Current sample number (for timing)

    // Pre-rendered buffer playback
    std::vector<float> renderBuffer;  // Pre-rendered audio samples (mono)
    std::atomic<bool> useRenderBuffer;  // true = play from buffer, false = live synthesis
    std::atomic<size_t> renderPlaybackPosition;  // Current position in render buffer
    std::mutex renderBufferMutex;  // Protect render buffer during creation/playback
    std::atomic<bool> renderCacheDirty;  // true = need to re-render, false = can reuse buffer
    QVector<Note> cachedNotes;  // The notes that were last rendered (for comparison)

    unsigned int sampleRate;
    bool initialized;
    std::mutex graphMutex;  // Protect graph access between audio thread and UI thread
};

#endif // AUDIOENGINE_H
