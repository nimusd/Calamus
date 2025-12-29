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

```bash
# Configure (from project root)
cmake -S . -B build -G "MinGW Makefiles"

# Build
cmake --build build

# Run executable
./build/Desktop_Qt_6_10_1_MinGW_64_bit-Debug/Calamus.exe
```

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
- Manages 3-tab interface: Sound Engine (tab 0), Composition (tab 1), Preferences (tab 2)
- Creates and orchestrates SounitBuilder and ScoreCanvasWindow
- Signal: `onTabChanged(int)` shows/hides appropriate child windows

**SounitBuilder** (`sounitbuilder.h/.cpp`)
- Sound synthesis module (tab 0)
- Graph-based interface for connecting audio containers
- Manages toolbar with 11 audio processor types (color-coded by category)
- Handles connection creation between container ports
- Current state: UI framework with basic patching logic

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
- Emits signals: `portClicked()`, `moved()`, `clicked()`

**ScoreCanvasWindow** (`scorecanvaswindow.h/.cpp`)
- Music composition interface (tab 1)
- Toolbar with draw/record tools for discrete/continuous notes
- Current state: Minimal implementation (UI framework only)

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

```
Container Port Click Flow:
  User clicks port circle → Container::portClicked() signal
  → SounitBuilder::onPortClicked() slot
  → Stores start point OR creates connection
  → Canvas::addConnection() or Canvas::startPendingConnection()
  → Canvas::update() triggers repaint

Container Movement Flow:
  Container::moved() signal
  → Canvas::update() (redraws all connections)
```

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
- **Selection highlighting:** Blue (#0066CC) for selected connections, 1px border for containers
- **Hit testing:** 10px tolerance for connection selection

### Connection Logic
- Connections are **directional**: Output → Input only
- Validation prevents same-type connections (no input→input or output→output)
- First port click starts pending connection; second click completes or cancels
- Connection direction auto-flips: if start is input, swap from/to in Connection struct

### Port Representation
```cpp
struct PortInfo {
    QString name;          // Port identifier
    bool isOutput;         // Direction
    QLabel *circle;        // Visual indicator (●)
};
```

### Connection Representation
```cpp
struct Connection {
    Container *fromContainer;  // Source (must be output port)
    QString fromPort;
    Container *toContainer;    // Destination (must be input port)
    QString toPort;
};
```

## Threading Model (Planned)

Per Architecture v1.4:

- **Audio Thread (highest priority):** Mix pre-rendered buffers, run live engine for active selection. Never allocates memory, waits on locks, or touches UI.
- **UI Thread (normal):** User interaction, display updates. No heavy computation.
- **Render Thread (low priority):** Pre-render dirty phrases in background.

**Communication:**
- UI ↔ Render: Qt signals/slots
- UI → Audio: Lock-free SPSC queue for commands
- Audio → UI: Lock-free SPSC queue for status updates

**Current State:** Threading not yet implemented; all runs on UI thread.

## File Structure

```
D:\C++ code\Calamus\
├── CMakeLists.txt              # Build configuration
├── main.cpp                    # Entry point (minimal)
├── calamusmain.h/.cpp/.ui      # Main tabbed window
├── sounitbuilder.h/.cpp/.ui    # Sound engine module
├── canvas.h/.cpp               # Graph rendering surface
├── container.h/.cpp/.ui        # Audio unit widget
├── scorecanvaswindow.h/.cpp/.ui # Composition module (stub)
├── docs/md format/             # Design specifications
│   ├── Calamus Architecture v1.4.md
│   ├── calamus_sounit_builder_canvas_specification_v1_0.md
│   ├── calamus_score_canvas_specification_v1.md
│   ├── Container Port Specification v1.2.md
│   └── [other spec files]
└── build/                      # Qt build artifacts
```

## Important Specifications

**Container Port Design Principles** (from Container Port Specification v1.2):
1. **No Hard-Coded Limits:** All numeric constraints configurable via Preferences
2. **Everything Modulatable is an Input:** Any parameter that benefits from modulation is exposed as input port with default value
3. **Modular Composition:** Instantiate containers multiple times, chain freely, signal path must end with Signal output

**Connection Functions** (how outputs combine with inputs):
- `passthrough` — source replaces input entirely
- `add` — input + source × weight
- `multiply` — input × source × weight
- `subtract` — input - source × weight
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

### Not Yet Implemented
- Audio backend (no actual sound generation)
- Data persistence (save/load)
- ScoreCanvasWindow functionality (composition interface is UI skeleton only)
- Undo/redo (planned: Qt QUndoStack)
- Connection validation beyond direction checking
- Marquee selection
- Zoom/pan navigation
- Transport controls functionality
- Threading model
- Preferences tab

### Future Features (Roadmap)
- **User-Configurable Parameter Limits** — Allow users to set min/max ranges for container parameters (e.g., Physics System springK: 0.0001-1.0 could be customized to 0.0001-10.0). Could be implemented per-container or globally in Preferences tab. This would enable more extreme/experimental sound design while keeping default ranges sensible for typical use.

## Key Gotchas

1. **Connection Direction:** Connections internally always store output→input, but UI logic may start from input side (direction flip happens in `SounitBuilder::onPortClicked`)

2. **Qt Designer UI Files:** Changes to .ui files require rebuild to regenerate `ui_*.h` headers

3. **MOC (Meta-Object Compiler):** Any class with Q_OBJECT macro needs MOC processing — CMake handles this automatically

4. **Container Colors:** Each category has distinct color scheme (Blue=Essential, Orange=Shaping, Green=Modifiers) — used for both container headers and connection rendering

5. **Pending Connection State:** Canvas maintains separate state for in-progress connections (`hasPendingConnection`, `pendingFromContainer`, etc.) distinct from completed connections vector

6. **Selection State:** Only one container or connection can be selected at a time (no multi-select yet)

## Related Documentation

All design specifications are in `docs/md format/`. Key documents:
- **Calamus Architecture v1.4.md** — System overview, threading, scope
- **calamus_sounit_builder_canvas_specification_v1_0.md** — Sound engine canvas interaction patterns
- **calamus_score_canvas_specification_v1.md** — Composition interface design
- **Container Port Specification v1.2.md** — Complete port definitions for all 11 container types
- **Calamus Input Engine Specification v1.2.md** — Pen input capture design

Refer to these when implementing features or understanding design intent.
