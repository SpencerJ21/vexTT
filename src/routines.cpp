#include "routines.hpp"
#include "robot.hpp"

int selection = 0;

std::vector<AutonomousRoutine> routines = {

AutonomousRoutine(
  "Disable",
  &lv_style_plain,
  [](screen::ttField *field) { field->finishDrawing(); },
  [](){},
  [](){}
),

AutonomousRoutine(
  "Small Zone Red",
  &screen::resources::redAlliance,
  [](screen::ttField *field) {
    field->draw(screen::cubeGroup::left4, 0);
    field->draw(screen::scoringZone::nearRed, screen::color::orange, 5);
    field->finishDrawing();
  },
  [](){},
  [](){
    robot::chassisProfiler->loadPath("tt/nearCubeApproach", "nearCubeApproach");
    robot::chassisProfiler->loadPath("tt/nearCubeRecede", "nearCubeRecede");
    robot::chassisProfiler->loadPath("tt/nearZoneApproach", "nearZoneApproach");

    robot::intake->controllerSet(1);
    robot::chassisProfiler->setTarget("nearCubeApproach");
    robot::chassisProfiler->waitUntilSettled();

    robot::intake->controllerSet(0);
    robot::chassisProfiler->setTarget("nearCubeRecede", true);
    robot::chassisProfiler->waitUntilSettled();

    robot::chassisProfiler->setTarget("nearZoneApproach");
    robot::chassisProfiler->waitUntilSettled();

    robot::angler->moveAbsolute(910, 30);
  }
)

};
