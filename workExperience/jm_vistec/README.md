# Machine Vision \& Industrial Automation Portfolio

A collection of C++/Qt industrial vision, metrology, and automation systems I designed and
built for manufacturing quality-control and inspection applications. Source code is
proprietary to the clients these were built for, and due to multiple NDAs and confidentiality agreements,
so each folder here contains a write-up of the system's architecture, tech stack, and my role, without any implementation code.



|#|Project|Domain|Highlights|
|-|-|-|-|
|1|[Industrial Vision Inspection Platform](./01-industrial-vision-inspection-platform)|Multi-station factory inspection|OPC UA server/client, layered C++ architecture, Matrox MIL|
|2|[Honeycomb Panel AOI System](./02-honeycomb-panel-aoi-system)|Aerospace panel defect inspection|Deep-learning defect classification, machine vision, lighting control|
|3|[Structured-Light 3D Vision System](./03-structured-light-3d-vision-system)|Real-time 3D object detection|Point cloud reconstruction, PCL/VTK, pose \& orientation estimation|
|4|[Seatbelt Automated Inspection System](./04-seatbelt-automated-inspection-system)|Automotive component inspection|Object detection/segmentation/classification pipeline, licensing, TCP streaming|
|5|[Synchronized Line-Scan Imaging System](./05-synchronized-linescan-imaging-system)|Motion-synchronized imaging|GigE Vision camera, motion controller sync, multi-threaded capture|
|6|[Fruit Color Grading Tool](./06-fruit-color-grading-tool)|Agricultural quality grading|Color-space analysis (HSV/Lab), PCA-based statistical modeling|
|7|[Precision Cutter Metrology System](./07-precision-cutter-metrology-system)|Tool/part dimensional metrology|Sub-pixel edge fitting, rotation-invariant classification, dual-view measurement|
|8|[Fastener Defect Inspection Tool](./08-fastener-defect-inspection-tool)|Engine Components inspection|Classical CV bolt/screw defect detection|
|9|[Multi-Image Stitching Tool](./09-multi-image-stitching-tool)|Image composition|Guided ROI-crop-and-stitch workflow|
|10|[Barcode/QR Tunnel Scanning System](./10-barcode-tunnel-scanning-system)|Logistics/traceability scanning|Multi-camera TCP/FTP pipeline, barcode/QR decoding|
|11|[Plant Phenotyping Camera System](./11-plant-phenotyping-camera-system)|Agricultural research imaging|Synchronized mono/RGB dual-camera capture, I/O-signal triggering|
|12|[Deep-Learning-Assisted Vision Inspection System](./12-vision-inspection-system-neurot)|Production-line part inspection|Matrox MIL + Neurocle deep-learning inference|

## Core Skills Demonstrated

* **C++ / Qt** application and GUI development for industrial control systems
* **Machine vision**: camera SDK integration (GigE Vision, TCP/IP), image
acquisition pipelines, Matrox MIL
* **Deep learning inference** integration (Neurocle) for defect classification and detection
* **3D vision**: structured-light point cloud reconstruction and processing (PCL, VTK, OpenCV)
* **Precision metrology**: sub-pixel edge detection, rotation-invariant feature
classification, least-squares geometric fitting
* **Industrial communication**: OPC UA (open62541), TCP/IP client-server, FTP, Modbus
* **Multi-threaded, real-time systems**: producer-consumer queues, thread-safe state machines
* **Software architecture**: layered design, dependency inversion, design patterns (Strategy, Adapter, Observer, Singleton)
* **Motion control**: multi-axis stage synchronization with camera triggering
* **Classical image processing**: color-space analysis, PCA, contour/edge-based detection

