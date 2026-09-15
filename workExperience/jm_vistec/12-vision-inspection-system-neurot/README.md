# Deep-Learning-Assisted Vision Inspection System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt industrial vision inspection application combining Matrox MIL imaging with a
Neurocle deep-learning inference engine (Neuro-T) to classify parts and detect defects on
a production line.

## Tech Stack

C++ · Qt 5.9 · OpenCV 4.2 · Matrox MIL · Neurocle Neuro-T deep-learning inference

## Architecture

- **Imaging**: Matrox MIL handles camera acquisition and image buffer management.
- **AI inference**: a Neuro-T deep-learning model classifies/inspects captured images,
  producing per-part pass/fail or defect classification results.
- **Application shell**: a Qt-based desktop application wraps the imaging and inference
  pipeline with an operator-facing interface.

## My Contribution

- Integrated the Matrox MIL imaging pipeline with the Neuro-T inference engine
- Built the inspection application shell and operator workflow
