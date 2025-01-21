/****************************************************************************************************************************
  RP2040_RTC_Time.ino 
  pico oled clock
  + rgb control
  2 button/ single/double tap
  mode up     on/off
  mode down   brightness/color
  ****************/
 
#include "defines.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <Timezone_Generic.h>             // https://github.com/khoih-prog/Timezone_Generic

//////////////////////////////////////////

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};    //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};     //Standard time = UTC - 5 hours
Timezone *myTZ;

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev

//////////////////////////////////////////

/*
typedef struct 
{
  int16_t year;     ///< 0..4095
  int8_t  month;    ///< 1..12, 1 is January
  int8_t  day;      ///< 1..28,29,30,31 depending on month
  int8_t  dotw;     ///< 0..6, 0 is Sunday
  int8_t  hour;     ///< 0..23
  int8_t  min;      ///< 0..59
  int8_t  sec;      ///< 0..59
} datetime_t; 
*/

// Example set at 05:00:00 Fri 21 Jan 2022 UTC or 00:00:00 Fri 21 Jan 2022 EST
                     //year, mon, date, day, hour, min, sec
datetime_t currTime = { 2025,  1,    19,   1,    4,   57,   0 };

//////////////////////////////////////////

/*
 * oled stuff
 */
#include "analog.h"
#include "button.h"
int bpin = 16;
int b1pin = 17;
Button key(bpin, true);
Button key1(b1pin, true);

analog bb(A2, false); //fake analog for brightness fading button.
bool at = false;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define i2c_Address 0x3c
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET); 
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);


//#define LOGO_HEIGHT   16
//#define LOGO_WIDTH    16
//static const unsigned char PROGMEM logo_bmp[] =
//{ 0b00000000, 0b11000000,
//  0b00000001, 0b11000000,
//  0b00000001, 0b11000000,
//  0b00000011, 0b11100000,
//  0b11110011, 0b11100000,
//  0b11111110, 0b11111000,
//  0b01111110, 0b11111111,
//  0b00110011, 0b10011111,
//  0b00011111, 0b11111100,
//  0b00001101, 0b01110000,
//  0b00011011, 0b10100000,
//  0b00111111, 0b11100000,
//  0b00111111, 0b11110000,
//  0b01111100, 0b11110000,
//  0b01110000, 0b01110000,
//  0b00000000, 0b00110000 };

void cClick(){
  int ww = key.tap(); 
   if( ww== 0) return;
   if(ww == 1){ 
    // Serial.print("click.");
    // seeeime();
    if(at) changeMode();
   }
} 
void mClick(){
  int ee = key1.tap();
    // Serial.print("tap..");
    // Serial.print( ee);
   if( ee== 0) return;
   if(ee == 1){    
    if(at){
      at = false;
      setTime();
    } else at = true;
    
    // Serial.print( "at: ");
    // Serial.println( at);
   }
    
} 


String getWeekday(time_t t);
String getDate(time_t t);
int yr = 2020, mth = 1, dte = 1, dy = 1;
int hr = 0;
int mn = 0;
int md = 0;

void changeMode(){
  if(++md > 4) md = 0;
}

int lastv = -1;
void adjustTime(){
  int vv = bb.getVal();
  if(vv != lastv){
    lastv = vv;
  switch(md){
      case 0: hr = vv/5;
        break;  
      case 1: mn = vv;
        break;
      case 2: mth = vv/5;
        break;
      case 3: dte = vv/2 + 1;
        break;
      case 4: dy = vv/8.571;
        break;
  }
  }
  currTime = { 2025,  1,    20,   1,    hr,   mn,   0 };
  char buf[32], bff[8], bf[8], bc[32], by[16];
  sprintf(buf, "%.1d%.2d", hr, mn );  
  // sprintf(bff, "%s", yr );  
  // String yy(bff);
  // yy = yy.substring(2);
  sprintf(bc, "%.1d/%.2d", mth, dte ); 
  sprintf(by, "%d", dy ); 
  sprintf(bf, "xx");
  String s(buf), ss(bf), sss(bc), ssss(by);
  
  testdrawoled(s, ss, sss, ssss );  
}

void setTime(){
// Example set at 05:00:00 Fri 21 Jan 2022 UTC or 00:00:00 Fri 21 Jan 2022 EST
                         //year, mon, date, day, hour, min, sec
  // datetime_t currTime = { 2025,  1,    20,   1,    15,   40,   0 };
  rtc_set_datetime(&currTime);
} 

