

Calamus UI Specification
## Composition Tab
## Version 1.0 — December 2025
## Overview
The Composition Tab is where music is written. When this tab is active, the Canvas Window
displays the Score Canvas — horizontal scale lines with note curves, phrase containers, and
gesture data.
The Control Panel provides phrase/note/gesture inspection and editing, pen configuration,
and scale display. The Canvas provides the visual score and is the primary drawing surface.
## Key Concepts
## Phrases
A phrase is a container for notes and gestures. Phrases have their own properties
(transpose, envelopes, easing, physics) that affect all contents.
AspectSpecification
Visual shapeContour following notes — hull/halo with consistent width
around note curves
ColorMatches track/sounit color
Name displayInspector only — no clutter on canvas
SelectionClick the boundary to select phrase
CreationDraw notes first → group afterward (right-click, 'P' shortcut, or
toolbar)
NestingPhrases can contain other phrases, up to full composition as
top-level phrase
## Notes
Notes are the atomic compositional elements. Each note captures pen gesture data and has
an embedded vibrato container (inactive by default).
•Click a note curve to select it
•Short notes: attack is primary concern, limited envelope time
•Long notes: benefit most from gesture/envelope definition
## Gestures
A gesture is 6-dimensional captured input from the Wacom pen, carrying expressive
information beyond a simple curve.
DimensionTypical Mapping
X positionTime
Y positionPitch (for notes) or primary parameter (for gestures)
PressureDynamics / Intensity (visual: line thickness)
Tilt XBrightness (visual: transparency)
Tilt YBreath

DimensionTypical Mapping
RotationVibrato depth (visual: edge shape)
## Gesture Lifecycle:
•Can be captured as a note (performance data embedded)
•Can be captured as standalone gesture (reusable modifier)
•Can be promoted from note to library for reuse
•Can function as multi-dimensional envelope
•Auto-named, renameable, saveable to Gesture Library
Modulation and Scale System
## Note Naming: Numeric Degrees
Calamus uses numeric note names (1, 2, 3, 4, 5, 6, 7...) rather than letter names. This unifies
all tuning traditions — Western (ABCDEFG), French (do ré mi...), Persian, Indian, Chinese —
under a single, intuitive system.
The number IS the function. Note 3 is the third degree. No separate learning of "E is the third
of C major." The name carries its meaning.
## Scale Definition
Every composition begins with a Scale Definition. Three components:
ComponentDescription
Tonal CenterNote number serving as "home" (e.g., 1, or 5 after modulation)
Tuning SystemIntervallic ratios (Just Intonation, Pythagorean, 12-TET, Maqam Rast,
etc.)
## Base
## Frequency
Hz value for note 1 (the reference frequency for all pitches)
The Base Frequency is the lowest frequency across all registers in the score. It appears at
the left edge of the staff, with other scale degree frequencies labeled on their respective
lines.
## Modulation
A modulation changes any component of the Scale Definition at a specific point in time.
Setting a Modulation:
Two workflows, same result:
-   During composition: Change any Scale Definition value in the Composition Tab →
modulation inserted at current "now" marker position
-   Intentional placement: Position "now" marker at desired moment → change Scale
Definition → modulation starts there

