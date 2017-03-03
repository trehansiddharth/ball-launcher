#include <Servo.h>

// Pin to timer lookup table
volatile uint8_t* timers[] = {0x00, 0x00, &TCCR3B, &TCCR3B, &TCCR0B, &TCCR3B, &TCCR4B, &TCCR4B, &TCCR4B, &TCCR2B, &TCCR2B, &TCCR1B, &TCCR1B, &TCCR0B};

// DC motor configuration
int motorPin1 = 2;
int motorPin2 = 3;
byte speed = 0x40;

// Servo motor configuration
Servo servo1;
Servo servo2;
byte servoPin1 = 9;
byte servoPin2 = 10;
byte servoPos1 = 0;
byte servoPos2 = 0;

void setup() {
  // Set the correct PWM frequency on the two motor pins
  *timers[motorPin1] = (*timers[motorPin1] & 0xF8) | 0x01;
  *timers[motorPin1] = (*timers[motorPin1] & 0xF8) | 0x01;

  // Run the motor pins at their initial speed
  analogWrite(motorPin1, ~speed);
  analogWrite(motorPin2, ~speed);

  // Begin serial communication over USB CDC on pins 0 and 1
  Serial.begin(9600);

  // Set up the servos
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
}

void loop() {
  // Change the speed of the DC motor if a command arrives over serial port
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'S') {
      if (Serial.available()) {
        // Next byte should be hex code indicating speed (between 0 and F)
        char speedInput = Serial.read();
        speed = hexChar(speedInput) << 3;

        // Run the DC motors at this speed
        analogWrite(motorPin1, ~speed);
        analogWrite(motorPin2, ~speed);
      }
    } else {
      // Do nothing
    }
  }
  
  // in steps of 1 degree
  /*servo1.write(pos);              // tell servo to go to position in variable 'pos'
  pos = (pos + 1) % 180;
  delay(15);                       // waits 15ms for the servo to reach the position
  if (pos == 0x00) {
    delay(1000);
  }*/
}

byte hexChar(char s) {
  if ((s & 0xF0) == 0x30) {
    return (byte) s & 0x0F;
  } else if ((s & 0xF0) == 0x40) {
    return (byte) (s & 0x0F) + 9;
  }
}

