#include <CytronMotorDriver.h>

#define BUTTON_OUTPUT 13                // Button to make shit happen

#define ULTRASONIC_ECHO 10
#define ULTRASONIC_TRIG 11
#define ULTRASONIC_TIMEOUT 300

#define IR_L A0                         // IR (Bot) Sensor,   facing left (Analog/Digital)
#define IR_R A2                         // IR (Bot) Sensor,  facing right (Analog/Digital)
#define IR_C A1                         // IR (Bot) Sensor, facing center (Analog/Digital)

#define IR_LINE_L 2                     // IR (Line [color]) Sensor,  Left Side
#define IR_LINE_R 3                     // IR (Line [color]) Sensor, Right Side

#define Pivot_Spin 1                    // If we should move motors in opposing directions to spin arround axis 0=No, 1=Yes
#define Pivot_Speed 255                 // Speed of Pivot Spin, not dependant on Pivot_Spin

#define Travel_Speed 255                // Speed of Travel
#define Scan_While_Travel 1             // If we should scan for enemies while moving 0=No, 1=Yes

#define Attack_Speed 255                // Speed of Attack
#define Attack_Distance 25              // Distance to Attack (cm)

#define Defend_Speed 255                // Speed of Defend
#define Defend_Distance 70              // Distance to Defend (cm)
#define Defend_Velocity_Check_Time 300  // Time to check the velocity of the enemy (ms)
#define Defend_Velocity_Threshold 10    // Threshold for the velocity of the enemy (cm/s)

// NOTE: Cant be define as needs to be changed
int State = -1;                         // -1=WAIT/DELAY, 0=IDLE, 1=ATTACK, 2=DEFEND
int Is_Near_White_Line = 0;             // 0=No, 1=Left, 2=Right

// Configure the motor driver.
CytronMD MOTOR_L(PWM_DIR, 5, 4);
CytronMD MOTOR_R(PWM_DIR, 6, 7);

void setup()
{
  // 9600 baud serial monitor
  Serial.begin(9600);

  // Bot IR Pins
  pinMode(IR_L, INPUT); // D Pin 10
  pinMode(IR_C, INPUT); // D Pin 11
  pinMode(IR_R, INPUT); // D Pin 12

  // Line IR Pins
  pinMode(IR_LINE_L, INPUT);
  pinMode(IR_LINE_R, INPUT);

  // Ultrasonic IR Pins
  pinMode(ULTRASONIC_TRIG, OUTPUT); // HC SR04  (Trigger)
  pinMode(ULTRASONIC_ECHO, INPUT);  // HC SR04 (Response)

  // Button Pin
  pinMode(BUTTON_OUTPUT, INPUT);
}

void loop()
{
  // Check if we are near white line or not
  LookForLine(Is_Near_White_Line);
  Serial.println(Is_Near_White_Line);

  // Start Code
  if (digitalRead(BUTTON_OUTPUT) && State == -1)
  {
    delay(3000); // Wait 3 seconds
    State = 0;
  }
  // Stop code
  if (!digitalRead(BUTTON_OUTPUT))
  {
    State = -1;
  }
  // Functions based on state
  if (State == 0)
  {
    Idle();
  }
  else if (State == 1)
  {
    Attack();
  }
  else if (State == 2)
  {
    Defend();
  }
}

void Idle()
{
  if (Is_Near_White_Line > 0)
  {
    // Turn in the direction away from the line, dependant on which sensor sees white line
    if (Is_Near_White_Line == 1)
    {
      // Turn Right
      MOTOR_L.setSpeed(Pivot_Speed);
      MOTOR_R.setSpeed(Pivot_Spin ? -Pivot_Speed : 0);
    }
    else if (Is_Near_White_Line == 2)
    {
      // Turn Left
      MOTOR_L.setSpeed(Pivot_Spin ? -Pivot_Speed : 0);
      MOTOR_R.setSpeed(Pivot_Speed);
    }
    ScanForEnemies();
  }
  else
  {
    // Move Forward
    MOTOR_L.setSpeed(Travel_Speed);
    MOTOR_R.setSpeed(Travel_Speed);
    if (Scan_While_Travel)
    {
      ScanForEnemies();
    }
  }
}

void Attack()
{

  if (Is_Near_White_Line)
  {
    State = 0;
  }
  // Check if the enemy is in front of us
  if (isSomethingInFront(IR_C))
  {
    // Move Forward
    MOTOR_L.setSpeed(Attack_Speed);
    MOTOR_R.setSpeed(Attack_Speed);
  }

  // Turn in the direction of the enemy
  else if (isSomethingInFront(IR_R))
  {
    // Turn Right
    MOTOR_L.setSpeed(Attack_Speed);
    MOTOR_R.setSpeed(Pivot_Spin ? -Attack_Speed : 0);
  }
  else if (isSomethingInFront(IR_L))
  {
    // Turn Left
    MOTOR_L.setSpeed(Pivot_Spin ? -Attack_Speed : 0);
    MOTOR_R.setSpeed(Attack_Speed);
  }
  else
  {
    State = 0;
  }
}

void Defend()
{
  // Check if the enemy is in front of us and not on white line
  if (isSomethingInFront(IR_C) && Is_Near_White_Line == 0)
  {
    // Check the enemies speed
    float velocity = GetVelocity(Defend_Velocity_Check_Time);
    if (velocity >= Defend_Velocity_Threshold)
    {
      // Avoid the enemy on right
      while (isSomethingInFront(IR_C) || isSomethingInFront(IR_L) || isSomethingInFront(IR_R))
      {
        if (Is_Near_White_Line == 1) // White on Left => Turn Right => More Speed Left
        {
          MOTOR_L.setSpeed(Defend_Speed);
          MOTOR_R.setSpeed(Pivot_Spin ? -Defend_Speed : 0);
        } else { // White on Right or No White => Turn Left => More Speed Right;
          MOTOR_R.setSpeed(Defend_Speed);
          MOTOR_L.setSpeed(Pivot_Spin ? -Defend_Speed : 0);
        }
        LookForLine(Is_Near_White_Line);
      }
      State = 0;
    }
    else
    {
      State = 1;
    }
  }
  else
  {
    State = 0;
  }
}
