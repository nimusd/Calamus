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

- **Continuous note playback end cut-off** - Fixed issue where continuous notes weren't playing completely to the end. The note progress calculation now properly reaches 1.0 at the final sample by using `(noteDurationSamples - 1)` as the denominator instead of `noteDurationSamples`. This ensures the entire pitch curve is sampled during rendering. Fixed in `AudioEngine::renderNotes()` at audioengine.cpp:394-401.

- **Invalid graph warning at startup** - Removed the false warning dialog that appeared before the main UI when the app started with an empty canvas. The warning now only appears when making actual connections that create invalid graphs. Fixed in `SounitBuilder::rebuildGraph()` at sounitbuilder.cpp:144.

- **Track color conflict bug** - Fixed issue where loading a sounit to track 0 and then adding a second track would result in both tracks having identical deep blue colors. Implemented centralized color management system:
  - Added `trackColorPalette` and `nextColorIndex` members to ScoreCanvasWindow (scorecanvaswindow.h:115-116)
  - Created `getNextTrackColor()` method that cycles through 6 colors and tracks usage (scorecanvaswindow.cpp:1550-1557)
  - Updated `onAddTrackTriggered()` to use `getNextTrackColor()` instead of static color index (scorecanvaswindow.cpp:1495)
  - Updated `onLoadSounit()` to use `getNextTrackColor()` when assigning sounit to track (calamusmain.cpp:1341)
  - Removed duplicate `updateSounitTrack()` call from `onSounitNameChanged()` to prevent double color assignment (calamusmain.cpp:1416-1418)
  - Now each track gets a unique color from the palette: Deep blue, Deep pink, Lime green, Orange, Purple, Cyan (cycles after 6 tracks)

- **Track assignment bug** - Fixed issue where loading a sounit would always assign to track 0 regardless of user selection. The dialog allowed selecting any track, but the sounit always went to track 0.
  - Added `trackIndex` parameter to `SounitBuilder::rebuildGraph()` with default value 0 for compatibility (sounitbuilder.h:78, sounitbuilder.cpp:136)
  - Added `trackIndex` parameter to `ScoreCanvasWindow::updateSounitTrack()` (scorecanvaswindow.h:150, scorecanvaswindow.cpp:1355)
  - Updated `onLoadSounit()` to pass the user-selected track index to both `rebuildGraph()` and `updateSounitTrack()` (calamusmain.cpp:1338, 1342)
  - Now sounits are correctly assigned to the track selected by the user, with proper graph building and name/color updates

### Recently Added

- **Sounit Navigation in Sound Engine Tab** - Navigate between loaded sounits using a two-row interface in the Sounit Info section:
  - **Row 1 - "Edit Sounit:"** Non-editable dropdown (`comboSounitSelector`) for selecting which sounit to edit
    - Shows all loaded sounits in format "Track N: Sounit Name"
    - Selecting a different sounit reloads it into the Sound Engine canvas for editing
  - **Row 2 - "Name:"** Editable text field (`editSounitName`) for renaming the current sounit
    - Type a new name and press Enter to rename
    - Name changes are reflected in the dropdown and track display
  - Automatically tracks which sounit file belongs to which track (`trackSounitFiles`, `trackSounitNames` maps)
  - Maintains current editing track state (`currentEditingTrack`)
  - When switching sounits, reloads from file and rebuilds the graph for that track
  - Allows editing any loaded sounit without having to reload from disk manually
  - Implementation:
    - `updateSounitSelector()` refreshes dropdown with all loaded sounits (calamusmain.cpp:1242-1270)
    - `switchToSounit(trackIndex)` loads selected sounit into canvas (calamusmain.cpp:1272-1304)
    - `onSounitSelectorChanged(index)` handles dropdown selection (calamusmain.cpp:1306-1317)
    - `onSounitNameEdited()` handles renaming current sounit (calamusmain.cpp:1526-1531)
    - `onLoadSounit()` updated to track file paths and update selector (calamusmain.cpp:1429-1435)
  - Files: calamusmain.h, calamusmain.cpp, calamusmain.ui

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

