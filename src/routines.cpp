#include "routines.hpp"

int selection = 0;

std::vector<AutonomousRoutine> routines = {

AutonomousRoutine(
  "Disable",
  &screen::resources::whiteText,
  [](){},
  [](screen::ttField *field) { field->finishDrawing(); }
)

};
