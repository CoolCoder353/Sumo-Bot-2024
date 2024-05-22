#include <CytronMotorDriver.h>
// Configure the motor driver.
CytronMD motorL(PWM_DIR, 5, 4);
CytronMD motorR(PWM_DIR, 6, 7);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  motorL.setSpeed(255);
  motorR.setSpeed(255);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Running");
}
