
#define KONTAKTRON 0
#define hallReading 2
int HallSensor = A0;
int value = 0;
int PrevValue = 0;
int StepsForFullCircle = 200;

int absolutePosition = 0;
int absolutePositionContinuePositiveCounter = 0;
int absolutePositionContinueNegativeCounter = 0;

int LastHallSignalAbsolutePosition = 0;
bool Hall = false;
bool prevHall = false;

int HallIncrementation = 0;

bool safetyBrake = false;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(KONTAKTRON, INPUT_PULLUP);
  pinMode(hallReading, INPUT_PULLUP);
  Serial.begin(115200);
}


void loop() {



  if (!safetyBrake) {
    moveEngineClockwiseMock(702);
    Serial.println(absolutePosition);
  }
  if (!safetyBrake) {
    moveEngineCounterClockwiseMock(701);
    Serial.println(absolutePosition);
  }

  processMagneticSignals();
  if (digitalRead(KONTAKTRON) == LOW )
    calibrateShutter();

  if (safetyBrake )
  {
    Serial.println("Safety brake on!");
    delay(500);
    //calibrateShutter();
  }




  delay(100);

}
float test = 0;
void moveEngineClockwiseMock(int amount)
{
  Serial.println("Move Clockwise");
  absolutePositionContinueNegativeCounter = 0;
  for (int i = 0; i < amount; i++)
  {
    processMagneticSignals();
    if (absolutePosition % 5 == 0) {
      Serial.println(absolutePositionContinuePositiveCounter);
      test = (float(absolutePositionContinuePositiveCounter) / float(StepsForFullCircle) - float(HallIncrementation) + 1.0);
      Serial.println(test);
    }


    if (absolutePositionContinuePositiveCounter / StepsForFullCircle < HallIncrementation + 1 || (HallIncrementation == 0 && absolutePositionContinuePositiveCounter < StepsForFullCircle))
    {
      if ((float(absolutePositionContinuePositiveCounter) / float(StepsForFullCircle) - float(HallIncrementation) + 1.0) < 0.0)
      {
        Serial.println("End of shutter - STOP required");
        safetyBrake = true;
        i = amount;
      }

      absolutePositionContinuePositiveCounter++;
      absolutePosition++;
      delay(50);
    } else
    {
      safetyBrake = true;
      i = amount;
    }
  }
  HallIncrementation = 0;
}



void moveEngineCounterClockwiseMock(int amount)
{
  Serial.println("Move Counter Clockwise");
  absolutePositionContinuePositiveCounter = 0;
  for (int i = 0; i < amount; i++)
  {
    processMagneticSignals();
    if (absolutePosition % 5 == 0)
      Serial.println(absolutePositionContinueNegativeCounter);

    if (absolutePositionContinueNegativeCounter / StepsForFullCircle < HallIncrementation + 1 || (HallIncrementation == 0 && absolutePositionContinueNegativeCounter < StepsForFullCircle) )
    {

      absolutePositionContinueNegativeCounter++;
      absolutePosition--;
      delay(30);
    } else
    {
      safetyBrake = true;
      i = amount;
    }
  }
  HallIncrementation = 0;
}


