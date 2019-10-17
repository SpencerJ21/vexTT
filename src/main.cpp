#include "main.h"
#include "robot.hpp"
#include "routines.hpp"

using namespace okapi::literals;

extern void screenControllerFN(void* param);

inline void initChassis(){
  robot::chassis = okapi::ChassisControllerBuilder()
                      .withSensors(robot::lEnc, robot::rEnc, robot::mEnc)
                      .withOdometry()
                      .withGearset(okapi::AbstractMotor::gearset::green)
                      .withDimensions(okapi::ChassisScales({2.75_in, 4.625_in, 3.125_in, 2.75_in}, 360))
                      .withMotors(robot::lDrive, robot::rDrive)
                      .buildOdometry();
}

void initialize() {
  okapi::Logger::setDefaultLogger(std::make_shared<okapi::Logger>(std::make_unique<okapi::Timer>(), "/usd/tt/log.txt", okapi::Logger::LogLevel::debug));

  robot::angler = std::make_shared<okapi::Motor>(-1);
  robot::intake = std::make_shared<okapi::MotorGroup>(okapi::MotorGroup({-2,10}));
  robot::lDrive = std::make_shared<okapi::MotorGroup>(okapi::MotorGroup({ 8, 9}));
  robot::rDrive = std::make_shared<okapi::MotorGroup>(okapi::MotorGroup({-3,-4}));

  robot::lEnc = std::make_shared<okapi::ADIEncoder>(7,8, true);
  robot::rEnc = std::make_shared<okapi::ADIEncoder>(1,2);
  robot::mEnc = std::make_shared<okapi::ADIEncoder>(5,6);

  robot::intake->setBrakeMode(okapi::AbstractMotor::brakeMode::brake);

  robot::angler->setBrakeMode(okapi::AbstractMotor::brakeMode::brake);
  robot::angler->setGearing(okapi::AbstractMotor::gearset::red);

  robot::screen::controller = new pros::Task(screenControllerFN, NULL, "Screen");
  robot::screen::state = screenMode::notification;
}

void disabled() {}

void competition_initialize() {
  robot::screen::state = screenMode::selection;
}

void autonomous() {
  if(!robot::chassis) initChassis();
  routines.at(selection).run();
}

void opcontrol() {
  if(!robot::chassis) initChassis();

  while(true){
    if(robot::controller.getDigital(okapi::ControllerDigital::L2)){
      robot::chassis->getModel()->tank(robot::controller.getAnalog(okapi::ControllerAnalog::leftY) * 0.5,
                                       robot::controller.getAnalog(okapi::ControllerAnalog::leftY) * 0.5);
    }else{
      robot::chassis->getModel()->tank(robot::controller.getAnalog(okapi::ControllerAnalog::leftY),
                                       robot::controller.getAnalog(okapi::ControllerAnalog::rightY));
    }

    if(robot::controller.getDigital(okapi::ControllerDigital::R1)){
      robot::intake->moveVoltage(12000);
    }else if(robot::controller.getDigital(okapi::ControllerDigital::R2)){
      robot::intake->moveVoltage(6000);
    }else if(robot::controller.getDigital(okapi::ControllerDigital::Y)){
      robot::intake->moveVoltage(-6000);
    }else{
      robot::intake->moveVoltage(0);
    }

    if(robot::controller.getDigital(okapi::ControllerDigital::right)){
      robot::angler->moveVelocity(40);
    }else if(robot::controller.getDigital(okapi::ControllerDigital::L1)){
      robot::angler->moveVelocity(-100);
    }else{
      robot::angler->moveVelocity(0);
    }

    auto state = robot::chassis->getState();

    std::cout << robot::lEnc->get() << " " << robot::mEnc->get() << " " << robot::rEnc->get() << " | " << state.x.convert(okapi::inch) << " " << state.y.convert(okapi::inch) << " " << state.theta.convert(okapi::degree) << "\n";

    pros::delay(10);
  }
}
