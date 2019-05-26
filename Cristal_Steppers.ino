#include <EEPROM.h>
#include <Wire.h>
#include <AccelStepper.h>

#define MAX_LENGTH 12
int SL_ADDR;

byte command[MAX_LENGTH];
byte command_length;
boolean processingMessage = false;
boolean commandComplete = false;

const long MOTOR_STEPS = 200;
const long MICROSTEPS = 16;

const int stepPins[5] = {54, 60, 46, 26, 36};
const int dirPins[5] = {55, 61, 48, 28, 34};
const int enPins[5] = {38, 56, 62, 24, 30};

AccelStepper  stepper1(AccelStepper::DRIVER,  stepPins[0],  dirPins[0]);
AccelStepper  stepper2(AccelStepper::DRIVER,  stepPins[1],  dirPins[1]);
AccelStepper  stepper3(AccelStepper::DRIVER,  stepPins[2],  dirPins[2]);
AccelStepper  stepper4(AccelStepper::DRIVER,  stepPins[3],  dirPins[3]);
AccelStepper  stepper5(AccelStepper::DRIVER,  stepPins[4],  dirPins[4]);

AccelStepper* steppers[] = {
  &stepper1,
  &stepper2,
  &stepper3,
  &stepper4,
  &stepper5,
};

long MAX_SPEED = 500 * MICROSTEPS;
long ACCEL = 300 * MICROSTEPS;
long NSTEPS = 500 * MICROSTEPS;
int NMOT = 5;

void setup()
{
  SL_ADDR = EEPROM.read(0);
  TWAR = (SL_ADDR << 1) | 1;  // enable broadcasts to be received

  Wire.begin(SL_ADDR);
  Wire.onReceive(receiveEvent); // Registra evento de recibir comando
  Wire.onRequest(requestEvent); // Registra evento de request de posicion
  for (int n = 0; n < NMOT; n++) {
    steppers[n]->setMaxSpeed(MAX_SPEED);
    steppers[n]->setAcceleration(ACCEL);
    steppers[n]->setEnablePin(enPins[n]);
    steppers[n]->setPinsInverted(false, false, true);
  }
  command_length = 0;
}

void loop() {
  if (commandComplete) {
    commandComplete = false;
    processCommand();
  }
  for (int i = 0; i < 5; i++) {
    if (steppers[i]->distanceToGo() == 0) steppers[i]->disableOutputs();
    else steppers[i]->run();
  }
}

void receiveEvent(int numBytes)
{
  if (processingMessage) {
    unsigned long t = millis();
    while (processingMessage) {
      if (millis() > t + 3000) break;
    }
  }
  processingMessage = true;
  byte tmpcommand[MAX_LENGTH];  
  for (byte n = 0; n < numBytes; n++) {
    if (n < MAX_LENGTH)
    {
      tmpcommand[n] = Wire.read();
      command_length++;
    }
    else {
      Wire.read();
    }
  }
  memcpy(command,tmpcommand,MAX_LENGTH);
  commandComplete = true;
  processingMessage = false;
}

//Devuelve un array de 20 bytes con con las posiciones y los target de todos los motores
//Formato: Motor 0 Position HSB, Motor 0 Position LSB, Motor 0 targer HSB, Motor 0 target LSB, Motor 1 pos HSB...
void requestEvent() {
  noInterrupts();
  byte reply[20];
  for (int i = 0; i < 5; i++) {
    int pos = steppers[i]->currentPosition();
    int distance = steppers[i]->distanceToGo();
    reply[4 * i] = highByte(pos);
    reply[4 * i + 2] = lowByte(pos);
    reply[4 * i + 3] = highByte(distance);
    reply[4 * i + 4] = lowByte(distance);
  }
  interrupts();
}

//Procesa el comando
void processCommand() {
  //registro (opcional)
  byte p_command[MAX_LENGTH];
  memcpy(p_command, command, MAX_LENGTH); 
  //byte REG = p_command[0];
  char CMD = p_command[1];
  unsigned char nmot = p_command[2];
  int value = (p_command[3] << 8) | (p_command[4]);
  switch (CMD) {
    case 's': //Move to target un motor
      steppers[nmot]->enableOutputs();
      steppers[nmot]->moveTo(value * MICROSTEPS);
      break;
    case 'z': //Set Zero un motor
      steppers[nmot]->enableOutputs();
      steppers[nmot]->setCurrentPosition(0);
      break;
    case 'p': //Stop  un motor
      steppers[nmot]->enableOutputs();
      steppers[nmot]->stop();
      break;
    case 'v': //Set max speed  un motor
      steppers[nmot]->enableOutputs();
      steppers[nmot]->setMaxSpeed(value * MICROSTEPS);
      break;
    case 'a': //Set accel un motor
      steppers[nmot]->enableOutputs();
      steppers[nmot]->setAcceleration(value * MICROSTEPS);
      break;
    case 'S': //Move to target todos los motores
      for (int i = 0; i < 5; i++) {
        steppers[i]->enableOutputs();
        steppers[i]->moveTo(value * MICROSTEPS);
      }
      break;
    case 'Z': //Set Pos todos los motores
      for (int i = 0; i < 5; i++) {
        steppers[i]->enableOutputs();
        steppers[i]->setCurrentPosition(0);
      }
      break;
    case 'P': //Stop todos los motores
      for (int i = 0; i < 5; i++) {
        steppers[i]->enableOutputs();
        steppers[i]->stop();
      }
      break;
    case 'V': //Set max speed  un motor
      for (int i = 0; i < 5; i++) {
        steppers[i]->enableOutputs();
        steppers[i]->setMaxSpeed(value * MICROSTEPS);
      }
      break;
    case 'A': //Set accel un motor
      for (int i = 0; i < 5; i++) {
        steppers[i]->enableOutputs();
        steppers[i]->setAcceleration(value * MICROSTEPS);
      }
      break;
    default:
      break;
  }
  command_length = 0;
}
