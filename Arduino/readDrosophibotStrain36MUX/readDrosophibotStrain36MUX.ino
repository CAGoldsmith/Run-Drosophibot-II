#include <Wire.h>
float targetStrainBaseline = 300;
float strainBaselineErrorMargin = 50;
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
}

void loop() {
  //Only read values and broadcast them when MATLAB asks for it by sending a value ~=0 over Serial
    value = 1;
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
  //for(int L=0;L<numAnalogPorts;L++)
  //{
    for(int i=0;i<MUXPinNum;i++)
    {
      Serial.print(allSGValues[0][i]);
      Serial.print(" ");
    }
  //}
  //Print the carriage return to tell the MATLAB script that this is one complete timestep of data
  Serial.println(" ");
  delay(10); 
  }
}
