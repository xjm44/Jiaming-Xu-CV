# Precision Cutter Metrology System

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A C++/Qt machine-vision metrology application that measures the diameter and corner wear
of a rotary cutting tool from high-resolution images, using sub-pixel edge fitting rather
than simple thresholding, to catch out-of-round and worn-tool conditions that naive
measurement would miss or hide by averaging.

## Tech Stack

C++ · Qt · OpenCV · CMake

## Architecture

The system runs three independent, complementary measurements on the same part:

- **Top view (diameter, per blade pair)**: segments each blade's ground face, classifies
  every boundary pixel by the angle between its normal and the radial direction to
  isolate the true side/end edges (rotation-invariant, so it works regardless of the
  tool's orientation in frame), fits a line to each edge over a defined radial band, and
  computes the corner as a true line-line intersection (not a perpendicular projection,
  which would introduce a cosine error). Two diameters are computed — one per
  opposite-corner pair — and reported separately rather than averaged, since disagreement
  between them is itself the out-of-round signal being measured. Each diameter is summed
  through a least-squares-fit axis rather than measured as a chord, avoiding a second
  cosine-error source.
- **Side view (diameter, dual-rule)**: measures a single diameter across the tool from a
  side-on view using two independent rules plus a width profile along the tool body; both
  rules are always computed and reported, with a rule selection only determining which one
  counts as "the" measurement.
- **Corner defect view**: a separate side-view inspection that measures how far a single
  blade corner has worn back, rather than measuring diameter at all.

Each view has its own ROI, golden reference values, and result panel — the three
measurements are intentionally independent and are not cross-validated against each other,
since each observes a different physical feature of the same ground geometry.

## My Contribution

- Designed and implemented all three detectors (top-view corner/diameter, side-view
  dual-rule diameter, corner-defect wear measurement)
- Developed the rotation-invariant, angle-based edge classification and sub-pixel
  line-fitting/intersection methodology
- Built the Qt GUI, configuration store, and image-canvas visualization for reviewing
  measurements against golden references
