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