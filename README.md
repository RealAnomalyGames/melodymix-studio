# MelodyMix Studio

**MelodyMix Studio** is a native desktop music-making application focused on straightforward step-based drum sequencing. It features a Windows 95-inspired industrial design, combining classic desktop UI conventions with realistic metal textures and hardware-inspired controls.

The application is being developed in C++ with Qt Widgets and CMake.

## Studio 1

Studio 1 establishes the core MelodyMix Studio experience.

### Features

- **9 patterns** that can be selected and edited independently.
- **4 audio channels** per pattern.
- **16 steps per channel**, for 64 step buttons per pattern.
- Play and Stop transport controls.
- Adjustable tempo from **40 to 250 BPM**.
- User-provided WAV sound files, with no fixed limit on the number of available sounds.
- Save and load projects using the `.mmx` file format.
- A playback indicator showing the currently playing step.

### Industrial design

The interface takes inspiration from Windows 95-era desktop software and industrial audio hardware.

The visual style will feature:

- Black, grey, and olive-colored metal finishes.
- Realistic metal textures rather than flat-colored panels alone.
- Recessed step buttons and tactile-looking controls.
- Compact menus, toolbars, and clearly defined panels.
- High-contrast playback and active-step indicators.

### Technology

- **Language:** C++
- **Interface:** Qt Widgets
- **Build system:** CMake
- **Initial sound format:** WAV
- **Project format:** `.mmx`

The minimum supported Windows version will be determined after evaluating dependencies and testing compatibility.

## Development

Development is organized into stages:

1. Establish the native application and build configuration.
2. Implement the main window and industrial visual theme.
3. Build the pattern editor and channel controls.
4. Implement WAV loading and reliable audio playback.
5. Add `.mmx` project saving and loading.
6. Configure automated Windows builds and test releases.

## Project status

MelodyMix Studio is in early development. Features described here represent the intended Studio 1 scope and may change as implementation progresses.
