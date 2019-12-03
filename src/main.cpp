#include "main.h"
#include "robot.hpp"
#include "routines.hpp"

using namespace okapi::literals;

extern void generatePaths();

void disabled() {}

void competition_initialize() {
  /* this funciton runs immediatly after initialization when plugged into a
   * competetion / tournament switch
   *
   * setting this will bring the thread defined in screenController.cpp to
   * autonomous selection mode
  **/
  robot::screen::state = screenMode::selection;
}

void autonomous() {
  /* this function is run when directed by a competition / tournament switch
   * this will follow the function pointer "run" to the selected
   * AutonomousRoutines object. All of these routines are defined in routines.cpp
  **/

  routines[selection].run();
}

void opcontrol() {
  /* this function runs when directed by a competition / tournament switch
   * or when not connected to either
  **/
  
  // ensure there is no velocity cap on the chassis
  robot::chassis->getModel()->setMaxVelocity(200);
  
  // set the screen to display notification (random text for the time being)
  robot::screen::state = screenMode::notification;

  // disable the profile controller, as the driver will control it manually
  if (robot::chassisProfiler) robot::chassisProfiler->flipDisable(true);

  // in order to read rising/falling cases, button objects are needed
  okapi::ControllerButton buttonDown = robot::controller[okapi::ControllerDigital::down];


  while(true){

    // If A is pressed, lock control to forwards/backwards at 40% speed, otherwise do standard tank control
    if(robot::controller.getDigital(okapi::ControllerDigital::A)){
      robot::chassis->getModel()->arcade(robot::controller.getAnalog(okapi::ControllerAnalog::leftY) * 0.4, 0);
    }else{
      robot::chassis->getModel()->tank(robot::controller.getAnalog(okapi::ControllerAnalog::leftY),
                                       robot::controller.getAnalog(okapi::ControllerAnalog::rightY));
    }

    /*
     * Intake control
     * R1 -> full power intake
     * R2 -> half power intake
     * R3/Y -> half power outtake
    **/
    if(robot::controller.getDigital(okapi::ControllerDigital::R1)){
      robot::intake->moveVoltage(12000);

    }else if(robot::controller.getDigital(okapi::ControllerDigital::R2)){
      robot::intake->moveVoltage(6000);

    }else if(robot::controller.getDigital(okapi::ControllerDigital::Y)){
      robot::intake->moveVoltage(-6000);
      
    }else{
      robot::intake->moveVoltage(0);
    }

    /*
     * Angler/Tray control (async)
     * L1 -> prime tray (a little over halfway up)
     * L2 -> reset/down tray position
     * L3/right -> stack/up tray position
     * down -> while held, stop task and retract, when released return to normal operation
    **/ 
    if(robot::controller.getDigital(okapi::ControllerDigital::right)){
      robot::angler->stack();

    }else if(robot::controller.getDigital(okapi::ControllerDigital::L1)){
      robot::angler->prime();

    }else if(robot::controller.getDigital(okapi::ControllerDigital::L2)){
      robot::angler->reset();

    }else if(buttonDown.changed()){
      if(buttonDown.isPressed()){
        robot::angler->getTask()->suspend();
        robot::angler->getMotor()->moveVoltage(-6000);

      }else{
        robot::angler->getTask()->resume();
        robot::angler->tare();
      }
    }

    if(robot::controller.getDigital(okapi::ControllerDigital::X) && robot::controller.getDigital(okapi::ControllerDigital::B)){
      generatePaths();
    }

    pros::delay(10);
  }
}
