#include <Servo.h>

// Pin to timer lookup table
volatile uint8_t* timers[] = {0x00, 0x00, &TCCR3B, &TCCR3B, &TCCR0B, &TCCR3B, &TCCR4B, &TCCR4B, &TCCR4B, &TCCR2B, &TCCR2B, &TCCR1B, &TCCR1B, &TCCR0B};

// DC motor configuration
int motorPin = 2;
byte speed = 0x40;

// Servo motor configuration
Servo servoFrame;
Servo servoLatch;
byte servoFramePin = 9;
byte servoLatchPin = 10;

void setup() {
  // Set the correct PWM frequency on the two motor pins
  *timers[motorPin] = (*timers[motorPin] & 0xF8) | 0x01;

  // Run the motor pins at their initial speed
  analogWrite(motorPin, ~speed);

  // Begin serial communication over USB CDC on pins 0 and 1
  Serial.begin(9600);

  // Set up the servos
  servoFrame.attach(servoFramePin);
  servoLatch.attach(servoLatchPin);
}

void loop() {
  // Change the speed of the DC motor if a command arrives over serial port
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'L') {
      // L: Set the speed, move the frame to the right angle, and open the latch
      
      // Wait until another byte arrives
      while (!Serial.available()) {}
      
      // Next byte should be hex code indicating speed (between 0 and F)
      char speedInput = Serial.read();
      speed = hexChar(speedInput) << 4 | 0x0F;

      // Run the DC motors at this speed
      analogWrite(motorPin, ~speed);

      // Wait until another byte arrives
      while (!Serial.available()) {}

      // Next byte should be hex code indicating high nibble of angle (between 0 and F)
      byte angle = 0x00;
      char highByteInput = Serial.read();
      angle = hexChar(highByteInput) << 4;

      // Wait until another byte arrives
      while (!Serial.available()) {}

      // Next byte should be hex code indicating low nibble of angle (between 0 and F)
      char lowByteInput = Serial.read();
      angle = angle | hexChar(lowByteInput);

      // Move the servo to the angle
      servoFrame.write(angle);

      // Delay a little bit and open the latch
      delay(1000);
      servoLatch.write(135);
      delay(2000);
      servoLatch.write(0);
    } else {
      Serial.write('x');
    }
  }
}

byte hexChar(char s) {
  if ((s & 0xF0) == 0x30) {
    return (byte) s & 0x0F;
  } else if ((s & 0xF0) == 0x40) {
    return (byte) (s & 0x0F) + 9;
  }
}