void calibrateShutter()
{
  Serial.println("Calibraton!");

  HallIncrementation = 0;
  LastHallSignalAbsolutePosition = 0;

  int tolerance = 40;

  int FirstHallSignalPosition = 0;
  int SecondHallSignalPosition = 0;

  absolutePosition = 0;

  for (int i = 0; i < (2 * StepsForFullCircle) + (StepsForFullCircle * 0.1); i++)
  {
    processMagneticSignals();

    if (i > StepsForFullCircle && HallIncrementation < 1 )
    {
      Serial.println("\n\n!!!ABORT CALIBRATION!!!\n\n");
      safetyBrake = true;
      return;
    }

    if (HallIncrementation == 1 && FirstHallSignalPosition == 0 )
    {
      FirstHallSignalPosition = LastHallSignalAbsolutePosition;

      Serial.println("FirstHallSignalPosition - " + String(FirstHallSignalPosition));
      Serial.println("Next signal expected to be between - " + String(StepsForFullCircle + FirstHallSignalPosition - tolerance) + " and " + String(StepsForFullCircle + FirstHallSignalPosition + tolerance));
    }
    else if (HallIncrementation == 2 && SecondHallSignalPosition == 0)
    {
      if (StepsForFullCircle + FirstHallSignalPosition - tolerance < LastHallSignalAbsolutePosition &&  StepsForFullCircle + FirstHallSignalPosition + tolerance > LastHallSignalAbsolutePosition )
      {
        SecondHallSignalPosition = LastHallSignalAbsolutePosition;
        Serial.println("SecondHallSignalPosition - " + String(LastHallSignalAbsolutePosition));
      }
      else
      {
        Serial.println("SecondHallSignalPosition - " + String(LastHallSignalAbsolutePosition));
        Serial.println("\n\n!!!ABORT CALIBRATION!!!\n\n");
        safetyBrake = true;
        return;
      }
    }
    if (absolutePosition % 5 == 0)
      Serial.println("absolutePosition - " + String(absolutePosition));
    absolutePosition++;
    delay(30);
  }


  HallIncrementation = 0;
  int PrevHallIncrementation = HallIncrementation;
  bool ShutterIsClosed = false;
  int PreviousLastHallSignalAbsolutePosition = 0;
  int PotentialNewSignalPosition = 0;

  PotentialNewSignalPosition = LastHallSignalAbsolutePosition;

  while (!ShutterIsClosed)
  {

    processMagneticSignals();

    if (PrevHallIncrementation != HallIncrementation)
    {
      if ( PotentialNewSignalPosition - tolerance < LastHallSignalAbsolutePosition &&  PotentialNewSignalPosition  + tolerance > LastHallSignalAbsolutePosition )
      {
        PrevHallIncrementation = HallIncrementation;
      }
      else
      {
        if (LastHallSignalAbsolutePosition > PotentialNewSignalPosition - tolerance)
        {
          //new signal came too quick

          Serial.println("\n\n!!!CALIBRATION FINISHED!!!\n");
          Serial.println("\n!!!End of shutter reached!!!\n");
          
          ShutterIsClosed = true;
          HallIncrementation = 0;
          absolutePositionContinueNegativeCounter = 0;
          absolutePositionContinuePositiveCounter = 0;
          safetyBrake = false;
          HallIncrementation = 0;
          LastHallSignalAbsolutePosition = 0;

          Serial.println("absolutePosition - " + String(absolutePosition) + "\n -- Calibration Done --\n\n");

          delay(300);
          return;
        }


        Serial.println("\n\n!!!ABORT CALIBRATION!!!\n\n");
        safetyBrake = true;
        delay(300);
        return;
      }
    }

    if (absolutePosition % 5 == 0)
    {
      Serial.println("Next signal expected to be between - " + String(PotentialNewSignalPosition - tolerance) + " and " + String(PotentialNewSignalPosition + tolerance));
      Serial.println("absolutePosition - " + String(absolutePosition));
      Serial.println("");
    }


    if (absolutePosition < PotentialNewSignalPosition - tolerance)
    { //no new signal on time, motor might be slipping :c
      Serial.println("\n\n!!! No new signal on time, motor might be slipping :c !!!");
      Serial.println("!!!ABORT CALIBRATION!!!\n\n");
      safetyBrake = true;
      return;
    }



    if (PrevHallIncrementation != 0)
      PotentialNewSignalPosition = LastHallSignalAbsolutePosition - StepsForFullCircle;

    //actual movement
    absolutePosition--;
    delay(30);
  }
}






void processMagneticSignals()
{
  /*
    absolutePositionContinueNegativeCounter
    absolutePositionContinuePositiveCounter
    absolutePosition
  */
  prevHall = Hall;
  if (digitalRead(hallReading) == LOW && !prevHall) {
    Hall = true;
  }
  else
  {
    if (digitalRead(hallReading) != LOW)
      Hall = false;
  }

  if (!prevHall && Hall)
  {
    LastHallSignalAbsolutePosition = absolutePosition;
    HallIncrementation++;
  }


  if (absolutePosition % 5 == 0)
  {
    Serial.println("hall increment - " + String(HallIncrementation));
    Serial.println("LastHallSignalAbsolutePosition - " + String(LastHallSignalAbsolutePosition));
  }
}
