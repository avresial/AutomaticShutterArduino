#include "Arduino.h"

//Initial program
int CoilPin = 12;
int ButtonPin = 8;

#define DirPin 2
#define StepPin 7
#define stepsPerRevolution 400

bool Dir = true;// if true clockwise

void setup() {
  pinMode(CoilPin, OUTPUT);
  pinMode(StepPin, OUTPUT);
  pinMode(DirPin, OUTPUT);
  pinMode(ButtonPin, INPUT);
  Serial.begin(9600);

}

void loop() {

  if (digitalRead(ButtonPin) == 1)
  {
    SwitchPowerTo(true);
    int HowFar = 40;// mm
    int testLaps = 1;
    for (int j = 0; j < testLaps; j++)
    {
      RotateCounterClockwise(CalculateDistanceToSteps(HowFar));
      delay(1000);
      RotateClockwise(CalculateDistanceToSteps(HowFar));
      delay(1000);
    }
  }
  else
  {
    SwitchPowerTo(false);
  }

  delay(100);
}

// Move
void RotateClockwise(int amount)
{
  Serial.println("Going Clockwise " + String(amount) + " steps.");
  digitalWrite(DirPin, LOW);
  for (int i = 0; i < amount; i++)
    MakeOneStep();
}

void RotateCounterClockwise(int amount)
{
  Serial.println("Going counter clockwise " + String(amount) + " steps.");
  digitalWrite(DirPin, HIGH);
  for (int i = 0; i < amount; i++)
    MakeOneStep();
}

void MakeOneStep()
{
  int gapBetweenStepsInMicroseconds = 4000;

  digitalWrite(StepPin, HIGH);
  delayMicroseconds(gapBetweenStepsInMicroseconds);
  digitalWrite(StepPin, LOW);
  delayMicroseconds(gapBetweenStepsInMicroseconds);
}

void SwitchPowerTo(bool state)
{
  switch (state) {
    case true:
      digitalWrite(CoilPin, HIGH);
      delay(100);
      break;
    case false:
      digitalWrite(CoilPin, LOW);
      delay(100);
      break;

  }
}

int CalculateDistanceToSteps(float distance)
{
  float R = 16;
  float AnglePerStep = 1.8;
  float howManyFullRotations = distance / (2 * PI * R);

  int steps = (int)((howManyFullRotations * 360) / AnglePerStep);

  Serial.println("Distance - " + String(distance) + " is " + String(steps) + " - steps.");
  return steps;
}