## Modulation Behavior:
AspectSpecification
TimingInstantaneous — occurs at precise time point
Notes spanning
boundary
Split into two notes at modulation point
TransitionsNone automatic — preparation and resolution are
composed
StackingMultiple modulations can occur; each is independent
Visual Effect at Modulation Point:
ElementChange
Staff line colorsShift to reflect new harmonic functions
Staff line spacingAdjust to new intervallic ratios
Hz labelsUpdate to new frequencies
Background color    Changes to indicate new tonal center
The modulation is immediately visible as a vertical moment where the staff transforms.
Modulation Within vs. Between Systems
Within same tuning system: New tonal center is a ratio relative to the original Base
Frequency. Example: In Just Intonation starting at 220 Hz, modulating to note 5 as tonal
center — the new "home" frequency is 220 × 3/2 = 330 Hz, but the original 220 Hz remains
the reference.
Between tuning systems: New tonal center with entirely different intervallic DNA. Example:
Moving from Just Intonation to Maqam Rast. The Base Frequency may be kept or changed
(composer's choice in Scale Settings).
## Vibrato
Vibrato lives in the composition layer (not sound engine). Every note has an embedded
vibrato container, inactive by default.
## Vibrato Parameters:
•active (boolean, default: false)
•rate — speed of oscillation
•pitchDepth — how much pitch varies
•intensityDepth — how much loudness varies
•breathDepth — how much breath varies

•onset — when vibrato starts (% into note)
•development — how vibrato grows over note lifetime
•regularity — physics variation (0 = mechanical, 1 = organic)
Each parameter can be assigned an envelope, easing, or mass modifier. Phrase-level vibrato
applies to all notes; note-level vibrato provides full override.
## Control Panel Layout
## Phrase Selector
Top of control panel. Provides quick access to all phrases in the composition.
•Dropdown listing all phrases by name
•Selecting from dropdown: selects phrase AND navigates canvas to show it
•[Load] button — import .phrase file
•[Save] button — export selected phrase
## Track List / Sounit Selector
Colored vertical bars representing each track. Mirrored on both Control Panel and Canvas
(left edge).
•Bar height = register range
•Bar color = track/sounit color
•Active track = solid, others = ghosted
•Click to select track
•Drag to reorder
## Tabbed Inspectors
Inspector area uses tabs based on selection. Full space available for active tab content.
SelectionTabs ShownNote
Note selectedNote | GestureGesture tab if note has
gestures
Phrase selectedPhrase | GestureGesture tab if phrase has
gestures
Nothing selected(blank)No tabs, empty space
## Phrase Inspector
FieldDescription
NamePhrase identifier (editable)
TransposeMode dropdown (Diatonic, Relative to tonic, Hz, Cents) +
value
Parameter EnvelopesList of parameter → envelope assignments, [+ Add] button
Easing AssignmentsList of segment → easing assignments, [+ Add] button
VibratoPreset selector, applies to all notes in phrase
CommentNotes field (editable)
## Note Inspector
FieldDescription
PositionStart time, duration, base pitch
Parameter CurvesThumbnail previews of captured pen data, [Edit...] opens
## Envelope Library
Envelope AssignmentsList of parameter → envelope, [+ Add] button

FieldDescription
Easing AssignmentsList of segment → easing, [+ Add] button
VibratoActive toggle, preset selector, [Edit] opens Vibrato Editor
PhysicsMass value for note's parameters
CommentNotes field (editable)
Note: Note-level vibrato provides full override of phrase-level vibrato.
## Gesture Inspector
Appears as a tab when selected note or phrase contains gestures. Includes navigation for
multiple gestures.
FieldDescription
Navigation[◄] 1 of 3 [►] — arrows when multiple gestures
NameGesture identifier (editable)
PreviewVisual of gesture shape with thickness/transparency
variations
Parameter AssignmentsEach dimension (Y, Pressure, TiltX, TiltY, Rotation) →
parameter dropdown
Actions[Save to Library] [Remove]
## Pen Settings
Configuration for Wacom pen input. Pressure curve adjustment handled in Wacom app (not
duplicated here).
SettingOptions
ModeDraw | Select | Edit (radio buttons)
Draw TypeNotes | Gesture (visible only in Draw mode)
Parameter MappingDropdowns: Pressure→[param], TiltX→[param],
TiltY→[param], Rotation→[param]
## Pen Modes:
•Draw: Create notes or gestures on canvas
•Select: Lasso selection, or single-click to select note/phrase/gesture
•Edit: Existing gestures become bezier curves with manipulable control points
## Pen Buttons:
•Top shaft button: Right-click (configured in Wacom app)
•Bottom shaft button: Double-click (configured in Wacom app)
•Pen tip: Single click
•Reverse side: Erase
## Scale Display
Read-only summary with quick access to Scale Settings dialog. Minimal — no clutter.
●    Shows: Tonal Center + Tuning system + Base Frequency
## ●    Example: "1 · Just Intonation · 220 Hz"
●    [Edit] button opens Scale Settings dialog
## •
## Canvas Elements
The Score Canvas displays the composition with minimal chrome.

ElementDescription
TimelineTop — tick marks for time position, click to set 'now' marker
Track SelectorLeft edge — colored vertical bars (mirrored from Control
## Panel)
Scale LinesHorizontal colored lines representing scale degrees
PhrasesContour shapes following notes, track-colored, click boundary
to select
NotesCurved lines with variable thickness/transparency from
gesture data
TransportBottom — Play, Stop, Rewind, Forward, etc.
Status BarBottom — cursor time, pitch at cursor, selection info
## Related Dialogs
Dialogs opened from the Composition Tab or Compose menu:
DialogPurposeOpened From
Scale SettingsDefine scale, tuning, keyCompose menu, Scale
Display [Edit]
Vibrato LibraryCreate/select/save vibrato
presets
Compose menu
Vibrato EditorEdit note's vibrato
parameters
Note Inspector [Edit]
Gesture LibraryCreate/select/save gesturesCompose menu
Envelope LibraryCreate/edit curves (also used
for curve editing)
Sound menu, Note Inspector
[Edit...]
## Scale Settings Dialog
FieldDescription
Tonal CenterNote number dropdown (1-7 or extended for larger scales)
ScaleScale pattern selection
Tuning SystemDropdown with tuning system options
## Base
## Frequency
Hz value input (with A=440 and other standard reference buttons)
## Compose Menu — Updated
Based on new features discovered during specification:
•New Track
•Delete Track
## •—
•New Phrase
•Group into Phrase (shortcut: P)
•Load Phrase...
•Save Phrase As...

## •—
•Scale Settings...
•Vibrato Library...
•Gesture Library...
## •—
•Assign Sounit to Track...
## —
Calamus UI Specification — Composition Tab v1.0