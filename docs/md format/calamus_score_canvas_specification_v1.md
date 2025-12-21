

## Calamus — Score Canvas Specification
Calamus UI Specification
## Score Canvas
## Version 1.0 — December 2025
## Overview
The Score Canvas is the primary composition workspace in Calamus. It displays horizontal
scale lines with note curves, phrase containers, and gesture data. This is where music is
drawn, edited, and visualized.
The Score Canvas appears in the Canvas Window when the Composition Tab is active. It
works in concert with the Control Panel, which provides inspection and editing of selected
elements.
## Canvas Layout
The Score Canvas is organized from top to bottom as follows:
ElementDescription
ToolstripDrawing tools, selection, zoom controls
TimelineTick marks, time labels, 'now' marker, loop
region
Track SelectorLeft edge — colored vertical bars for each
track
Score AreaScale lines, notes, phrases — the main
drawing surface
TransportPlay/Stop buttons, playback time, tempo, time
signature
Status BarCursor time, cursor pitch, tempo, time
signature
## Scale Lines
Unlike traditional staff notation with fixed chromatic spacing, Calamus displays the current
scale as the visual infrastructure. Lines represent scale degrees, with spacing proportional to
actual pitch intervals in Hz or cents.
Color System — ROYGBIV
Scale degrees follow the light spectrum, cycling back to red at each octave:
DegreeColorExample (C Major)
1 (Tonic)RedC
2OrangeD
3Yellow (ochre/gold)E
Page 1 of 9

## Calamus — Score Canvas Specification
DegreeColorExample (C Major)
4GreenF
5BlueG
6IndigoA
7VioletB
Chromatic and Extended Scales
Scale TypeColor Treatment
Chromatic notesIn-between colors (C# = red-orange, F# =
blue-green)
PentatonicUses base colors from corresponding C scale
degrees
Scales with 8+ notesExtended spectrum blends
## Line Properties
AspectSpecification
SpacingProportional to Hz/cents (not equal grid)
ThicknessTonic and Fifth slightly thicker than other
degrees
Hz LabelsFixed at left edge, always visible regardless
of horizontal pan
## Note Visualization
Notes appear as 'split blobs' — shapes divided horizontally in the middle, with parameter
curves displayed on the top and bottom edges. This visualization shows the note's sonic
character, not just pitch and duration.
## Default Display
EdgeShows
Top edge contourAmplitude waveform (loudness evolution)
Bottom edge contourSpectrum (timbral content)
Horizontal positionTime
Vertical positionPitch
Horizontal extentDuration
ColorSounit/track color
## Data Source
Page 2 of 9

## Calamus — Score Canvas Specification
The amplitude waveform and spectrum are derived from pre-rendering just that note and
extracting the visualization data. This is less expensive than calculating for all composition
data at once. The visualization updates when the note changes.
## View Modes
A toolbar toggle switches between two view modes:
ModeBehavior
Global DefaultAll notes show amplitude (top) + spectrum
## (bottom)
Local Per-NoteEach note shows whatever parameters are
set in its inspector
Per-Note Customization
When a note is selected, all parameters appear in the Inspector. Any parameter with an
envelope or easing shows a small preview visualization. A toggle in the Inspector allows
showing that parameter on the score with a choice of top or bottom position. This replaces
the default amplitude/spectrum view for that note only.
## Phrase Visualization
Phrases have two visual components: a hull for selection and a line for parameter display.
Hull (Boundary)
AspectSpecification
ShapeContour following notes — consistent width
around note curves
ColorMatches track/sounit color but 25%
transparent
PurposeClick to select phrase
## Parameter Line
AspectSpecification
DisplaySingle line following phrase contour
ParametersOne parameter visible at a time
ToggleCycle through available parameters
## Selection Visuals
## Dimming
StateAppearance
Selected itemsFull opacity, true colors
Page 3 of 9

## Calamus — Score Canvas Specification
StateAppearance
Unselected itemsModerate dimming (40-50% opacity)
Multi-selectAll selected items at full visibility
When multiple items are selected, the Inspector shows only parameters common to the
entire selection.
## Selection Rectangle — Notes
A selected note displays a rectangle with manipulation dots:
AspectSpecification
Rectangle styleSolid line, neutral gray
Top edge dots4 dots (round) — shape top parameter curve
Bottom edge dots4 dots (round) — shape bottom parameter
curve
Left edge dots3 dots — middle is square (resize start),
others round
Right edge dots3 dots — middle is square (resize end),
others round
Total default dots14
Adding dotsClick on edge
Corner dotsNone by default, can add
Curve responseLinear interpolation
Upper half dots affect the top parameter curve; lower half dots affect the bottom parameter
curve. Square dots on the middle of vertical sides are resize handles.
## Selection Rectangle — Phrases
Same structure as notes (gray rectangle with dots on edges), but all dots affect the single
visible parameter line.
## Parameter Change Behavior
When you change parameters in the Inspector, the selection rectangle resets to default dot
positions (except resize squares). You can then shape the newly visible parameter.
## Edit Acceptance
ActionResult
Deselect, select other, or change parameterAccept current edit
Ctrl+ZUndo
'Now' Marker
The 'now' marker indicates where playback will resume when you press Play.
Page 4 of 9

