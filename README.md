# VocAdvance
VOCALOID-like software for GBA

![Screenshot](screenshots/1.png)

> Note at the top of the README: if you just wanna build this, don't `git clone`!! Download the .zip file from GitHub. It's way faster!

This program for the GameBoy Advance allows you to create vocal songs with a preset voicebank (provided ones are generated with espeak and the script in `generator/`). It's pretty basic, but does support:

- Different BPMs
- Muiltiple tracks (4 by default)
- Different voicebanks (via different ROMs)
- Saving/autoloading songs

> Note: this doesn't synthesize anything at runtime, it merely plays back voice clips with a bit of fade in and fade out. No magic here!

## Downloading

Head over to the releases tab and download the latest release.

## Controls

|Button|Action|
|-|-|
|<kbd>D-Pad</kbd>|Move cursor|
|<kbd>A</kbd>|Place or remove note|
|<kbd>B</kbd> + <kbd>D-Pad</kbd>|Scale note|
|<kbd>L</kbd>, <kbd>R</kbd>|Switch between tracks|
|<kbd>B</kbd> + <kbd>L</kbd>, <kbd>B</kbd> + <kbd>R</kbd>|Move cursor (by beat)|
|<kbd>Select</kbd> + <kbd>D-Pad</kbd>|Change BPM|
|<kbd>Start</kbd>|Start song from cursor|
|<kbd>B</kbd> + <kbd>Start</kbd>|Start song from beginning|

## Compiling

Install [devkitPro](https://devkitpro.org/wiki/devkitPro_pacman), then install the `gba-dev` package, after that run `make`. Simple as that!