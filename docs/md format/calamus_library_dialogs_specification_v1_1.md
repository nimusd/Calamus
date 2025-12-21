

Calamus UI Specification
## Library Dialog Framework
## Version 1.1 — December 2025
## Overview
Calamus uses a consistent Library Dialog Framework for managing presets across multiple domains: DNA
sequences, envelopes, easing functions, vibrato settings, and gestures. This framework provides a unified
user experience while accommodating type-specific editing and preview capabilities.
Design Principle: One pattern, many payloads. Users learn the library interaction once and apply
it everywhere.
## Common Framework
Window Properties (All Libraries)
PropertyValue
ModalYes
Size500 × 400 px (resizable)
Minimum Size400 × 300 px
## Layout Structure

## Preset List Panel
ElementDescription
List widgetScrollable list showing all presets
SelectionSingle-select; clicking loads preset into Editor Panel
Visual indicatorSelected item highlighted; modified item shows bullet (•)
[New] buttonCreates blank preset, adds to list as "Untitled"
[Delete] buttonRemoves selected preset (with confirmation)
## Preset Categories:
- Factory — Built-in presets (read-only, cannot delete)
- User — User-created presets
Factory and User presets shown in the same list with subtle visual distinction (e.g., factory presets in italics or with icon).

## Bottom Bar
ElementDescription
Name fieldEditable preset name
[Cancel]Close dialog, discard all changes
[Save]Save current editor state to selected preset
[OK]Save (if modified) and close, applying selection
## Common Behaviors
ActionResult
Select presetLoad into Editor Panel and Preview; Name field updates
Edit any parameterPreset marked as modified (•); [Save] enables
[New] clickedCreate "Untitled" preset, select it, clear Editor Panel to defaults
[Delete] clickedConfirmation dialog; if confirmed, remove preset, select next
[Save] clickedWrite current state to selected preset; clear modified flag
[OK] clickedIf modified, prompt to save; apply selection to caller; close
[Cancel] or EscapeIf modified, prompt "Discard changes?"; close without applying
Edit Name fieldPreset marked as modified
Double-click presetSame as select + [OK] (quick apply)
## Delete Confirmation
Library-Specific Implementations

- DNA Library
Window Title: "DNA Library"
Opened From: Sound menu→ DNA Library...
Purpose: Create and manage harmonic distribution presets that define the instrument's "genetic identity."
## Preview Area:
- Spectrum bar graph showing harmonic amplitudes (1–64)
- X-axis: harmonic number
- Y-axis: relative amplitude
- Updates in real-time as parameters change
- Interactive when Sequence = Custom (click/drag to edit)
## Editor Panel:
ControlTypeDescription
SequenceDropdownBase pattern: All, Odd, Even, OddDominant,
## Fundamental+, Octaves, Custom
PuritySlider 0–100%Blend: 0% = pure sequence, 100% = all harmonics
[Edit Table...]ButtonOpens table view for precise numeric entry
(visible when Sequence = Custom)
## Custom Pattern Editing:
- Click on spectrum bar to select harmonic; drag up/down to set amplitude
(0–100%) • Shift+click to toggle harmonic on/off
- Drag across multiple bars to "paint" a curve
- [Edit Table...] opens scrollable table for precise numeric entry
## Factory Presets:
## • All Harmonics
- Odd Only (Clarinet)
- Even Only (Hollow)
## • Odd Dominant
- Fundamental+ (Near-sine)
- Octaves Only (Organ)
## 2. Envelope Library

Window Title: "Envelope Library"
Opened From: Sound menu→ Envelope Library..., Note Inspector [Edit...]
Purpose: Create and manage arbitrary envelope curves for parameter modulation over note or gesture
lifetime.
## Preview Area:
- Curve graph with time (0–100%) on X-axis, value (0–100%) on Y-axis
- Control points shown as draggable handles
- Grid lines at 25% intervals
## Editor Panel:
ControlTypeDescription
Curve canvasInteractiveClick to add points; drag to move; right-click to delete
Point listTableTime, Value, Curve Type for each point
Curve TypeDropdown per
segment
Linear, Smooth, Step (extensible at code time)
Loop ModeDropdownNone, Loop, Ping-pong
[Reset]ButtonClear to default two-point envelope (0,0→ 100,100)
## Point Editing:
- Minimum 2 points (start and end)
- Points snap to grid when Shift held
- Drag beyond canvas edges to delete
## Factory Presets:
- Linear Rise • Linear Fall • Attack-Decay • Swell
## • Fade Out • Bell Curve • Step Up • Step Down
## 3. Easing Library
Window Title: "Easing Library"
Opened From: Sound menu→ Easing Library...
Purpose: Create and manage easing functions that shape parameter transitions.
## Preview Area:
- Curve graph showing easing shape
- X-axis: progress (0–1)
- Y-axis: eased value (0–1)
## Editor Panel:

