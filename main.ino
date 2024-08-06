#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RTC_DS1307 rtc;

int num = 1;
int number = 1;
const int sensorPin = 2;  // Pin sensor getar
unsigned long lastVibrationTime = 0;  // Waktu getaran terakhir terdeteksi
unsigned long debounceDelay = 50;     // Waktu debounce untuk menghindari pembacaan false
unsigned long interval = 30000;       // Interval 1 menit (60.000 ms)
bool vibrationDetected = false;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BUTTON_1 12
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include "eyes.h"

bool state_button_1 = false;
int sleepCounter = 0;

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPin, INPUT);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(BUTTON_1, INPUT_PULLUP);
  digitalWrite(BUTTON_1, HIGH);
  lastVibrationTime = millis();  // Inisialisasi dengan waktu sekarang
  
  Serial.begin(115200);
  Serial.println("Booting");

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  DateTime now = rtc.now();
  Serial.print("Current time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Update RTC with a new time, e.g., to the time when the Arduino was last reset
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Uncomment this line to use the compile time

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  delay(1000);  // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(WHITE);
}

unsigned char readkey(void){ 
  unsigned char ret=0;
    if (digitalRead(8)==0 ) ret=1;  //izquierda
    if (digitalRead(9)==0) ret+=2; //arriba
    if (digitalRead(10)==0) ret+=4; //abajo
    if (digitalRead(11)==0) ret+=8; //fuego
  return (ret);
}

String formatOrdinal(uint8_t x) {
  String suffix;

  if (x >= 11 && x <= 13) {
    suffix = "th";
  } else {
    switch (x % 10) {
      case 1:
        suffix = "st";
        break;
      case 2:
        suffix = "nd";
        break;
      case 3:
        suffix = "rd";
        break;
      default:
        suffix = "th";
        break;
    }
  }

  return String(x) + suffix;
}

String DayMonthYear(uint8_t Day, uint8_t Month, uint16_t Year){
  String DayMonthYearText;
  if (Month == 1)  DayMonthYearText = "JAN ";
  if (Month == 2)  DayMonthYearText = "FEB ";
  if (Month == 3)  DayMonthYearText = "MAR ";
  if (Month == 4)  DayMonthYearText = "APR ";
  if (Month == 5)  DayMonthYearText = "MAY ";
  if (Month == 6)  DayMonthYearText = "JUN ";
  if (Month == 7)  DayMonthYearText = "JUL ";
  if (Month == 8)  DayMonthYearText = "AUG ";
  if (Month == 9)  DayMonthYearText = "SEP ";
  if (Month == 10)  DayMonthYearText = "OCT ";
  if (Month == 11)  DayMonthYearText = "NOV ";
  if (Month == 12)  DayMonthYearText = "DEC ";

  DayMonthYearText = DayMonthYearText + formatOrdinal(Day) + " " + Year;
  
  return DayMonthYearText;
}

String AddLeadingZero(uint8_t x){
  String AddLeadingZeroText;
  if (x < 10) AddLeadingZeroText = "0"; 
  else AddLeadingZeroText = ""; 
  AddLeadingZeroText = AddLeadingZeroText + x;
  return AddLeadingZeroText;
}

String CurrentTime(uint8_t h, uint8_t i){
  String CurrentTimeText = "";
  CurrentTimeText = CurrentTimeText + AddLeadingZero(h) + ":" + AddLeadingZero(i);
  return CurrentTimeText;
}

void drawCentreString(String text, int y, int textSize=1, int textColor=SSD1306_WHITE) {
  // Calculate the length of the text to determine the center position
  int16_t positionX, positionY;
  uint16_t textWidth, textHeight;
  display.getTextBounds(text.c_str(), 0, 0, &positionX, &positionY, &textWidth, &textHeight);
  int16_t textX = (SCREEN_WIDTH - textWidth) / 2;

  // Display text on center
  display.setCursor(textX, y);
  display.setTextSize(textSize);
  display.setTextColor(textColor); 
  display.println(text);
}

void enterSleepMode(bool mode) {
  if (mode) display.ssd1306_command(SSD1306_DISPLAYOFF);
  else display.ssd1306_command(SSD1306_DISPLAYON);
}

int xp=16;
int mood=1;

