#include "angler.hpp"

Angler::Angler(std::shared_ptr<okapi::AbstractMotor> imotor, okapi::IterativePosPIDController::Gains igains, std::unique_ptr<okapi::Filter> iderivativeFilter):
  motor(imotor), controller(std::make_unique<okapi::IterativePosPIDController>(igains, okapi::TimeUtilFactory::createDefault(), std::move(iderivativeFilter))){

  target.store(0);
  tare();
  controller->setOutputLimits(0.4, -0.4);
}

Angler::~Angler(){
  thread->remove();
  delete thread;
}

void Angler::stack(){
  target.store(verticalPos);
  controller->setTarget(target);
}

void Angler::prime(){
  target.store(primedPos);
  controller->setTarget(target);
}

void Angler::reset(){
  target.store(restingPos);
  controller->setTarget(target);
}

bool Angler::isSettled(){
  auto currentPos = motor->getPosition();

  if(target <= restingPos){
    return currentPos < restingPos;

  }else{
    return currentPos > pidThreshold && controller->isSettled();
  }
}

void Angler::waitUntilSettled(){
  while(!isSettled()){
    pros::delay(50);
  }
}

void Angler::trim(int trimDistance){
  restingPos   += trimDistance;
  primedPos    += trimDistance;
  pidThreshold += trimDistance;
  verticalPos  += trimDistance;
}

void Angler::tare(){
  motor->tarePosition();
}

std::shared_ptr<okapi::AbstractMotor> Angler::getMotor(){
  return motor;
}

void Angler::startThread(){
  thread = new pros::Task(Angler::trampoline, this, "Angler");
}

pros::Task *Angler::getTask(){
  return thread;
}

void Angler::trampoline(void *angler){
  static_cast<Angler*>(angler)->loop();
}

void Angler::loop(){
  int currentPos = 0;
  double power = 0;

  while(true){
    currentPos = motor->getPosition();

    if(target == verticalPos && currentPos <= pidThreshold){
      power = 1;
    }else{
      power = controller->step(currentPos);
    }

    motor->controllerSet(power);

    pros::delay(10);
  }
}
