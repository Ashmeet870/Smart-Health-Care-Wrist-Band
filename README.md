# 🩺 IoT-Based Health Monitoring System with Wi-Fi and OLED Display

This project is an IoT-enabled real-time health monitoring system using ESP32. It integrates multiple sensors to measure vital health parameters and displays them on an OLED screen. The data is also sent to a cloud API for remote monitoring.

## 📦 Features

- 📡 Wi-Fi connectivity (ESP32)
- ❤️ Heart rate and SpO₂ measurement (MAX30100)
- 🌡️ Temperature and pressure sensing (BMP180)
- 📊 ECG signal monitoring
- 🔔 Buzzer alert for critical conditions
- 🖥️ OLED display (SH1106 128x64)
- ✋ Capacitive touch button to navigate screens and mute alerts
- ☁️ Data submission to REST API endpoint

## 🧰 Hardware Required

- ESP32 Dev Board
- MAX30100 Pulse Oximeter
- BMP180 / BMP085 Barometric Sensor
- ECG module (Analog output)
- SH1106 OLED Display (128x64)
- Buzzer
- Capacitive Touch Input (T0)
- Battery connection for monitoring (optional)
- Connecting wires, Breadboard

## 📲 Sensor Data Collected

- SpO₂ (%)
- Heart Rate (BPM)
- Body Temperature (°C)
- Barometric Pressure (hPa)
- ECG Signal (Analog)
- Battery Voltage (optional)

## 📡 API Configuration

Sensor readings are posted to the following API endpoint: