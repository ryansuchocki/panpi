# PanPI
'Raspberry Pi' based *panadapter*.

![screenshot](https://user-images.githubusercontent.com/278474/185504104-e697e979-ecaf-4e6b-b76a-1a81a1da90f1.png)

## About

PanPI is primarily an application which renders a realtime 'spectrogram' and 'waterfall' display based on an I/Q sample stream. While it is fully portable software, PanPI has been developed and tested on a Raspberry Pi (model 4B).

PanPI is designed to perform a specific job very well, with minimal bells, whistles and other distractions.

### Background

I started this project after finding myself in need of a self-contained panadapter unit for my Elecraft KX3 transceiver and being told (by Elecraft's UK distributor) that the companion 'PX3 Panadapter' is no longer available. Sadly: PX3s on the second-hand market seem to be rare to non-existent.

I was pleased to find that some high quality pressed steel enclosures are available to fit the Raspberry Pi along with an inexpensive TFT display. I found that running established desktop SDR software such as 'Gqrx' on a small screen is *just about* feasible however performance is severely compromised and screen space is wasted. I also found a few existing software projects intended to render a straight-forward panadapter display however the software on offer was either poorly designed, unmaintained or had unnecessary and/or obsolete dependencies.

### Design

PanPI is written in C and is designed to use a small fraction of the power/resources available on the Raspberry Pi 4. This leaves open the possibility of porting to a much lower power platform in the future.

Although it is possible to run PanPI in a desktop window for off-target development, it is intended to run "full screen" on a dedicated display. The software design takes advantage of this fact to minimise the amount of rendering work needed.

In particular: the FFT size (i.e. number of frequency bins) is chosen to be close to the horizontal width of the spectrogram/waterfall displays. This maximises the fidelity of the image while avoiding the need for a graphical "stretching" (interpolating) or curve-drawing stage. Simple integer-based routines are used to produce the curves/lines of the spectrogram graphic and fast recursive filtering is used to 'tweak' the characteristics of the display for maximum usefulness.

PanPI is designed to have minimal external dependencies and make as few assumptions about the platform as possible. It does currently depend on the source data taking the form of baseband I/Q samples delivered by a 'sound card', however the software has been written with future extensions (such as an RTL-SDR source option) in mind.

## Platform

The platform on which PanPI has been developed and tested is as follows: (Note that the software is portable and is in no way restricted to the following)

### Hardware
* [Raspberry Pi 4B](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/)
* [Pi 4 metal case with 3.5" TFT display](https://www.mouser.co.uk/ProductDetail/DFRobot/FIT0820?qs=pBJMDPsKWf1f%252B%2Fx0HFXbyA%3D%3D)
* [96kHz stereo USB sound card](https://www.startech.com/en-gb/cards-adapters/icusbaudio2d)

### Operating System
* [Arch Linux ARM](https://archlinuxarm.org/platforms/armv8/broadcom/raspberry-pi-4) (Using the 'ARMv7' version)
* The following packages are needed to build and run the software. The package versions used for development are recorded here but should not be taken as requirements:
  * `git 2.37.1-1`
  * `make 4.3-3.1`
  * `gcc 12.1.0-2.1`
  * `fftw 3.3.10-3`
  * `libx11 1.8.1-3`
  * `alsa-lib 1.2.8-1`

![photo](https://user-images.githubusercontent.com/278474/186992126-67d05c45-e872-48e6-be8e-5f827d8043ff.jpg)

### Display config

The core drivers for my particular TFT screen were already built-in to the kernel, but a "device tree overlay" file is needed to tell Linux how it is connected.

The documentation for the display directed me to the [LCD-Show repository](https://github.com/goodtft/LCD-show). The top-level 'install' scripts provided in that repository are very fragile, dependent on one particular operating system and assume that the user wants to install a full desktop manager along with the display driver. Fortunately, after a little digging, the necessary 'device tree overlay' file can be extracted easily.

I ran the following commands on the Pi in order to download the file and copy it into place:

1. `$ curl -O "https://github.com/goodtft/LCD-show/raw/master/usr/tft35a-overlay.dtb"`
2. `$ sudo mv tft35a-overlay.dtb /boot/overlays/`

The following lines were added to my `/boot/config.txt` file:

```
dtparam=spi=on

dtoverlay=tft35a:rotate=270,speed=160000000,fps=60
```

Note that the order of entries in `config.txt` is significant. My full `config.txt`, at time of writing, is as follows:

```
# /boot/config.txt
# See /boot/overlays/README for all available options

#dtparam=audio=on
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

After rebooting, I was able to verify that the display driver was loaded by displaying random data to the screen using:

`$ sudo cat /dev/urandom > /dev/fb0`

## Building and Running

1. `$ git clone git@github.com:ryansuchocki/panpi.git --recurse-submodules`
2. `$ cd panpi`
3. `$ make`
4. `$ cp panpi.cfg.template panpi.cfg`
5. Customise `panpi.cfg` (see below)
6. `$ ./panpi`

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
