# Honeycomb Panel AOI (Automated Optical Inspection) System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt automated optical inspection application for aerospace honeycomb composite
panels. The system captures high-resolution images of a panel from both top and bottom
sides, runs each through a deep-learning inspection flowchart, and renders a live
pass/fail decision to the operator on the factory floor.

## Tech Stack

C++ · Qt 5.15 · OpenCV 4.3 · Neurocle deep-learning inference (flowchart-based models) ·
machine-vision camera SDK · Gardasoft lighting controller SDK

## Architecture

- **Image acquisition**: a camera manager handles connection, live capture, and
  asynchronous acquisition (Qt Concurrent) of high-resolution panel images.
- **Lighting control**: a dedicated manager drives Gardasoft strobe/intensity controllers,
  synchronizing illumination with each capture for consistent, repeatable imaging.
- **AI inspection**: two independent Neurocle flowcharts process the top and bottom image
  sets respectively. Each flowchart classifies a grid of cells across the panel surface,
  and the results are spatially clustered into rows to detect patterns (e.g. consecutive
  defect cells) rather than relying on a single cell classification.
- **Decision logic**: top and bottom results are combined into a single pass/fail decision,
  with the result and defect locations displayed to the operator in real time.
- **Data logging**: per-inspection data (cell classifications, coordinates, scores) is
  persisted for traceability and offline analysis.

## My Contribution

- Designed the end-to-end capture → AI inference → decision pipeline
- Integrated the machine-vision camera and lighting controller SDKs
- Built the dual-flowchart (top/bottom) inspection logic and row-based defect clustering
- Implemented the operator-facing Qt GUI and real-time pass/fail feedback
