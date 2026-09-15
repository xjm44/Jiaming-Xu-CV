# Fastener Defect Inspection Tool

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt machine-vision application for inspecting bolts and screws (e.g. on aerospace
engine components) to detect defects and verify correct fastener installation from
photographs, using classical image-processing techniques rather than deep learning.

## Tech Stack

C++ · Qt · OpenCV · CMake

## Architecture

- **Bolt detection**: locates and inspects bolt features (head condition, positioning,
  presence/absence) via classical CV techniques (edge/contour analysis, geometric
  fitting).
- **Screw detection**: a parallel detector tuned to screw-specific geometry and defect
  modes.
- **Interactive review UI**: a zoomable image widget lets an operator pan/zoom into
  high-resolution inspection images to visually verify automated detection results
  against the source photo.

## My Contribution

- Designed and implemented the bolt and screw detection algorithms
- Built the zoomable image review widget and inspection GUI
