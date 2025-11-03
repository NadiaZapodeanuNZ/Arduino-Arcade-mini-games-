
# Arduino Mini-Arcade (Snake & T-Rex) — Wokwi + OLED + Buttons

A compact project created for the optional course “Developing Physical Systems Using Microprocessors” at UAIC – Faculty of Computer Science.
The goal is to build a small menu-driven console on Arduino UNO with an OLED display and push buttons, simulated in Wokwi. The project focuses on real-time constraints, frame timing, and smooth rendering on limited hardware.

---

## Overview

    - Two selectable games: Snake and Chrome T-Rex Runner
    - Simple menu to choose the game at runtime
    - Frame-based loop with predictable timing (steady motion, no flicker)
    - Non-blocking input (button polling + debouncing)
    - Designed for reproducibility in Wokwi; easy to port to real hardware

---

## Course Context

This project consolidates topics from the following modules:
 - Introduction – tools, workflow, evaluation
 - Fundamentals – number systems, MCU basics, digital I/O
 - Electronics I – passive components, reading schematics
 - Peripherals – sensors, displays, buttons, encoders
 - Advanced Arduino – timers, interrupts, state machines, memory

---


## Features

 - Menu UX: clear navigation and game selection at runtime.
 - Deterministic timing: fixed update interval using millis(); rendering every cycle for smooth visuals.
 - Input reliability: button polling + debouncing without blocking the loop,that can be replaced with
   keyboards arrow up , arrow down , arrow right , arrow left and of course the spacebar for confirmation.
 - OLED pipeline: text/sprite rendering via Adafruit GFX + SSD1306.
 - State machines: separate states for menu and games; easier testing and maintenance.
 - Wokwi keyboard mapping: Arrow keys + Enter drive the virtual buttons.
 - Portable by design: same codebase runs in Wokwi and on physical UNO.


---


## How It Works (Game Loop)
 - Target a constant frame interval using millis()
 - Accumulate elapsed time; when the interval is reached, run update()
 - Render each cycle; avoid blocking calls
 - Benefits: predictable physics and collision detection, smoother visuals

Controls (example)
Action	Button  --  Space
Navigate menu	-- Up / Down
Select / Start	-- Confirm /Space
Snake: move	Up / Down / Left / Right
T-Rex: jump	Confirm (short press)

---


## Check the pin mapping in code or diagram.json for your exact wiring.


--- 


## Getting Started
- 1.Run in Wokwi (recommended)
- 2.Open the project in Wokwi (insert your project link here).
- 3.Verify wiring in diagram.json. (or simply copy paste it)
- 4.Press Start and use the virtual buttons.


## Credits

Developed as part of UAIC’s optional “Developing Physical Systems Using Microprocessors.”
Thanks to instructors and colleagues for their guidance and feedback.