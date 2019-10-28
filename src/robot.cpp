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



std::atomic_int Angler::restingPos = 150;
std::atomic_int Angler::pidThreshold = 1800;
std::atomic_int Angler::verticalPos = 4500;

void initialize() {
  okapi::Logger::setDefaultLogger(std::make_shared<okapi::Logger>(std::make_unique<okapi::Timer>(), "/ser/sout", okapi::Logger::LogLevel::debug));

  robot::angler = std::make_shared<Angler>(std::make_shared<okapi::Motor>(-1),
    okapi::IterativePosPIDController::Gains({0.00075, 0, 0.00005, 0}));
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
                      .withLimits({0.5, 2, 10})
                      .buildMotionProfileController();


  robot::intake->setBrakeMode(okapi::AbstractMotor::brakeMode::brake);

  robot::angler->getMotor()->setBrakeMode(okapi::AbstractMotor::brakeMode::brake);
  robot::angler->getMotor()->setGearing(okapi::AbstractMotor::gearset::red);

  robot::angler->startThread();
  robot::screen::controller = new pros::Task(screenControllerFN, NULL, "Screen");
  robot::screen::notification =
  "Somebody once told me the world is gonna roll me I ain t the sharpest tool in the shed She was looking kind of dumb with her finger and her thumb In the shape of an L on her forehead Well the years start coming and they don t stop coming Fed to the rules and I hit the ground running Didn t make sense not to live for fun Your brain gets smart but your head gets dumb So much to do, so much to see So what s wrong with taking the back streets? You ll never know if you don t go You ll never shine if you don t glow Hey now, you re an all-star, get your game on, go play Hey now, you re a rock star, get the show on, get paid And all that glitters is gold Only shooting stars break the mold It s a cool place and they say it gets colder You re bundled up now, wait till you get older But the meteor men beg to differ Judging by the hole in the satellite picture The ice we skate is getting pretty thin The water s getting warm so you might as well swim My world s on fire, how about yours? That s the way I like it and I never get bored Hey now, you re an all-star, get your game on, go play Hey now, you re a rock star, get the show on, get paid All that glitters is gold Only shooting stars break the mold";
}
