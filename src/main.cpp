#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define SCREEN_WIDTH 64 
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Preferences prefs;

// Pins (Maintained as requested)
const int PIN_BUZZER = 13;
const int PIN_JUMP   = 14;
const int PIN_RESET  = 27;

// PWM Settings for Buzzer
const int BUZZER_CHAN = 0;
const int BUZZER_RES  = 8;

// Physics & Assets
#define GROUND_Y      30
#define GRAVITY       0.7f
#define JUMP_FORCE    -7.0f
#define DINO_W        12
#define DINO_H        12

// Legacy Dino Bitmap (12x12)
/*static const unsigned char dino_bmp[] = {
  0b00000111, 0b11000000,
  0b00000101, 0b11100000,
  0b00000111, 0b11100000,
  0b00000111, 0b00000000,
  0b01111111, 0b00000000,
  0b11111111, 0b11000000,
  0b11111111, 0b11100000,
  0b11111111, 0b10000000,
  0b11111111, 0b10000000,
  0b00011011, 0b00000000,
  0b00011011, 0b00000000,
  0b00011011, 0b00000000
};*/

const unsigned char dino_bmp[] = {
  0x07, 0xC0,
  0x05, 0xE0,
  0x07, 0xF0,
  0x07, 0x00,
  0x7F, 0x00,
  0xFF, 0xC0,
  0xFF, 0xE0,
  0xFF, 0x80,
  0xFF, 0x80,
  0x1B, 0x00,
  0x1B, 0x00,
  0x1B, 0x00
};
// Shared State
SemaphoreHandle_t xMutex;
float dinoY = GROUND_Y - DINO_H;
float dinoVy = 0;
float obsX = 80;      
int obsH = 8;
int score = 0;
int highScore = 0;
bool isGameOver = false;
float gameSpeed = 1.3f; 
volatile bool requestJumpSound = false;
volatile bool requestCrashSound = false;

// Sound Helper Functions
void playJumpTone() {
  ledcWriteTone(BUZZER_CHAN, 600);
  vTaskDelay(pdMS_TO_TICKS(50));
  ledcWriteTone(BUZZER_CHAN, 0);
}

void playCrashTone() {
  ledcWriteTone(BUZZER_CHAN, 150);
  vTaskDelay(pdMS_TO_TICKS(200));
  ledcWriteTone(BUZZER_CHAN, 100);
  vTaskDelay(pdMS_TO_TICKS(300));
  ledcWriteTone(BUZZER_CHAN, 0);
}

void vSoundTask(void * pvParameters) {
  for (;;) {
    if (requestJumpSound) {
      requestJumpSound = false;
      playJumpTone();
    }

    if (requestCrashSound) {
      requestCrashSound = false;
      playCrashTone();
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void vLogicTask(void * pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20)); 

    bool localJumpSound = false;
    bool localCrashSound = false;

    if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(10))) {
      if (!isGameOver) {
        // Jump Logic
        if (digitalRead(PIN_JUMP) == LOW && dinoY >= (GROUND_Y - DINO_H)) {
          dinoVy = JUMP_FORCE;
          localJumpSound = true;
        }

        dinoVy += GRAVITY;
        dinoY += dinoVy;

        if (dinoY > (GROUND_Y - DINO_H)) {
          dinoY = GROUND_Y - DINO_H;
          dinoVy = 0;
        }

        obsX -= gameSpeed;
        if (obsX < -6) {
          obsX = 70 + random(0, 30);
          obsH = random(8, 12);
          score++;
          gameSpeed += 0.04f; 
          if (score > highScore) {
            highScore = score;
            prefs.putInt("highScore", highScore);
          }
        }

        // Refined Collision (Hitbox)
        if (obsX < (8 + DINO_W) && obsX > 4) {
          if (dinoY + DINO_H > (GROUND_Y - obsH + 2)) { // +2 for fair play
            isGameOver = true;
            localCrashSound = true;
          }
        }
      }
      xSemaphoreGive(xMutex);
    }

    if (localJumpSound) {
      requestJumpSound = true;
    }
    if (localCrashSound) {
      requestCrashSound = true;
    }
  }
}

void setup() {
  pinMode(PIN_JUMP, INPUT_PULLUP);
  pinMode(PIN_RESET, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { for(;;); }
  display.clearDisplay();

  prefs.begin("game", false);
  highScore = prefs.getInt("highScore", 0);
  
  xMutex = xSemaphoreCreateMutex();
  ledcSetup(BUZZER_CHAN, 2000, BUZZER_RES);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHAN);
  xTaskCreatePinnedToCore(vLogicTask, "Logic", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(vSoundTask, "Sound", 2048, NULL, 1, NULL, 0);
}

void loop() {
  display.clearDisplay();
  
  if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(10))) {
    if (!isGameOver) {
      // 1. Draw Ground
      display.drawFastHLine(0, GROUND_Y, 64, WHITE);
      
      // 2. Draw Legacy Dino Image
      display.drawBitmap(5, (int)dinoY, dino_bmp, DINO_W, DINO_H, WHITE);
      
      // 3. Draw Obstacle
      display.fillRect((int)obsX, GROUND_Y - obsH, 3, obsH, WHITE);
      
      // 4. Score + High Score Tracker (Top-Right Corner)
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("HI:");
      display.print(highScore);
      display.setCursor(46,0 );
      display.print("S:");
      display.print(score);

    } else {
      display.setCursor(4, 5);
      display.print("Game Over!");
      display.setCursor(4, 15);
      display.print("Score:"); display.print(score);
      display.setCursor(4, 25);
      display.print("HI:"); display.print(highScore);
      
      if (digitalRead(PIN_RESET) == LOW) {
        obsX = 85;
        score = 0;
        gameSpeed = 1.3f;
        dinoY = GROUND_Y - DINO_H;
        isGameOver = false;
      }
    }
    xSemaphoreGive(xMutex);
  }
  
  display.display();
}