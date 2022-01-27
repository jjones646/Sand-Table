//
// Rotary encoder controller for sand table plotting mechanism
//

#include <Arduino.h>
#include <BasicEncoder.h>
#include <TimerOne.h>
#include <Stepper.h>
#include <A4988.h>

const uint8_t ENCODER_A_SIGNAL_PIN_1 = 2;
const uint8_t ENCODER_A_SIGNAL_PIN_2 = 3;
const uint8_t ENCODER_B_SIGNAL_PIN_1 = 18;
const uint8_t ENCODER_B_SIGNAL_PIN_2 = 19;

const uint8_t STEPPER_A_ENABLE_PIN = 38;
const uint8_t STEPPER_A_STEP_PIN = 54;
const uint8_t STEPPER_A_DIR_PIN = 55;

const uint8_t STEPPER_B_ENABLE_PIN = 56;
const uint8_t STEPPER_B_STEP_PIN = 60;
const uint8_t STEPPER_B_DIR_PIN = 61;

const double stepperStepAngle = 1.8;
const uint16_t stepperStepsPerRev = 360/stepperStepAngle;
const uint8_t stepperMicrosteps = 16;

const unsigned int stepperRpm = 200;


BasicEncoder encoderA(ENCODER_A_SIGNAL_PIN_1, ENCODER_A_SIGNAL_PIN_2);
BasicEncoder encoderB(ENCODER_B_SIGNAL_PIN_1, ENCODER_B_SIGNAL_PIN_2);

BasicStepperDriver stepperA(stepperStepsPerRev, STEPPER_A_DIR_PIN, STEPPER_A_STEP_PIN, STEPPER_A_ENABLE_PIN);
BasicStepperDriver stepperB(stepperStepsPerRev, STEPPER_B_DIR_PIN, STEPPER_B_STEP_PIN, STEPPER_B_ENABLE_PIN);


void timerService()
{
  encoderA.service();
  encoderB.service();
}

void initStepper(BasicStepperDriver& stepper)
{
  stepper.begin(stepperRpm, stepperMicrosteps);
  stepper.setEnableActiveState(LOW);
  stepper.enable();
}

void refreshStepperInputs(BasicStepperDriver& stepper, long steps)
{
  stepper.enable();
  stepper.stop();
  stepper.startMove(steps);
}

bool updateStepperOutputs(BasicStepperDriver& stepper)
{
  unsigned long waitTimeUs = stepper.nextAction();

  if (waitTimeUs <= 0)
  {
    return false;
  }
  
  return true;
}

void setup()
{
  Serial.begin(115200);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerService);

  initStepper(stepperA);
  initStepper(stepperB);
}

void loop()
{
  const int stepMultiplier = -1000;
  
  int encoderAChange = encoderA.get_change();
  int encoderBChange = encoderB.get_change();

  if (encoderAChange || encoderBChange)
  {
    refreshStepperInputs(stepperA, encoderAChange * stepMultiplier);
    refreshStepperInputs(stepperB, encoderBChange * stepMultiplier);
  }

  bool stepperARunning = updateStepperOutputs(stepperA);
  bool stepperBRunning = updateStepperOutputs(stepperB);
}
