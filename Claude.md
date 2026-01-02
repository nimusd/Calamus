# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Development Workflow

⚠️ **CRITICAL:** Always wait for express user request before writing code. Go step by step: **build, test, celebrate, repeat**.

Never write code proactively. The user will explicitly tell you when to implement features.

## Project Overview

**Calamus** is a compositional instrument built on Qt 6 that breaks from keyboard thinking. It uses pen tablet input as the primary interface, capturing expressive gesture as notation itself. The sound engine uses "physics modeling" (invented laws governing mass, inertia, parameter coupling) to create sounds that are alive but controllable. No MIDI. No piano roll.

**Core Philosophy:**

- The act of composing IS the performance  
- Voices share sonic DNA built on tuning-agnostic foundation  
- Pre-render model for composition (not real-time performance)  
- No hard-coded limits — all numeric constraints are configurable

## Build Commands

This project uses **CMake** with **Qt 6.10.1 MinGW 64-bit**.

\# Configure (from project root)

cmake \-S . \-B build \-G "MinGW Makefiles"

\# Build

cmake \--build build

\# Run executable

./build/Desktop\_Qt\_6\_10\_1\_MinGW\_64\_bit-Debug/Calamus.exe

The current build directory is `build/Desktop_Qt_6_10_1_MinGW_64_bit-Debug/`.

**Note:** There are currently no tests defined in the project.

## Architecture Overview

### Three-Pillar System

1. **Sound Engine** — Physics-based synthesis with container architecture  
     
   - Renders phrases to audio buffers  
   - Real-time playback of pre-rendered content  
   - Live synthesis only for actively-edited content

   

2. **Data Layer** — Contract between systems  
     
   - Defines structures (Note, Phrase, Sounit, Curve, Scale)  
   - Manages persistence (planned: .calamus, .sounit, .phrase files in JSON)

   

3. **Input Engine** — Gesture capture and notation display  
     
   - Wacom pen input (6 dimensions planned)  
   - Scale-centric staff display  
   - Curve-based note visualization

### Main Components

**CalamusMain** (`calamusmain.h/.cpp`)

- Application root window (QMainWindow)  
- Manages 3-tab interface: Sound Engine (tab 0), Composition (tab 1), Preferences (tab 2\)  
- Creates and orchestrates SounitBuilder and ScoreCanvasWindow  
- Signal: `onTabChanged(int)` shows/hides appropriate child windows

**SounitBuilder** (`sounitbuilder.h/.cpp`)

- Sound synthesis module (tab 0\)  
- Graph-based interface for connecting audio containers  
- Manages toolbar with 11 audio processor types (color-coded by category)  
- Handles connection editing between container ports  
- Displays container and connection inspectors  
- Current state: UI framework with functional editing logic

**Canvas** (`canvas.h/.cpp`)

- Core rendering surface for sound graph visualization  
- Draws Bezier curves for connections between containers  
- Manages pending connections (visual feedback during dragging)  
- Handles selection and deletion of containers/connections  
- Hit testing with 10px tolerance for connection clicks

**Container** (`container.h/.cpp`)

- Represents single audio processing unit in graph
- Visual representation: colored header, input/output port circles
- Drag-and-drop repositioning
- Parameter management: `setParameter()`, `getParameter()`, batch updates with `beginParameterUpdate()` / `endParameterUpdate()`
- Emits signals: `portClicked()`, `moved()`, `clicked()`, `parameterChanged()`

**ScoreCanvasWindow** (`scorecanvaswindow.h/.cpp`)

- Music composition interface (tab 1\)  
- Toolbar with draw/record tools for discrete/continuous notes  
- Current functional state:  
  - time line with now marker and loop setting   
  - note drawing in discrete and continuous modes  
  - Single and multi note select. Move selection around  
  - Delete, cut, copy and paste  
  - Zoom and pan  
  - Playback pre-rendered from loaded sounit.  
- Current half finished state  
  - Playback pre-rendered from loaded sounit (still some issues)  
  - Phrases and gesture (lots of issues)  
- Not implemented: Inspectors

### Container Categories (Audio Processors)

**Essential (Blue):**

- Harmonic Generator  
- Spectrum to Signal

**Shaping (Orange):**

- Rolloff Processor  
- Formant Body  
- Breath Turbulence  
- Noise Color Filter

**Modifiers (Green):**

- Physics System  
- Easing Applicator  
- Envelope Engine  
- Drift Engine  
- Gate Processor

### Signal/Slot Architecture

All inter-component communication uses Qt signals/slots:

