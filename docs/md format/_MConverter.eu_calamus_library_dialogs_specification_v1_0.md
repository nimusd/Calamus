**Calamus UI Specification** **Library Dialog Framework**

Version 1.0 — December 2025

**Overview**

Calamus uses a consistent **Library Dialog Framework** for managing presets across multiple domains: DNA sequences, envelopes, easing functions, vibrato settings, and gestures. This framework provides a unified user experience while accommodating type-specific editing and preview capabilities. 

**Design Principle:** One pattern, many payloads. Users learn the library interaction once and apply it everywhere. 

**Common Framework**

**Window Properties \(All Libraries\)** **Property**

**Value**

Modal

Yes

Size

500 × 400 px \(resizable\)

Minimum Size

400 × 300 px

**Layout Structure**

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ \[Library Type\] Library \[X\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ ■ ■

■ PRESET LIST ■ PREVIEW AREA ■

■ \(200px wide\) ■ \(type-specific visualization\) ■

■ ■ ■

■ ■■■■■■■■■■■■■ ■ Height: ~40% of content area ■

■ ■ ■ ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ ■ ■ ■ ■

■ ■ ■ ■ EDITOR PANEL ■

■ ■ ■ ■ \(type-specific controls\) ■

■ ■■■■■■■■■■■■■ ■ ■

■ ■ Height: ~60% of content area ■

■ \[New\] \[Delete\] ■ ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ Name: \[\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\] \[Cancel\] \[Save\] \[OK\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

**Preset List Panel**

**Element**

**Description**

List widget

Scrollable list showing all presets Selection

Single-select; clicking loads preset into Editor Panel Visual indicator

Selected item highlighted; modified item shows bullet \(•\)

\[New\] button

Creates blank preset, adds to list as "Untitled" 

\[Delete\] button

Removes selected preset \(with confirmation\) **Preset Categories:**

• **Factory** — Built-in presets \(read-only, cannot delete\)

• **User** — User-created presets *Factory and User presets shown in the same list with subtle visual distinction \(e.g., factory presets in italics or with icon\). *

**Bottom Bar**

**Element**

**Description**

Name field

Editable preset name

\[Cancel\]

Close dialog, discard all changes

\[Save\]

Save current editor state to selected preset

\[OK\]

Save \(if modified\) and close, applying selection **Common Behaviors**

**Action**

**Result**

Select preset

Load into Editor Panel and Preview; Name field updates Edit any parameter

Preset marked as modified \(•\); \[Save\] enables

\[New\] clicked

Create "Untitled" preset, select it, clear Editor Panel to defaults

\[Delete\] clicked

Confirmation dialog; if confirmed, remove preset, select next

\[Save\] clicked

Write current state to selected preset; clear modified flag

\[OK\] clicked

If modified, prompt to save; apply selection to caller; close

\[Cancel\] or Escape

If modified, prompt "Discard changes?"; close without applying Edit Name field

Preset marked as modified

Double-click preset

Same as select \+ \[OK\] \(quick apply\)

***Delete Confirmation***

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ Delete "\[Preset Name\]"? ■

■ ■

■ This cannot be undone. ■

■ ■

■ \[Cancel\] \[Delete\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

**Library-Specific Implementations** **1. DNA Library**

**Window Title:** "DNA Library" 

**Opened From:** Sound menu → DNA Library... 

**Purpose:** Create and manage harmonic distribution presets that define the instrument's "genetic identity." 

***Preview Area:***

• Spectrum bar graph showing harmonic amplitudes \(1–64\)

• X-axis: harmonic number

• Y-axis: relative amplitude

• Updates in real-time as parameters change

• Interactive when Sequence = Custom \(click/drag to edit\)

***Editor Panel:***

**Control**

**Type**

**Description**

Sequence

Dropdown

Base pattern: All, Odd, Even, OddDominant, Fundamental\+, Octaves, Custom

Purity

Slider 0–100%

Blend: 0% = pure sequence, 100% = all harmonics

\[Edit Table...\]

Button

Opens table view for precise numeric entry \(visible when Sequence = Custom\)

***Custom Pattern Editing:***

• Click on spectrum bar to select harmonic; drag up/down to set amplitude \(0–100%\)

• Shift\+click to toggle harmonic on/off

• Drag across multiple bars to "paint" a curve

• \[Edit Table...\] opens scrollable table for precise numeric entry

***Factory Presets:***

• All Harmonics

• Odd Only \(Clarinet\)

• Even Only \(Hollow\)

• Odd Dominant

• Fundamental\+ \(Near-sine\)

• Octaves Only \(Organ\)

**2. Envelope Library**

**Window Title:** "Envelope Library" 

**Opened From:** Sound menu → Envelope Library..., Note Inspector \[Edit...\]

**Purpose:** Create and manage arbitrary envelope curves for parameter modulation over note lifetime. 

***Preview Area:***

• Curve graph with time \(0–100%\) on X-axis, value \(0–100%\) on Y-axis

• Control points shown as draggable handles

• Grid lines at 25% intervals

***Editor Panel:***

**Control**

**Type**

**Description**

Curve canvas

Interactive

Click to add points; drag to move; right-click to delete Point list

Table

Time, Value, Curve Type for each point Curve Type

Dropdown per segment Linear, Smooth, Step \(extensible at code time\) Loop Mode

Dropdown

None, Loop, Ping-pong

\[Reset\]

Button

Clear to default two-point envelope \(0,0 → 100,100\)

***Point Editing:***

• Minimum 2 points \(start and end\)

• Points snap to grid when Shift held

• Drag beyond canvas edges to delete

***Factory Presets:***

• Linear Rise • Linear Fall • Attack-Decay • Swell

• Fade Out • Bell Curve • Step Up • Step Down

**3. Easing Library** **Window Title:** "Easing Library" 

**Opened From:** Sound menu → Easing Library... 

**Purpose:** Create and manage easing functions that shape parameter transitions. 

***Preview Area:***

• Curve graph showing easing shape

• X-axis: progress \(0–1\)

• Y-axis: eased value \(0–1\)

***Editor Panel:***

**Control**

**Type**

**Description**

Easing Type

Dropdown

Linear, Quad, Cubic, Quart, Quint, Sine, Expo, Circ, Back, Elastic, Bounce Mode

Radio buttons

In, Out, InOut

Strength

Slider

Available for Back, Elastic

\(controls overshoot/oscillation\)

***Factory Presets:***

• Linear • Ease In \(Quad\) • Ease Out \(Quad\) • Ease In-Out \(Quad\)

• Ease In \(Cubic\) • Ease Out \(Cubic\) • Ease In-Out \(Cubic\)

• Bounce Out • Elastic Out • Back Out **4. Vibrato Library**

**Window Title:** "Vibrato Library" 

**Opened From:** Compose menu → Vibrato Library... 

**Purpose:** Create and manage vibrato presets applicable to phrases or notes. 

***Preview Area:***

• Waveform visualization showing vibrato shape over time

• Displays rate, depth variation, onset ramp, development curve

***Editor Panel:***

**Control**

**Type**

**Range**

**Description**

Rate

Slider \+ field

0.1–20 Hz

Vibrato speed

Depth

Slider \+ field

0–100 cents

Pitch deviation

Onset

Slider \+ field

0–100%

When vibrato starts \(% into note\)

Development

Dropdown

Instant, Linear, 

How vibrato grows

Exponential, S-Curve, 

Envelope

Dev. Envelope

Selector

\(from Envelope Library\)Visible when Development = Envelope Regularity

Slider \+ field

0–100%

0 = mechanical, 100 = organic

Physics Min

Numeric input

0–10000

Minimum mass

Physics Max

Numeric input

100–10000

Maximum mass

***Factory Presets:***

• None \(flat\) • Subtle • Classical • Romantic

• Wide & Slow • Tight & Fast • Late Entry • Building **5. Gesture Library**

**Window Title:** "Gesture Library" 

**Opened From:** Compose menu → Gesture Library... 

**Purpose:** Create and manage reusable gesture templates that can be applied to notes. 

***Preview Area:***

• Stroke visualization showing gesture path

• Line thickness varies with pressure data

• Tilt indicated by slight offset/shadow

***Editor Panel:***

**Control**

**Type**

**Description**

Gesture preview

Canvas

Visual representation of captured stroke Duration

Display \(read-only\)

Length of gesture in ms

Parameter

Table

Six rows: X, Y, Pressure, TiltX, TiltY, Rotation Assignments

→ Parameter dropdown each

\[Capture New\]

Button

Opens Gesture Capture Dialog

\[Scale to Fit\]

Checkbox

Stretch/compress gesture to match note duration

***Parameter Assignment Targets:***

• None • Pitch Offset • Brightness • Breath • F1 Offset • F2 Offset • Dynamics

*• \(extensible per sounit parameters\)*

***Factory Presets:***

• Linear Swell • Accent Attack • Fade Out

• Scoop Up • Fall Away • Tremolo Pressure

**Gesture Capture Dialog** When \[Capture New\] is clicked in the Gesture Library, a separate dialog opens providing adequate space for natural gesture capture. 

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ Capture Gesture \[X\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ ■

■ ■

■ \[Large Capture Canvas\] ■

■ \(400 × 300 px minimum\) ■

■ ■

■ ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ Duration: 0.00 ms \[Clear\] \[Accept\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

**Action**

**Result**

Draw on canvas

Captures gesture; Duration updates in real-time

\[Clear\]

Resets canvas for another attempt

\[Accept\]

Returns to Gesture Library with captured data Escape or \[X\]

Cancels, returns without changes **Vibrato Editor \(Separate Dialog\)** The **Vibrato Editor** is distinct from the Vibrato Library. It edits vibrato parameters for a *specific note*, with the option to load presets from the library. 

**Window Title:** "Vibrato Editor" 

**Opened From:** Note Inspector → Vibrato \[Edit\]

***Window Properties:***

**Property**

**Value**

Modal

Yes

Size

350 × 300 px

Resizable

No

***Layout:***

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ Vibrato Editor \[X\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ Preset: \[Dropdown\_\_\_\_\_\_\_▼\] \[Load\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ ■

■ \[Waveform Preview\] ■

■ ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ Rate: \[====|====\] 5.0 Hz ■

■ Depth: \[====|====\] 30 cents ■

■ Onset: \[====|====\] 20% ■

■ Development: \[Dropdown\_\_\_▼\] ■

■ Regularity: \[====|====\] 60% ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■ \[Cancel\] \[OK\] ■

■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

***Behavior:***

**Action**

**Result**

Preset dropdown

Lists all Vibrato Library presets

\[Load\]

Copies selected preset values into editor fields Adjust any slider

Updates preview; values now differ from loaded preset

\[OK\]

Apply values to note's vibrato; close

\[Cancel\]

Discard changes; close

*Note: The Vibrato Editor does not save back to the library — it only applies values to the current note. To save a new* *preset, use the Vibrato Library dialog. *

**Menu Integration Summary** **Menu**

**Item**

**Opens**

Sound

DNA Library... 

DNA Library

Sound

Envelope Library... 

Envelope Library

Sound

Easing Library... 

Easing Library

Compose

Vibrato Library... 

Vibrato Library

Compose

Gesture Library... 

Gesture Library

Note Inspector

Vibrato \[Edit\]

Vibrato Editor

Note Inspector

Parameter Curves \[Edit...\]

Envelope Library

**File Storage**

All user presets stored in:

\[User Documents\]/Calamus/Presets/

DNA/

Envelopes/

Easing/

Vibrato/

Gestures/

File format: JSON with .calamus-\[type\] extension \(e.g., .calamus-dna, .calamus-env\)

— End of Specification —



