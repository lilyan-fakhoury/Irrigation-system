#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define soilMoistureSensor A2
#define temperature 9
#define waterLevel A1
#define greenLed 2
#define yellowLed 3
#define redLed 6
#define waterPump A3
#define buzzer 4
#define DHTTYPE DHT11

DHT dht(temperature, DHTTYPE);


const int screenWidth = 128;  // OLED display width, in pixels
const int screenHeight = 64;  // OLED display height, in pixels

int soilMoisture;
int waterLevelVal;
float t;
const int resetPinOled = -1;
Adafruit_SSD1306 c(screenWidth, screenHeight, &Wire, -1);

//DHT dht(temperature, DHTTYPE);
void setup() {
  Serial.begin(9600);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(waterPump, OUTPUT);
  pinMode(buzzer, OUTPUT);
  //for the oled s
  c.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // initialize OLED display with address 0x3C for 128x64
  if (!c.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true) {}
  }

  dht.begin();
  c.display();
  delay(300);  //showing the default logo for 2 sec
  welcoming(c);
  c.display();
  delay(3000);  //3000
}

void loop() {
  soilMoisture = map(analogRead(soilMoistureSensor), 0, 1023, 1023, 0);
  waterLevelVal = map(analogRead(waterLevel), 250, 700, 0, 100);
  t = dht.readTemperature();

  if (soilMoisture < 750 && checkWater())  //monitoring the soil if it's dry (the sensor reading more than 750)=> turn on the pump
  {
    digitalWrite(waterPump, HIGH);
    c.println("Watering Plant in progress");  //d
    delay(2000);
    digitalWrite(waterPump, LOW);
  }

  displayStats();
  delay(300);
}

void welcoming(Adafruit_SSD1306 a) {
  a.clearDisplay();
  a.setTextSize(1);
  a.setTextColor(WHITE);
  a.setCursor(10, 0);
  // Display static text
  a.println("Welcome to our ");
  a.println("planting system!");
}

bool checkWater(void) {
  //now check water level:
  if (waterLevelVal < (20)) {  //400 -0.20 * 2000
    tone(buzzer, 200);                  //turn on the buzzer
    delay(100);
    digitalWrite(redLed, HIGH);  // turn on red
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(buzzer, HIGH);
  } else if (waterLevelVal >= (20) && waterLevelVal <= (80)) {  //400-500-0.20 * 2000
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);  // turn on yellow
    digitalWrite(redLed, LOW);
  } else if (waterLevelVal > (80)) {  // >500-0.50 * 1000
    digitalWrite(greenLed, HIGH);              //// turn on green
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, LOW);
  }
  digitalWrite(buzzer, LOW);
  return true;
}

void displayStats(void) {
  c.clearDisplay();
  c.setTextSize(1);
  c.setTextColor(WHITE);
  c.setCursor(0, 0);
  c.print("Moisture: ");
  c.print(map(soilMoisture, 0, 1023, 0, 100));
  c.println("%");
  c.println("  ");
  c.print("WaterLevel: ");
  c.print(waterLevelVal);
  c.println(" ");
  c.println("  ");
  c.print("Tempreture: ");
  c.print(t);
  c.println(" C ");
  c.display();
}