#include <CytronMotorDriver.h>

#define MOTOR_L 3
#define MOTOR_R 10
#define ULTRASONIC_ECHO 8
#define ULTRASONIC_TRIG 9
#define ULTRASONIC_TIMEOUT 300
#define COCAINE_LINE1

// Configure the motor driver.
CytronMD motorL(PWM_DIR, 5, 4);
CytronMD motorR(PWM_DIR, 6, 7);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(MOTOR_L, INPUT); // D Pin 3
  pinMode(MOTOR_R, INPUT); // D Pin 10
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);

  motorL.setSpeed(255);
  motorR.setSpeed(255);

  // findOpps();
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Serial.println(!digitalRead(SPASTICUNT_L)); // Is IR detecting
  Serial.println(GetDistance());
}
