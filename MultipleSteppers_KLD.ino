// MultiStepper.pde
// -*- mode: C++ -*-
//
// Shows how to multiple simultaneous steppers
// Runs one stepper forwards and backwards, accelerating and decelerating
// at the limits. Runs other steppers at the same time
//
// Copyright (C) 2009 Mike McCauley
// $Id: MultiStepper.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $


//Import the library required
#include <AccelStepper.h>
#include <Wire.h>


//Slave Address for the I2C Communication
#define SLAVE_ADDRESS 0x14
// i2c baudrate
#define BAUD 9600 



// Define stepper motor connections
#define dirPin_1 4
#define stepPin_1 7
#define dirPin_2 3
#define stepPin_2 6
#define dirPin_3 2
#define stepPin_3 5

#define motorInterfaceType 1



int serial_in;
byte counter; // compte le nombre de données reçues

// "unsigned int" range of 0 to 65,535
unsigned int _MaxSpeed[3] = {1000, 1000, 1000}; 
unsigned int _Acceleration[3] = {100, 100, 100};
int _direction[3] = {0, 0, 0};
int _MoveTo[3] = {1000, 1000, 1000};
unsigned int selectedMotor;

String mode;

    
#define SpeedPin 12 // toggle full speed / 1:4 speed


// Define some steppers and the pins the will use


// DEPRECATED
// AccelStepper stepper1(AccelStepper(motorInterfaceType, stepPin_1, dirPin_1));
//AccelStepper stepper2(AccelStepper(motorInterfaceType, stepPin_2, dirPin_2));
//AccelStepper stepper3(AccelStepper(motorInterfaceType, stepPin_3, dirPin_3));

AccelStepper stepper[3] = {AccelStepper(motorInterfaceType, stepPin_1, dirPin_1), AccelStepper(motorInterfaceType, stepPin_2, dirPin_2), AccelStepper(motorInterfaceType, stepPin_3, dirPin_3)};

void setup()
{  


    pinMode(SpeedPin, OUTPUT);
    digitalWrite(SpeedPin, HIGH);
      //// i2c config
  
    Wire.begin(SLAVE_ADDRESS);                // join i2c bus with address #8
    Wire.onReceive(receiveData); // register event
    Serial.begin(BAUD);           // start serial for output
    
    set_zero();
}

void loop()
{
    if (mode == "run_steppers"){
      for (int i = 0; i <= 2; i++) {
      stepper[i].run(); }}                                              //run
}


void define_mode(String x){                                                       // initialise le counter et defini le mode
  counter = 0;
  mode = x;  
  Serial.print("In mode ");
  Serial.println(x);
  //Serial.print("counter = ");
  //Serial.println(counter);
}

// callback for received data
void receiveData(int byteCount) { 
  // nomenclature 
  // Z(zero) 
  // M(move) {int int int}
  // A(acceleration) {int int int}
  // S(speed) {int int int}
  // R(run)
  
  while (Wire.available()) {
    serial_in = Wire.read();
    //Serial.print("serial_in-");
    //Serial.println(serial_in);
  }
 // TEST DU PREMIER BYTE
 if (serial_in == 90 or serial_in == 122) {
  define_mode("set_zero");                                                // Z set-zero (stepper)
 }
 
  else if (serial_in == 83 or serial_in == 115) {                               // S speed}
    for (int i = 0; i <= 2; i++) {_MaxSpeed[i] = 0;}                            // raz
    define_mode("MaxSpeed");} 
    
  else if (serial_in == 77 or serial_in == 109) {                               // M move 
    for (int i = 0; i <= 2; i++) {_MoveTo[i] = 0;}                              // raz
    define_mode("MoveTo");} 
    
  else if (serial_in == 65 or serial_in == 97) {                                // A acceleration
    for (int i = 0; i <= 2; i++) {_Acceleration[i] = 0;}                        // raz
    define_mode("Acceleration");}
    
  else if (serial_in == 82 or serial_in == 114) {
    define_mode("run_steppers");                                      // R run steppers
  }    
  
  else if (serial_in == 68 or serial_in == 100) {
    define_mode("change_direction");                                      // D run steppers
  }
    
  else if (serial_in == 32) {
    counter++;                                          // CHANGE CHAQUE ESPACE
  }
  
  
  else {
      // CALCUL DES VARIABLES
      if (mode == "set_zero") set_zero();
      else if (mode == "MaxSpeed") MaxSpeed();
      else if (mode == "MoveTo") MoveTo();
      else if (mode == "Acceleration") Acceleration();
      else if (mode == "change_direction") change_direction();
      else if (mode == "run_steppers") run_steppers();
      }

}

void set_zero() 
{                                                                                   // zero les 3 moteurs
  if (serial_in == 10) {
    for (int i = 0; i <= 2; i++) {
      stepper[i].setCurrentPosition(i);                                               //set
      Serial.print("zero[");
      Serial.print(i);
      Serial.println("]");}}}  
                     
void MaxSpeed() 
{                                                                                   // modifie les vitesses max des moteurs
    if (serial_in == 10){
      for (int i = 0; i <= 2; i++) {stepper[i].setMaxSpeed(_MaxSpeed[i]);                                               //set
      Serial.print("_MaxSpeed[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(_MaxSpeed[i]);}}         
    else{_MaxSpeed[counter - 1] = _MaxSpeed[counter - 1] * 10 + serial_in - 48;}}       //calcul valeur

void MoveTo()  
{                                                                                   // modifie la position cible
    if (serial_in == 10){
      for (int i = 0; i <= 2; i++) {stepper[i].moveTo(_MoveTo[i]);                                               //set
      Serial.print("_MoveTo[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(_MoveTo[i]);}}
    else{_MoveTo[counter - 1] = _MoveTo[counter - 1] * 10 + serial_in - 48;}}         //calcul valeur

void Acceleration()                                                               
{                                                                                   // modifie l'acceleration des moteurs
    if (serial_in == 10){
      for (int i = 0; i <= 2; i++) {stepper[i].setAcceleration(_Acceleration[i]) ;                                               //set
      Serial.print("_Acceleration[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(_Acceleration[i]);}}
    else{_Acceleration[counter - 1] = _Acceleration[counter - 1] * 10 + serial_in - 48;}}             //calcul valeur

void change_direction()                                                               
{                                                                                   // modifie l'acceleration des moteurs
    if (serial_in == 10){
      for (int i = 0; i <= 2; i++) {
        _MoveTo[i] = _MoveTo[i] * _direction[i] ;                                               //set
        stepper[i].moveTo(_MoveTo[i]);
        Serial.print("_MoveTo[") ;
        Serial.print(i) ;
        Serial.print("] = ") ;
        Serial.println(_MoveTo[i]) ;
      }
    }
    else if (serial_in == 48){ _direction[counter-1] = -1; }
    else if (serial_in == 49){ _direction[counter-1] = 1; }
}             //calcul valeur


void run_steppers() // run les 3 moteurs
{
  if (serial_in == 10) {
      for (int i = 0; i <= 2; i++) {
      Serial.print("run[");
      Serial.print(i);
      Serial.println("]");}}}                                          
