#include "routines.hpp"

int selection = 0;

std::vector<AutonomousRoutine> routines = {

AutonomousRoutine(
  "Disable",
  &lv_style_plain,
  [](){},
  [](screen::ttField *field) { field->finishDrawing(); }
)

};
