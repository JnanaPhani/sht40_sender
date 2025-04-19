# sht40_sender

# ESP32 Bluetooth SHT40 Sensor with LED Indicators

This project uses an **ESP32** to read temperature and humidity data from an **SHT40 sensor**, and transmit it over **Bluetooth SPP (Serial Port Profile)** to a paired device (e.g., Android). It also includes **on-board LED indications** for Bluetooth states and data transmission.

---

## 🚀 Features

- 📡 **Bluetooth SPP Server**  
  ESP32 runs as a Bluetooth Classic SPP server named `esp32_bt_temp`.

- 🌡️ **Temperature & Humidity Sensing**  
  Reads data from the SHT40 sensor via I2C.

- 💡 **LED Indications**  
  On-board LED (GPIO 2) shows Bluetooth status and activity:

  | Event                           | LED Pattern                                |
  |--------------------------------|---------------------------------------------|
  | Bluetooth Initialized          | 10 short blinks                             |
  | Bluetooth Device Connected     | 5 short blinks                              |
  | Bluetooth Device Disconnected | 1 long pulse + 4 quick flickers             |
  | Data Transmitted via Bluetooth | 2 short blinks                              |

---

## 🧰 Requirements

- ESP32 board  
- SHT40 Sensor (I2C)
- LED connected to GPIO2 (onboard LED on many ESP32 boards)
- ESP-IDF 5.x or later
- Android device with Bluetooth terminal app (e.g., Serial Bluetooth Terminal)

---

## 🔧 Setup Instructions

1. **Clone the Project:**

   ```bash
   git clone https://github.com/yourusername/esp32-bt-sht40-led.git
   cd esp32-bt-sht40-led

