#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <XPT2046_Touchscreen.h>




#define MY_SYMBOL_HAPPY      "\xEF\x83\x98"   /* Custom happy smiley face */
#define MY_SYMBOL_SAD        "\xEF\x83\x99"   /* Custom sad smiley face */

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240, touchScreenMaximumY = 3800;


/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];
int g_correct_answer;
int g_correct_answer_idx;
volatile int g_score = 0;
char g_exercise[32];
typedef enum { PLUS,
               MINUS,
               DOUBLE,
               DIV,
               PLUS10,
               MINUS10 } Ops;

volatile Ops g_operation;
const int TOTAL_CORRECT_ANSWER_STR = 5;
const char *result_correct[TOTAL_CORRECT_ANSWER_STR] = { "מעולה!", "כל הכבוד!", "מצויין!", "מדהים!", "מקסים!" };

volatile lv_obj_t *test_arr[1] = { ui_Answer1Label };
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */




// Helper function to generate a random number within a range
int rand_in_range(int min, int max) {

  return min + rand() % (max - min + 1);
}


// Helper function to generate optional answers
void generate_optional_answers(int correct_answer) {
  lv_obj_t *answer_button_text_lable[4] = { ui_Answer1Label, ui_Answer2Label, ui_Answer3Label, ui_Answer4Label };

  int correct_pos = random(0, 4);

  for (int i = 0; i < 4; i++) {
    if (!answer_button_text_lable[i]) {
      printf("answer_button_text_lable[%d] is null ! \n", i);
      continue;
    }
    if (i == correct_pos) {
      g_correct_answer_idx = i;
      printf("set correct answer %d , idx= %d \n ", g_correct_answer, g_correct_answer_idx);

      lv_label_set_text_fmt((lv_obj_t *)answer_button_text_lable[i], "%d", g_correct_answer);

    } else {
      int offset = rand_in_range(-3, 3);
      printf("offset = %d \n", offset);
      int optional_answer = abs(correct_answer + offset);
      if (correct_answer == optional_answer)
        optional_answer += 1;

      lv_label_set_text_fmt((lv_obj_t *)answer_button_text_lable[i], "%d", optional_answer);

      printf("set optional answer %d \n ", optional_answer);
    }
  }
}
void register_cb() {
  /// Copied from ui_Screen1.c like https://www.youtube.com/watch?v=PTKjjDPNIdM

  lv_obj_add_event_cb(ui_Answer1Button, ui_event_AnswerButtonClicked, LV_EVENT_PRESSED, (void *)0);
  lv_obj_add_event_cb(ui_Answer2Button, ui_event_AnswerButtonClicked, LV_EVENT_PRESSED, (void *)1);
  lv_obj_add_event_cb(ui_Answer3Button, ui_event_AnswerButtonClicked, LV_EVENT_PRESSED, (void *)2);
  lv_obj_add_event_cb(ui_Answer4Button, ui_event_AnswerButtonClicked, LV_EVENT_PRESSED, (void *)3);


  lv_obj_add_event_cb(ui_PlusButton, ui_event_OpButtoncClicked, LV_EVENT_PRESSED, (void *)PLUS);
  lv_obj_add_event_cb(ui_MinusButton, ui_event_OpButtoncClicked, LV_EVENT_PRESSED, (void *)MINUS);
  lv_obj_add_event_cb(ui_DoubleButton, ui_event_OpButtoncClicked, LV_EVENT_PRESSED, (void *)DOUBLE);
  lv_obj_add_event_cb(ui_DivButton, ui_event_OpButtoncClicked, LV_EVENT_PRESSED, (void *)DIV);
  lv_obj_add_event_cb(ui_Plus10Button, ui_event_OpButtoncClicked, LV_EVENT_PRESSED, (void *)PLUS10);
  lv_obj_add_event_cb(ui_Minus10Button, ui_event_OpButtoncClicked, LV_EVENT_PRESSED, (void *)MINUS10);
}
void generate_plus_exercise() {
  printf("generate_plus_exercise\n");
  int a = rand_in_range(1, 50);
  int b = rand_in_range(1, 50);
  g_correct_answer = a + b;

  snprintf(g_exercise, sizeof(g_exercise), "%d + %d = ?", a, b);
}

void generate_subtract_exercise() {
  printf("generate_subtract_exercise\n");
  int a = rand_in_range(1, 100);
  int b = rand_in_range(1, a);  // Ensure result is not negative
  g_correct_answer = a - b;
  snprintf(g_exercise, sizeof(g_exercise), "%d - %d = ?", a, b);
}

void generate_double_exercise() {
  printf("generate_double_exercise\n");
  int a = rand_in_range(1, 10);
  int b = rand_in_range(1, 5);
  g_correct_answer = a * b;
  snprintf(g_exercise, sizeof(g_exercise), "%d * %d = ?", a, b);
}

void generate_div_exercise() {
  printf("generate_div_exercise\n");
  int divisor = rand_in_range(1, 10);
  int quotient = rand_in_range(1, 10);
  int dividend = divisor * quotient;
  g_correct_answer = quotient;
  snprintf(g_exercise, sizeof(g_exercise), "%d : %d = ?", dividend, divisor);
}

void generate_plus10_exercise() {
  printf("generate_plus10_exercise\n");
  int a = rand_in_range(1, 9);
  int b = rand_in_range(1, 9 - a);

  g_correct_answer = a + b;
  snprintf(g_exercise, sizeof(g_exercise), "%d + %d = ?", a, b);
}

