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
#include <QMap>
#include <QSet>

/**
 * RenderSegment - Fixed-duration segment of pre-rendered audio
 *
 * Enables partial re-rendering: only segments with changed notes need re-rendering.
 * Segments divide the timeline into fixed chunks (e.g., 1 second each).
 */
struct RenderSegment {
    double startTimeMs;           // Segment start time in milliseconds
    double endTimeMs;             // Segment end time in milliseconds
    std::vector<float> samples;   // Pre-rendered audio samples for this segment
    QSet<QString> noteIds;        // IDs of notes affecting this segment
    bool isDirty;                 // True if segment needs re-rendering
    uint64_t hash;                // Hash of note states for quick comparison (future use)

    RenderSegment()
        : startTimeMs(0.0)
        , endTimeMs(0.0)
        , isDirty(true)
        , hash(0)
    {}
};

/**
 * AudioEngine - RTAudio wrapper for real-time synthesis (Phase 3)
 *
 * Manages audio stream and connects HarmonicGenerator to output.
 * Supports multiple tracks, each with its own sounit graph.
 *
 * Each track (identified by trackIndex) can have its own SounitGraph.
 * During rendering, notes use the graph associated with their trackIndex.
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

    // Graph-based synthesis (multi-track support)
    bool buildGraph(class Canvas *canvas, int trackIndex);  // Build graph for specific track
    void clearGraph(int trackIndex);   // Clear graph for specific track
    void clearAllGraphs();              // Clear all track graphs
    bool hasGraph(int trackIndex) const;  // Check if track has a valid graph

    // Parameter access
    HarmonicGenerator& getGenerator() { return generator; }

private:
    // RTAudio callback (must be static)
    static int audioCallback(void *outputBuffer, void *inputBuffer,
                            unsigned int nFrames, double streamTime,
                            RtAudioStreamStatus status, void *userData);

    RtAudio audioDevice;
    HarmonicGenerator generator;  // Fallback for direct playback
    QMap<int, SounitGraph*> trackGraphs;  // Graph-based synthesis (one per track)

    std::atomic<bool> gateOpen;  // Simple gate for now
    std::atomic<double> amplitude;  // Envelope state
    std::atomic<double> currentPitch;  // Current note pitch in Hz

    // Note timing for envelope engine
    std::atomic<double> noteDuration;  // Note duration in milliseconds
    std::atomic<uint64_t> noteStartSample;  // Sample number when note started
    std::atomic<uint64_t> currentSample;  // Current sample number (for timing)

    // Pre-rendered buffer playback (segment-based)
    std::vector<RenderSegment> renderSegments;  // Pre-rendered audio segments
    double segmentDurationMs;  // Duration of each segment in milliseconds (default: 1000ms)
    std::atomic<bool> useRenderBuffer;  // true = play from buffer, false = live synthesis
    std::atomic<size_t> renderPlaybackPosition;  // Current position in render buffer (global sample index)
    std::atomic<size_t> renderPlaybackSegmentIndex;  // Current segment being played
    std::mutex renderBufferMutex;  // Protect render buffer during creation/playback
    std::atomic<bool> renderCacheDirty;  // true = need to re-render, false = can reuse buffer
    QVector<Note> cachedNotes;  // The notes that were last rendered (for comparison)

    unsigned int sampleRate;
    bool initialized;
    std::mutex graphMutex;  // Protect graph access between audio thread and UI thread
};

#endif // AUDIOENGINE_H
