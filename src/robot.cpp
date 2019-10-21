#include "robot.hpp"

namespace robot {

okapi::Controller controller;

std::shared_ptr<okapi::Motor> angler;
std::shared_ptr<okapi::MotorGroup> intake;
std::shared_ptr<okapi::MotorGroup> lDrive;
std::shared_ptr<okapi::MotorGroup> rDrive;

std::shared_ptr<okapi::ADIEncoder> lEnc;
std::shared_ptr<okapi::ADIEncoder> rEnc;
std::shared_ptr<okapi::ADIEncoder> mEnc;

std::shared_ptr<okapi::OdomChassisController> chassis;
std::shared_ptr<okapi::AsyncMotionProfileController> chassisMPC;

namespace screen {
  pros::Task *controller;
  volatile screenMode state;
  std::string notification;
}

}
