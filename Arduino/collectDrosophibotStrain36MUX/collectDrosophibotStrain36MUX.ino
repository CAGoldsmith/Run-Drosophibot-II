#include <Wire.h>
float targetStrainBaseline = 300;
float strainBaselineErrorMargin = 20;
int digiPotIDs[3] = {44,46,45}; //The three unique digipot IDs that each amplifier board uses
int numDigiPotIDs = 3;
int analogPorts[6] = {0, 1, 2, 3, 4, 5}; //The ports that will recieve the strain data from the amplifier boards. Each leg get a dedicated port.
int numAnalogPorts = 6;
int MUXPins[6] = {10, 11, 12, 13, 14, 15}; //The digital pins that control how the strain data is routed on the MUX
int MUXPinNum = 6;
int ampMUXPins[2] = {22,23}; //The digital pins that control the switches on the amplifier board. Used to calibrate the strain signals.
int ampMUXPinNum = 2;
int I2CMUXPins[6] = {16,17,18,19,20,21}; //The digital pins that control which leg the I2C connection is connected to. Used for calibrating the strain signals
int I2CMUXPinNum = 6;
byte wiperVal = 50; //The wiper value for the digipot from which to start the calibration
float allSGValues[6][6] = {}; //Empty array for the strain data
float value;

void setup() {
  Serial.begin(115200);
  while(!Serial); //Initialize Serial and don't continue until it's ready
  Wire.begin(); //Initialize Wire to be able to communicate with the digipots over I2C

  // Set all digital pins on the microcontroller as OUTPUTS and set them LOW
  for(int i=0;i<MUXPinNum;i++)
  {
    pinMode(MUXPins[i], OUTPUT);
    digitalWrite(MUXPins[i], LOW);
  }
  for(int i=0;i<ampMUXPinNum;i++)
  {
    pinMode(ampMUXPins[i], OUTPUT);
    digitalWrite(ampMUXPins[i], LOW);
  }
  for(int i=0;i<I2CMUXPinNum;i++)
  {
    pinMode(I2CMUXPins[i], OUTPUT);
    digitalWrite(I2CMUXPins[i], LOW);
  }
  
  //Set the chosen analog ports as inputs
  for(int i=0;i<numAnalogPorts;i++)
  {
    pinMode(analogPorts[i], INPUT);
  }

  // Wait until MATLAB code startup is complete and servos are on, then calibrate each of the strain gauges
  Serial.println("Waiting on MATLAB...");
  int waiting = 1;
  while(waiting == 1)
  {
    if(Serial.available()>0) //Check if anything has been sent by MATLAB over Serial
    {
      value = Serial.read(); //If there has been, read it
    }
    if(value>0) //If that value is something other than zero, then we have the go-ahead to start calibration from the MATLAB script
    {
      waiting = 0; //Stop waiting
      Serial.println("Starting calibration.");
    }
  }
  
  // CALIBRATION CODE
  for(int L=0;L<numAnalogPorts;L++) //FOR each each leg...
  {
    digitalWrite(I2CMUXPins[L], HIGH); // Set the appropriate pin for the I2C analog switches HIGH to direct the I2C channels to your desired leg
    for(int i=0;i<ampMUXPinNum;i++) //FOR each possible state of the switches on the amplifier board (corresponding to either "side" of the board)...
    {
      wiperVal = 50; //Begin the wiper at its midpoint
      digitalWrite(ampMUXPins[i], HIGH); //Set the corresponding pin of the switch HIGH to flip the switch and direct the I2C channels to the desired "side" of the board
      for(int j=0;j<numDigiPotIDs;j++) //FOR each of the unique digipot IDs...
      {
        int currMUXID = j+(3*i); //Calculate which strain signal ID (from 0 to 6 as notated on the amplifier boards) we're on currently based on the "side" of the board and the digipot ID
        digitalWrite(MUXPins[currMUXID], HIGH); //Set the MUX pin HIGH to connect the strain signal to that leg's dedicated analog port
        allSGValues[L][currMUXID] = analogRead(analogPorts[L]); //Collect the strain signal for that leg in the strain gage data array
        while(allSGValues[L][currMUXID] < targetStrainBaseline - strainBaselineErrorMargin || allSGValues[L][currMUXID] > targetStrainBaseline + strainBaselineErrorMargin) //WHILE the strain value isn't within our desired calibration range...
        {
          Wire.beginTransmission(digiPotIDs[j]); //Prepare to send a new wiper value to the digipot
          Wire.write(byte(0x00));
          if(allSGValues[L][currMUXID] < targetStrainBaseline - strainBaselineErrorMargin) //IF the strain value is less than the desired level, increment the wiper value
          {
            wiperVal++;
          }
          else if(allSGValues[L][currMUXID] > targetStrainBaseline + strainBaselineErrorMargin) //ELSE decrement the wiper value to lower the strain 
          {
            wiperVal--;
          }
          Wire.write(wiperVal); //Finish sending the new wiper value to the digipot
          Wire.endTransmission();
          allSGValues[L][currMUXID] = analogRead(analogPorts[L]); //Retake the strain data to see the effect of the wiper change. Repeat until you hit the desired calibration range
        }
        // Set all of the pins you set HIGH back to LOW to start the loops again
        digitalWrite(MUXPins[currMUXID], LOW);
      }
      digitalWrite(ampMUXPins[i], LOW);
    }
    digitalWrite(I2CMUXPins[L], LOW);
    Serial.print("Leg ");
    Serial.print(L);
    Serial.println(" complete.");
  }
  Serial.println("Done."); //Let MATLAB know you're done calibrating
}

void loop() {
  //Only read values and broadcast them when MATLAB asks for it by sending a value ~=0 over Serial
  if(Serial.available()>0)
  {
    value = Serial.read();
  }
  if(value>0) 
  {
   //If the MATLAB script wants Serial data, cycle through which digital pin is HIGH and collect the strain data from each leg for each state of the switches. By doing so, you can collect strain from each location on the leg for all legs simultaneously, then switch
   //which location you're collecting from.
   for(int i=0;i<MUXPinNum;i++)
  {
    digitalWrite(MUXPins[i], HIGH);
    for(int L=0;L<numAnalogPorts;L++)
    {
      allSGValues[L][i] = analogRead(analogPorts[L]);
    }
    digitalWrite(MUXPins[i], LOW);
  }
  //Print all of the collected data as one big line in Serial with a space between each point of data. The MATLAB script can then parse this data accordingly for each leg and location.
  for(int L=0;L<numAnalogPorts;L++)
  {
    for(int i=0;i<MUXPinNum;i++)
    {
      Serial.print(allSGValues[L][i]);
      Serial.print(" ");
    }
  }
  //Print the carriage return to tell the MATLAB script that this is one complete timestep of data
  Serial.write(13);
  Serial.write(10);
  //Delay for 2ms then start the read/print cycle again
  delay(2); 
  }
}
