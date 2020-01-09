#pragma once
#include "main.h"
#include "mechanism/angler.hpp"
#include "mechanism/lift.hpp"

enum class screenMode{
  disabled, notification, selection, ez, NUMPANELS
};

namespace robot {
extern okapi::Controller controller;

extern std::shared_ptr<Lift> lift;
extern std::shared_ptr<Angler> angler;
extern std::shared_ptr<okapi::MotorGroup> intake;
extern std::shared_ptr<okapi::MotorGroup> lDrive;
extern std::shared_ptr<okapi::MotorGroup> rDrive;

extern std::shared_ptr<okapi::ChassisController> chassis;
extern std::shared_ptr<okapi::AsyncMotionProfileController> chassisProfiler;

namespace screen {
  extern pros::Task *controller;
  extern volatile screenMode state;
  extern std::string notification;
}
}
