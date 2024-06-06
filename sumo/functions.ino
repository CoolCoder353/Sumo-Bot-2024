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
  float velocity = (finald - firstd) / ellapsed;
  return velocity;
}

bool isSomethingInFront(uint8_t port) // IR Sensor
{
  return !digitalRead(port);
}
