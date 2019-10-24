#include "main.h"
#include "robot.hpp"
#include "routines.hpp"
#include <vector>
#include <string.h>
#include <iostream>

//extern const lv_img_t logo;

void screenControllerFN(void* param){
  auto logger = okapi::Logger::getDefaultLogger();

  screen::resources::initialize();

  lv_obj_t *scr = lv_obj_create(NULL, NULL);
  lv_scr_load(scr);

  const int number_of_routines = routines.size();

  screenMode lastScreenState = screenMode::disabled;

  //object pointers, may or may not be valid at any given time, depending on if they are in use

  //notification
  lv_obj_t* notification_label;

  //selection
  lv_obj_t* main_list;
  screen::ttField *field = nullptr;
  std::vector<lv_obj_t*> routine_buttons;
  std::vector<lv_obj_t*> routine_labels;
  lv_obj_t* confirm_button;
  bool selected;

  //diagnostic

  LOG_INFO(std::string("ScreenController: Initialized"));

  while(true){
    switch(robot::screen::state){
      case screenMode::notification:
        if(lastScreenState != robot::screen::state){
          LOG_INFO(std::string("ScreenController: Entering notification mode"));

          if(field) {
            delete field;
            field = nullptr;
          }

          lv_obj_clean(scr);

          notification_label = lv_label_create(scr, NULL);
          lv_label_set_long_mode(notification_label, LV_LABEL_LONG_BREAK);
          lv_label_set_align(notification_label, LV_LABEL_ALIGN_CENTER);
          lv_obj_set_size(notification_label, 480, 240);
          lv_obj_align(notification_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
          lv_obj_set_style(notification_label, &screen::resources::redText);

          lastScreenState = robot::screen::state;
        }

        lv_label_set_text(notification_label, robot::screen::notification.c_str());

        break;
      case screenMode::selection:
        if(lastScreenState != robot::screen::state){
          LOG_INFO(std::string("ScreenController: Entering selection mode"));

          lv_obj_clean(scr);

          field = new screen::ttField(scr);

          selected = false;

          main_list = lv_obj_create(scr, NULL);
          lv_obj_set_style(main_list, &screen::resources::listStyle);
          lv_obj_set_pos(main_list, 0, 0);
          lv_obj_set_size(main_list, 240, 40 * number_of_routines);
          lv_obj_set_drag(main_list, true);

          routine_buttons.resize(number_of_routines);
          routine_labels.resize(number_of_routines);

          for(int i = 0; i < number_of_routines; i++){

            routine_buttons[i] = lv_btn_create(main_list, NULL);

            lv_obj_set_size(routine_buttons[i], 200, 40);

            lv_btn_set_style(routine_buttons[i], LV_BTN_STYLE_REL, &screen::resources::listStyle);
            lv_btn_set_style(routine_buttons[i], LV_BTN_STYLE_PR, &screen::resources::pressedButton);

            if(i == 0){
              //first term only
              lv_obj_align(routine_buttons[i], NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

            }else{
              lv_obj_align(routine_buttons[i], routine_buttons[i-1], LV_ALIGN_OUT_BOTTOM_MID , 0, 0);
            }


            routine_labels[i] = lv_label_create(routine_buttons[i], NULL);
            lv_obj_set_style(routine_labels[i], routines[i].textStyle);
            lv_label_set_text(routine_labels[i], routines[i].title);
            lv_obj_align(routine_labels[i], NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
          }

          confirm_button = lv_btn_create(main_list, NULL);
          lv_obj_set_style(confirm_button, &screen::resources::greenBox);
          lv_obj_set_size(confirm_button, 40, 40);
          lv_obj_set_pos(confirm_button, 200, 40 * selection);

          field->setX(240);
          routines[selection].print(field);

          lastScreenState = robot::screen::state;
        }

        if(!selected && lv_btn_get_state(confirm_button) == LV_BTN_STATE_PR){
          routines[selection].init();
        }

        selected = selected || lv_btn_get_state(confirm_button) == LV_BTN_STATE_PR;

        if(!selected){
          for(int i = 0; i < number_of_routines; i++){
            if(lv_btn_get_state(routine_buttons[i]) == LV_BTN_STATE_PR){
              selection = i;
              lv_obj_set_y(confirm_button, 40 * selection);

              field->clean();
              routines[selection].print(field);
            }
          }

          lv_obj_set_x(main_list, 0);
        }else{
          lv_obj_set_hidden(main_list, true);
          field->setX(120);
        }

        break;

      /*case screenMode::logo:
        if(lastScreenState != robot::screen::state){
          std::cout << "screen controller - initializing logo mode\n";
          lv_obj_clean(scr);
          lv_obj_set_size(scr, 480, 240);
          lv_obj_t* logo_img = lv_img_create(scr, NULL);
          lv_img_set_src(logo_img, &logo);
          lastScreenState = robot::screen::state;
        }*/

        break;
      case screenMode::disabled:
        if(lastScreenState != robot::screen::state){
          LOG_INFO(std::string("ScreenController: Disabled"));

          if(field) {
            delete field;
            field = nullptr;
          }

          lv_obj_clean(scr);

          lastScreenState = robot::screen::state;
        }

        break;
    }
    pros::delay(50);
  }
}
