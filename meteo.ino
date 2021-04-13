#include <Adafruit_CCS811.h>

#include <LiquidCrystal.h>

#include <Adafruit_HTU21DF.h>

int lcd_key = 0;
int adc_key_in = 0;

float tempUpHold = 20.5;
float tempDownHold = -10.5;
float humiUpHold = 95.0;
float humiDownHold = 10.0;
int gasTreshold = 800;

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // lcd init
Adafruit_HTU21DF htu = Adafruit_HTU21DF(); // htu21d init
Adafruit_CCS811 ccs; // ccs811 init

// button handle method
int read_LCD_buttons() {
  adc_key_in = analogRead(0); // read value from the sensor 
  // threshold manually adjusted
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50) return btnRIGHT;
  if (adc_key_in < 250) return btnUP;
  if (adc_key_in < 300) return btnDOWN;
  if (adc_key_in < 500) return btnLEFT;
  if (adc_key_in < 850) return btnSELECT;
  return btnNONE;
}

void setup() {
  Serial.begin(9600);
  if (!ccs.begin()) {
    while (1);
  }

  // gas sensor init
  while (!ccs.available());
  ccs.begin();

  // htu init
  htu.begin();

  // lcd init (16x2)
  lcd.begin(16, 2);

  // splash screen
  lcd.print(" METEO  STATION ");
  lcd.setCursor(0, 1);
  lcd.print(" v0.0.4 Welcome ");
  delay(2000);
  lcd.clear();
  Serial.println("Ready");
}

int selectedMenu = 0;
bool selectedOption = false; // because the display is only 16x2 I can get away
// with using just a bool variable
bool splashShown = false;
bool clearScreen = false;
bool exitSettings = false;

void readButtons() {
  lcd_key = read_LCD_buttons(); // read the buttons
  switch (lcd_key) { // depending on which button was pushed, we perform an action
  case btnRIGHT: {
    switch (selectedMenu) {
    case 0: {
      break;
    }
    case 1: {
      if (selectedOption) {
        tempDownHold += 0.5;
      } else {
        tempUpHold += 0.5;
      }
      break;
    }
    case 2: {
      if (selectedOption) {
        humiDownHold += 0.5;
      } else {
        humiUpHold += 0.5;
      }
      break;
    }
    case 3: {
      if (!selectedOption) {
        exitSettings = true;
      }
      break;
    }
    }
    break;
  }
  case btnLEFT: {
    switch (selectedMenu) {
    case 0: {
      break;
    }
    case 1: {
      if (selectedOption) {
        tempDownHold -= 0.5;
      } else {
        tempUpHold -= 0.5;
      }
      break;
    }
    case 2: {
      if (selectedOption) {
        humiDownHold -= 0.5;
      } else {
        humiUpHold -= 0.5;
      }
      break;
    }
    case 3: {
      if (!selectedOption) {
        exitSettings = false;
      }
      break;
    }
    }
    break;
  }
  case btnUP: {
    switch (selectedMenu) {
    case 0: {
      break;
    }
    case 1: {
      if (selectedOption) {
        selectedOption = false;
      } else {
        selectedOption = true;
      }
      break;
    }
    case 2: {
      if (selectedOption) {
        selectedOption = false;
      } else {
        selectedOption = true;
      }
      break;
    }
    case 3: {
      selectedOption = false;
      break;
    }
    }
    break;
  }
  case btnDOWN: {
    switch (selectedMenu) {
    case 0: {
      break;
    }
    case 1: {
      if (selectedOption) {
        selectedOption = false;
      } else {
        selectedOption = true;
      }
      break;
    }
    case 2: {
      if (selectedOption) {
        selectedOption = false;
      } else {
        selectedOption = true;
      }
      break;
    }
    case 3: {
      selectedOption = false;
      break;
    }
    }
    break;
  }
  case btnSELECT: {
    switch (selectedMenu) {
    case 0: {
      selectedMenu = 1;
      splashShown = false;
      break;
    }
    case 1: {
      selectedMenu = 2;
      splashShown = false;
      break;
    }
    case 2: {
      selectedMenu = 3;
      splashShown = false;
      break;
    }
    case 3: {
      selectedMenu = 0;
      splashShown = false;
      clearScreen = true;
      break;
    }
    }
    break;
  }
  case btnNONE: {
    break;
  }
  }
}

void selectionShow() {
  lcd.setCursor(0, 0);
  if (!selectedOption) {
    lcd.print('>');
  } else {
    lcd.print(" ");
  }
  lcd.setCursor(0, 1);
  if (selectedOption) {
    lcd.print('>');
  } else {
    lcd.print(" ");
  }
}

void status1() {
  // gas sensor print
  if (ccs.available()) {
    if (!ccs.readData()) {
      lcd.setCursor(0, 0);
      lcd.print(ccs.geteCO2());
      lcd.print("ppm");
      lcd.setCursor(10, 0);
      lcd.print(ccs.getTVOC());
    }
  }
  float h = htu.readHumidity();
  // Read temperature as Celsius
  float t = htu.readTemperature();

  lcd.setCursor(0, 1);
  lcd.print(t, 1);
  lcd.print(char(223));
  lcd.print('C');
  lcd.setCursor(10, 1);
  lcd.print(h, 1);
  lcd.print("%");
}

void settings1() {
  selectionShow();
  lcd.setCursor(1, 0);
  lcd.print("T" + String(char(223)) + " UP   ");
  lcd.print(tempUpHold, 1);
  lcd.print(String(char(223)) + "C");
  lcd.setCursor(1, 1);
  lcd.print("T" + String(char(223)) + " DOWN ");
  lcd.print(tempDownHold, 1);
  lcd.print(String(char(223)) + "C");
  delay(150);
}

void settings2() {
  selectionShow();
  lcd.setCursor(1, 0);
  lcd.print("H% UP    ");
  lcd.print(humiUpHold, 1);
  lcd.setCursor(1, 1);
  lcd.print("H% DOWN  ");
  lcd.print(humiDownHold, 1);
  delay(150);
}

void settings3() {
  selectionShow();
  lcd.setCursor(0, 0);
  lcd.print("Exit?     ");
  if (exitSettings) {
    lcd.print("YES");
  }
  else {
    lcd.print("NO");
  }
  lcd.setCursor(1, 1);
  lcd.print("Press > to exit");
  clearScreen = true;
  delay(150);
}

void loop() {
  readButtons();
  if (exitSettings) {
    selectedMenu = 0;
    exitSettings = false;
  }
  switch (selectedMenu) {
  case 0: {
    if (clearScreen) {
      lcd.clear();
      clearScreen = false;
    }
    if ((millis() / 1000) % 4 == 0) {
      status1();
    }
    break;
  }
  case 1: {
    if (!splashShown) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temperature");
      lcd.setCursor(0, 1);
      lcd.print("settings");
      delay(1000);
      lcd.clear();
      splashShown = true;
    }
    settings1();
    break;
  }
  case 2: {
    if (!splashShown) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Humidity");
      lcd.setCursor(0, 1);
      lcd.print("settings");
      delay(1000);
      lcd.clear();
      splashShown = true;
    }
    settings2();
    break;
  }
  case 3: {
    if (!splashShown) {
      lcd.clear();
      splashShown = true;
    }
    settings3();
    break;
  }
  }
}
