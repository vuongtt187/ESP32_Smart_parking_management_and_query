#include <ESP32Servo.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>

#define SENSOR_0 12
#define SENSOR_1 27
#define SENSOR_2 33
#define SENSOR_3 21
#define SENSOR_4 18

#define LED_1 26
#define LED_2 25
#define LED_3 4
#define LED_4 19
#define DOOR_PIN 13

#define DOOR_OPEN_ANGLE 180
#define DOOR_CLOSED_ANGLE 85

const char* ssid = "Vuongdz";//Vuongdz Tran Phuong T2
const char* password = "12345678";//12345678 12341234

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebServer server(80);

String displayText = "";

class SmartDoorSystem {
  public:
    void setup() {
      Wire.begin(23, 22);//22 SCL,23 SDA	

      if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("OLED không khởi động được"));
        for (;;);
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("ESP32 Ready");
      display.display();

      Serial.begin(115200);
      WiFi.begin(ssid, password);
      Serial.print("Connecting to WiFi");
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("\nWiFi connected");
      Serial.println(WiFi.localIP());

      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("WiFi connected");
      display.setCursor(0, 10);
      display.print("IP: ");
      display.println(WiFi.localIP());
      display.display();

      server.on("/plate", HTTP_POST, []() {
        String body = server.arg("plain");
        if (body == "") body = server.arg(0);

        if (body.length() >= 32) {
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(40, 0);
          display.println("KHU A");

          int y = 12;
          for (int i = 0; i < 4; i++) {
            String raw = body.substring(i * 8, (i + 1) * 8);
            raw.trim();
            String plate = "Empty";
            if (raw != "________") {
            plate = raw.substring(0, 3) + "-" + raw.substring(3, 6) + "." + raw.substring(6, 8);
            }
            display.setCursor(0, y);
            display.printf("S%d: %s", i + 1, plate.c_str());
            y += 12;
          }
          display.display();
          server.send(200, "text/plain", "OK");
        } else {
          server.send(400, "text/plain", "Invalid length");
        }
      });

      server.begin();

      myServo.attach(DOOR_PIN);
      myServo.write(DOOR_CLOSED_ANGLE);

      pinMode(SENSOR_0, INPUT);
      pinMode(SENSOR_1, INPUT);
      pinMode(SENSOR_2, INPUT);
      pinMode(SENSOR_3, INPUT);
      pinMode(SENSOR_4, INPUT);

      pinMode(LED_1, OUTPUT);
      pinMode(LED_2, OUTPUT);
      pinMode(LED_3, OUTPUT);
      pinMode(LED_4, OUTPUT);

      resetStates();
    }

    void loop() {
      server.handleClient();
      readSensors();
      handleDoor();
      handleLEDs();
    }

  private:
    Servo myServo;
    bool isDoorOpening = false;
    bool isDoorOpen = false;
    bool isDoorClosing = false;
    bool wasLowDetected = false;
    unsigned long doorOpenTime = 0;

    int sensorValue0, sensorValue1, sensorValue2, sensorValue3, sensorValue4;

    void resetStates() {
      isDoorOpening = false;
      isDoorOpen = false;
      isDoorClosing = false;
      wasLowDetected = false;
    }

    void readSensors() {
      sensorValue0 = digitalRead(SENSOR_0);
      sensorValue1 = digitalRead(SENSOR_1);
      sensorValue2 = digitalRead(SENSOR_2);
      sensorValue3 = digitalRead(SENSOR_3);
      sensorValue4 = digitalRead(SENSOR_4);
    }

    void handleDoor() {
      if (sensorValue0 == LOW && !isDoorOpening && !isDoorOpen && !isDoorClosing && !(sensorValue1 == LOW && sensorValue2 == LOW && sensorValue3 == LOW && sensorValue4 == LOW)) {
        myServo.write(DOOR_OPEN_ANGLE);
        isDoorOpening = true;
        isDoorOpen = true;
        doorOpenTime = millis();
        wasLowDetected = true;
      }

      if (isDoorOpen && (millis() - doorOpenTime >= 3000)) {
        isDoorClosing = true;
        isDoorOpen = false;
      }

      if (isDoorClosing) {
        myServo.write(DOOR_CLOSED_ANGLE);
        isDoorClosing = false;
        isDoorOpening = false;
        wasLowDetected = false;
      }
    }

    void handleLEDs() {
      digitalWrite(LED_1, sensorValue1 == LOW ? HIGH : LOW);
      digitalWrite(LED_2, sensorValue2 == LOW ? HIGH : LOW);
      digitalWrite(LED_3, sensorValue3 == LOW ? HIGH : LOW);
      digitalWrite(LED_4, sensorValue4 == LOW ? HIGH : LOW);
    }
};

SmartDoorSystem doorSystem;

void setup() {
  doorSystem.setup();
}

void loop() {
  doorSystem.loop();
}