void generate_subtract10_exercise() {
  printf("generate_subtract10_exercise\n");
  int a = rand_in_range(1, 10);
  int b = rand_in_range(0, a);  // Ensure result is not negative
  g_correct_answer = a - b;
  snprintf(g_exercise, sizeof(g_exercise), "%d - %d = ?", a, b);
}


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp_drv);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    //Some very basic auto calibration so it doesn't go out of range
    if (p.x < touchScreenMinimumX) touchScreenMinimumX = p.x;
    if (p.x > touchScreenMaximumX) touchScreenMaximumX = p.x;
    if (p.y < touchScreenMinimumY) touchScreenMinimumY = p.y;
    if (p.y > touchScreenMaximumY) touchScreenMaximumY = p.y;
    //Map this to the pixel position
    data->point.x = map(p.x, touchScreenMinimumX, touchScreenMaximumX, 1, screenWidth);  /* Touchscreen X calibration */
    data->point.y = map(p.y, touchScreenMinimumY, touchScreenMaximumY, 1, screenHeight); /* Touchscreen Y calibration */
    data->state = LV_INDEV_STATE_PR;

    //Serial.print( "Touch x " );
    //Serial.print( data->point.x );
    //Serial.print( " y " );
    //Serial.println( data->point.y );
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}


void process_answer(lv_timer_t *timer) {
  printf("process_answer\n");
  bool is_correct_answer = (bool)timer->user_data;
  lv_obj_add_flag(ui_ResultLable, LV_OBJ_FLAG_HIDDEN);

  if (is_correct_answer) {
    generate_exercise();
  } else {
    lv_obj_clear_flag(ui_AnswerContainer, LV_OBJ_FLAG_HIDDEN);
  }

  lv_obj_clear_flag(ui_OpsContainer, LV_OBJ_FLAG_HIDDEN);
}


void generate_exercise() {
  printf("\n\ngenerate_exercise\n");
  lv_obj_add_flag(ui_ResultLable, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ui_AnswerContainer, LV_OBJ_FLAG_HIDDEN);
  switch (g_operation) {
    //PLUS,MINUS,DOUBLE,DIV,PLUS10,MINUS10
    case PLUS:
      generate_plus_exercise();
      break;
    case MINUS:
      generate_subtract_exercise();
      break;
    case DOUBLE:
      generate_double_exercise();
      break;
    case DIV:
      generate_div_exercise();
      break;
    case PLUS10:
      generate_plus10_exercise();
      break;
    case MINUS10:
      generate_subtract10_exercise();
      break;
  }
  lv_label_set_text(ui_ExerciseLabel, g_exercise);
  printf("Exercise: %s \n", g_exercise);
  printf("Correct answer: %d\n", g_correct_answer);
  generate_optional_answers(g_correct_answer);
}

int update_score() {
  ///PLUS,MINUS,DOUBLE,DIV,PLUS10,MINUS10
  int increase_score;
  switch (g_operation) {
    case PLUS10:
    case MINUS10:
    case PLUS:
    case MINUS:
      increase_score = 1;
      break;
    case DOUBLE:
      increase_score = 3;
      break;
    case DIV:
      increase_score = 5;
      break;
  }
  g_score += increase_score;
  printf("update score to %d \n", g_score);
  lv_label_set_text_fmt(ui_ScoreLabel, "%d", g_score);
  return increase_score;
}
void ui_event_OpButtoncClicked(lv_event_t *e) {
  lv_obj_clear_flag(ui_AnswerContainer, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ui_ExerciseLabel, LV_OBJ_FLAG_HIDDEN);
  g_operation = (Ops)((int)lv_event_get_user_data(e));
  generate_exercise();
}

void ui_event_AnswerButtonClicked(lv_event_t *e) {
  bool is_correct_answer;
  printf("ui_event_AnswerButtonClicked\n");
  lv_obj_clear_flag(ui_ResultLable, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_AnswerContainer, LV_OBJ_FLAG_HIDDEN);  //hide until process_answer will called
  lv_obj_add_flag(ui_OpsContainer, LV_OBJ_FLAG_HIDDEN);
  int answer_idx = (int)lv_event_get_user_data(e);
  if (answer_idx == g_correct_answer_idx) {
    printf("correct answer!\n");
    is_correct_answer = true;

    int increase_score = update_score();

     lv_label_set_text_fmt(ui_ResultLable, "%s %s  + %d", LV_SYMBOL_OK, result_correct[rand() % TOTAL_CORRECT_ANSWER_STR], increase_score);
  } else {
    is_correct_answer = false;
    printf("idx = %d is wrong answer! , corrent answer is %d \n", answer_idx, g_correct_answer);
    lv_label_set_text_fmt(ui_ResultLable,"%s %s" ,"נסו שוב",LV_SYMBOL_CLOSE);
  }


  lv_timer_t *t = lv_timer_create(process_answer, 3000, (void *)is_correct_answer);
  lv_timer_set_repeat_count(t, 1);
}
void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */
  srand(time(NULL));
  String LVGL_Arduino = "LVGL version ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);

  lv_init();


  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS); /* Start second SPI bus for touchscreen */
  ts.begin(mySpi);                                                  /* Touchscreen init */
  ts.setRotation(1);                                                /* Landscape orientation */

  tft.begin();        /* TFT init */
  tft.setRotation(1); /* Landscape orientation */

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);


  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_t *my_indev = lv_indev_drv_register(&indev_drv);

  ui_init();

  register_cb();

  lv_obj_set_style_text_font(ui_ResultLable, &lv_font_dejavu_16_persian_hebrew, 0);
  lv_obj_set_style_base_dir(ui_ResultLable, LV_BASE_DIR_RTL, 0);
  //MUST #define LV_USE_BIDI 1 for rtl

  Serial.println("Setup done");
}
void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}
