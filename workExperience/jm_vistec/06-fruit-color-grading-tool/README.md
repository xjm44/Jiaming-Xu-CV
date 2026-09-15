# Fruit Color Grading Tool

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/OpenCV tool for analyzing and grading fruit surface color, used to establish
objective, repeatable color standards (e.g. ripeness/quality grade) from photographs
instead of relying on manual visual inspection.

## Tech Stack

C++ · OpenCV · Matrox MIL (image processing prototyping) · Python (data exploration)

## Architecture

- **Color-space analysis**: images are converted between RGB, HSV, and Lab color spaces
  to isolate the channels most correlated with visual ripeness/quality (e.g. Lab's L
  channel for lightness, HSV's hue for color category).
- **Statistical modeling**: per-sample color statistics are computed and exported to CSV,
  then reduced via PCA to identify the principal axes of color variation across a sample
  set (minimum / standard / maximum reference profiles).
- **Prototyping workflow**: Python scripts were used alongside the C++ pipeline for rapid
  visualization and validation of color-space transforms before porting logic into the
  production C++ application.

## My Contribution

- Designed the color-space analysis pipeline (RGB → HSV/Lab) and channel selection
- Built the PCA-based statistical color-grading model from sample image sets
- Prototyped and validated approaches in Python before C++ implementation