void dim(){


}

//start/title screen screen
void mRec(){ 
    display.drawRect(1, 1, display.width()-2, display.height()-4, SH110X_WHITE);
    
    display.setTextSize(3);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.setCursor(6,4);             // Start at top-left corner
    display.println("PiPico");
    display.setCursor(15,30);    
    display.println("Clock");
    display.display();
}

 
bool showtime = false;  //show time or animation
bool pacd = false;  //anamation flag
int stepsize = 11;  //width of dots//jump width
int tt = 0, stage = 0;    //tik //level
int astart = 0, aend = 1, asteps = 15, apausetik =0;

void mTri(int x ) {
    int w = 23, h = 5+(x%2)*15, y = 30, xx = stepsize*x;
    display.fillTriangle( xx+3, y, xx+w, y+h, xx+w, y-h, SH110X_BLACK);
   
}
void mdrawCircle(int x, int y, int d){ 
    display.fillCircle(x, y, d, SH110X_WHITE);  
    display.display();
}

void drawPac(int x){
      int y = 30;
      int i = x-1;
      mdrawCircle((i)*stepsize, 30, 22);
      display.fillCircle(stepsize*i+7, y-11, 2, SH110X_BLACK);  
      mTri(i); 
      display.display(); 
}
//left, right, steps, stage counter
void pac(){ 
    if(++tt < 100000) return;    //end here
     
      tt = 0;  //draw dots... 
          display.clearDisplay(); 
      if(stage ==0 and aend++ > asteps){
        stage = 1;
        for(int i=astart; i<aend; i++)
        mdrawCircle(11+i*stepsize, 32, 1);
      
      } else if(stage == 1 and astart++ <aend){
          display.clearDisplay(); 
          drawPac(astart);
         for(int i=astart; i<aend; i++)
           mdrawCircle(11+i*stepsize, 32, 1); 
           if(astart>asteps/2) stage = 2;
      }else if(stage == 2){    //pause keep chomping
          if(apausetik++ > asteps/2){ 
              stage = 3;
              apausetik = 0;
          }
          display.clearDisplay(); 
          drawPac(astart);
         for(int i=astart; i<aend; i++)
           mdrawCircle(11+i*stepsize, 32, 1);
           
      } else if(stage == 3 and astart++ <=asteps){    //pause keep chomping
          if(astart>= asteps) stage = 4;
          display.clearDisplay(); 
          drawPac(astart);
         for(int i=astart; i<aend; i++)
           mdrawCircle(11+i*stepsize, 32, 1);
           
      } else if(stage == 4){
         if(apausetik++ > asteps/2){ 
              stage = 5;
              apausetik = 0;
          }
          testdrawoled("ACME", "00", "000", "000");    
      } else if(stage == 5){
         if(apausetik++ > asteps/2){ 
              stage = 6;
              apausetik = 0;
          }
          testdrawoled("TIME", "01", "000", "000");    
      }else if(stage == 6){
         showtime = true;
         astart= 0;
         aend = 1;
         pacd = false;
      }
}
void playPac(){
  pacd = true;
  showtime = false;
  stage = 0;
}

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz)
{
  char buf[32];
  char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
  Serial.println(buf);
}

int formatTime(time_t t, int i){
    int d = 0;
    switch(i){
      case 0: d =  hour(t); break;
      case 1: d =  minute(t); break;
      case 2: d =  second(t); break;
      
      case 3: d =  weekday(t); break;
      case 4: d =  day(t); break;
      case 5: d =  month(t); break;
      case 6: d =  year(t); break; 
     
    }

  return d;
}

String getWeekday(time_t t){
  return dayShortStr( formatTime(t, 3) );
}
String getMonthstr(time_t t){
  return monthShortStr( formatTime(t, 5) );
}

String getDate(time_t t){
    int d = formatTime(t, 4);
    int m = formatTime(t, 5);
    // int y = formatTime(t, 6); 
  // String yy = ""+ y;
  // yy = yy.substring(2);
    char buf[32];
    sprintf(buf, "%.1d/%.2d",d, m);  
    String s(buf);
    return s;
}

