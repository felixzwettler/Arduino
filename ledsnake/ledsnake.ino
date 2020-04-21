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

#define spielgeschw 500

#define js_deadzone 200
#define js_x_offset 0
#define js_y_offset 0

// lc Initialisierung mit Argumenten: LedControl(data_PIN, clk_PIN, load_PIN, Reihenfolgennummer)
LedControl lc=LedControl(pin_lc_data , pin_lc_clk , pin_lc_load, 1);

// === Globale Variablen ===
short snake_pos[2] = {0,0};
short snake_pos_davor[2][64] = {0};
short snake_vektor[2] = {0,0};
short snake_laenge = 1;
short essen_pos[2] = {6,6};
unsigned long zeit_aktuell = 0;
unsigned long zeit_davor = 0;
int game_over_status = 0;

// === Funktionen ===
void snake_vektor_aktualisieren(int js_x, int js_y){
  int snake_vektor_davor_x = snake_vektor[0];
  int snake_vektor_davor_y = snake_vektor[1];
  
  if(js_x > (512 - js_x_offset) + js_deadzone){
    snake_vektor[0] = 1;
    snake_vektor[1] = 0;
  }
  else if(js_x < (512 - js_x_offset) - js_deadzone){
    snake_vektor[0] = -1;
    snake_vektor[1] = 0;
  }
  else if(js_y > (512 - js_y_offset) + js_deadzone){
    snake_vektor[0] = 0;
    snake_vektor[1] = 1;
  }
  else if(js_y < (512 - js_y_offset) - js_deadzone){
    snake_vektor[0] = 0;
    snake_vektor[1] = -1;
  }
  if(snake_vektor[0] != 0 && snake_vektor[0] == (-1) * snake_vektor_davor_x){
    snake_vektor[0] = snake_vektor_davor_x;
  }
  if(snake_vektor[1] != 0 && snake_vektor[1] == (-1) * snake_vektor_davor_y){
    snake_vektor[1] = snake_vektor_davor_y;
  }
}

int snake_bewegung(int snake_pos, int snake_vektor){
  if(snake_pos + snake_vektor > 7){
    snake_pos = 0;
  }
  else if(snake_pos + snake_vektor < 0){
    snake_pos = 7;
  }
  else {
    snake_pos = snake_pos + snake_vektor;
  }
  return snake_pos;
}

void lc_zeichnen(void){
  int i;
  int j;
  int k;
  lc.clearDisplay(0);
  lc.setLed(0, snake_pos[1], snake_pos[0], true);
  lc.setLed(0, essen_pos[1], essen_pos[0], true);
  for(k = 1; k < snake_laenge; k++){
    if(k < snake_laenge){
      lc.setLed(0, snake_pos_davor[1][k], snake_pos_davor[0][k], true);
    }
  }
}

void reset(void){
  int i;
  int j;
  lc.clearDisplay(0);
  delay( 5 * spielgeschw);
  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++){
      lc.setLed(0, j, i, true);
    }
  }
  delay( 5 * spielgeschw);
  snake_pos[0] = 0;
  snake_pos[1] = 0;
  for(i = 0; i < 64; i++){
    snake_pos_davor[0][i] = 0;
    snake_pos_davor[1][i] = 0;
  }
  snake_vektor[0] = 0;
  snake_vektor[1] = 0;
  snake_laenge = 1;
  essen_pos_erzeugen();
}

int kollision_abfrage(void){
  int i;
  int j;
  for(i = 1; i < snake_laenge; i++){
    if(snake_pos_davor[0][i] == snake_pos[0] && snake_pos_davor[1][i] == snake_pos[1]){
      return 1;
    }
  }
  return 0;
}

void essen_pos_erzeugen(void){
  int i;
  essen_pos[0] = rand() % 8;
  essen_pos[1] = rand() % 8;
  for( i = 0; i < snake_laenge; i++){
    if(essen_pos[0] == snake_pos_davor[0][i] && essen_pos[0] == snake_pos_davor[0][i]){
      essen_pos_erzeugen();
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
  essen_pos_erzeugen();
}

// === loop() ===
void loop() {
  char eingabe;
  int js_x;
  int js_y;
  // Koordinaten links (0) nach rechts (7)(X-Achse), und von oben (0) nach unten (7)(Y-Achse)
  js_x = analogRead(pin_js_x);
  js_y = analogRead(pin_js_y);
  
  snake_vektor_aktualisieren(js_x, js_y);
  zeit_aktuell = millis();
  
  if( zeit_aktuell - zeit_davor > spielgeschw ){
    zeit_davor = zeit_aktuell;
    int i;
    
    snake_pos[0] = snake_bewegung(snake_pos[0], snake_vektor[0]);
    snake_pos[1] = snake_bewegung(snake_pos[1], snake_vektor[1]);
    for(i = 63; i > 0; i--){
      snake_pos_davor[0][i] = snake_pos_davor[0][i - 1];
      snake_pos_davor[1][i] = snake_pos_davor[1][i - 1];
    }
    snake_pos_davor[0][0] = snake_pos[0];
    snake_pos_davor[1][0] = snake_pos[1];

    if(snake_pos[0] == essen_pos[0] && snake_pos[1] == essen_pos[1]){
      essen_pos_erzeugen();
      snake_laenge = snake_laenge + 1;
    }

    game_over_status = kollision_abfrage();
    if(game_over_status == 1){
      reset();
      game_over_status = 0;
    }
    
    lc_zeichnen();
    
    // === Debugging ===
    Serial.print("js: ");
    Serial.print(js_x);
    Serial.println(js_y);
    Serial.print("snake_laenge: ");
    Serial.println(snake_laenge);
    Serial.print("snake_pos[]: ");
    Serial.print(snake_pos[0]);
    Serial.println(snake_pos[1]);
    Serial.print("snake_pos_davor[][1]: ");
    Serial.print(snake_pos_davor[0][1]);
    Serial.println(snake_pos_davor[1][1]);
    Serial.println();
    
  }
}
