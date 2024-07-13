float GetDistance() // Ultrasonic Distance
{
  // Send Pulse to record data
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  // Receive Response
  int dura_ms = pulseIn(ULTRASONIC_ECHO, HIGH);
  if (dura_ms >= 38000)
    return -1; // Too far
  float dist_cm = dura_ms / 58;
  if (dist_cm < 0)
    return -1; // Invalid Read
  return dist_cm;
}

float GetVelocity(int time_ms) // Get Average Speed (over 60ms increments/intervals)
{
  float firstd = 0;
  float finald = 0;
  float start = millis();
  float dist = 0;
  while (millis() - start < time_ms)
  {
    dist = GetDistance();
    if (dist < 0)
      continue;
    if (firstd <= 0)
      firstd = dist;
    finald = dist;
    delay(60);
  }
  float ellapsed = millis() - start;
  float velocity = (finald - firstd) / ellapsed; // cm/ms
  velocity *= 1000; // cm/s
  return velocity;
}

bool isSomethingInFront(uint8_t port) // IR Sensor
{
  return !digitalRead(port);
}

void ScanForEnemies()
{
  Serial.println("Scanning for enemies");
  // Scan for enemies, not dependant on Scan_While_Travel
  if (isSomethingInFront(IR_C) || isSomethingInFront(IR_L) || isSomethingInFront(IR_R))
  {
    float distance = GetDistance();
    if (distance <= Attack_Distance)
    {
      State = 1;
    }
    else if (distance <= Defend_Distance)
    {
      State = 2;
    }
  }
}

void LookForLine(int &var) // &var is pointer to variable
{
  // Check both IR Line Sensors for Line
  var = (isSomethingInFront(IR_LINE_L) ? 1 : (isSomethingInFront(IR_LINE_R) ? 2 : 0));
}

void AvoidWhiteLine()
{
  if (Is_Near_White_Line == 1)
    {
      Serial.println("White on left, turning right");
      // Go Backward
      MOTOR_L.setSpeed(Pivot_Speed);
      MOTOR_R.setSpeed(-Pivot_Speed);
      delay(IR_LINE_DELAY);
      // Turn Right
      MOTOR_L.setSpeed(-Pivot_Speed);
      MOTOR_R.setSpeed(Pivot_Spin ? -Pivot_Speed : 0);
      delay(IR_LINE_DELAY);
    }
    else if (Is_Near_White_Line == 2)
    {
      Serial.println("White on right, turning left");
      // Go Backward
      MOTOR_L.setSpeed(Pivot_Speed);
      MOTOR_R.setSpeed(-Pivot_Speed);
      delay(IR_LINE_DELAY);
      // Turn Left
      MOTOR_L.setSpeed(Pivot_Spin ? -Pivot_Speed : 0);
      MOTOR_R.setSpeed(Pivot_Speed);
      delay(IR_LINE_DELAY);
    }
}

void SetSpeed(bool chnl_a, int speed)
{
  if (chnl_a)
  {
    // Apply to motor channel A
    digitalWrite(CHNL_A_BRK, (speed == 0) ? HIGH : LOW);
    digitalWrite(CHNL_A_DIR, (speed < 0) ? LOW : HIGH);
    analogWrite(CHNL_A_PWM, (speed < 0) ? -speed : speed);
  }
  else
  {
    // Apply to motor channel B
    digitalWrite(CHNL_B_BRK, (speed == 0) ? HIGH : LOW);
    digitalWrite(CHNL_B_DIR, (speed < 0) ? LOW : HIGH);
    analogWrite(CHNL_B_PWM, (speed < 0) ? -speed : speed);
  }
}