String getmTime(time_t t){
  char buf[32];
    int h = formatTime(t, 0);
    int m = formatTime(t, 1);
    if(m == 1 and !pacd) playPac();
    if(h > 12) h %= 12; //24 => 12 hr time
    //remove the : to keep the layout
    if(h > 9)   sprintf(buf, "%.2d%.2d", h, m );  
    else sprintf(buf, "%.1d:%.2d", h, m );  
    String s(buf);
    return s;
}

String getSec(time_t t){
  char buf[32];
    sprintf(buf, "%.2d",  second(t) );  
//    sprintf(buf, "%.3d",  tt/100 );  //todo micro secs
    String s(buf);
    return s;
}

// format and print a time_t value to oled display
void mtime(time_t t)
{ 
    testdrawoled(getmTime(t), getSec(t), getDate(t), getWeekday(t) );  
}
void drawMbox(){
  int xx = 0;
  int yy = 45;
  int ww = 10;
  switch(md){
    case 0: break;
    case 1: xx = 60; break;
    case 2: xx = 0;  yy = 60; break;
    case 3: xx = 12; yy = 60; break;
    case 4: xx = 90; yy = 60; break;
  }
  display.drawRect(xx, yy, ww, 1, SH110X_WHITE);
}
//draw oled/ 2 lines
void testdrawoled( String currentTime, String sec, String date, String day) {
  display.clearDisplay();
  if(at) drawMbox();
  display.setTextSize(5);             // Normal 1:1 pixel scale
  display.setTextColor(SH110X_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(currentTime.substring(0,4) ); //only 4 chars wide on the main line...
    
  display.setTextSize(3); 
  display.setCursor(50,44);             // Start at top-left corner
  display.println(sec);

  display.setTextSize(1); 
  display.setCursor(0,54);             
  display.println(date); 
  display.setCursor(90,54);              
  display.println(day);
  //add day/date in the empty space
  display.display(); 
}

//void testdrawcircle(void) {
//  display.clearDisplay();
//
//  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
//    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE); //fillCircle/drawCircle
//    display.display();
//    delay(1);
//  }
//
//  delay(2000);
//}



void setup()
{
  Serial.begin(115200);
  
//  while (!Serial && millis() < 5000);

  
    // Wire.setSDA(20);
    // Wire.setSCL(21);
    

    Wire1.setSDA(2);
    Wire1.setSCL(3);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for(;;); // Don't proceed, loop forever
  // }
//  display.setRotation(2);
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.


  display.begin(i2c_Address, true); // Address 0x3C default

  display.display();
  delay(100); // Pause for 2 seconds

  display.setContrast(24);
  // Clear the buffer
  display.clearDisplay();
  testdrawoled("ACME", "pico", "000", "000"); //CALLS DISPLAY()
   
  delay(1000);
  display.clearDisplay();  
  mRec();  //sbow title screen

  Serial.print(F("\nStart RP2040_RTC_Time on ")); Serial.println(BOARD_NAME);
  Serial.println(RP2040_RTC_VERSION);
  Serial.println(TIMEZONE_GENERIC_VERSION);

  myTZ = new Timezone(myDST, mySTD);

  // Start the RTC
  rtc_init();

  // setTime();

  attachInterrupt(bpin, cClick, FALLING );  
  attachInterrupt(b1pin, mClick, FALLING );  
  key.ini(); //ini button
  key1.ini(); //ini button
}


void displayTime()
{
  rtc_get_datetime(&currTime);

  // Display time from RTC
  DateTime now = DateTime(currTime);

  time_t utc = now.get_time_t();
  time_t local = myTZ->toLocal(utc, &tcr);

  printDateTime(utc, "UTC");
  printDateTime(local, tcr -> abbrev);
  if(showtime) mtime(local);    //update oled time.
}

void displayRTCTime()
{
  static unsigned long displayRTCTime_timeout = 0;

#define DISPLAY_RTC_INTERVAL     1000L  //10000L = 10 sec update

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to display frequently.
  if ((millis() > displayRTCTime_timeout) || (displayRTCTime_timeout == 0))
  {
    Serial.println("============================");
    displayTime();

    displayRTCTime_timeout = millis() + DISPLAY_RTC_INTERVAL;
  }
}

void loop()
{ 
  if(at){
    adjustTime();
    Serial.println("setting time");
  }
  else{
      pac();
      displayRTCTime();
  }
}
