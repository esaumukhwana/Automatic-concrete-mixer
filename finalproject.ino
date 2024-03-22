//LIBRARIES INSTALLED
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "HX711.h"
#include <Servo.h>

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the keypad layout
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to row pinouts
byte colPins[COLS] = {5, 4, 3, 13}; // Connect to column pinouts

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//PINS TO SENSORS AND OUTPUT DEVICES
String cementWeight = ""; // Store user input
HX711 scale;
const int DOUT_PIN = 10; // Connect to HX711 DOUT pin
const int CLK_PIN = 11; // Connect to HX711 CLK pin

//CONSTANTS.
const float WATER_FACTOR = 0.6; // Water-to-cement weight ratio
const float SAND_FACTOR = 1.725; //sand to cement weight ratio
const float GRAVEL_FACTOR = 3.135; //gravel to cement weight ratio

// Pin definitions
const int flowSensorPin = 2; // Flow rate sensor input pin
const int solenoidValvePin = 12; // Solenoid valve control pin
// Variables
double waterFlow = 0.0;
unsigned long pulse_freq;

//SERVO MOTOR FOR UNLOADING
int servoPin = A1;
Servo Servo1;
int pos =0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Input cement:");
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2189.49); // Set the scale factor
  scale.tare();
  Servo1.attach(servoPin);
   // Configure pins
  pinMode(flowSensorPin, INPUT);
  pinMode(solenoidValvePin, OUTPUT);
   // Attach interrupt for flow sensor
  attachInterrupt(0, pulse, RISING);
}

void loop() {
  Servo1.write(0);
  char key = keypad.getKey();
  if (key != NO_KEY){
    if (key == 'C') {
      // User pressed 'C', clear the input
      cementWeight = "";
      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear the line
    }else {
      // Append the key to the input
      cementWeight += key;
      lcd.setCursor(0, 1);
      lcd.print(cementWeight);
    }

    if(key=='A'){
      // User pressed 'A', measure weight
      lcd.setCursor(0, 1);
      lcd.print("Measuring.......");
      delay(5000); // Wait for stability (optional)
      float weight = scale.get_units(); // Get weight in grams
      lcd.setCursor(0, 1);
      lcd.print("Weight: " + String(weight) + " g");
      delay(4000);

      if(abs(cementWeight.toFloat() - weight) <= 10){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Unloading...");

        for (pos = 0; pos <= 180; pos += 1) {
        Servo1.write(pos);
        delay(15);
        }
        for (pos = 180; pos >= 0; pos -= 1) {
            Servo1.write(pos);
            delay(15);
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        float sandWeight = float(SAND_FACTOR*cementWeight.toFloat());
        lcd.print("Add "+String(sandWeight)+ "g Sand");
        delay(3000);
      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("CEM UNMATCH");
        lcd.setCursor(0,1);
        lcd.print("PressAtoReload");
      }
    }
    if(key=='B'){
      lcd.setCursor(0, 1);
      lcd.print("Measuring.......");
      delay(5000); // Wait for stability (optional)
      float sandWeight = scale.get_units(); // Get weight in grams
      lcd.setCursor(0, 1);
      lcd.print("Weight: " + String(sandWeight) + " g");
      delay(4000);

      if(abs(sandWeight-float(cementWeight.toFloat()*SAND_FACTOR)) <= 10){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Unloading...");

        for (pos = 0; pos <= 180; pos += 1) {
        Servo1.write(pos);
        delay(15);
        }
      // Move from 180 to 0 degrees
        for (pos = 180; pos >= 0; pos -= 1) {
            Servo1.write(pos);
            delay(15);
        }
        lcd.clear();
        lcd.setCursor(0,0);
        float gravelWeight = float(GRAVEL_FACTOR * cementWeight.toFloat());
        lcd.print("Add "+String(gravelWeight)+ "g Grvl");
        delay(3000);
      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("SAND UNMATCH");
        lcd.setCursor(0,1);
        lcd.print("PressBtoReload");
      }
    }

    if(key=='D'){
      lcd.setCursor(0, 1);
      lcd.print("Measuring.......");
      delay(5000); // Wait for stability (optional)
      float gravelWeight = scale.get_units(); // Get weight in grams
      lcd.setCursor(0, 1);
      lcd.print("Weight: " + String(gravelWeight) + " g");
      delay(4000);

      if(abs(gravelWeight - float(cementWeight.toFloat()*GRAVEL_FACTOR)) <= 10){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Unloading...");

        for (pos = 0; pos <= 180; pos += 1) {
        Servo1.write(pos);
        delay(15);
        }
      // Move from 180 to 0 degrees
        for (pos = 180; pos >= 0; pos -= 1) {
            Servo1.write(pos);
            delay(15);
        }
        lcd.clear();
        lcd.setCursor(0,0);
        float volume =float(WATER_FACTOR * cementWeight.toFloat());
        lcd.print("Water: "+ String(volume) + "mL");
        // delay(3000);
        // lcd.setCursor(0, 1);
        // lcd.print("Pumping...");
        // delay(3000);
        // digitalWrite(solenoidValvePin, HIGH);

        // waterFlow = 2.25 * pulse_freq;

        //   lcd.clear();
        //   lcd.setCursor(0,0);
        //   lcd.print("Volume: ");
        //   lcd.setCursor(7,0);
        //   lcd.print(waterFlow,2);
        //   lcd.setCursor(13,0);
        //   lcd.print(" mL");
          // Serial.print(waterFlow, DEC);
          // Serial.println("mL");
          // delay(500);

      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("GRAVEL UNMATCH");
        lcd.setCursor(0,1);
        lcd.print("PressDtoReload");
       }
    }  
  }

}


// Interrupt service routine for flow sensor
void pulse() {
  pulse_freq++;
}



