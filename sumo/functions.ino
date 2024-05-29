#include <CytronMotorDriver.h>

#define SPASTICUNT_L 3
#define PENISLICKR_R 10

// Configure the motor driver.
CytronMD motorL(PWM_DIR, 5, 4);
CytronMD motorR(PWM_DIR, 6, 7);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(SPASTICUNT_L, INPUT); // D Pin 3
  pinMode(PENISLICKR_R, INPUT); // D Pin 10

  motorL.setSpeed(255);
  motorR.setSpeed(255);

  findOpps();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(!digitalRead(SPASTICUNT_L));
}


