#include <Keypad.h>
#include <LiquidCrystal_I2C.h> 
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
{'1','2','3', 'A'},
{'4','5','6', 'B'},
{'7','8','9', 'C'},
{'*','0','#', 'D'}
};
LiquidCrystal_I2C lcd(0x27, 16, 2); // ligne 8 
byte pin_rows[ROW_NUM] = {9,8,7,6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5,4,3,13}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

int sensorInterrupt = 0;  // interrupt 0
int sensorPin       = 2; //Digital Pin 2
int solenoidValve = 5; // Digital pin 5
unsigned int SetPoint = 4000; //4000 milileter
String code="";
/*The hall-effect flow sensor outputs pulses per second per litre/minute of flow.*/
float calibrationFactor = 90; //You can change according to your datasheet

volatile byte pulseCount =0;  

float flowRate = 0.0;
unsigned int flowMilliLitres =0;
unsigned long totalMilliLitres = 0,volume=0;

unsigned long oldTime ;
const int relais_moteur = 12; // the relay is connected to pin 3 of the Adruino board

void setup()
{ totalMilliLitres = 0;
  pinMode(relais_moteur, OUTPUT);
  lcd.init(); // display initialization
  lcd.clear();
  lcd.backlight(); // activate the backlight
  lcd.setCursor(0, 0); // stand in the front line
  lcd.print("Input volume:");

  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
  pinMode(solenoidValve , OUTPUT);
  digitalWrite(solenoidValve, HIGH);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  /*The Hall-effect sensor is connected to pin 2 which uses interrupt 0. Configured to trigger on a FALLING state change (transition from HIGH
  (state to LOW state)*/
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING); //you can use Rising or Falling
}

void loop()
{
  char key = keypad.getKey(); 
    //lcd.clear();

  if(key) // A key on the keyboard is pressed
    { 
  code+=key;
  lcd.setCursor(0, 1); // stand on the second line
  lcd.print(code);  // show volume value
  delay(100);
    }

    if (key=='D') { // if you press the 'D' key
     if(code.toInt()<=1500){
        volume=code.toInt();
     }
     else{
        lcd.clear();
        lcd.backlight(); 
        lcd.setCursor(0, 0); 
        lcd.print("donner volume:");  
     }
    code="";
    }

if (totalMilliLitres<volume) {
    digitalWrite(relais_moteur, HIGH); // Start the water pump

   if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to the host
    detachInterrupt(sensorInterrupt);

    // Because this loop may not complete in exactly 1 second intervals we calculate the number of milliseconds that have passed since the last execution and use that to scale the output. We also apply the calibrationFactor to scale the output based on the number of pulses per second per units of measure (litres/minute in this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60)*1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(flowMilliLitres, DEC);  // Print the integer part of the variable
    Serial.print("mL/Second");
    Serial.print("\t");           
    lcd.clear();
    lcd.backlight(); 
    lcd.setCursor(0, 0); 
    lcd.print("debit:");
    lcd.print(flowMilliLitres);  // Show the flow rate on the lcd display   
        lcd.print(" ml/s");    
    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");        
    Serial.print(totalMilliLitres,DEC);

    Serial.println("mL"); 
    Serial.print("\t");     
        lcd.setCursor(0, 1); 
        lcd.print("volume:");           
        lcd.print(totalMilliLitres);  // Show quantity filled   
        lcd.print(" ml"); 
    if (totalMilliLitres > 40)
    {
      SetSolinoidValve();
    }

// Reset the pulse counter so we can start incrementing again
    pulseCount = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}else  {
  digitalWrite(relais_moteur, LOW); // Stop the water pump
  volume=0;
}

}

//Insterrupt Service Routine

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void SetSolinoidValve()
{
  digitalWrite(solenoidValve, LOW);
}




// #include <Keypad.h>
// const int ROW_NUM = 4;
// const int COLUMN_NUM = 4;
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0X27, 16, 2);
 
// char keys[ROW_NUM][COLUMN_NUM] = {
//   { '1', '2', '3', 'A' },
//   { '4', '5', '6', 'B' },
//   { '7', '8', '9', 'C' },
//   { '*', '0', '#', 'D' }
// };
 
// byte pin_rows[ROW_NUM] = { 9,8,7,6 };
// byte pin_column[COLUMN_NUM] = { 5,4,3,13};
// Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
 
// int sensorInterrupt = 0;
// int sensorPin = 2;
// int solenoidValve = 5;
// unsigned int SetPoint = 4000;
// String code = "";
 
// float calibrationFactor = 2.25;
 
// volatile byte pulseCount = 0;
 
// float flowRate = 0.0;
// unsigned int flowMilliLitres = 0;
// unsigned long totalMilliLitres = 0, volume = 0;
 
// unsigned long oldTime;
// const int relais_moteur = 12;
 
// void setup() {
//   totalMilliLitres = 0;
//   pinMode(relais_moteur, OUTPUT);
//   lcd.init();
//   lcd.backlight();
//   lcd.setCursor(0, 0);
//   lcd.print("Input Vol:");
//   Serial.begin(9600);
//   pinMode(solenoidValve, OUTPUT);
//   digitalWrite(solenoidValve, HIGH);
//   pinMode(sensorPin, INPUT);
//   digitalWrite(sensorPin, HIGH);
//   attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
// }
 
// void loop() {
//   char key = keypad.getKey();
 
//   if (key) {
//     code += key;
//     lcd.setCursor(0, 1);
//     lcd.print(code);
//     delay(100);
//   }
 
//   if (key == 'D') {
//     if (code.toInt() <= 4000) {
//       volume = code.toInt();
//     } else {
//       lcd.clear();
 
