# Multi-Image Stitching Tool

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt desktop application for stitching a set of overlapping images into a single
composite, with a guided operator workflow for selecting, ordering, and cropping images
before stitching.

## Tech Stack

C++ · Qt · OpenCV

## Architecture

- **Folder-based workflow**: an operator loads a folder of candidate images, selects and
  orders the ones to stitch through a step-by-step wizard-style UI (next/back navigation).
- **ROI cropping**: a dedicated dialog lets the operator define a region of interest per
  image before stitching, to exclude unwanted image borders/artifacts.
- **Stitching**: selected, ordered, and cropped images are composited into a single result
  image (OpenCV-based stitching), viewable with zoom in/out controls and savable to disk.
- **Configurable settings**: a settings dialog exposes stitching parameters for tuning to
  different image sets.

## My Contribution

- Designed the end-to-end folder → select/order → ROI crop → stitch → review workflow
- Implemented the ROI cropping and zoomable result-review UI
- Built the settings/configuration system for tuning the stitching pipeline