Container Port Click Flow:

  User clicks port circle → Container::portClicked() signal

  → SounitBuilder::onPortClicked() slot

  → Stores start point OR creates connection

  → Canvas::addConnection() or Canvas::startPendingConnection()

  → Canvas::update() triggers repaint

Container Movement Flow:

  Container::moved() signal

  → Canvas::update() (redraws all connections)

Graph Rebuild Flow:

  Container parameter change → Container::parameterChanged() signal

  → SounitBuilder::rebuildGraph() slot

  → AudioEngine::buildGraph() rebuilds audio graph

  Connection added/removed → Canvas::graphChanged() signal

  → SounitBuilder::rebuildGraph() slot

  Container added → Direct call to SounitBuilder::rebuildGraph()

## Design Patterns

- **Model-View-Controller:** Canvas (view/model), SounitBuilder (controller), Container (domain)  
- **Signal-Slot:** Decoupled component communication via Qt meta-object system  
- **Composite Pattern:** Canvas contains multiple Container widgets  
- **Strategy Pattern:** Toolbar buttons use lambdas to configure different container types  
- **Event-Driven Interaction:** Mouse events → port selection → pending connections → model updates → repaints

## Code Conventions

### Rendering and Visual Feedback

- **Bezier curves** for connections: Cubic interpolation with 50+ pixel control offset  
- **Dashed lines** for pending connections during dragging  
- **Color coding:** Connection color matches source container  
- **Selection highlighting:** Blue (\#0066CC) for selected connections, 1px border for containers  
- **Hit testing:** 10px tolerance for connection selection

### Connection Logic

- Connections are **directional**: Output → Input only  
- Validation prevents same-type connections (no input→input or output→output)  
- First port click starts pending connection; second click completes or cancels  
- Connection direction auto-flips: if start is input, swap from/to in Connection struct

### Port Representation

struct PortInfo {

    QString name;          // Port identifier

    bool isOutput;         // Direction

    QLabel \*circle;        // Visual indicator (●)

};

### Connection Representation

struct Connection {

    Container \*fromContainer;  // Source (must be output port)

    QString fromPort;

    Container \*toContainer;    // Destination (must be input port)

    QString toPort;

};

## Threading Model (Planned)

Per Architecture v1.4:

- **Audio Thread (highest priority):** Mix pre-rendered buffers, run live engine for active selection. Never allocates memory, waits on locks, or touches UI.  
- **UI Thread (normal):** User interaction, display updates. No heavy computation.  
- **Render Thread (low priority):** Pre-render dirty phrases in background.

**Communication:**

- UI ↔ Render: Qt signals/slots  
- UI → Audio: Lock-free SPSC queue for commands  
- Audio → UI: Lock-free SPSC queue for status updates

**Current State:** Threading partly implemented, not optimized.

## File Structure

D:\\C++ code\\Calamus\\

├── CMakeLists.txt              \# Build configuration

├── main.cpp                    \# Entry point (minimal)

├── calamusmain.h/.cpp/.ui      \# Main tabbed window

├── sounitbuilder.h/.cpp/.ui    \# Sound engine module

├── canvas.h/.cpp               \# Graph rendering surface

├── container.h/.cpp/.ui        \# Audio unit widget

├── scorecanvaswindow.h/.cpp/.ui \# Composition module (stub)

├── docs/md format/             \# Design specifications

│   ├── Calamus Architecture v1.4.md

│   ├── calamus\_sounit\_builder\_canvas\_specification\_v1\_0.md

│   ├── calamus\_score\_canvas\_specification\_v1.md

│   ├── Container Port Specification v1.2.md

│   └── \[other spec files\]

└── build/                      \# Qt build artifacts

## Important Specifications

**Container Port Design Principles** (from Container Port Specification v1.2):

1. **No Hard-Coded Limits:** All numeric constraints configurable via Preferences  
2. **Everything Modulatable is an Input:** Any parameter that benefits from modulation is exposed as input port with default value  
3. **Modular Composition:** Instantiate containers multiple times, chain freely, signal path must end with Signal output

**Connection Functions** (how outputs combine with inputs):

- `passthrough` — source replaces input entirely  
- `add` — input \+ source × weight  
- `multiply` — input × source × weight  
- `subtract` — input \- source × weight  
- `replace` — weighted blend  
- `modulate` — bipolar modulation around input

**Data Types:**

- Signal: Audio-rate stream (-1.0 to 1.0)  
- Spectrum: Array of harmonic amplitudes  
- Control: Single value (control or audio rate)  
- Trigger: Discrete event

## Current Implementation Status

### Implemented

- Main tabbed window with show/hide logic for child windows
- SounitBuilder with 11 container types via toolbar
- Canvas with Bezier curve rendering for connections
- Container drag-and-drop, port click signaling
- Connection creation with direction validation
- Selection and deletion (Del key) for containers and connections
- **Continuous note support**: Pitch and dynamics curves are sampled per-sample during rendering
  - Discrete notes: constant pitch/dynamics
  - Continuous notes: pitch/dynamics vary over time via curves
  - Audio engine samples `getPitchAt(progress)` and `getDynamicsAt(progress)` every sample

### Not Yet or Partly Implemented

- Data persistence (save/load)
- Undo/redo (planned: Qt QUndoStack)
- Connection validation beyond direction checking
- Marquee selection in sounit builder (works in composition)
- Zoom/pan navigation in sounit builder  (works in composition)
- Threading model
- Preferences tab

### Recently Fixed

- **10-note playback limit removed** - Previously playback was limited to 10 notes maximum. Now all notes in the score are rendered and played back. Changed in `AudioEngine::renderNotes()` (default `maxNotes = -1` means all notes), `SounitBuilder::startPlayback()`, and `ScoreCanvasWindow::startPlayback()`.

### Recently Added

- **Composition Settings Dialog** - Accessible via File > Composition Settings (Ctrl+,) in the composition tab. Allows editing:
  - Composition name
  - Time mode (Absolute/Musical)
  - Tempo (20-300 BPM)
  - Time signature (numerator/denominator)
  - Length (duration for Absolute mode, bars for Musical mode)
  - Default values: "Untitled", Musical mode, 100 BPM, 5/4 time signature, 5 minutes/20 bars
  - Settings sync bidirectionally with toolbar controls (tempo spinbox, time sig spinboxes, time mode toggle)
  - Ready for persistence to .calamus file format (JSON serialization implemented)
  - Files: `compositionsettings.h/cpp`, `compositionsettingsdialog.h/cpp`

- **Goto Dialog (Navigation)** - Triggered by 'g' keyboard shortcut in the composition tab. Allows navigation to specific time positions:
  - **Absolute mode**: Input format MM:SS:MS (Minutes:Seconds:Milliseconds)
  - **Musical mode**: Input format Measure:Beat:MS (Bar:Beat:Milliseconds, both 1-based)
  - Automatically adapts to current composition time mode
  - Displays calculated target time in info label for verification
  - Scrolls the score canvas to center the target position in the viewport
  - Musical mode calculations use current tempo and time signature
  - Files: `gotodialog.h/cpp`
  - Implementation: scorecanvaswindow.cpp:790-815 (keyboard handler)

- **Snap Continuous Note to Scale** - Right-click on a continuous note to quantize its pitch curve to scale degrees:
  - Converts smooth glissando/portamento into stepped pitch changes
  - Uses hysteresis algorithm: holds current pitch until crossing halfway to next scale degree
  - Only affects continuous notes (notes drawn with pitch curves)
  - Preserves original note timing and dynamics
  - Useful for converting free-form gestures into quantized melodies
  - Implementation: scorecanvas.cpp:1681-1762 (quantizePitchCurveToScale), scorecanvas.cpp:1120-1138 (right-click handler)

### Keyboard Shortcuts (Composition Tab)

- **Space** - Toggle playback (start/stop)
- **Ctrl+Space** - Toggle pan mode (allows panning with mouse/tablet drag)
- **L** - Enter loop mode (click timeline to set loop points)
- **Delete** - Clear loop (when loop end marker is selected) OR delete selected notes/phrases
- **G** - Open goto dialog (navigate to specific time position)
- **Ctrl+X** - Cut selected notes
- **Ctrl+C** - Copy selected notes
- **Ctrl+V** - Paste notes at current position
- **Ctrl+,** - Open Composition Settings dialog

### Mouse Interactions (Composition Tab)

- **Left-click + drag** - Draw discrete (snapped) or continuous (free pitch) notes depending on active tool
- **Left-click on note** - Select note, shows resize handles and curve editing dots
- **Left-drag selected note** - Move note in time and pitch
- **Left-drag resize handles** - Change note duration
- **Left-drag curve dots** - Edit dynamics or pitch curves
- **Right-click on continuous note** - Quantize pitch curve to scale degrees (snap to scale)

### Future Features (Roadmap)

- **User-Configurable Parameter Limits** — Allow users to set min/max ranges for container parameters (e.g., Physics System springK: 0.0001-1.0 could be customized to 0.0001-10.0). Could be implemented per-container or globally in Preferences tab. This would enable more extreme/experimental sound design while keeping default ranges sensible for typical use.

## Key Gotchas

1. **Connection Direction:** Connections internally always store output→input, but UI logic may start from input side (direction flip happens in `SounitBuilder::onPortClicked`)

2. **Qt Designer UI Files:** Changes to .ui files require rebuild to regenerate `ui_*.h` headers

3. **MOC (Meta-Object Compiler):** Any class with Q\_OBJECT macro needs MOC processing — CMake handles this automatically

4. **Container Colors:** Each category has distinct color scheme (Blue=Essential, Orange=Shaping, Green=Modifiers) — used for both container headers and connection rendering

5. **Pending Connection State:** Canvas maintains separate state for in-progress connections (`hasPendingConnection`, `pendingFromContainer`, etc.) distinct from completed connections vector

6. **Selection State:** Only one container or connection can be selected at a time (no multi-select yet)

7. **Automatic Graph Rebuild:** The audio graph automatically rebuilds when:
   - Container parameters change (via `Container::parameterChanged()` signal)
   - Connections are added/removed (via `Canvas::graphChanged()` signal)
   - Containers are added (via direct call)

   This means you don't need to manually call `rebuildGraph()` after parameter changes - the signal/slot architecture handles it automatically.

   **Batching Parameter Updates:** When setting multiple parameters (e.g., custom DNA with 64 harmonics), use `beginParameterUpdate()` / `endParameterUpdate()` to batch updates and trigger only one rebuild:
   ```cpp
   container->beginParameterUpdate();
   for (int i = 0; i < 64; i++) {
       container->setParameter(QString("customDna_%1").arg(i), values[i]);
   }
   container->endParameterUpdate();  // Triggers single rebuild
   ```
   Without batching, each `setParameter()` would trigger a rebuild, causing 64 rebuilds instead of 1.

8. **Graph Validation:** When a connection creates an invalid graph, a warning message box appears and the audio engine reverts to "direct mode" (fallback). Common causes of invalid graphs:
   - Missing or multiple Spectrum to Signal containers (exactly one required)
   - Disconnected containers
   - Circular dependencies

   The validation happens automatically during graph rebuild in `AudioEngine::buildGraph()`.

9. **Input Port Default Values:** When a container input port has no connection, it uses a default value during graph processing. These defaults are defined in `SounitGraph::processContainer()`:
   - Harmonic Generator: purity=0.0 (pure DNA), drift=0.0
   - Other containers: check sounitgraph.cpp for specific defaults

   **Important:** The default purity of 0.0 means "pure DNA pattern" - do NOT change this to 1.0 or DNA changes will be inaudible (1.0 = flat spectrum).

10. **Continuous Notes (Curves):** The Note class supports both discrete and continuous notes via curves:
   - **Discrete notes**: Constant pitch (no pitch curve) and constant dynamics (2-point flat curve at 0.7)
   - **Continuous notes**: Pitch and dynamics vary over time via multi-point curves captured from pen input

   During note creation in continuous mode (`ScoreCanvas`):
   - `pitchPoints` are collected during mouse/pen movement and converted to a pitch curve
   - `pressurePoints` are collected during mouse/pen movement and converted to a dynamics curve
   - Curves are normalized to 0.0-1.0 time within the note duration

   During rendering (`AudioEngine::renderNotes()`), the engine samples curves at each audio sample:
   - `note.getPitchAt(noteProgress)` returns Hz value at that point in the note (0.0-1.0)
   - `note.getDynamicsAt(noteProgress)` returns amplitude multiplier at that point

   This enables glissando, portamento, crescendo, diminuendo, and other expressive techniques. The render cache detects curve changes by comparing point counts.

11. **Note Envelope:** Each rendered note has a simple ADSR envelope applied (in addition to the dynamics curve):
   - **Attack**: First 5% of note - quick fade in
   - **Sustain**: Middle 85% of note - full amplitude
   - **Release**: Last 10% of note - gentle fade out

   This envelope prevents clicks at note boundaries. The dynamics curve is applied on top of this envelope as a multiplier. Defined in `AudioEngine::renderNotes()` at audioengine.cpp:397-411.

## Related Documentation

All design specifications are in `docs/md format/`. Key documents:

- **Calamus Architecture v1.4.md** — System overview, threading, scope  
- **calamus\_sounit\_builder\_canvas\_specification\_v1\_0.md** — Sound engine canvas interaction patterns  
- **calamus\_score\_canvas\_specification\_v1.md** — Composition interface design  
- **Container Port Specification v1.2.md** — Complete port definitions for all 11 container types  
- **Calamus Input Engine Specification v1.2.md** — Pen input capture design

Refer to these when implementing features or understanding design intent.  
