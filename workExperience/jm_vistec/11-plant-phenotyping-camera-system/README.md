# Plant Phenotyping Camera System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt customized plant photography system built for plant phenotyping research, using
two independent camera stations — each pairing a monochrome and an RGB camera — to
capture synchronized image pairs of plants for downstream analysis.

## Tech Stack

C++ · Qt 5.12 · OpenCV 3.4 · machine-vision camera SDK

## Architecture

- **Dual camera-station design**: two independent stations each run a mono + RGB camera
  pair, capturing both cameras' images simultaneously on trigger for direct pixel-aligned
  comparison between spectral and true-color imagery.
- **I/O-signal-driven workflow**: each station waits for an "in-position" input signal
  before triggering capture, and a "reset done" input signal before attempting to
  reconnect any disconnected cameras — integrating cleanly with an external positioning
  mechanism (e.g. a conveyor or turntable).
- **Connection management & recovery**: the GUI displays live per-camera connection status
  and automatically attempts reconnection of dropped cameras.
- **Automated image saving**: captured image pairs are saved automatically per capture
  cycle for later phenotyping analysis.

## My Contribution

- Designed the dual mono/RGB camera-station architecture and synchronized capture logic
- Implemented the I/O-signal-driven trigger and reset workflow
- Built the camera connection monitoring, auto-reconnect, and status GUI
