
#define KONTAKTRON 0
#define hallReading 2
int HallSensor = A0;
int value = 0;
int PrevValue = 0;
int StepsForFullCircle = 200;

int absolutePosition = 0;
int absolutePositionContinuePositiveCounter = 0;
int absolutePositionContinueNegativeCounter = 0;

bool Hall = false;
bool prevHall = false;

int HallIncrementation = 0;

bool safetyBrake = false;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(KONTAKTRON, INPUT_PULLUP);
  pinMode(hallReading, INPUT_PULLUP);
  Serial.begin(9600);
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
  

 for (int i = 0; i < 300; i++)
  {
    if (absolutePosition % 5 == 0)
    Serial.println("absolutePosition - " + String(absolutePosition));
    absolutePosition++;
    delay(20);
  }

  int amount = absolutePosition;
  for (int i = 0; i < amount; i++)
  {
    if (absolutePosition % 5 == 0)
    Serial.println("absolutePosition - " + String(absolutePosition));
    absolutePosition--;
    delay(20);
  }
  HallIncrementation = 0;
  absolutePositionContinueNegativeCounter = 0;
  absolutePositionContinuePositiveCounter = 0;
  safetyBrake = false;
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
    HallIncrementation++;

  if (absolutePosition % 5 == 0)
    Serial.println("hall inc - " + String(HallIncrementation));

}
