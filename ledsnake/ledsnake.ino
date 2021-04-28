// Snake_8x8_Matrix
// Autor: Felix Zwettler

#include <math.h>
#include "LedControl.h"

#define pin_lc_data 2
#define pin_lc_clk 3
#define pin_lc_load 4
#define pin_js_x A0
#define pin_js_y A1
#define pin_js_sw 5

#define gamespeed 500

#define js_deadzone 200
#define js_x_offset 0
#define js_y_offset 0

// lc Initialisierung mit Argumenten: LedControl(data_PIN, clk_PIN, load_PIN, Reihenfolgennummer)
LedControl lc=LedControl(pin_lc_data , pin_lc_clk , pin_lc_load, 1);

// === Globale Variablen ===
short snake_pos[2] = {0,0};
short snake_pos_prev[2][64] = {0};
short snake_vector[2] = {0,0};
short snake_length = 1;
short food_pos[2] = {6,6};
unsigned long time_current = 0;
unsigned long time_prev = 0;
int game_over_status = 0;

// === Funktionen ===
void snake_vector_update(int js_x, int js_y){
  int snake_vector_prev_x = snake_vector[0];
  int snake_vector_prev_y = snake_vector[1];
  
  if(js_x > (512 - js_x_offset) + js_deadzone){
    snake_vector[0] = 1;
    snake_vector[1] = 0;
  }
  else if(js_x < (512 - js_x_offset) - js_deadzone){
    snake_vector[0] = -1;
    snake_vector[1] = 0;
  }
  else if(js_y > (512 - js_y_offset) + js_deadzone){
    snake_vector[0] = 0;
    snake_vector[1] = 1;
  }
  else if(js_y < (512 - js_y_offset) - js_deadzone){
    snake_vector[0] = 0;
    snake_vector[1] = -1;
  }
  if(snake_vector[0] != 0 && snake_vector[0] == (-1) * snake_vector_prev_x){
    snake_vector[0] = snake_vector_prev_x;
  }
  if(snake_vector[1] != 0 && snake_vector[1] == (-1) * snake_vector_prev_y){
    snake_vector[1] = snake_vector_prev_y;
  }
}

int snake_move(int snake_pos, int snake_vector){
  if(snake_pos + snake_vector > 7){
    snake_pos = 0;
  }
  else if(snake_pos + snake_vector < 0){
    snake_pos = 7;
  }
  else {
    snake_pos = snake_pos + snake_vector;
  }
  return snake_pos;
}

void lc_draw(void){
  int i;
  int j;
  int k;
  lc.clearDisplay(0);
  lc.setLed(0, snake_pos[1], snake_pos[0], true);
  lc.setLed(0, food_pos[1], food_pos[0], true);
  for(k = 1; k < snake_length; k++){
    if(k < snake_length){
      lc.setLed(0, snake_pos_prev[1][k], snake_pos_prev[0][k], true);
    }
  }
}

void reset(void){
  int i;
  int j;
  lc.clearDisplay(0);
  delay( 5 * gamespeed);
  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++){
      lc.setLed(0, j, i, true);
    }
  }
  delay( 5 * gamespeed);
  snake_pos[0] = 0;
  snake_pos[1] = 0;
  for(i = 0; i < 64; i++){
    snake_pos_prev[0][i] = 0;
    snake_pos_prev[1][i] = 0;
  }
  snake_vector[0] = 0;
  snake_vector[1] = 0;
  snake_length = 1;
  food_pos_create();
}

int collision_check(void){
  int i;
  int j;
  for(i = 1; i < snake_length; i++){
    if(snake_pos_prev[0][i] == snake_pos[0] && snake_pos_prev[1][i] == snake_pos[1]){
      return 1;
    }
  }
  return 0;
}

void food_pos_create(void){
  int i;
  food_pos[0] = rand() % 8;
  food_pos[1] = rand() % 8;
  for( i = 0; i < snake_length; i++){
    if(food_pos[0] == snake_pos_prev[0][i] && food_pos[0] == snake_pos_prev[0][i]){
      food_pos_create();
    }
  }
}

// === setup() ===
void setup() {
  Serial.begin(57600);
  lc.shutdown(0,false);     // lc aufwecken
  lc.setIntensity(0,3);     // lc Helligkeit zw. 0-15
  lc.clearDisplay(0);       // lc clearen
  Serial.println("=== Arduino ist bereit! ===");
  pinMode(pin_js_x, INPUT);
  pinMode(pin_js_x, INPUT);
  food_pos_create();
}

// === loop() ===
void loop() {
  char eingabe;
  int js_x;
  int js_y;
  // Koordinaten links (0) nach rechts (7)(X-Achse), und von oben (0) nach unten (7)(Y-Achse)
  js_x = analogRead(pin_js_x);
  js_y = analogRead(pin_js_y);
  
  snake_vector_update(js_x, js_y);
  time_current = millis();
  
  if( time_current - time_prev > gamespeed ){
    time_prev = time_current;
    int i;
    
    snake_pos[0] = snake_move(snake_pos[0], snake_vector[0]);
    snake_pos[1] = snake_move(snake_pos[1], snake_vector[1]);
    for(i = 63; i > 0; i--){
      snake_pos_prev[0][i] = snake_pos_prev[0][i - 1];
      snake_pos_prev[1][i] = snake_pos_prev[1][i - 1];
    }
    snake_pos_prev[0][0] = snake_pos[0];
    snake_pos_prev[1][0] = snake_pos[1];

    if(snake_pos[0] == food_pos[0] && snake_pos[1] == food_pos[1]){
      food_pos_create();
      snake_length = snake_length + 1;
    }

    game_over_status = collision_check();
    if(game_over_status == 1){
      reset();
      game_over_status = 0;
    }
    
    lc_draw();
    
    // === Debugging ===
    Serial.print("js: ");
    Serial.print(js_x);
    Serial.println(js_y);
    Serial.print("snake_laenge: ");
    Serial.println(snake_length);
    Serial.print("snake_pos[]: ");
    Serial.print(snake_pos[0]);
    Serial.println(snake_pos[1]);
    Serial.print("snake_pos_davor[][1]: ");
    Serial.print(snake_pos_prev[0][1]);
    Serial.println(snake_pos_prev[1][1]);
    Serial.println();
    
  }
}
