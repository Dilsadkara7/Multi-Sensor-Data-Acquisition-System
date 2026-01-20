# Multi-Sensor Data Acquisition & Monitoring System
**Developer:** Dilşad Kara  
**Student ID:** 230504044  
**Institution:** Turkish-German University (TAÜ)  
**Department:** Electrical and Electronics Engineering  

---

## 📌 Project Overview
An integrated engineering solution featuring a custom-designed PCB for real-time sensor monitoring, C++ firmware with digital signal processing, and a Python-based telemetry suite for MATLAB data analysis.


## 🛠 Technical Architecture

### 1. Hardware (PCB Design)
- **Software:** KiCad E.D.A. 9.0.6
- **Layers:** 2-layer optimized layout for signal integrity.
- **Key Modules:** - **ACS712:** Hall-effect current sensing.
  - **HC-SR04 & DHT11:** Distance and thermal monitoring.
  - **Actuator:** Servo-controlled braking mechanism.

### 2. Firmware (Embedded C++)
The firmware prioritizes reliability and smooth mechanical response through advanced control logic:
- **Digital Filtering:** Implementation of an Exponential Moving Average (EMA) filter to suppress sensor noise:
  $$y[n] = \alpha \cdot x[n] + (1 - \alpha) \cdot y[n-1]$$
- **Thermal Compensation:** Dynamic sound speed calculation for accurate distance measurements:
  $$v_{sound} \approx 331.5 + 0.6 \cdot T$$
- **Control Logic:** Sinusoidal braking curve for non-linear mechanical response:
  $$\theta = 180 \cdot (1 - \sin(\text{norm} \cdot \frac{\pi}{2}))$$

### 3. Middleware & Telemetry (Python)
A robust data logger designed with `pyserial` for secure data transfer:
- **Sampling:** High-frequency data logging to CSV format.
- **Fail-Safe:** Exception handling and serial port management for data integrity.

### 4. Analysis & Visualization (MATLAB)
Post-processing and real-time visualization of sensor datasets:
- Noise analysis of the ACS712 current sensor.
- System response validation through multi-channel plots.

## 📂 Repository Structure
- `/Hardware`: KiCad project files and PDF schematics.
- `/Firmware`: C++/Arduino source code.
- `/Scripts`: Python data logger.
- `/Docs`: Technical reports and MATLAB
