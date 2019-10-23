#pragma once
#include "main.h"

enum class screenMode{
  disabled, notification, selection
};

namespace robot {

extern okapi::Controller controller;

extern std::shared_ptr<okapi::Motor> angler;
extern std::shared_ptr<okapi::MotorGroup> intake;
extern std::shared_ptr<okapi::MotorGroup> lDrive;
extern std::shared_ptr<okapi::MotorGroup> rDrive;

extern std::shared_ptr<okapi::ADIEncoder> lEnc;
extern std::shared_ptr<okapi::ADIEncoder> rEnc;
extern std::shared_ptr<okapi::ADIEncoder> mEnc;

extern std::shared_ptr<okapi::ThreeEncoderSkidSteerModel> chassisModel;
extern std::shared_ptr<okapi::OdomChassisController> chassisController;
extern std::shared_ptr<okapi::AsyncMotionProfileController> chassisProfiler;

namespace screen{
  extern pros::Task *controller;
  extern volatile screenMode state;
  extern std::string notification;
};

}
