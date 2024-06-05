float penislength() // Ultrasonic Distance
{
  // Send Pulse to record data
  digitalWrite(FUCKER_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(FUCKER_TRIG, LOW);
  // Receive Response
  int dura_ms = pulseIn(FUCKER_ECHO, HIGH);
  if (dura_ms >= 38000) return -1; // Too far
  float dist_cm = dura_ms/58;
  if (dist_cm < 0) return -1; // Invalid Read
  return dist_cm;
}

float cum_velocity(int time_ms) // Get Average Speed (over 60ms increments/intervals)
{
  float firstd = 0;
  float finald = 0;
  float start = millis();
  float dist = 0;
  while (millis() - start < time_ms)
  {
    dist = penislength();
    if (dist < 0) continue;
    if (firstd <= 0) firstd = dist;
    finald = dist;
    delay(60);
  }
  float ellapsed = millis() - start;
  float velocity = (finald-firstd)/ellapsed;
  return velocity;
}

bool somethingsInMyWay(uint8_t port) // IR Sensor
{
  return !digitalRead(port);
}
