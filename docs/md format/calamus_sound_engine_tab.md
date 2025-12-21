

Calamus UI Specification
## Sound Engine Tab
## Version 1.0 — December 2025
## Overview
The Sound Engine Tab is where sounits are built and configured. When this tab is active, the
Canvas Window displays the Sounit Builder — a visual node editor showing containers and
their connections.
The Control Panel provides detailed editing capabilities while the Canvas provides visual
layout and connection creation.
Division of Labor
Tasks are divided between the Canvas Window and Control Panel:
Canvas WindowControl Panel
Visual layout of containersEdit default values
Selection (click to select)Edit connection function and weight
Connection visualizationEdit container config options
Create connections (drag port to port)Edit instance names
Show selection and connection context
Sounit metadata (name, comment)
Principle: Canvas is your map. Inspector is your workbench.
## Control Panel Layout
The Control Panel contains three areas that provide context-sensitive information and
editing:
## Sounit Info
Always visible when Sound Engine tab is active. Shows metadata for the current sounit.
FieldEditableDescription
NameYesSounit identifier (e.g., "Violin I")
Used byNo (read-only)List of tracks using this sounit
CommentYesNotes to distinguish similar sounits
## Container Inspector
Visible when a container is selected on the Canvas. Provides complete editing access to all
container properties.
## Header Section
•Container type (read-only)
•Instance name (editable) — auto-generated as "Type 1", "Type 2", can be renamed

## Inputs Section
Lists all input ports for the selected container. Each input shows:
•Port name
•Default value (editable)
•Unit label where applicable (Hz, %, etc.)
•Connection indicator (if connected)
## Connection Display Format:
When an input has a connection, it displays below the port line:
- ● (dot indicator)
•Function (add, multiply, modulate, etc.)
•Weight value
•Source: Container(instance).port
Important: The default value is not overridden by connections (except passthrough).
Connection functions combine the source with the default value: add, multiply, subtract,
replace, modulate.
## Outputs Section
Lists all output ports with their destinations (if connected). Read-only, informational.
## Config Section
Container-specific configuration options from the Container Port Specification. Each
container type has different config options (e.g., resonance model for FormantBody, easing
mode for EasingApplicator).
## Empty State
When no container is selected, the Container Inspector is blank. No placeholder text, no
prompts — just empty space.
## Connection Inspector
Visible when a connection is selected. A connection can be selected by:
•Clicking the connection line on the Canvas
•Clicking the ● indicator in the Container Inspector
## Fields
FieldEditableDescription
FromNoSource container and port
ToNoDestination container and port
FunctionYes (dropdown)How values combine (see table below)
WeightYesMultiplier for the source value
[Disconnect] button at bottom of inspector.
## Connection Functions
FunctionBehavior
passthroughSource replaces input entirely
addinput + source × weight
multiplyinput × source × weight
subtractinput − source × weight
replaceinput × (1−weight) + source × weight
modulateBipolar modulation around input

## Empty State
When no connection is selected, the Connection Inspector is blank.
## Canvas Visual States
Containers and connections have visual states to provide feedback without clutter.
## Container States
StateAppearance
NormalStandard colors per container category
SelectedHighlighted border (strong visual emphasis)
Connected-to-selectionSlight opacity/tint change (subtle context cue)
No hover effects. Canvas stays clean — click to select, that's it.
## Connection States
When a connection is selected, both the source and destination containers show the
"connected-to-selection" state for visual context without losing focus on the connection itself.
## Workflow Summary
TaskWhere / How
Create connectionCanvas — drag from output port to input port
Select connectionCanvas (click line) or Inspector (click ●)
Edit connectionInspector — function dropdown, weight field
Delete connectionCanvas (select + Del) or Inspector (Disconnect)
Edit default valuesInspector — Container Inspector inputs section
Edit config optionsInspector — Container Inspector config section
Rename containerInspector — Container Inspector header
Edit sounit metadataInspector — Sounit Info section
## —
Calamus UI Specification — Sound Engine Tab v1.0