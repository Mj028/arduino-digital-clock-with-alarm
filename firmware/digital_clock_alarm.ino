#include <ThreeWire.h>
#include <RtcDS1302.h>

// ======================================
// 1. PIN CONFIGURATION
// ======================================

// RTC pins
const int DAT_PIN = 4;
const int CLK_PIN = 5;
const int RST_PIN = 3;

// Buttons
const int hourButton = A3;
const int minuteButton = A4;

// Buzzer
const int buzzerPin = A5;

// Display segments: A, B, C, D, E, F, G, DP
const int segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};

// Display digit controls: left to right
const int digitPins[] = {2, A0, A1, A2};


// ======================================
// 2. RTC INITIALIZATION
// ======================================

ThreeWire myWire(DAT_PIN, CLK_PIN, RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);


// ======================================
// 3. ALARM VARIABLES
// ======================================

int alarmHour = 6;
int alarmMinute = 30;

bool settingAlarm = false;
bool alarmRinging = false;

unsigned long alarmStartTime = 0;

const unsigned long alarmDuration = 10000;


// ======================================
// 4. BUTTON VARIABLES
// ======================================

bool lastHourButton = HIGH;
bool lastMinuteButton = HIGH;

unsigned long bothButtonsStart = 0;

bool bothButtonsWerePressed = false;
bool modeChanged = false;
bool suppressButtonsUntilRelease = false;

const unsigned long modeHoldTime = 1500;


// ======================================
// 5. DISPLAY VARIABLES
// ======================================

int displayedDigits[4] = {0, 0, 0, 0};

unsigned long lastBlinkTime = 0;

bool separatorOn = true;


// ======================================
// 6. NUMBER PATTERNS
// ======================================

// Segment order: A, B, C, D, E, F, G
// 1 = ON, 0 = OFF

const byte numbers[10][7] = {

  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9

};


// ======================================
// 7. TURN OFF ALL DISPLAY DIGITS
// ======================================

void allDigitsOff() {

  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW);
  }

}


// ======================================
// 8. DISPLAY ONE DIGIT
// ======================================

void showDigit(int position, int number) {

  allDigitsOff();

  // Set seven segments
  for (int s = 0; s < 7; s++) {

    digitalWrite(
      segmentPins[s],
      numbers[number][s] ? HIGH : LOW
    );

  }

  // Decimal point indicates the mode
  if (position == 1 && (settingAlarm || separatorOn)) {

    digitalWrite(segmentPins[7], HIGH);

  } else {

    digitalWrite(segmentPins[7], LOW);

  }

  // Enable selected digit
  digitalWrite(digitPins[position], HIGH);

}


// ======================================
// 9. SETUP
// ======================================

void setup() {

  // Configure display segments
  for (int i = 0; i < 8; i++) {

    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);

  }

  // Configure digit controls
  for (int i = 0; i < 4; i++) {

    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);

  }

  // Initialize RTC
  Rtc.Begin();

  Rtc.SetIsWriteProtected(false);
  Rtc.SetIsRunning(true);

  // Configure buttons
  pinMode(hourButton, INPUT_PULLUP);
  pinMode(minuteButton, INPUT_PULLUP);

  // Configure buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

}


// ======================================
// 10. MAIN LOOP
// ======================================

