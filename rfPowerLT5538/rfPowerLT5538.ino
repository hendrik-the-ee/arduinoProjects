/***************************************************************************
  RT5538
 ***************************************************************************/


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  int enablePin = 2;

  digitalWrite(enablePin, HIGH); 
}

// the loop routine runs over and over again forever:
void loop() {

  int numReads=0;
  double rfPowerSum= 0;
  int rfPowerLevel;
  unsigned int timeBefore = millis();
  unsigned int timeAfter = timeBefore;

  while (timeAfter - timeBefore < 1000) {

    numReads=numReads+1;
    rfPowerLevel = analogRead(A0);
    rfPowerSum = rfPowerSum + double(rfPowerLevel);

    timeAfter = millis();
  }

  double rfMean = rfPowerSum / numReads;
  double rfVoltage = rfMean * 5 / 1024;

  Serial.print("timeBefore: ");Serial.print(timeBefore);
  Serial.print(", timeAfter: ");Serial.print(timeAfter);
  Serial.print(", numReads: ");Serial.print(numReads);
  Serial.print(", rfMean: ");Serial.print(rfMean);
  Serial.print(", rfVoltage: ");Serial.println(rfVoltage);
}

