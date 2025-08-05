#include <SimpleDHT.h>
#include <LiquidCrystal.h>

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#define BLUE 6
#define GREEN 5
#define RED 3

// Second RGB LED on A3 (blue), A4 (green), A5 (red)
#define RED2    A5
#define GREEN2  A4
#define BLUE2   A3

#define JOY_Y A0  // Joystick Y-axis connected to A0
#define SOIL_SENSOR 13  // D0 pin of soil sensor

#define SOIL_SENSOR_PIN 13
#define RED_LED_PIN A2      // Connect red LED to D4
  

int currentScreen = 0; // 0 = temperature, 1 = humidity, 2 = soil
unsigned long lastSwitch = 0;
const int debounceDelay = 300;

void setup() {
  Serial.begin(115200);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  pinMode(RED2, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  pinMode(BLUE2, OUTPUT);

  pinMode(JOY_Y, INPUT);
  pinMode(SOIL_SENSOR, INPUT);

  lcd.begin(16, 2);
  lcd.clear();

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);

  digitalWrite(RED2, LOW);
  digitalWrite(GREEN2, LOW);
  digitalWrite(BLUE2, LOW);

    pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);
    delay(1000);
    return;
  }
  int soilState = digitalRead(SOIL_SENSOR_PIN);

if (soilState == HIGH) {
    // Soil is dry
    digitalWrite(RED_LED_PIN, HIGH);
    
  } else {
    // Soil is wet
    digitalWrite(RED_LED_PIN, LOW);
    
  }
  int tempF = (int)(temperature * 9 / 5) + 32;

  // First RGB reacts to temperature
  if (tempF > 78) {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
  } else if (tempF < 68) {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
  } else {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
  }

  // Second RGB reacts to humidity
  if (humidity > 60) {
    digitalWrite(RED2, HIGH);
    digitalWrite(GREEN2, LOW);
    digitalWrite(BLUE2, LOW);
  } else if (humidity < 40) {
    digitalWrite(RED2, LOW);
    digitalWrite(GREEN2, LOW);
    digitalWrite(BLUE2, HIGH);
  } else {
    digitalWrite(RED2, LOW);
    digitalWrite(GREEN2, HIGH);
    digitalWrite(BLUE2, LOW);
  }

  // Joystick scroll logic
  int joyY = analogRead(JOY_Y);
  unsigned long currentTime = millis();

  if (currentTime - lastSwitch > debounceDelay) {
    if (joyY < 400) {
      currentScreen = (currentScreen + 1) % 3;
      lastSwitch = currentTime;
    } else if (joyY > 600) {
      currentScreen = (currentScreen - 1 + 3) % 3;
      lastSwitch = currentTime;
    }
  }

  // LCD Display
  lcd.clear();
  if (currentScreen == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.setCursor(0, 1);
    lcd.print(tempF);
    lcd.print(" F");
  } else if (currentScreen == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity:");
    lcd.setCursor(0, 1);
    lcd.print((int)humidity);
    lcd.print("%");
  } else if (currentScreen == 2) {
    int soil = digitalRead(SOIL_SENSOR);
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture:");
    lcd.setCursor(0, 1);
    if (soil == HIGH) {
      lcd.print("Dry");
    } else {
      lcd.print("Wet");
    }
  }

  delay(200);
}