//       lcd.setCursor(0, 0);
//       lcd.print("Input Vol:");
//     }
//     code = "";
//   }
 
//   if (totalMilliLitres < volume) {
//     digitalWrite(relais_moteur, HIGH);
 
//     if ((millis() - oldTime) > 1000) {
//       detachInterrupt(sensorInterrupt);
 
//       flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
//       oldTime = millis();
//       flowMilliLitres = (flowRate / 60) * 1000;
//       totalMilliLitres += flowMilliLitres;
 
//       unsigned int frac;
//       Serial.print("Flow rate :-");
//       Serial.print(flowMilliLitres, DEC);
//       Serial.print("mL/Second");
//       Serial.print("\t");
//       lcd.clear();
 
//       lcd.setCursor(0, 0);
//       lcd.print("Rate: ");
//       lcd.print(flowMilliLitres);
//       lcd.print(" ml/s");
//       Serial.print("Output Liquid Quantity: ");
//       Serial.print(totalMilliLitres, DEC);
 
//       Serial.println("mL");
//       Serial.print("\t");
//       lcd.setCursor(0, 1);
//       lcd.print("Filled:");
//       lcd.print(totalMilliLitres);
//       lcd.print(" ml");
//       if (totalMilliLitres > 40) {
//         SetSolinoidValve();
//       }
//       pulseCount = 0;
//       attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
//     }
//   } else {
//     digitalWrite(relais_moteur, LOW);
//     volume = 0;
//   }
// }
 
// void pulseCounter() {
 
//   pulseCount++;
// }
 
// void SetSolinoidValve() {
//   digitalWrite(solenoidValve, LOW);
// }



// #include <Keypad.h>
// #include <LiquidCrystal_I2C.h>

// // Define keypad layout
// const byte ROWS = 4;
// const byte COLS = 4;
// char keys[ROWS][COLS] = {
//   {'1', '2', '3', 'A'},
//   {'4', '5', '6', 'B'},
//   {'7', '8', '9', 'C'},
//   {'*', '0', '#', 'D'}
// };

// // Keypad pins
// byte rowPins[ROWS] = {9, 8, 7, 6};
// byte colPins[COLS] = {5, 4, 3, 13};
// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// // Relay control pin
// const int relayPin = 12;

// // Flow sensor variables
// volatile int nbTopsFan = 0;
// float totalFlowed = 0;

// // LCD setup
// LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns and 2 rows

// void setup() {
//   pinMode(relayPin, OUTPUT);
//   lcd.init();
//   lcd.backlight();
//   attachInterrupt(0, countFlow, RISING); // Attach interrupt for flow sensor
// }

// void loop() {
//   char key = keypad.getKey();
//   if (key) {
//     handleKeypadInput(key);
//   }
// }

// void handleKeypadInput(char key) {
//   static float desiredVolume = 0;
//   if (key >= '0' && key <= '9') {
//     desiredVolume = desiredVolume * 10 + (key - '0');
//     lcd.print(key);
//   } else if (key == 'D') {
//     startMotor();
//   } else if (key == 'C') {
//     stopMotor();
//     lcd.clear();
//     lcd.print("Total: ");
//     lcd.print(totalFlowed, 2); // Display total volume
//     lcd.setCursor(0, 1);
//     lcd.print("Flow: ");
//     lcd.print(totalFlowed / millis() * 1000, 2); // Display flow rate (L/s)
//   }
// }

// void startMotor() {
//   digitalWrite(relayPin, HIGH);
// }

// void stopMotor() {
//   digitalWrite(relayPin, LOW);
// }

// void countFlow() {
//   nbTopsFan++;
//   totalFlowed += 0.001; // Increment total flow (adjust based on your flow sensor)
// }






// //arduino library for I2C
// #include <Wire.h>

// //INCLUDE LCD DISPLAY FOR I2C
// #include <LiquidCrystal_I2C.h>


// //variable for flow sensor
// volatile int flow_frequency;
// float vol = 0.0,L_minute;
// unsigned char flowsensor =2;
// unsigned long currentTime;
// unsigned long cloopTime;

// //CREATE LCD OBJECT
// LiquidCrystal_I2C lcd(0X27, 16, 2);

// //interrupt function to increment flow
// void flow(){
//   flow_frequency++;
// }
// void setup() {
//   pinMode(flowsensor, INPUT);
//   attachInterrupt(digitalPinToInterrupt(flowsensor),flow, RISING); //SETUP INTERRUPT
//   // SETUP LCD WITH BACKLIGHT AND INITIALIZE
//   lcd.backlight();
//   lcd.init();
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Flow meter");
//   currentTime = millis();
//   cloopTime = currentTime;


// }

// void loop() {
//   currentTime = millis();

//   if(currentTime >= (cloopTime + 1000)){
//     cloopTime = currentTime;
//     if(flow_frequency !=0){
//       L_minute = (flow_frequency/7.5);
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Rate: ");
//       lcd.print(L_minute);
//       lcd.print(" L/M");
//       L_minute = L_minute/60;
//       lcd.setCursor(0, 1);
//       vol = vol + L_minute;
//       lcd.print("Vol:");
//       lcd.print(vol);
//       lcd.print(" L");
//       flow_frequency = 0;

//     }

//     else{
//       lcd.clear();
//       lcd.setCursor(0,0);
//       lcd.print("Rate: ");
//       lcd.print(flow_frequency);
//       lcd.print(" L/M");
//       lcd.setCursor(0,1);
//       lcd.print("Vol:");
//       lcd.print(vol);
//       lcd.print(" L");
//     }
//   }


// }