// #include <CytronMotorDriver.h>

#define BUTTON_OUTPUT 36                // Button to make shit happen

#define ULTRASONIC_ECHO 30
#define ULTRASONIC_TRIG 28
#define ULTRASONIC_TIMEOUT 300

#define IR_L 22                         // IR (Bot) Sensor,   facing left
#define IR_C 24                         // IR (Bot) Sensor,  facing right
#define IR_R 26                         // IR (Bot) Sensor, facing center
#define IR_FRAME_STEP 10                // Amount of "frames" before reading IR Sensors
#define IR_TURN_DELAY 100               // Time (ms) to turn if IR sees bot

#define IR_LINE_L 32                    // IR (Line [color]) Sensor,  Left Side
#define IR_LINE_R 34                    // IR (Line [color]) Sensor, Right Side
#define IR_LINE_DELAY 750               // Time (ms) of backwards, then turn/spin

#define CHNL_A_DIR 12                   // Channel A Motor Direction
#define CHNL_A_PWM 3                    // Channel A Motor PWM
#define CHNL_A_BRK 9                    // Channel A Motor Brake
#define CHNL_A_CUR A0                   // Channel A Motor Current Sensing
#define MOTOR_L 1                       // Make MOTOR_L evaluate to true (improves readbility of code)

#define CHNL_B_DIR 13                   // Channel B Motor Direction
#define CHNL_B_PWM 11                   // Channel B Motor PWM
#define CHNL_B_BRK 8                    // Channel B Motor Brake
#define CHNL_B_CUR A1                   // Channel B Motor Current Sensing
#define MOTOR_R 0                       // Make MOTOR_R evaluate to false (improves readability of code)

#define Pivot_Spin 0                    // If we should move motors in opposing directions to spin arround axis 0=No, 1=Yes
#define Pivot_Speed 100                 // Speed of Pivot Spin, not dependant on Pivot_Spin

#define Travel_Speed 100                // Speed of Travel
#define Scan_While_Travel 1             // If we should scan for enemies while moving 0=No, 1=Yes

#define Attack_Speed 150                // Speed of Attack
#define Attack_Distance 25              // Distance to Attack (cm)

#define Defend_Speed 100                // Speed of Defend
#define Defend_Distance 70              // Distance to Defend (cm)
#define Defend_Velocity_Check_Time 300  // Time to check the velocity of the enemy (ms)
#define Defend_Velocity_Threshold 10    // Threshold for the velocity of the enemy (cm/s)

// NOTE: Cant be define as needs to be changed
int State = -1;                          // -1=WAIT/DELAY, 0=IDLE, 1=ATTACK, 2=DEFEND
int Is_Near_White_Line = 0;             // 0=No, 1=Left, 2=Right

// IR Variables (fuck you oscar)
bool isirl = false;
bool isirr = false;
bool isirc = false;
int ircount = 0;

// Configure the motor driver.
// CytronMD MOTOR_L(PWM_DIR, 5, 4); // Needs Negative Direction
// CytronMD MOTOR_R(PWM_DIR, 6, 7);

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

  // Motor Pins
  pinMode(CHNL_A_DIR, OUTPUT);
  pinMode(CHNL_A_PWM, OUTPUT);
  pinMode(CHNL_A_BRK, OUTPUT);

  pinMode(CHNL_B_DIR, OUTPUT);
  pinMode(CHNL_B_PWM, OUTPUT);
  pinMode(CHNL_B_BRK, OUTPUT);

  // Button Pin
  pinMode(BUTTON_OUTPUT, INPUT);

  // Wait 3 Seconds (once it receives power)
  delay(3000);
  State = 0; // Start in IDLE
}

void loop()
{
  //////////////
  // BUTTON CODE
  //////////////
  // Start
  // if (digitalRead(BUTTON_OUTPUT) && State == -1)
  // {
  //   delay(3000); // Wait 3 seconds
  //   State = 0;
  // }
  // Stop
  // if (!digitalRead(BUTTON_OUTPUT))
  // {
  //   SetSpeed(MOTOR_L, 0);
  //   SetSpeed(MOTOR_R, 0);
  //   State = -1;
  //   return;
  // }

  //////////////////
  // COLOR/LINE CODE
  //////////////////
  // Check if we are near white line or not
  LookForLine(Is_Near_White_Line);
  // if (Is_Near_White_Line > 0)
  // {
  //   AvoidWhiteLine();
  //   return; // Immediately skip to next update to avoid double turns
  // }

  /////////////////
  // IR SENSOR CODE
  /////////////////
  // Update IR vars if 10 frames
  if (ircount % IR_FRAME_STEP == 0)
  {
    isirc = isSomethingInFront(IR_C);
    isirl = isSomethingInFront(IR_L);
    isirr = isSomethingInFront(IR_R);
  }
  ircount++;

  // LOGGING
  Serial.println("COLOR SENSORS: ");
  Serial.println(!digitalRead(IR_LINE_L));
  Serial.println(!digitalRead(IR_LINE_R));
  // Serial.println(Is_Near_White_Line);
  // Serial.println("IR SENSORS: ");
  // Serial.println(!digitalRead(IR_L));
  // Serial.println(!digitalRead(IR_C));
  // Serial.println(!digitalRead(IR_R));
  // Serial.println("ULTRASONIC SENSORS");
  // Serial.println(GetDistance());

  ////////////////
  // STATE MACHINE
  ////////////////
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
    // Defend();
    Idle();
  }
  // Attack();
}

void Idle()
{
  Serial.println("IDLING");
  if (Is_Near_White_Line > 0)
  {
    AvoidWhiteLine();
    ScanForEnemies();
  }
  else
  {
    Serial.println("Continuing Forward");
    // Move Forward
    SetSpeed(MOTOR_L, Travel_Speed);
    SetSpeed(MOTOR_R, Travel_Speed);
    if (Scan_While_Travel)
    {
      ScanForEnemies();
    }
  }
}

void Attack()
{
  Serial.println("ATTACKING");
  if (Is_Near_White_Line > 0)
  {
    State = 0;
  }
  // Check if the enemy is in front of us
  if (isirc)
  {
    Serial.println("Enemy In Front");
    // Move Forward
    SetSpeed(MOTOR_L, Attack_Speed);
    SetSpeed(MOTOR_R, Attack_Speed);
  }

  // Turn in the direction of the enemy
  else if (isirr)
  {
    Serial.println("Enemy to right");
    // Turn Right
    SetSpeed(MOTOR_L, Attack_Speed);
    SetSpeed(MOTOR_R, Pivot_Spin ? -Attack_Speed : 0);
    delay(IR_TURN_DELAY);
  }
  else if (isirl)
  {
    Serial.println("Enemy to left");
    // Turn Left
    SetSpeed(MOTOR_L, Pivot_Spin ? -Attack_Speed : 0);
    SetSpeed(MOTOR_R, Attack_Speed);
    delay(IR_TURN_DELAY);
  }
  else
  {
    Serial.println("No enemy to attack");
    State = 0;
  }
}

void Defend()
{
  Serial.println("DEFENDING");
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
          SetSpeed(MOTOR_L, Defend_Speed);
          SetSpeed(MOTOR_R, Pivot_Spin ? -Defend_Speed : 0);
        } else { // White on Right or No White => Turn Left => More Speed Right;
          SetSpeed(MOTOR_R, Defend_Speed);
          SetSpeed(MOTOR_L, Pivot_Spin ? -Defend_Speed : 0);
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
