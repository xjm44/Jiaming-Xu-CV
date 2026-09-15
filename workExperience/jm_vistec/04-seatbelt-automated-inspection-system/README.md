# Seatbelt Automated Inspection System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt automated inspection system for automotive seatbelt webbing and hardware,
combining industrial imaging with deep-learning object detection, segmentation, and
classification to identify defects and verify part correctness on a production line.

## Tech Stack

C++ · Qt 5.15 · OpenCV 4.10 · Matrox MIL (imaging) · Neurocle deep-learning inference ·
TCP sockets · SQLite · Sentinel LDK licensing

## Architecture

- **Imaging**: Matrox MIL handles camera acquisition and buffer management, feeding
  frames into the inspection pipeline as `MIL_ID` buffers.
- **Inspection pipeline**: a sequence of deep-learning stages — object detection,
  segmentation, and classification — each wrapping a Neurocle model and executor,
  producing bounding boxes, region masks, and class/confidence results.
- **Networking**: a Qt-based TCP server/client layer streams inspection results and
  images to and from external systems and clients in real time.
- **Data & traceability**: inspection results and configuration are persisted to a local
  SQLite database, with configurable camera settings and per-model result logging.
- **Licensing & security**: the application is protected by a Sentinel hardware dongle
  (LDK), with password-gated configuration screens.
- **Localization**: full English/Chinese UI translation support.

## My Contribution

- Designed the multi-stage detection → segmentation → classification inspection pipeline
- Integrated the Matrox MIL imaging layer with the deep-learning inference stages
- Built the TCP server/client architecture for real-time result and image streaming
- Implemented camera configuration, result database, and licensing integration
