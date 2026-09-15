# Barcode/QR Tunnel Scanning System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt tunnel-scanning system that captures images from multiple industrial cameras as
items pass through a scan tunnel, decodes barcodes/QR codes from the captured images, and
reports pass/fail results through a live GUI.

## Tech Stack

C++ · Qt 5.12 · OpenCV 4.5 · nlohmann/json · TCP/IP camera connectivity · FTP

## Architecture

- **Multi-camera capture**: multiple industrial cameras (connected over TCP/IP) are
  triggered by sensors as items pass through the tunnel, each capturing an image of a
  different face/angle of the item.
- **FTP-based image retrieval**: captured images are written by the camera software to a
  shared folder (organized into pass/fail subfolders) and retrieved by the application
  over FTP for display and logging.
- **Decode & result reporting**: barcode/QR codes are decoded from the retrieved images,
  and results are visualized live in the Qt GUI as each item completes its scan cycle.
- **Configuration**: camera and system parameters are managed via JSON configuration
  (nlohmann/json).

## My Contribution

- Designed the multi-camera, sensor-triggered capture and FTP-based image retrieval
  pipeline
- Implemented the barcode/QR decode integration and pass/fail result visualization
- Built the Qt GUI for live monitoring of the scan tunnel
