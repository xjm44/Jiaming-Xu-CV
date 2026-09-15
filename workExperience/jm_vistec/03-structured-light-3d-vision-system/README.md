# Structured-Light 3D Vision System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt structured-light 3D vision system that reconstructs real-time point clouds of
objects moving on a conveyor belt, then detects, localizes, and classifies the orientation
of each object for downstream robotic or sorting operations.

## Tech Stack

C++ · Qt 5.15 · OpenCV 4.10 · PCL 1.13 (Point Cloud Library) · VTK 9.2 · structured-light
3D reconstruction and calibration SDKs

## Architecture / Processing Pipeline

1. **Point cloud acquisition**: real-time structured-light point clouds are captured from
   connected cameras (or loaded from saved data for offline testing).
2. **3D ROI filtering & downsampling**: points are filtered to a configured 3D bounding
   volume and voxel-downsampled to keep processing real-time.
3. **Belt surface fitting**: the dominant conveyor surface is fit using iterative
   trimmed least-squares, establishing a local coordinate frame aligned to the belt.
4. **Color segmentation & clustering**: non-belt points are segmented in HSV color space
   and grouped into discrete objects using breadth-first-search spatial clustering.
5. **Orientation detection**: object clusters are projected to 2D image texture, where
   circular hole patterns are detected to classify rotational orientation (clock position).
6. **Facing detection**: point-density comparison between top and bottom Z-zones
   determines whether each object is right-side-up or inverted.
7. **Output**: detected object centers (offset from a configured origin), orientation, and
   facing are published as JSON over TCP, logged to file, and rendered in a live 3D
   visualizer.

## My Contribution

- Designed the full acquisition-to-classification pipeline
- Implemented belt-surface fitting and the local coordinate-frame computation
- Built the HSV segmentation and BFS spatial clustering for object isolation
- Developed the 2D texture-based orientation and facing-detection logic
- Implemented the TCP/JSON output interface and 3D visualization
