#include <Servo.h>

#define numOfValsRec 5
#define digitsPerValRec 1

Servo servoThumb;
Servo servoIndex;
Servo servoMiddle;
Servo servoRing;
Servo servoPinky;

int valsRec[numOfValsRec];
int stringLength = numOfValsRec * digitsPerValRec + 1;  //$00000
int counter = 0;
bool counterStart = false;
String receivedString;

// Define the analog pins for the four FSR sensors
#define FSR1 A0
#define FSR2 A1
#define FSR3 A2
#define FSR4 A3

// Define the maximum and minimum values for the sensors
#define FSR_MAX 1023
#define FSR_MIN 0

// Define the threshold for stopping motors
#define FSR_THRESHOLD 700

// Define the scaling factor to map the sensor values to 0-90 range
#define SCALE 90.0 / (FSR_MAX - FSR_MIN)

void setup() {
  Serial.begin(9600);
  servoThumb.attach(13);
  servoIndex.attach(9);
  servoMiddle.attach(10);
  servoRing.attach(12);
  servoPinky.attach(11);

  servoThumb.write(0);
  servoIndex.write(0);
  servoMiddle.write(0);
  servoRing.write(0);
  servoPinky.write(0);
}

void receieveData() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '$') {
      counterStart = true;
    }
    if (counterStart) {
      if (counter < stringLength) {
        receivedString = String(receivedString + c);
        counter++;
      }
      if (counter >= stringLength) {
        for (int i = 0; i < numOfValsRec; i++) {
          int num = (i * digitsPerValRec) + 1;
          valsRec[i] = receivedString.substring(num, num + digitsPerValRec).toInt();
        }
        receivedString = "";
        counter = 0;
        counterStart = false;
      }
    }
  }
}

void loop() {
  receieveData();

  // Read the analog values from the sensors
  int fsr1 = analogRead(FSR1);
  int fsr2 = analogRead(FSR2);
  int fsr3 = analogRead(FSR3);
  int fsr4 = analogRead(FSR4);

  // Map the values to 0-90 range using the scaling factor
  int fsr1_scaled = (fsr1 - FSR_MIN) * SCALE;
  int fsr2_scaled = (fsr2 - FSR_MIN) * SCALE;
  int fsr3_scaled = (fsr3 - FSR_MIN) * SCALE;
  int fsr4_scaled = (fsr4 - FSR_MIN) * SCALE;

  // Move motors based on received data and FSR sensor readings
  if (valsRec[0] == 0 && fsr1_scaled <= FSR_THRESHOLD) {
    servoThumb.write(fsr1_scaled);
  } else {
    servoThumb.write(0); // Stop at 0 degrees
  }
  
  if (valsRec[1] == 0 && fsr2_scaled <= FSR_THRESHOLD) {
    servoIndex.write(fsr2_scaled);
  } else {
    servoIndex.write(0); // Stop at 0 degrees
  }
  
  if (valsRec[2] == 0 && fsr3_scaled <= FSR_THRESHOLD) {
    servoMiddle.write(fsr3_scaled);
  } else {
    servoMiddle.write(0); // Stop at 0 degrees
  }
  
  if (valsRec[4] == 0 && fsr4_scaled <= FSR_THRESHOLD) {
    servoRing.write(fsr4_scaled);
  } else {
    servoRing.write(0); // Stop at 0 degrees
  }
  
  if (valsRec[3] == 0) {
    servoPinky.write(0); // Stop at 0 degrees
  } else {
    servoPinky.write(90);
  }

  // Wait for a short delay
  delay(100);
}
