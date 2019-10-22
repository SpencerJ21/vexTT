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
    robot::chassisMPC->loadPath("tt/nearCubeApproach", "nearCubeApproach");
    robot::chassisMPC->loadPath("tt/nearCubeRecede", "nearCubeRecede");
    robot::chassisMPC->loadPath("tt/nearZoneApproach", "nearZoneApproach");

    robot::intake->controllerSet(1);
    robot::chassisMPC->setTarget("nearCubeApproach");
    robot::chassisMPC->waitUntilSettled();

    robot::intake->controllerSet(0);
    robot::chassisMPC->setTarget("nearCubeRecede", true);
    robot::chassisMPC->waitUntilSettled();

    robot::chassisMPC->setTarget("nearZoneApproach");
    robot::chassisMPC->waitUntilSettled();

    robot::angler->moveAbsolute(910, 30);
  }
)

};
