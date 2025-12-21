

Calamus UI Specification
## Dialogs
## Version 1.0 — December 2025
## Overview
This document specifies all dialogs in the Calamus application. Dialogs are modal windows
that appear for specific tasks and close when complete.
## Complete Dialogs List
DialogPurpose
About CalamusVersion, credits
Keyboard ShortcutsShow shortcut reference
Go ToNavigate to specific time position
Scale SettingsDefine tuning/scale, tonal center, base frequency
Export AudioRender settings, stems/mix
DNA LibraryCreate/select/save DNA presets
Envelope LibraryCreate/select/save envelopes, curve editing
Easing LibraryCreate/select/save easing functions
Vibrato LibraryCreate/select/save vibrato presets
Gesture LibraryCreate/select/save gestures
Vibrato EditorEdit vibrato parameters for a note
## Dialog 1: About Calamus
Simple information display showing application identity and credits.
## Window Properties
PropertyValue
Window title"About Calamus"
ModalYes
SizeFixed, approximately 300 × 200 px
ResizableNo
Contents (top to bottom)

ElementValue
IconCalamus application icon (centered)
Name"Calamus" (large, centered)
Tagline"No keyboard lives here" (italic, centered)
Version"Version X.Y.Z" (centered)
## Copyright"© 2025 Nimus"(centered)
Button[OK] — closes dialog (centered, default focus)
## Behavior
ActionResult
Escape keyCloses dialog
Enter keyCloses dialog(OK has focus)
Click outsideNothing (modal)
## Dialog 2: Keyboard Shortcuts
Read-only reference dialog showing all available keyboard shortcuts, organized by category.
## Window Properties
PropertyValue
Window title"Keyboard Shortcuts"
ModalYes
SizeFixed, approximately 400 × 500 px
ResizableNo
## Contents
ElementDescription
Scrollable listCategories with shortcuts listed below each
Button[OK] — closes dialog (bottom, centered, default focus)
## Shortcut Categories
## File
ActionShortcut
New ProjectCtrl+N

OpenCtrl+O
SaveCtrl+S
Save AsCtrl+Shift+S
Export AudioCtrl+E
## Edit
ActionShortcut
UndoCtrl+Z
RedoCtrl+Y
CutCtrl+X
CopyCtrl+C
PasteCtrl+V
DuplicateCtrl+D
DeleteDel
Select AllCtrl+A
DeselectEsc
## Transport
ActionShortcut
Play/StopSpace
Go to BeginningHome
Go to EndEnd
Go ToG
Loop ModeL
## Navigation
ActionShortcut
Zoom InCtrl++
Zoom OutCtrl+-
Zoom to FitCtrl+0
Zoom (drag)Ctrl+Space + drag
Pan (drag)Alt+Space + drag
Pan Left/RightArrow keys
## View
ActionShortcut

Full Screen CanvasF11
Sound Engine TabCtrl+1
Composition TabCtrl+2
Preferences TabCtrl+3
## Compose
ActionShortcut
Group into PhraseP
## Help
ActionShortcut
DocumentationF1
## Behavior
ActionResult
Escape keyCloses dialog
Enter keyCloses dialog
Mouse wheelScrolls list
## Dialog 3: Go To
Quick navigation dialog for jumping to a specific time position. Accepts both absolute and
musical time formats with automatic conversion between them.
## Window Properties
PropertyValue
Window title"Go To"
ModalYes
SizeFixed, approximately 250 × 150 px
ResizableNo
Contents (top to bottom)
ElementDescription
Label"Absolute:"
Input fieldFormat: min:sec.ms(e.g., 1:23.456)

Label"Musical:"
Input fieldFormat: bar:beat(e.g., 12:3)
Buttons[Cancel] [Go To] — right-aligned
## Behavior
ConditionResult
Both fields empty[Go To] button disabled
Invalid format in active field   [Go To] button disabled, other field unchanged
Valid format in active field[Go To] button enabled, other field updates to equivalent time
Enter keyActivates [GoTo] if enabled
Escape keyCloses dialog (same as Cancel)
## Field Synchronization
Typing a valid Absolute time automatically updates the Musical field to the equivalent value,
and vice versa. Conversion is based on current tempo and time signature.
## Dialog 4: Scale Settings
Configuration dialog for the composition's tuning system. Calamus is tuning-agnostic by
design — any tuning system is native, not 12-TET with microtonal patches.
## Window Properties
PropertyValue
Window title"Scale Settings"
ModalYes
SizeFixed, approximately 300 × 200 px
ResizableNo
Contents (top to bottom)
ElementDescription
Label"Tuning/Scale:"
DropdownList of predefined tuningsystems (12-TET, Just Intonation,
Pythagorean, Maqam Rast, etc.)
Label"Tonal Center:"
DropdownNote number (1 throughN, where N depends on selected
tuning)

Label"Base Frequency:"
Input fieldHz value(numeric, e.g., 220)
Buttons[Cancel] [OK] — right-aligned
## Behavior
ConditionResult
Change tuningTonal Center dropdown updates to show available degrees
Invalid Hz value[OK] button disabled
OK pressedSettings applied, dialog closes, canvas updates
Escape keyCloses dialog, no changes
## Design Notes
In Calamus, a scale is a tuning and a tuning is a scale — they are unified concepts. There is
no separate "scale pattern" selection (Major, Minor, etc.) as these are Western-centric
nomenclatures. Which degrees to use is at the discretion of the composer.
Changes affect the composition from the current 'now' marker forward, creating a modulation
point if mid-piece.
## Dialog 5: Export Audio
Render dialog for exporting audio. Calamus generates sound; post-processing happens
elsewhere. WAV format only, consistent with the "not a DAW" philosophy.
## Window Properties
PropertyValue
Window title"Export Audio"
ModalYes
SizeFixed, approximately 350 × 350 px
ResizableNo
Contents (top to bottom)
ElementDescription
Export ModeRadio buttons: Mix / Stems / Selection
(Selection grayed out if nothing selected)
Label"Sample Rate:"

Dropdown44100 Hz / 48000Hz / 96000 Hz
Label"Bit Depth:"
## Dropdown16-bit / 24-bit /32-bit
Label"Channels:"
DropdownMono / Stereo
CheckboxNormalize
CheckboxInclude metadata
Buttons[Cancel] [Export...] — right-aligned
## Behavior
ActionResult
[Export...] clickedOpens standard Save File dialog
Mix modeSave single.wav file
Stems modeSave to folder; creates one file pertrack
Selection modeSave single .wav file containing only selected notes
Cancel or EscapeCloses dialog, no export
File Naming (Stems)
[ProjectName]_[TrackName].wav
Example:"Quartet_Soprano.wav","Quartet_Alto.wav",etc.
Metadata (if included)
- Project name
- Export date
- Calamus version
— End of Part 1 —
Library dialogs and Vibrato Editor to follow