ControlTypeDescription
Easing TypeDropdownLinear, Quad, Cubic, Quart, Quint, Sine,
## Expo, Circ, Back, Elastic, Bounce
ModeRadio buttonsIn, Out, InOut
StrengthSliderAvailable for Back, Elastic
(controls overshoot/oscillation)
## Factory Presets:
- Linear • Ease In (Quad) • Ease Out (Quad) • Ease In-Out (Quad)
- Ease In (Cubic) • Ease Out (Cubic) • Ease In-Out (Cubic)
## • Bounce Out • Elastic Out • Back Out
## 4. Vibrato Library
Window Title: "Vibrato Library"
Opened From: Compose menu→ Vibrato Library...
Purpose: Create and manage vibrato presets applicable to phrases or notes.
## Preview Area:
- Waveform visualization showing vibrato shape over time
- Displays rate, depth variation, onset ramp, development curve
## Editor Panel:
ControlTypeRangeDescription
RateSlider + field0.1–20 HzVibrato speed
DepthSlider + field0–100 centsPitch deviation
OnsetSlider + field0–100%When vibrato starts (% into note)
DevelopmentDropdownInstant, Linear,
## Exponential,
S-Curve,Envelope
How vibrato grows
Dev. EnvelopeSelector
(from Envelope Library)Visible when Development =
RegularitySlider + field0–100%0 = mechanical, 100 = organic
Physics MinNumeric input0–10000Minimum mass

Physics MaxNumeric input0–10000Maximum mass
## Factory Presets:
## • None (flat) • Subtle • Classical • Romantic
## • Wide & Slow • Tight & Fast • Late Entry • Building
## 5. Gesture Library
Window Title: "Gesture Library"
Opened From: Compose menu→ Gesture Library...
Purpose: Create and manage reusable gesture templates that can be applied to notes.
## Preview Area:
- Stroke visualization showing gesture path
- Line thickness varies with pressure data
- Tilt indicated by slight offset/shadow
## Editor Panel:
ControlTypeDescription
Gesture previewCanvasVisual representation of captured stroke
DurationDisplay (read-only)Length of gesture in ms
## Parameter
## Assignments
TableSix rows: X, Y, Pressure, TiltX, TiltY,
Rotation→ Parameter dropdown each
[Capture New]ButtonOpens Gesture Capture Dialog
[Scale to Fit]CheckboxStretch/compress gesture to match note duration
## Parameter Assignment Targets:
## • None • Pitch Offset • Brightness • Breath • F1 Offset • F2 Offset • Dynamics
- (extensible per sounit parameters)
## Factory Presets:
## • Linear Swell • Accent Attack • Fade Out
## • Scoop Up • Fall Away • Tremolo Pressure
## Gesture Capture Dialog
When [Capture New] is clicked in the Gesture Library, a separate dialog opens providing adequate space for
natural gesture capture.

## ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■ Capture Gesture [X] ■
## ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
## ■ ■
## ■ ■
■ [Large Capture Canvas] ■
■ (400 × 300 px minimum) ■
## ■ ■
## ■ ■
## ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■ Duration: 0.00 ms [Clear] [Accept] ■
## ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ActionResult
Draw on canvasCaptures gesture; Duration updates in real-time
[Clear]Resets canvas for another attempt
[Accept]Returns to Gesture Library with captured data
Escape or [X]Cancels, returns without changes
Vibrato Editor (Separate Dialog)
The Vibrato Editor is distinct from the Vibrato Library. It edits vibrato parameters for a specific note, with the
option to load presets from the library.
Window Title: "Vibrato Editor"
Opened From: Note Inspector→ Vibrato [Edit]
## Window Properties:
PropertyValue
ModalYes
Size350 × 300 px
ResizableNo

## Behavior:
ActionResult
Preset dropdownLists all Vibrato Library presets
[Load]Copies selected preset values into editor fields
Adjust any sliderUpdates preview; values now differ from loaded preset
[OK]Apply values to note's vibrato; close
[Cancel]Discard changes; close
Note: The Vibrato Editor does not save back to the library — it only applies values to the current note. To save a new
preset, use the Vibrato Library dialog.
## Menu Integration Summary
MenuItemOpens
SoundDNA Library...DNA Library
SoundEnvelope Library...Envelope Library

SoundEasing Library...Easing Library
ComposeVibrato Library...Vibrato Library
ComposeGesture Library...Gesture Library
Note InspectorVibrato [Edit]Vibrato Editor
Note InspectorParameter Curves [Edit...]Envelope Library
## File Storage
All user presets stored in:
[User Documents]/Calamus/Presets/
## DNA/
## Envelopes/
## Easing/
## Vibrato/
## Gestures/
File format: JSON with.calamus-[type]  extension (e.g.,.calamus-dna,
## .calamus-env)
— End of Specification —