void loop() {

  // Read RTC time
  RtcDateTime now = Rtc.GetDateTime();

  unsigned long currentMillis = millis();


  // ====================================
  // BUTTON DETECTION
  // ====================================

  bool currentHourButton = digitalRead(hourButton);
  bool currentMinuteButton = digitalRead(minuteButton);

  bool bothPressed =
    (currentHourButton == LOW &&
     currentMinuteButton == LOW);

  bool hourJustPressed =
    (lastHourButton == HIGH &&
     currentHourButton == LOW);

  bool minuteJustPressed =
    (lastMinuteButton == HIGH &&
     currentMinuteButton == LOW);


  // ====================================
  // SILENCE RINGING ALARM
  // ====================================

  if (alarmRinging &&
      (hourJustPressed || minuteJustPressed)) {

    alarmRinging = false;
    digitalWrite(buzzerPin, LOW);

    suppressButtonsUntilRelease = true;

  }


  // ====================================
  // DETECT BOTH BUTTONS HELD
  // ====================================

  if (bothPressed && !bothButtonsWerePressed) {

    bothButtonsStart = currentMillis;

    bothButtonsWerePressed = true;

  }

  if (!bothPressed) {

    bothButtonsWerePressed = false;

  }

  if (bothPressed &&
      !modeChanged &&
      currentMillis - bothButtonsStart >= modeHoldTime) {

    settingAlarm = !settingAlarm;

    modeChanged = true;

    suppressButtonsUntilRelease = true;

    // Confirmation beep
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);

  }

  if (!bothPressed) {

    modeChanged = false;

  }


  // ====================================
  // HANDLE INDIVIDUAL BUTTON PRESSES
  // ====================================

  if (suppressButtonsUntilRelease) {

    if (currentHourButton == HIGH &&
        currentMinuteButton == HIGH) {

      suppressButtonsUntilRelease = false;

    }

  } else if (!bothPressed) {

    // Hour button
    if (hourJustPressed) {

      if (settingAlarm) {

        alarmHour = (alarmHour + 1) % 24;

      } else {

        int newHour = (now.Hour() + 1) % 24;

        RtcDateTime updated(
          now.Year(),
          now.Month(),
          now.Day(),
          newHour,
          now.Minute(),
          now.Second()
        );

        Rtc.SetDateTime(updated);
        now = updated;

      }

    }

    // Minute button
    if (minuteJustPressed) {

      if (settingAlarm) {

        alarmMinute = (alarmMinute + 1) % 60;

      } else {

        int newMinute = (now.Minute() + 1) % 60;

        RtcDateTime updated(
          now.Year(),
          now.Month(),
          now.Day(),
          now.Hour(),
          newMinute,
          now.Second()
        );

        Rtc.SetDateTime(updated);
        now = updated;

      }

    }

  }

  lastHourButton = currentHourButton;
  lastMinuteButton = currentMinuteButton;


  // ====================================
  // ALARM TRIGGER
  // ====================================

  static uint32_t lastAlarmMinuteKey = 0xFFFFFFFFUL;

  uint32_t minuteKey =
    (uint32_t)now.TotalSeconds() / 60UL;

  if (!settingAlarm &&
      !alarmRinging &&
      now.Hour() == alarmHour &&
      now.Minute() == alarmMinute &&
      minuteKey != lastAlarmMinuteKey) {

    alarmRinging = true;

    alarmStartTime = millis();

    lastAlarmMinuteKey = minuteKey;

  }


  // ====================================
  // ALARM DURATION
  // ====================================

  if (alarmRinging) {

    if (millis() - alarmStartTime >= alarmDuration) {

      alarmRinging = false;

    }

  }

  digitalWrite(
    buzzerPin,
    alarmRinging ? HIGH : LOW
  );


  // ====================================
  // DECIMAL POINT BLINKING
  // ====================================

  if (millis() - lastBlinkTime >= 1000) {

    separatorOn = !separatorOn;

    lastBlinkTime = millis();

  }


  // ====================================
  // SELECT DISPLAY TIME
  // ====================================

  int hour;
  int minute;

  if (settingAlarm) {

    hour = alarmHour;
    minute = alarmMinute;

  } else {

    hour = now.Hour();
    minute = now.Minute();

  }


  // ====================================
  // CONVERT TIME INTO FOUR DIGITS
  // ====================================

  displayedDigits[0] = hour / 10;
  displayedDigits[1] = hour % 10;

  displayedDigits[2] = minute / 10;
  displayedDigits[3] = minute % 10;


  // ====================================
  // REFRESH DISPLAY
  // ====================================

  for (int repeat = 0; repeat < 2; repeat++) {

    for (int position = 0; position < 4; position++) {

      showDigit(position, displayedDigits[position]);

      delay(3);

    }

  }

}
