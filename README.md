Touchless Gesture-Based Neural Keyboard using TinyML

Overview

A wearable, touchless keyboard that uses flex sensors and an IMU to recognize hand gestures and translate them into wireless keyboard input in real time. Gesture inference runs entirely on-device via TinyML, removing the need for cloud processing and making the system suitable for wearables, AR/VR, and assistive technology applications.

Background

Traditional input devices like keyboards and mice are efficient but constrain how and where users interact with computers. Bulky hardware-based keyboards are impractical in emerging domains such as wearable computing, AR/VR, and assistive technologies, which increasingly demand portable, flexible, and accessible interfaces.

Significance


Accessibility: Touchless neural keyboards powered by TinyML make technology accessible for people with disabilities or limited mobility, providing intuitive input without dependency on traditional devices.
Portability: Local gesture inference enables lightweight input systems that run efficiently on resource-constrained hardware — ideal for wearables, AR/VR, and mobile use.
Efficiency & Privacy: TinyML enables real-time gesture recognition directly on microcontrollers, reducing latency, power consumption, and privacy risks associated with cloud-based processing.


System Architecture & Interfacing

ComponentInterfacePurposeIMU Sensor (MPU-6050) → ESP32I2C (SDA/SCL)Motion and orientation dataFlex Sensors → ESP32ADCAnalog bending valuesESP32 → PC/SmartphoneBLE HIDWireless keyboard command transmissionPower SupplyUSB / BatteryMCU-compatible power source

This combination of I2C, ADC, and BLE HID interfaces enables the full pipeline: capturing sensor data, processing gestures via TinyML, and transmitting wireless keyboard output.

Pipeline


Data Acquisition — Read Flex Sensor and MPU6050 data at 50 Hz for 1.5 s per sample; collect 20 samples per gesture.
Dataset Creation — Extract and save sensor readings as labeled .csv files.
Model Training — Map each gesture to a keyboard keycode using Python/TensorFlow; train and export the model as a C library.
Deployment — Load the trained TinyML model (TFLite Micro) onto the ESP32 and integrate it into embedded firmware.
Live Inference — Acquire and preprocess sensor data on-device, extracting features from incoming frames in real time.
Output — Transmit the recognized keycode wirelessly via BLE HID to the target device (PC/Laptop).


Dataset Format


Each gesture's .csv file records: flex1, flex2, flex3, ax, ay, az, gx, gy, gz, label
Files follow the naming convention GESTURE_NAME_dataset.csv
20 samples collected per gesture for consistency and model optimization


Outcomes


Built labeled, multi-gesture datasets via systematic .csv collection.
Trained and mapped gestures to keycodes in Google Colab using TensorFlow.
Exported the trained model as gesture_model_data.cc, a C library compatible with ESP32 firmware.
Achieved real-time, wireless keyboard input via BLE HID using on-device gesture recognition on resource-constrained hardware.


Future Scope


Real-life integration: Embedding the gesture keyboard into wearable devices or smart environments for touchless interaction with computers, mobile devices, or assistive technology.
Use cases: Particularly valuable for individuals with disabilities, elderly users, and rehabilitation patients — offering an independent tool for communication, therapy tracking, and daily interaction with technology.


Key Learnings


Interfacing flex sensors and an MPU6050 IMU with the ESP32 to collect synchronized, ML-ready sensor data.
Building labeled datasets and training/deploying TinyML models via Python, TensorFlow, and Google Colab workflows.
Applying BLE HID for real-time wireless gesture-controlled input, applicable to both standard devices and health-monitoring setups.


Conclusions


Wearable, gesture-based keyboards are feasible, reliable, and customizable across diverse environments and user needs.
TinyML on embedded hardware like the ESP32 enables fast, on-device gesture recognition with minimal power consumption.
Real-time BLE communication extends accessibility, making the system suitable for assistive technology, rehabilitation monitoring, and hands-free control in smart environments.


Tools & Technologies


Hardware: ESP32, MPU-6050 IMU, Flex Sensors
ML Framework: Python, TensorFlow, TFLite Micro
Development Environment: Google Colab
Communication: BLE HID
