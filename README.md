# 🦖 Legacy Dino: An ESP32 Retro Adventure
[![PlatformIO](https://img.shields.io/badge/Platform-PlatformIO-orange.svg)](https://platformio.org/)
[![Framework-Arduino](https://img.shields.io/badge/Framework-Arduino-blue.svg)](https://www.arduino.cc/)
[![Hardware-ESP32](https://img.shields.io/badge/Hardware-ESP32-lightgrey.svg)](https://www.espressif.com/en/products/socs/esp32)


During a quiet weekend, I decided to swap social media scrolling for some hands-on engineering. Instead of playing someone else's game, I built my own. **Legacy Dino** is a high-performance recreation of the classic Chrome Dino game, optimized for the ESP32 and a $64 \times 32$ OLED display.

---

![Dino Gameplay](assets/videos/DinoGame.gif)


## 🚀 The Story
I don’t know much about PC game development, but I do know embedded systems. I asked myself: *Why not build a game with what I already have?* I recreated the logic from scratch, converted a high-fidelity T-Rex into a custom bitmap, and implemented a FreeRTOS-based engine to ensure the smoothest gameplay possible on a micro-display. The result? A satisfying, low-latency retro experience that fits in the palm of your hand.

---

## 🛠️ Tech Stack & Features
- **Hardware:** ESP32 (Dual-Core), $64 \times 32$ I2C OLED (SSD1306).
- **Core Logic:** Multi-threaded execution using **FreeRTOS**. Core 0 handles physics and collision, while Core 1 manages the UI rendering.
- **Physics:** Constant acceleration gravity model for realistic jumping.
- **Audio:** Real-time PWM-based sound effects for jumping and game-over states.
- **Graphics:** Custom dithered bitmaps for a "Pro" legacy aesthetic.

---

## 🔌 Circuit Diagram & Pinout

| Component | ESP32 Pin | Connection |
| :--- | :--- | :--- |
| **OLED SCL** | GPIO 22 | I2C Clock |
| **OLED SDA** | GPIO 21 | I2C Data |
| **Jump Button** | GPIO 14 | Momentary Switch to GND |
| **Reset Button** | GPIO 27 | Momentary Switch to GND |
| **Buzzer** | GPIO 13 | Passive Buzzer (+) |

---

## 🕹️ How to Build
1. **Clone the Repo:**
   ```bash
   git clone [https://github.com/SaiSundarP/Dino_Game_in_esp32.git](https://github.com/SaiSundarP/Dino_Game_in_esp32.git)
---
## 📺 Watch the Demo
<div align="center">
  <video src="assets/videos/Dino_Game.mp4" width="100%" autoplay loop muted playsinline>
    Your browser does not support the video tag.
  </video>
</div>