- **Snap Continuous Note to Scale** - Quantize continuous notes to scale degrees using two methods:
  - **Right-click** on any continuous note to quantize it immediately
  - **Toolbar button** "Snap to Scale" - quantizes all selected continuous notes at once
  - Converts smooth glissando/portamento into stepped pitch changes
  - Uses hysteresis algorithm: holds current pitch until crossing halfway to next scale degree
  - Only affects continuous notes (notes drawn with pitch curves)
  - Preserves original note timing and dynamics
  - Useful for converting free-form gestures into quantized melodies
  - Implementation: scorecanvas.cpp:1681-1762 (quantizePitchCurveToScale), scorecanvas.cpp:765-783 (right-click handler), scorecanvas.cpp:375-408 (toolbar button handler)

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
- **Right-click on continuous note** - Quantize single note's pitch curve to scale degrees

### Toolbar Actions (Composition Tab)

- **Draw Discrete Notes** (D) - Draw notes that snap to scale lines
- **Draw Continuous Notes** (C) - Draw notes with free pitch (glissando/portamento)
- **Select** (S) - Selection mode for editing existing notes
- **Zoom** (Z) - Zoom mode for adjusting frequency and time range
- **Snap to Scale** - Quantize selected continuous notes to scale degrees (with hysteresis)

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

   The validation happens automatically during graph rebuild in `AudioEngine::buildGraph()`. Note: The warning is suppressed when the canvas is empty (e.g., at app startup) to avoid showing a false warning before any containers are added.

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

