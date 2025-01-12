/*
  Get the time in seconds since January 1st, 1970.

  The time is retrieved with the WiFi module by fetching the NTP time from an NTP server.

  It requires at least version 0.5.0 of USB Wifi bridge firmware and WiFiS3 library.

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the WiFi.begin() call accordingly.

  created 21 february 2024

*/

#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "RTC.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;             // your network key index number (needed only for WEP)

/// set offsets to GMT from local
#define GMTOffset_hour  1     // # hours difference to GMT
#define DayLightSaving  0     // 1 = daylight saving is active

int status = WL_IDLE_STATUS;

/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  matrix.begin();

  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(100);

  const char text[] = " FBELLSAN - Informatica Arduino UNO r4  ";
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();

  RTC.begin();

   // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed.");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  printWifiStatus();
}

/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */

  unsigned long EpochTime;

  EpochTime = WiFi.getTime();
  IPAddress ip = WiFi.localIP();

  // Make it scroll!
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);

  // add the text
  const char text[] = "    Scrolling text!    ";
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(ip);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
  
  if (EpochTime > 0) {
    UpdateRTC(EpochTime);
  }
  else {
    Serial.println("Error during reading epoch time.");
    Serial.println("Make sure your WiFi firmware version is at least 0.5.0");
  }

  Serial.println();
  delay(10000);
}

/* -------------------------------------------------------------------------- */
void UpdateRTC(time_t EpochTime) {
/* -------------------------------------------------------------------------- */

  auto timeZoneOffsetHours = GMTOffset_hour + DayLightSaving;
  auto unixTime = EpochTime + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime);
  Serial.println("The RTC was just set to: " + String(currentTime));
}

/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