## Calamus — Score Canvas Specification
AspectSpecification
AppearanceMedium thick red line
LocationTimeline only (not full canvas height)
Position behaviorFixed in time, moves with timeline during pan
SettingDouble-click timeline
## Loop Region
Loops are for quick preview while working — this is a composition tool, not a DAW.
AspectSpecification
LocationTimeline background only
ColorOrange, 50% transparency
Start pointThe 'now' marker
End markerInverted triangle pointing at end moment
## Loop Workflow
StepAction
1Set 'now' marker where loop starts
(double-click timeline)
2Press L
3Scroll or Goto to make end point visible
4Click timeline at end point
## Loop Deletion
Click end triangle + Delete key to remove the loop. Loop markers are not draggable.
## Composition Settings Bar
Located at the top of the Composition Tab, this bar provides global composition settings.
ElementDescription
Time Mode ToggleSwitch between Absolute and Musical time
TempoOptional — BPM setting
Time SignatureOptional — beats per bar
## Time Representation
ModeFormatExample
AbsoluteMinutes:Seconds:Milliseconds1:23:456
Page 5 of 9

## Calamus — Score Canvas Specification
ModeFormatExample
MusicalBars:Beats:Milliseconds4:2:240
Milliseconds is the universal smallest time unit — no arbitrary tick resolution needed.
## Time Mode Effects
ModeTimelineScore Canvas
AbsoluteMin:Sec:Ms labelsNo bar lines
MusicalBars:Beats labelsBlack vertical bar lines top to
bottom
Mid-Piece Changes
Tempo and time signature changes are marked on the timeline at the moment they occur.
## Toolstrip
Located above the timeline. Contains drawing tools and other canvas controls. Tools must be
selected before recording — cannot change during capture.
## Drawing Tools
ToolModeDescription
Draw Notes DiscreteDrawingPen stroke → note, snaps to
scale lines
Draw Notes ContinuousDrawingPen stroke → note, literal pitch
Draw PhraseDrawingPen stroke → phrase contour
Record Notes DiscreteRecordingTime scrolls, pitch snaps to
scale
## Record Notes
## Continuous
RecordingTime scrolls, literal pitch
Record PhraseRecordingTime scrolls, phrase contour
Drawing mode: X = time, Y = pitch (you draw in pitch-time space). Recording mode: time
scrolls automatically, you control Y as time advances.
## Other Tools
ToolFunction
SelectionClick to select — rectangle with dots for
editing
ZoomZoom in/out controls
## Drawing Behavior
Page 6 of 9

## Calamus — Score Canvas Specification
AspectSpecification
Track assignmentNew notes go to currently selected/active
track
Note creationImmediately on pen-up
UndoCtrl+Z
## Track Selector
Located on the left edge of the Score Canvas.
AspectSpecification
AppearanceColored vertical bars
Bar heightSpans register range of that track's sounit
Bar widthAll same width
PositionCorresponds to Hz/pitch of that register
Active trackFull color
Inactive tracksDimmed
ClickSelects that track as active
Zoom and Pan
Navigation uses Photoshop-style conventions for familiarity.
## Zoom
MethodBehavior
Mouse wheelZoom in/out
Ctrl+Space + dragMagnifier cursor, zoom from start position
## Pan
MethodBehavior
Alt+Space + dragHand cursor, pan canvas
Arrow keys left/rightPan horizontally
## Scope
Zoom affects both axes together (proportional).
## Cursor Feedback
ModeCursor
Ctrl+Space heldMagnifier
Page 7 of 9

## Calamus — Score Canvas Specification
ModeCursor
Alt+Space heldHand
## Transport Controls
Located at the bottom of the canvas, below the score area.
ElementSpecification
ButtonsPlay (▶), Stop (■) — icons only
Position displayCurrent playback time
Tempo displayRepeated from Composition Settings
Time signature displayRepeated from Composition Settings
## Stop Behavior
StateWhat Happens
Press StopWindow freezes at current playback position
After stopSet new 'now' marker, OR hit Play to resume
from old 'now' position
## Status Bar
Located at the bottom of the canvas, below transport. Elements arranged left to right with
separators.
ElementFormat
Cursor timeCurrent time mode (absolute or musical)
Cursor pitchHz; Note name; Scale degree
TempoBPM
Time signaturee.g., 4/4
Example: 1:23:456 | 440 Hz; A4; 6 | 120 BPM | 4/4
## Keyboard Shortcuts
## Transport
KeyAction
SpaceToggle play/stop
HomeGo to beginning
EndGo to end
GOpen Goto dialog
Page 8 of 9

## Calamus — Score Canvas Specification
## Navigation
KeyAction
Ctrl+Space + dragZoom (magnifier cursor)
Alt+Space + dragPan (hand cursor)
Mouse wheelZoom
Arrow left/rightPan horizontally
## Editing
KeyAction
Ctrl+ZUndo
LEnter loop mode
DeleteDelete selected item (or loop end marker)
## Time Display Locations
LocationShows
TimelineTick marks + labels
Status barCursor position
Goto dialogInput field for target time
Transport areaCurrent playback position
## Related Documents
DocumentRelationship
Composition Tab SpecificationParent document — Score Canvas is the
canvas for Composition Tab
Input Engine SpecificationDefines gesture capture and pen input
behavior
Sound Engine Tab SpecificationSounit Builder — where sounds are designed
before composing
Page 9 of 9