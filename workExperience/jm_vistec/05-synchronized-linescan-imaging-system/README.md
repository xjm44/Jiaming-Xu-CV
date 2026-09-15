# Synchronized Line-Scan Imaging System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt imaging application that synchronizes a GigE Vision area/line-scan camera with a
multi-axis motion controller, enabling position-triggered image capture as a stage moves —
used for high-resolution scanning inspection where image acquisition must be precisely
tied to physical motion rather than free-running time.

## Tech Stack

C++ · Qt 5.15 · OpenCV · GigE Vision camera SDK (eBUS) · multi-axis motion controller SDK

## Architecture

- **Camera layer**: a Pimpl-based camera wrapper manages GigE Vision device discovery,
  buffer allocation, streaming, and Bayer-to-RGB conversion, with a background worker
  thread handling asynchronous disk saves so acquisition is never blocked by I/O.
- **Motion layer**: a motion controller abstraction exposes absolute/relative moves,
  jogging, homing, and repeat-cycle motion across multiple axes, with live status polling.
- **Synchronization**: position and velocity-encoder-based triggering configuration
  (source, delay, scale factor) ties camera triggers directly to stage position, so
  captured frames correspond to precise physical locations regardless of motion speed.
- **UI/threading**: a Qt GUI exposes connect/jog/move/trigger controls, with a
  cross-thread task queue marshaling camera and motion events safely back to the main
  Qt event loop for display and state updates.

## My Contribution

- Integrated the GigE Vision camera SDK and built the acquisition/save pipeline
- Implemented position/encoder-based camera-motion synchronization and triggering
- Built the multi-axis motion control UI (jog, absolute/relative move, repeat cycles)
- Designed the cross-thread task queue for safe UI updates from acquisition/motion threads
