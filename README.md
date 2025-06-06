## 🚗 Smart Parking ESP32 System

This project is an ESP32-based smart parking system that manages gate access and monitors parking slot availability using multiple sensors.

### 🔧 Features

- **Gate Control**  
  Sensor **SENSOR_0** (connected to GPIO 12) is used to detect vehicles approaching the gate. When a vehicle is detected and there are available slots, the system automatically opens the barrier using a servo motor.

- **Slot Monitoring**  
  Sensors **SENSOR_1 to SENSOR_4** (GPIOs 27, 33, 21, 18) are installed in individual parking spots to detect vehicle presence. Each sensor is linked to an LED indicator (GPIOs 26, 25, 4, 19) to show the occupancy status.

- **OLED Display**  
  An SSD1306 OLED screen displays the current Wi-Fi connection status and real-time license plate information of the parked vehicles in each slot.

- **Wi-Fi Enabled with WebServer**  
  The ESP32 connects to a Wi-Fi network and hosts a simple HTTP server that listens on port 80. License plate data can be sent via POST requests to the `/plate` endpoint.

### 🚀 How to Use

- Flash the code to an ESP32 device with 5 connected IR or ultrasonic sensors.
- Adjust pin assignments if your hardware differs.
- POST license plate strings (length 32) to `http://<esp32_ip>/plate` to update the display.

---

### 👤 Author
vuong.tt1807