void animation() {
  number++;
  int n;
  static int xd=0;
  static int espera=0;
  static int step=0;
  int x1,x2;
    if (espera>0) {
      espera--;
      delay(1);
    } else {
      x1=   xd+ (xp>16? (16+2*(xp-16)):xp);
      x2=64+xd+ (xp<16? (-16+(xp*2))  :xp);
      switch (step){
        case 0:
        display.clearDisplay(); // Clear the display buffer
        if (xp<6) { 
                display.drawBitmap(x1, 8, peyes[mood][2][0], 32, 32, WHITE);
                display.drawBitmap(x2, 8, peyes[mood][1][1], 32, 32, WHITE);
        } else if (xp<26) {
                display.drawBitmap(x1, 8, peyes[mood][0][0], 32, 32, WHITE);
                display.drawBitmap(x2, 8, peyes[mood][0][1], 32, 32, WHITE); 
        } else {
                display.drawBitmap(x1, 8, peyes[mood][1][0], 32, 32, WHITE); 
                display.drawBitmap(x2, 8, peyes[mood][2][1], 32, 32, WHITE);
        }
        display.display();
        espera=random(250, 1000);
        n=random(0,7);
        if (n==6) {
            step=1;
        } else {
            step=2;
        }
        break;
        case 1:
        display.clearDisplay(); // Clear the display buffer
        display.drawBitmap(x1, 8, eye0, 32, 32, WHITE);
        display.drawBitmap(x2, 8, eye0, 32, 32, WHITE);
        display.display();
        espera=100;
        step=0;
        break;
        case 2:
        n=random(0,10);
        if (n<5) xd--;
        if (n>5) xd++;
        if (xd<-4) xd=-3;
        if (xd>4) xd=3; 
        espera=0;
        step=0;
        break;
      }
    }

    //n=0;

    if (number < 10000) {
      n = 0;
      mood=1;
    }

    if (number > 10000 && number < 10200) {
      n = 0;
      mood=2;
    };

    if (number > 10200 && number < 10300) {
      n = 2;
      mood=3;
    }

    if (number > 10300 && number < 10400) {
      n = 4;
      mood=4;
    }

    if (number > 10400 && number < 10500) {
      n = 1;
      mood=0;
      xp=(xp<=16?16:xp-1);
    };

    if (number > 10500) {
      n = 0;
      mood=1;
      number = 0;
    };

if (n==2) xp=(xp<=0?0:xp-1); 
if (n==4) xp=(xp>=32?32:xp+1);
if (n!=0) { espera=0; step=0; }
}



void clockState() {
  display.clearDisplay();
  DateTime now = rtc.now();

  display.setCursor(3, 4); 
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE);
  String dayOfWeek;
  switch (now.dayOfTheWeek()) {
    case 0: dayOfWeek = "Minggu"; break;
    case 1: dayOfWeek = "Senin"; break;
    case 2: dayOfWeek = "Selasa"; break;
    case 3: dayOfWeek = "Rabu"; break;
    case 4: dayOfWeek = "Kamis"; break;
    case 5: dayOfWeek = "Jumat"; break;
    case 6: dayOfWeek = "Sabtu"; break;
  }
  display.println(dayOfWeek);

  display.setCursor(65, 4); 
  display.setTextSize(1);  
  display.setTextColor(SSD1306_WHITE);
  String dateStr = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  display.println(dateStr);

  display.drawLine(0, 15, 128, 15, 1);

  display.setCursor(0, 20); 
  display.setTextSize(3);  
  display.setTextColor(SSD1306_WHITE); 
  display.println(CurrentTime(now.hour(), now.minute()));

  display.setCursor(88, 20); 
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE); 
  display.println(":");

  display.setCursor(102, 24); 
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); 
  display.println(AddLeadingZero(now.second()));

  display.drawLine(0, 45, 128, 45, 1);

  display.setCursor(48, 54); 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); 
  display.println("SALVA");

  display.display();
  delay(1000);
}

bool animasi = true;


void loop() {
  // num++;
  // animation();
  //  if (num >= 5000) {
  //   num = 0;
  // } else if (num >= 15) {
  //   animation();
  // } else {
  //   clockState();
  // }
  int sensorValue = digitalRead(sensorPin);

  // Jika sensor mendeteksi getaran
  if (sensorValue == HIGH) {
    // Update waktu getaran terakhir terdeteksi
    lastVibrationTime = millis();
    vibrationDetected = false;  // Set false jika ada getaran
  }

  // Cek apakah sudah lebih dari 1 menit sejak getaran terakhir
  if (millis() - lastVibrationTime >= interval) {
    vibrationDetected = true;   // Set true jika tidak ada getaran selama 1 menit
  }

  // Tampilkan status getaran pada Serial Monitor
  if (vibrationDetected) {
    animation();
  } else {
    clockState();
  }

}
