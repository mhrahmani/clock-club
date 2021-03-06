/*
  Simple digital clock
  Sets the realtime clock on any of the MKR or M0 boards
  using the compile time and compile date.
  Prints the time once per second.
  prints the date once per minute.

  This sketch is designed as the basis for many different clock projects.

  This clock uses an Avago HCMS-2913 display,and the library
  that drives it. You can replace this display with any
  other display library.

  created 23 Nov. 2018
  by Tom Igoe
*/

#include <RTCZero.h>
#include <LedDisplay.h>

// Define pins for the LED display:
const int dataPin = 0;           // connects to the display's data in
const int registerSelect = 1;    // the display's register select pin
const int clockPin = 2;          // the display's clock pin
const int enable = 3;            // the display's chip enable pin
const int reset = 4;             // the display's reset pin
const int displayLength = 8;     // number of characters in the display

// create an instance of the LED display library:
LedDisplay myDisplay = LedDisplay(dataPin, registerSelect, clockPin,
                                  enable, reset, displayLength);
int brightness = 15;        // screen brightness (0-15)

RTCZero rtc;      // instance of the realtime clock
int thisSecond;   // variable to look for seconds chang

void setup() {
  Serial.begin(9600);
  // initialize the display library:
  myDisplay.begin();
  // set the brightness of the display:
  myDisplay.setBrightness(brightness);

  // initialize the realtime clock:
  rtc.begin();
  setTimeFromCompile();
  setDateFromCompile();

  // set alarm to show the date once a minute:
  rtc.setAlarmTime(0, 0, 59);
  rtc.enableAlarm(rtc.MATCH_SS);
  rtc.attachInterrupt(showDate);
}

void loop() {
  if (thisSecond != rtc.getSeconds()) {
    // on the zero second, skip the rest of the loop():
    if (rtc.getSeconds() == 0) return;

    // set the cursor to 0:
    myDisplay.home();
    // print the millis:
    myDisplay.print(getTimeStamp());
    Serial.println(getTimeStamp());

    // save current time for next loop:
    thisSecond = rtc.getSeconds();
  }
}

// realtime clock interrupt service routine:
void showDate() {
  // set the cursor to 0:
  myDisplay.home();
  // print the millis:
  myDisplay.print(getDateStamp());
  Serial.println(getDateStamp());
}

// format the time as hh:mm:ss
String getTimeStamp() {
  String timestamp = "";
  if (rtc.getHours() <= 9) timestamp += "0";
  timestamp += rtc.getHours();
  timestamp += ":";
  if (rtc.getMinutes() <= 9) timestamp += "0";
  timestamp += rtc.getMinutes();
  timestamp += ":";
  if (rtc.getSeconds() <= 9) timestamp += "0";
  timestamp += rtc.getSeconds();
  return timestamp;
}

// format the date as dd:mm:yy
String getDateStamp() {
  String dateStamp = "";
  if (rtc.getDay() <= 9) dateStamp += "0";
  dateStamp += rtc.getDay();
  dateStamp += "-";
  if (rtc.getMonth() <= 9) dateStamp += "0";
  dateStamp += rtc.getMonth();
  dateStamp += "-";
  dateStamp += rtc.getYear();
  return dateStamp;
}

// set the rtc time from the compile time:
void setTimeFromCompile() {
  // get the compile time string:
  String compileTime = String(__TIME__);
  // break the compile time on the colons:
  int h = compileTime.substring(0, 2).toInt();
  int m = compileTime.substring(3, 5).toInt();
  int s = compileTime.substring(6, 8).toInt();

  // set the time from the derived numbers:
  rtc.setTime(h, m, s);
}

// set the rtc time from the compile date:
void setDateFromCompile() {
  String months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
                    };
  // get the compile date:
  String compileDate = String(__DATE__);

  // get the date substring
  String monthStr = compileDate.substring(0, 3);

  int m = 0;    // variable for the date as an integer
  // see which month matches the month string:
  for (int i = 0; i < 12; i++) {
    if (monthStr == months[i]) {
      // the first month is 1, but its array position is 0, so:
      m = i + 1;
      // no need to continue the rest of the for loop:
      break;
    }
  }

  // get the day and year as substrings, and convert to numbers:
  int d = compileDate.substring(4, 6).toInt();
  int y = compileDate.substring(9, 11).toInt();
  // set the date from the derived numbers:
  rtc.setDate(d, m, y);
}
