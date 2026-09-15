# Industrial Vision Inspection Platform

> Code is proprietary and not included. This document describes the system architecture,
> technology stack, and my contributions.

## Overview

A hybrid industrial vision inspection framework pairing a Qt event-loop front-end (GUI and
console modes) with a real-time OPC UA back-end hosting all inspection logic. A Matrox
Imaging Library (MIL) pipeline handles 2D/3D image acquisition, while an embedded OPC UA
server exposes cameras, stations, recipes, and results as a structured address space —
enabling any OPC UA–compatible client (HMI, PLC, SCADA) to control and monitor the system.

## Tech Stack

C++17 · Qt 5.15 (Core, Gui, Widgets, Concurrent, Charts) · open62541 (OPC UA) · Matrox MIL ·
Eigen3 · OpenCV · CMake

## Architecture

The system is organized into 8 strictly layered modules (Interfaces → Common → Hardware →
Core → Algorithm → Service → Backend → Client/UI), with upper layers depending only on
abstract interfaces (`ICamera`, `IResultHandler`, `IIODevice`) rather than concrete hardware
classes — enforcing the Dependency Inversion Principle throughout.

Each inspection station runs as an "active object" on its own worker thread, driven by a
trigger queue and a state machine (`IDLE → ACQUIRING → PROCESSING → DONE/ERROR`). Captured
frames flow through thread-safe bounded queues from camera grab callbacks into the
inspection pipeline, where a strategy-based dispatcher routes recipe-defined inspection
items to typed executors. Results are assembled into JSON and fanned out to all registered
result handlers (console logging, live OPC UA nodes) in real time, including per-item
results as they complete.

Recipes and results are modeled dynamically: loading a recipe rebuilds the corresponding
OPC UA node sub-tree, so external systems always see a live, structured view of what the
station is currently configured to measure and what it has measured.

## Key Design Patterns

Singleton, Abstract Factory, Strategy, Observer/Pub-Sub, Adapter, Template Method, Active
Object, Producer-Consumer, Dependency Inversion, Composition Root.

## My Contribution

- Designed the overall layered architecture and interface boundaries
- Implemented the OPC UA server integration and dynamic node address space
- Built the multi-threaded station execution engine and inspection dispatcher
- Implemented the JSON-based recipe and result system
