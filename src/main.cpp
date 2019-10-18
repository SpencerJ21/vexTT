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
  robot::screen::notification =
  " it's a beautiful day outside. birds are singing, flowers are blooming... on days like these, kids like you...   Should be burning in hell.    huh. always wondered why people never use their strongest attack first.    anyway, as i was saying, it's a nice day out. why not relax and take a load off? ready? here we go. what? you think i'm just gonna stand there and take it?    our reports showed a massive anomaly in the timespace continuum. timelines jumping left and right, stopping and starting...    until suddenly, everything ends.    heh heh heh... that's your fault isn't it?    you can't understand how this feels.    knowing that one day, without any warning... it's all going to be reset.    look. i gave up trying to go back a long time ago.    and getting to the surface doesn't really appeal anymore, either.    cause even if we do... we'll just end up right back here, without any memory of it, right?    to be blunt... it makes it kind of hard to give it my all.    ... or is that just a poor excuse for being lazy...? hell if i know.    all i know is... seeing what comes next... i can't afford not to care anymore. ";
}

void disabled() {}

void competition_initialize() {
  robot::screen::state = screenMode::selection;
}

void autonomous() {
  if(!robot::chassis) initChassis();
  routines[selection].run();
}

void opcontrol() {
  robot::screen::state = screenMode::notification;

  if(!robot::chassis) initChassis();

  while(true){
    if(robot::controller.getDigital(okapi::ControllerDigital::A)){
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