12. **Track System (Multi-Sounit Support Implemented):**
   - **Track Selector Widget:** The `TrackSelector` widget exists and can display multiple tracks vertically. Each track shows its name, color, and frequency range. Located at trackselect.h/cpp.
   - **Note-Track Association:** Each `Note` has a `trackIndex` field (note.h:59) that defaults to 0. This determines which track/sounit the note uses during rendering.
   - **Active Track:** `ScoreCanvas` has `activeTrackIndex` (scorecanvas.h:137) which determines which track new notes are assigned to.
   - **Default Track:** At startup, one track "(No Sounit)" is created in gray (scorecanvaswindow.cpp:323).
   - **Loading Sounits:** When loading a sounit via File > Load Sounit (calamusmain.cpp:1272), it updates track 0 with the sounit's name and color (scorecanvaswindow.cpp:1338-1356).

   **Multi-Track Architecture (IMPLEMENTED):**
   - `AudioEngine` now stores multiple sounit graphs using `QMap<int, SounitGraph*> trackGraphs` (audioengine.h:61).
   - Each track can have its own sounit graph, identified by `trackIndex`.
   - **Graph Management:**
     - `buildGraph(Canvas *canvas, int trackIndex)` - Build/rebuild graph for specific track (audioengine.cpp:128)
     - `clearGraph(int trackIndex)` - Clear graph for specific track (audioengine.cpp:167)
     - `clearAllGraphs()` - Clear all track graphs (audioengine.cpp:181)
     - `hasGraph(int trackIndex)` - Check if track has valid graph (audioengine.cpp:194)
   - **Rendering:** During `renderNotes()`, each note looks up the correct graph using `note.getTrackIndex()` (audioengine.cpp:395-470).
   - **Fallback:** If a note's track has no graph, it uses the `HarmonicGenerator` fallback.
   - **SounitBuilder:** Always builds graph for track 0 (default track) when editing in the Sound Engine tab (sounitbuilder.cpp:140).

   **Current Status - What Works:**
   - ✅ Multiple sounit graphs can be loaded simultaneously
   - ✅ Each note renders using its track's graph
   - ✅ No crash when loading multiple sounits
   - ✅ Audio engine logs show which tracks have graphs loaded

   **UI for Track Management (IMPLEMENTED):**
   - **Add Track Button:** Located in the Tracks group box in the Composition tab (calamusmain.ui:845)
   - **Track Menu:** Score Canvas Window has a "Track" menu with "Add Track..." action (scorecanvas.ui:123-128)
   - **onAddTrack():** Slot in CalamusMain shows info dialog about track management (calamusmain.cpp:1626)
   - **onAddTrackTriggered():** Slot in ScoreCanvasWindow prompts for track name and creates new track (scorecanvaswindow.cpp:1469)
     - Uses QInputDialog to get track name from user
     - Assigns cycled colors to new tracks (blue, pink, green, orange, purple, cyan)
     - Adds track to TrackSelector with full frequency range (A0-C8)

   **Sounit Loading & Track Assignment (IMPLEMENTED):**
   - **onLoadSounit():** Modified to ask which track to assign the loaded sounit to (calamusmain.cpp:1360-1442)
     - Shows dropdown of available tracks with current names
     - User selects target track from dropdown
     - Calls `rebuildGraph(trackIndex)` to build audio graph for the selected track (calamusmain.cpp:1423)
     - Calls `updateSounitTrack(trackIndex, name, color)` to update the selected track's name and color (calamusmain.cpp:1427)
     - Track receives next color from palette via `getNextTrackColor()` (calamusmain.cpp:1426)
     - Tracks file path and name in maps for later navigation (calamusmain.cpp:1430-1432)
     - Updates sounit selector dropdown (calamusmain.cpp:1435)
     - Displays confirmation with track index

   **Sounit Navigation (IMPLEMENTED):**
   - **Sounit Selector UI:** Two-row interface in Sound Engine tab's Sounit Info section
     - **Edit Sounit dropdown:** Select which sounit to edit (shows "Track N: Sounit Name")
     - **Name text field:** Rename the currently selected sounit
     - Selecting a different sounit reloads it from file into the canvas for editing
     - Name changes update the dropdown list and track display automatically
     - Maintains state of which track is currently being edited
     - Automatically updates when new sounits are loaded

   **Auto-Rendering on Track Change (IMPLEMENTED):**
   - **onTrackSelected():** Triggered when user clicks a track in TrackSelector (scorecanvaswindow.cpp:1508)
     - Sets the clicked track as active in ScoreCanvas
     - Automatically calls `prerenderNotes()` to render all notes with current track configurations
   - **prerenderNotes():** Pre-renders all notes without starting playback (scorecanvaswindow.cpp:1519)
     - Called automatically whenever track selection changes
     - Ensures audio buffer is ready with correct sounit for each note's track
     - Logs rendering status to console
   - **TrackSelector Signal:** `trackSelected(int trackIndex)` connected to trigger auto-rendering (scorecanvaswindow.cpp:129)

   **Track Display Behavior:**
   - **Track Bar Labels:** The TrackSelector displays the track name vertically on each track bar (trackselect.cpp:215)
   - **Track Naming:** When a sounit is loaded to a track, the track is renamed to the sounit name (scorecanvaswindow.cpp:1358 calls `updateTrack()`)
   - **Initial State:** Track 0 starts as "(No Sounit)" in gray #999999 (scorecanvaswindow.cpp:329)
   - **After Sounit Load:** Track 0 is renamed to the sounit's name and recolored using the next available color from the palette
   - **Sounit Info Display:** The Sound Engine tab's "Sounit Info" section (calamusmain.ui:29-68) shows:
     - `editSounitName` - Displays the sounit name from the loaded JSON file (sounit.name field)
     - `editSounitUsedBy` - Read-only field showing which tracks use this sounit (not yet implemented)
     - `editSounitComment` - Displays the sounit comment from JSON (sounit.comment field)

   **Track Color Management:**
   - Centralized color palette stored in `ScoreCanvasWindow::trackColorPalette` (scorecanvaswindow.h:115)
   - Colors cycle through: Deep blue, Deep pink, Lime green, Orange, Purple, Cyan
   - `getNextTrackColor()` method returns next color and increments index (scorecanvaswindow.cpp:1550-1557)
   - Used by both "Add Track" and "Load Sounit" operations to ensure unique colors
   - Index tracked in `nextColorIndex` to avoid duplicate colors across tracks

   **Still To Implement:**
   1. Visual feedback in ScoreCanvas:
      - Active track notes: full color (100% alpha)
      - Selected tracks notes: pale color (~20% alpha)
      - Unselected tracks: notes not shown
   2. Clicking a note should activate its track
   3. Allow user to assign/change a sounit to an existing track (currently only during load)
   4. Track inspector tab to show/edit track properties
   5. Populate `editSounitUsedBy` field to show which tracks use the current sounit

   **Files Modified:**
   - audioengine.h/cpp - Multi-graph support with QMap
   - sounitbuilder.cpp - Pass track index to buildGraph()
   - trackselect.h/cpp - Track selector widget (already supports multiple tracks visually)
   - note.h/cpp - Note has trackIndex (already exists)
   - scorecanvas.h/cpp - Note rendering and track selection (visual changes pending)
   - calamusmain.cpp - Sounit loading logic (UI changes pending)
   - scorecanvaswindow.h/cpp - Track selector setup (UI changes pending)

