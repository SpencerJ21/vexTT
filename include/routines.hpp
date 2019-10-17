#ifndef ROUTINES_HPP_
#define ROUTINES_HPP_
#include "screen/api.hpp"
#include <functional>
#include <string>
#include <vector>

class AutonomousRoutine {
public:
  const char* title;
  lv_style_t* textStyle;

  std::function<void()> run;
  std::function<void(screen::ttField*)> print;

  AutonomousRoutine(const char* ititle, lv_style_t* istyle,
                    std::function<void()> irun,
                    std::function<void(screen::ttField*)> iprint):
                    title(ititle), textStyle(istyle),
                    run(irun), print(iprint)
  {}
};

extern std::vector<AutonomousRoutine> routines;
extern int selection;

#endif
