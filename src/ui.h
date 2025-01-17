// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: SquareLine_Project

#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"

void hideResult_Animation(lv_obj_t * TargetObject, int delay);
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
void ui_event_Screen1(lv_event_t * e);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_OpsContainer;
extern lv_obj_t * ui_PlusButton;
extern lv_obj_t * ui_PlusLabel;
extern lv_obj_t * ui_MinusButton;
extern lv_obj_t * ui_MinusLabel;
extern lv_obj_t * ui_DoubleButton;
extern lv_obj_t * ui_DoubleLabel;
extern lv_obj_t * ui_DivButton;
extern lv_obj_t * ui_DivLabel;
extern lv_obj_t * ui_Plus10Button;
extern lv_obj_t * ui_Plus10Label;
extern lv_obj_t * ui_Minus10Button;
extern lv_obj_t * ui_Minus10Label;
extern lv_obj_t * ui_AnswerContainer;
extern lv_obj_t * ui_Answer1Button;
extern lv_obj_t * ui_Answer1Label;
extern lv_obj_t * ui_Answer2Button;
extern lv_obj_t * ui_Answer2Label;
extern lv_obj_t * ui_Answer3Button;
extern lv_obj_t * ui_Answer3Label;
extern lv_obj_t * ui_Answer4Button;
extern lv_obj_t * ui_Answer4Label;
extern lv_obj_t * ui_ExerciseLabel;
extern lv_obj_t * ui_ResultLable;
extern lv_obj_t * ui_ScoreLabel;
extern lv_obj_t * ui____initial_actions0;








void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
