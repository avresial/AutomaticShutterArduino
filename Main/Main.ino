
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
    digitalWrite(CoilPin, HIGH);
    delay(100);
    int testLaps = 6;
    for (int j = 0; j < testLaps; j++)
    {
      if (Dir)
      {
        Serial.println("Clockwise");
        digitalWrite(DirPin, HIGH);
      }
      else {
        Serial.println("Counter clockwise");
        digitalWrite(DirPin, LOW);
      }

      for (int i = 0; i < stepsPerRevolution; i++)
      {
        digitalWrite(StepPin, HIGH);
        delayMicroseconds(4000);
        digitalWrite(StepPin, LOW);
        delayMicroseconds(4000);
      }
      Dir = !Dir;
    }
  }
  else
  {
    digitalWrite(CoilPin, LOW);
  }

  delay(100);




  //



}
