#include "angler.hpp"

Angler::Angler(std::shared_ptr<okapi::AbstractMotor> imotor, okapi::IterativePosPIDController::Gains igains, std::unique_ptr<okapi::Filter> iderivativeFilter):
  motor(imotor), controller(std::make_unique<okapi::IterativePosPIDController>(igains, okapi::TimeUtilFactory::create(), std::move(iderivativeFilter))){

  target.store(0);
  tare();
  controller->setOutputLimits(0.3, 0);
}

Angler::~Angler(){
  thread->remove();
  delete thread;
}

void Angler::stack(){
  target.store(verticalPos);
}

void Angler::reset(){
  target.store(restingPos);
}

void Angler::trim(int trimDistance){
  restingPos   -= trimDistance;
  pidThreshold -= trimDistance;
  verticalPos  -= trimDistance;
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
  int lastTarget = target;
  double power = 0;

  controller->setTarget(verticalPos);

  while(true){
    currentPos = motor->getPosition();

    //if(target != lastTarget){
    //  controller->reset();
    //}
    lastTarget = target;

    if(target <= restingPos){
      if(currentPos < restingPos){
        power = 0;
      }else{
        power = -1;
      }

    }else{
      if(currentPos < pidThreshold){
        power = 1;
      }else{
        power = controller->step(currentPos);
      }
    }

    motor->controllerSet(power);

    pros::delay(10);
  }
}
