#include "robot.hpp"

using namespace okapi::literals;

namespace robot {

okapi::Controller controller;

std::shared_ptr<Angler> angler;
std::shared_ptr<okapi::MotorGroup> intake;
std::shared_ptr<okapi::MotorGroup> lDrive;
std::shared_ptr<okapi::MotorGroup> rDrive;

std::shared_ptr<okapi::ADIEncoder> lEnc;
std::shared_ptr<okapi::ADIEncoder> rEnc;
std::shared_ptr<okapi::ADIEncoder> mEnc;

std::shared_ptr<okapi::ChassisController> chassis;
std::shared_ptr<okapi::AsyncMotionProfileController> chassisProfiler;

namespace screen {
  pros::Task *controller;
  volatile screenMode state;
  std::string notification;
}

}

extern void screenControllerFN(void* param);

std::atomic_int Angler::restingPos = 200;
std::atomic_int Angler::primedPos = 1400;
std::atomic_int Angler::pidThreshold = 1600;
std::atomic_int Angler::verticalPos = 4600;


void initialize() {
  okapi::Logger::setDefaultLogger(std::make_shared<okapi::Logger>(std::make_unique<okapi::Timer>(), "/ser/sout", okapi::Logger::LogLevel::debug));

  robot::angler = std::make_shared<Angler>(
    std::make_shared<okapi::MotorGroup>(okapi::MotorGroup({-1, 7})),
    okapi::IterativePosPIDController::Gains({0.00065, 0, 0.00005, 0}));
    
  robot::intake = std::make_shared<okapi::MotorGroup>(okapi::MotorGroup({-2,10}));
  robot::lDrive = std::make_shared<okapi::MotorGroup>(okapi::MotorGroup({ 8, 9}));
  robot::rDrive = std::make_shared<okapi::MotorGroup>(okapi::MotorGroup({-3,-4}));

  robot::lEnc = std::make_shared<okapi::ADIEncoder>(7,8, true);
  robot::rEnc = std::make_shared<okapi::ADIEncoder>(1,2);
  robot::mEnc = std::make_shared<okapi::ADIEncoder>(5,6);

  robot::chassis = okapi::ChassisControllerBuilder()
                      .withMotors(robot::lDrive, robot::rDrive)
                      .withDimensions(okapi::ChassisScales({{4.125_in, 10_in}, okapi::imev5GreenTPR}))
                      .build();

  robot::chassisProfiler = okapi::AsyncMotionProfileControllerBuilder()
                      .withOutput(robot::chassis)
                      .withLimits({1, 2, 10})
                      .buildMotionProfileController();


  robot::intake->setBrakeMode(okapi::AbstractMotor::brakeMode::brake);

  robot::angler->getMotor()->setBrakeMode(okapi::AbstractMotor::brakeMode::brake);
  robot::angler->getMotor()->setGearing(okapi::AbstractMotor::gearset::red);

  robot::angler->startThread();
  robot::screen::controller = new pros::Task(screenControllerFN, NULL, "Screen");
  robot::screen::notification = "You wouldn't get it";
}