## Pre-Rendering Optimization (Partial/Incremental Rendering)

**Current State (Monolithic Rendering):**

The `AudioEngine::renderNotes()` method (audioengine.cpp:292-494) currently renders ALL notes into a single monolithic buffer:
- Creates one continuous `std::vector<float> renderBuffer` for entire composition
- Buffer size = total composition duration in samples (e.g., 5 minutes = ~13M samples at 44.1kHz)
- All-or-nothing cache invalidation: ANY note change or graph change triggers full re-render
- Cache comparison checks note count, properties, and curve point counts (audioengine.cpp:302-334)
- `renderCacheDirty` flag invalidated on graph changes (audioengine.cpp:161, 175, 190)

**Problem:**
- Editing a few notes at 4:30 in a 5-minute piece re-renders all ~13 million samples
- No concept of "dirty regions" or partial re-rendering
- `Phrase::isDirty` flag exists (phrase.h:34-38) but is unused
- Inefficient for long compositions with localized edits

**Proposed Solution: Segment-Based Rendering**

Divide timeline into fixed-duration segments and only re-render segments containing changed notes.

**Architecture Design:**

```cpp
// New data structures in audioengine.h
struct RenderSegment {
    double startTimeMs;           // Segment start time
    double endTimeMs;             // Segment end time
    std::vector<float> samples;   // Pre-rendered audio for this segment
    QSet<QString> noteIds;        // Note IDs affecting this segment
    bool isDirty;                 // Needs re-rendering?
    uint64_t hash;                // Quick comparison hash
};

std::vector<RenderSegment> renderSegments;  // Replace single renderBuffer
double segmentDurationMs;  // e.g., 1000ms (configurable in Preferences)
```

**Algorithm:**

1. **Initialization**: Divide composition into segments (5-min piece = 300 segments at 1s each)
2. **Change Detection**: Compare incoming notes with `cachedNotes` to find changes
3. **Dirty Marking**: Mark segments overlapping changed notes' time ranges
4. **Partial Re-render**: Only render dirty segments, reuse clean segments
5. **Playback**: Stream from segments sequentially

**Benefits:**
- 5-minute piece with edits at 4:30 → re-render ~10 segments instead of 300
- Proportional performance: render time scales with edit scope, not composition length
- Memory overhead minimal (segment metadata only)

**Implementation Phases:**

1. **Phase 1 - Segment Infrastructure**: Add `RenderSegment` struct, build segment map, keep full re-render (validate correctness)
2. **Phase 2 - Change Detection**: Identify which notes changed, mark affected segments dirty
3. **Phase 3 - Partial Rendering**: Only render dirty segments, stream from segment cache
4. **Phase 4 - Optimization**: Tune segment duration, coalesce adjacent dirty segments, consider multi-threading

**Edge Cases:**
- Notes spanning multiple segments → add note ID to all affected segments
- Overlapping notes → mix samples (don't overwrite like current code at audioengine.cpp:482)
- Graph changes → mark ALL segments dirty (preserve current safety behavior)
- Continuous note curves → invalidate segments when curve points change

**Configuration:**
- `segmentDurationMs` user-configurable in Preferences (default: 1000ms)
- Trade-off: smaller segments = finer granularity but more overhead

**Files to Modify:**
- audioengine.h/cpp - Segment data structures and rendering logic
- scorecanvaswindow.cpp - Trigger partial renders on note edits
- phrase.h/cpp - Utilize existing `isDirty` flag to track changes

**Current Status:** Design phase - awaiting implementation.

## Related Documentation

All design specifications are in `docs/md format/`. Key documents:

- **Calamus Architecture v1.4.md** — System overview, threading, scope  
- **calamus\_sounit\_builder\_canvas\_specification\_v1\_0.md** — Sound engine canvas interaction patterns  
- **calamus\_score\_canvas\_specification\_v1.md** — Composition interface design  
- **Container Port Specification v1.2.md** — Complete port definitions for all 11 container types  
- **Calamus Input Engine Specification v1.2.md** — Pen input capture design

Refer to these when implementing features or understanding design intent.  
