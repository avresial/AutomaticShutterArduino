#include "Arduino.h"

//Initial program
int CoilPin = 12;
int ButtonPin = 8;

int AbsoluteSteps = 0; // tells how many steps was made from position 0, + means clockwise, - means counterClockwise
int PreviousAbsoluteSteps = 0;

const int pResistor = A0;
#define DirPin 2
#define StepPin 7

int PhotoResistorActualValue = 50;
int PhotoResistorNightValue = 300;
int PhotoResistorDayValue = 600;

bool Dir = true;// if true clockwise
bool MockServerBool = true;

struct Order {
  String Name;
  int Value;
};

Order CurrentOrder = {"Null", 0};

void setup() {
  pinMode(CoilPin, OUTPUT);
  pinMode(StepPin, OUTPUT);
  pinMode(DirPin, OUTPUT);
  pinMode(ButtonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  Order newOrder = WhatToDo(CurrentOrder);
  CurrentOrder = newOrder;

  //PhotoResistorActualValue = analogRead(pResistor);

  Do(CurrentOrder);
  CurrentOrder = {"Null", 0};

  if (PreviousAbsoluteSteps != AbsoluteSteps)
  {
    Serial.println("AbsoluteSteps - " + String(AbsoluteSteps));
    PreviousAbsoluteSteps = AbsoluteSteps;
  }
  if (PhotoResistorActualValue < 1)
    PhotoResistorActualValue = PhotoResistorDayValue + 100;
  PhotoResistorActualValue--;
  //delay(10);
}

// Move
void OpenTo(int value)
{
  int newRotation = value - AbsoluteSteps;
  Serial.println("I'm opening to - " + String(newRotation));
  if (newRotation < 0)
  {
    RotateCounterClockwise( -1 * newRotation);
  } else
  {
    RotateClockwise(newRotation);
  }
}
void RotateClockwise(int amountOfSteps)
{
  Serial.println("Going Clockwise " + String(amountOfSteps) + " steps.");
  digitalWrite(DirPin, LOW);
  for (int i = 0; i < amountOfSteps; i++)
    MakeOneStep();
  AbsoluteSteps += amountOfSteps;
}

void RotateCounterClockwise(int amountOfSteps)
{
  Serial.println("Going counter clockwise " + String(amountOfSteps) + " steps.");
  digitalWrite(DirPin, HIGH);
  for (int i = 0; i < amountOfSteps; i++)
    MakeOneStep();
  AbsoluteSteps -= amountOfSteps;
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

//Logic
Order WhatToDo(Order lastOrder)
{
  //Serial.println("PhotoResistorActualValue " + String(PhotoResistorActualValue));
  Order order;
  if (lastOrder.Name == "Null")
  {
    if (PhotoResistorActualValue < PhotoResistorNightValue && AbsoluteSteps != 100)
    {
      
      order = {"OpenTo", 100};
      return order;
    }

    if (PhotoResistorActualValue > PhotoResistorDayValue && AbsoluteSteps != 0)
    {
      order = {"OpenTo", 0};
      return order;
    }
    if (digitalRead(ButtonPin) == 1)
    {
      order = GetOrderFromServer();
      Serial.println(order.Name + " " + String(order.Value) + " mm");
      return order;
    }
  }
  return lastOrder;
}

Order GetOrderFromServer()
{
  Order order;
  if (MockServerBool)
  {
    order = {"RotateColckwise", 100};
    MockServerBool = false;
  } else
  {
    order = {"RotateCounterColckwise", 100};
    MockServerBool = true;
  }

  return order;
}

void Do(Order order)
{
  if (order.Name == "RotateColckwise")
  {
    SwitchPowerTo(true);
    RotateClockwise(CalculateDistanceToSteps(order.Value));
    SwitchPowerTo(false);
  }
  else if (order.Name == "RotateCounterColckwise")
  {
    SwitchPowerTo(true);
    RotateCounterClockwise(CalculateDistanceToSteps(order.Value));
    SwitchPowerTo(false);
  } else if (order.Name == "OpenTo")
  {
    SwitchPowerTo(true);
    OpenTo(order.Value);
    SwitchPowerTo(false);
  }
  SwitchPowerTo(false);
}
