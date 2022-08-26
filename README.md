# PanPI
'Raspberry Pi' based *panadapter*.

![screenshot](https://user-images.githubusercontent.com/278474/185504104-e697e979-ecaf-4e6b-b76a-1a81a1da90f1.png)

## About

PanPI is primarily an application which renders a realtime 'spectrogram' and 'waterfall' display based on an I/Q sample stream. While it is fully portable software, PanPI has been developed and tested on a Raspberry Pi (model 4B).

PanPI is designed to perform a specific job very well, with minimal bells, whistles and other distractions.

## Building and Running

`$ git clone git@github.com:ryansuchocki/panpi.git`

`$ cd panpi`

`$ make`

Customise `panpi.cfg`

`$ ./panpi`

## Background

I started this project after finding myself in need of a self-contained panadapter unit for my Elecraft KX3 transceiver and being told (by Elecraft's UK distributor) that the companion 'PX3 Panadapter' is no longer available. Sadly: PX3s on the second-hand market seem to be rare to non-existent.

I was pleased to find that some high quality pressed steel enclosures are available to fit the Raspberry Pi along with an inexpensive TFT display. I found that running established desktop SDR software such as 'Gqrx' on a small screen is *just about* feasible however performance is severely compromised and screen space is wasted. I also found a few existing software projects intended to render a straight-forward panadapter display however the software on offer was either poorly designed, unmaintained or had unnecessary and/or obsolete dependencies.

## Platform

The platform on which PanPI has been developed and tested is as follows: (Note that the software is portable and is in no way restricted to the following)

### Hardware
* [Raspberry Pi 4B](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/)
* [Pi 4 metal case with 3.5" TFT display](https://www.mouser.co.uk/ProductDetail/DFRobot/FIT0820?qs=pBJMDPsKWf1f%252B%2Fx0HFXbyA%3D%3D)
* [96kHz stereo USB sound card](https://www.startech.com/en-gb/cards-adapters/icusbaudio2d)

### Operating System
* [Arch Linux ARM](https://archlinuxarm.org/platforms/armv8/broadcom/raspberry-pi-4)
* The following packages are needed to build and run the software. The package versions used for development are recorded here but should not be taken as requirements:
  * `git 2.37.1-1`
  * `make 4.3-3.1`
  * `gcc 12.1.0-2.1`
  * `fftw 3.3.10-3`
  * `libx11 1.8.1-3`

### Configuration
The following lines were added to my `/boot/config.txt` file in order to get the display working.
```
dtparam=spi=on

dtoverlay=tft35a:rotate=270,speed=160000000,fps=60
```

Note that the order of entries in `config.txt` is significant. My full `config.txt`, at time of writing, is as follows:

```
# /boot/config.txt
# See /boot/overlays/README for all available options

dtparam=audio=on
dtparam=spi=on
dtparam=debug=7

dtoverlay=vc4-kms-v3d
dtoverlay=tft35a:rotate=270,speed=160000000,fps=60

initramfs initramfs-linux.img followkernel

# Uncomment to enable bluetooth
#dtparam=krnbt=on

[pi4]
# Run as fast as firmware / board allows
arm_boost=1
```

![photo](https://user-images.githubusercontent.com/278474/186992126-67d05c45-e872-48e6-be8e-5f827d8043ff.jpg)

## Design

TODO

C (fast)
Timeless
Minimal dependencies (maintainable)
No platform lock-in

## Configuration

PanPI is configured via the `panpi.cfg` file. This file follows a simple format whereby each valid line takes the form `key: value`. Lines not matching this format, or correctly formed lines with unrecognised keys, are simply ignored.

After initial loading: PanPI "watches" the file for changes and, where possible, applies changes straight away to match the new configuration. This provides a rudimentary form of runtime control, but this is liable to change in the future.

See comments within the file itself for details of the available configuration items.

## Roadmap

- MVP (Minimal Viable Product)
  - [x] Hardware/OS selection for development
  - [x] Capturing input samples using ALSA
  - [x] Calculating FFT using fftw
  - [x] Rendering a realtime 'spectrogram' display
  - [x] Rendering a scrolling 'waterfall' display
  - [x] Rendering to a cheap TFT screen using fbdev
- Goals for 'V1'
  - [x] Rendering to a desktop window (using X11) for off-target development
  - [ ] Runtime controls
  - [ ] Zoom/span controls
  - [ ] Useful documentation
- Possible future extensions
  - [ ] Porting to a lower power microcontroller platform
  - [ ] Alternative input sources (rtl-sdr?)
  - [ ] Per-band IQ balance calibration
  - [ ] Per-mode passband markers
  - [ ] Per-band scaling memory
  - [ ] Pre-amp compensation
  - [ ] Spectrogram peak hold

## License

```
MIT License

Copyright (c) 2022 Ryan Suchocki

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
