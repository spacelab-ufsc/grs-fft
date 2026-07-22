<h1 align="center">
    GRS FFT
    <br>
</h1>

<h4 align="center">FFT Calculator of the SpaceLab's Ground Station.</h4>

<p align="center">
    <a href="https://github.com/spacelab-ufsc/grs-fft">
        <img src="https://img.shields.io/badge/status-development-green?style=for-the-badge">
    </a>
    <a href="https://github.com/spacelab-ufsc/grs-fft/releases">
        <img alt="GitHub commits since latest release (by date)" src="https://img.shields.io/github/commits-since/spacelab-ufsc/grs-fft/latest?style=for-the-badge">
    </a>
    <a href="https://github.com/spacelab-ufsc/grs-fft/blob/main/LICENSE">
        <img src="https://img.shields.io/badge/license-GPL3-yellow?style=for-the-badge">
    </a>
</p>

<p align="center">
    <a href="#overview">Overview</a> •
    <a href="#dependencies">Dependencies</a> •
    <a href="#building">Building</a> •
    <a href="#installing">Installing</a> •
    <a href="#license">License</a>
</p>

## Overview

GRS FFT is a real-time Fast Fourier Transform (FFT) calculator for SpaceLab's Ground Station. It is part of the ground station's signal processing pipeline, sitting between the IQ sample receiver and the spectrum visualization components.

The program subscribes to a ZMQ publisher socket where it receives a continuous stream of raw IQ (In-phase/Quadrature) samples. Once enough samples are accumulated, it applies a Hann window to reduce spectral leakage and then computes a 16384-point complex FFT using the FFTW3 library. The resulting frequency-domain data can then be consumed by downstream components such as the Spectrum Monitor.

## Dependencies

* libfftw3-dev (>= 3.3.10-13)
* libczmq-dev (>= 4.2.1-2)

### Installation on Ubuntu

```sudo apt install libfftw3-dev libczmq-dev```

### Installation on Fedora

```sudo dnf install fftw-devel czmq-devel```

## Building

```make```

## Installing

```make install```

## License

This project is licensed under GPLv3 license.
