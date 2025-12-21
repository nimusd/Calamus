> **Calamus** **UI** **Specification** **Sounit** **Builder** **Canvas**

Version 1.0 — December 2025

**Overview**

The Sounit Builder Canvas is a floating window displayed on a secondary
screen where sounits are visually constructed. It provides a node-editor
interface for placing containers and connecting them to define the
signal flow of a sound unit.

This specification covers the visual elements and interaction patterns
for the canvas. For container editing (default values, config options,
connection functions), see the Sound Engine Tab specification. For
container port definitions, see the Container Port Specification.

**Principle:** Canvas is your map. Inspector is your workbench.

**Window** **Frame**

Standard Qt window decoration with title bar, minimize/maximize/close
buttons, and resizable edges.

||
||
||
||
||
||

**Toolbar**

Container insertion buttons organized by category. Each category has a
distinct background color. Clicking a button inserts that container type
at the center of the visible canvas.

**Container** **Categories**

||
||
||
||
||
||

essor

**Special** **Toolbar** **Buttons**

**Marquee** **Select:** Activates marquee selection mode for
drag-selecting multiple containers.

**Canvas** **Surface**

||
||
||
||
||
||

**Status** **Bar**

||
||
||
||
||

**Containers**

Containers are the visual nodes representing sound processing units.

||
||
||
||
||
||
||

**Ports**

||
||
||
||
||
||
||
||

**Connections**

||
||
||
||
||
||
||
||
||

**Interaction** **Patterns**

**Canvas** **Navigation**

Photoshop-style navigation:

||
||
||
||
||

**Selection**

> Action
>
> Select container
>
> Input

Click container

> Select connection
>
> Deselect all
>
> Add/remove from selection
>
> Marquee select

Click connection line or function oval

Click empty canvas

Shift + click

Toolbar button → drag rectangle → selects containers and connections

Marquee mode exits automatically on mouse/pen up.

**Container** **Operations**

||
||
||
||
||
||

**Connection** **Operations**

||
||
||
||
||
||

**Keyboard** **Shortcuts**

||
||
||
||
||
||
||
||
||

**Window** **States**

> State
>
> Empty
>
> Working
>
> Canvas

Blank white

Full interaction

> Title Bar

"Sounit Builder"

"Sounit Builder — \[name\]"

> Toolbar

Normal

Normal

> Notes

Can add containers to start building

All features available

> Switching Clears → loads new Updates to new name Normal Warns if
> unsaved, selection resets

**Related** **Documents**

• Sound Engine Tab Specification v1.0 — Inspector and editing controls

• Container Port Specification v1.2 — Port definitions for all
containers

• Calamus Architecture v1.4 — System overview and data relationships

• Calamus Class Diagram v0.3 — Container and Connection class
definitions
