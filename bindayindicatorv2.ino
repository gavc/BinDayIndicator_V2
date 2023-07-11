#include <ESP8266WiFi.h>          
#include <DNSServer.h>            
#include <ESP8266WebServer.h>     
#include <WiFiManager.h>          
#include <ESP8266HTTPClient.h>    
#include <ezTime.h> // https://github.com/ropg/ezTime
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

Timezone Brisbane;

const char appVersion[] = "20230711_2.0";

// Bin day variables
int binNight;// = 2; // 1 - Sunday, 2 - Monday, 3 - Tuesday...
int referenceGreenWeek = 23; // Reference week for Green Bin, used to calc odd/even weeks.

// Hours to blink the LED between
int startHour;// = 16; // Start blinking a 4p
int endHour;// = 22; //End blinking at 10p

// Led flashing update time
unsigned long previousMillis = 0; 
const long interval = 20000;  // interval at which to blink (milliseconds)
bool colorSwitch = false;    // false for first color, true for second
int ledBrightness;// = 20;         // brightness of the LED (0-255)

// Enum Bin type
enum BinType {
  NOBIN,
  GREEN_AND_RED,
  YELLOW_AND_RED
};

BinType currentBinType = NOBIN; // Start with no bin

// NeoPixel setup
#define LED_PIN    D2
#define LED_COUNT  1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// WebServer
ESP8266WebServer server(80); //Server on port 80

void setup() {
  Serial.begin(115200);

  // Setting EEPROM values
  EEPROM.begin(512);  // Initialize EEPROM with 512 bytes of space

  // These lines are used to set initial values in the EEPROM.
  //EEPROM.write(0, 2);
  //EEPROM.write(1, 16);
  //EEPROM.write(2, 22);
  //EEPROM.write(3, 20);
  //EEPROM.commit();  // Ensure changes are written

  // After setting the initial values, remove or comment out the above lines 
  // to avoid overwriting your saved values every time the device restarts.

  binNight = EEPROM.read(0);
  startHour = EEPROM.read(1);
  endHour = EEPROM.read(2);
  ledBrightness = EEPROM.read(3);

  // LED Activities  
  setLedColor(0, 0, 0); // Turn off the LED
  strip.setBrightness(ledBrightness);

  // WiFi connection setup using WiFiManager
  WiFiManager wifiManager;
  // wifiManager.resetSettings(); // Uncomment to reset settings and run it once.
  wifiManager.autoConnect("AutoConnectAP");
  
  // Print local IP to serial
  Serial.println(WiFi.localIP());

  waitForSync();

	Serial.println("UTC: " + UTC.dateTime());
	
	//Timezone Brisbane;
	Brisbane.setLocation("Australia/Brisbane");
	Serial.println("Brisbane time: " + Brisbane.dateTime()); // Specify your timezone

  // NeoPixel initialization
  strip.begin();

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();

  Serial.println("Setup complete");
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  server.handleClient();

  // Print the current time
  Serial.print("Current time: ");
  Serial.println(Brisbane.dateTime());

  // Print the weekday
  Serial.print("Weekday: ");
  Serial.println(Brisbane.weekday());
  
  Serial.println("-------------------------");

// Get the current time
unsigned short currentHour = Brisbane.hour();

if (Brisbane.weekday() == binNight && currentHour >= startHour && currentHour < endHour) {
    // Call your function here
    checkBinWeek();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        colorSwitch = !colorSwitch;

        // Update the LED color
        updateLedColor();
    }
} else if (Brisbane.weekday() != binNight || currentHour < startHour || currentHour >= endHour) {
    setLedColor(0, 0, 0); // Turn off LED
}

  delay(1000);
}

void checkBinWeek() {
  // Get the current week
  unsigned short currentWeekday = Brisbane.weekday();
  unsigned short currentWeek = Brisbane.weekISO();
  
  // Calculate the difference between the current week and the reference week
  short weekDifference = currentWeek - referenceGreenWeek;

  if(currentWeekday == binNight) {
    if (weekDifference % 2 == 0) { // Use modulus to check if the weekDifference is odd or even
      // It's an even week difference, so trigger the red_green condition
      //red_green();
      currentBinType = GREEN_AND_RED; 
    } else {
      // It's an odd week difference, so trigger the red_yellow condition
      //red_yellow();
      currentBinType = YELLOW_AND_RED; 
    } 
  }
  else {
    currentBinType = NOBIN; // No bin
  }
}

void setLedColor(int red, int green, int blue) {
  strip.setPixelColor(0, strip.Color(red, green, blue)); // Set pixel's color (in RAM)
  strip.show();   // Update strip to match
}

void blinkLed(int red, int green, int blue, int times, int wait) {
  for(int i=0; i<times; i++) {
    setLedColor(red, green, blue); // Turn on the LED
    delay(wait);
    setLedColor(0, 0, 0); // Turn off the LED
    delay(wait);
  }
}

void updateLedColor() {
  if (currentBinType == GREEN_AND_RED) {
    if (colorSwitch) {
      setLedColor(255, 0, 0); // Red
    } else {
      setLedColor(0, 255, 0); // Green
    }
  } else if (currentBinType == YELLOW_AND_RED) {
    if (colorSwitch) {
      setLedColor(255, 0, 0); // Red
    } else {
      setLedColor(255, 255, 0); // Yellow
    }
  }
}

void handleRoot() {
  String html = "<html>\
                  <body>\
                    <h1>Bin Night Indicator </h1>\
                    <small>Version: " ;
html += appVersion;
html += "</small>\
                    <br>\
                    <br>\
                    <style>\
                      span {\
                        min-width: 2em;\
                        display: inline-block;\
                      }\
                    </style>\
                    <form action=\"/set\">\
                    <label for=\"day\">Bin Night:</label>\
                    <select id=\"day\" name=\"day\">\
                      <option value=\"1\""; 
if(binNight == 1) html += " selected"; 
html += ">Sunday</option>\
                      <option value=\"2\"";
if(binNight == 2) html += " selected"; 
html += ">Monday</option>\
                      <option value=\"3\"";
if(binNight == 3) html += " selected"; 
html += ">Tuesday</option>\
                      <option value=\"4\"";
if(binNight == 4) html += " selected"; 
html += ">Wednesday</option>\
                      <option value=\"5\"";
if(binNight == 5) html += " selected"; 
html += ">Thursday</option>\
                      <option value=\"6\"";
if(binNight == 6) html += " selected"; 
html += ">Friday</option>\
                      <option value=\"7\"";
if(binNight == 7) html += " selected"; 
html += ">Saturday</option>\
                    </select>\
                      <br>\
                      <br>\
                      <label for=\"start\">Start Hour: <span id=\"startValue\">";
html += String(startHour);
html += "</span></label>\
                      <input type=\"range\" id=\"start\" name=\"start\" min=\"1\" max=\"24\" value=\"";
html += String(startHour);
html += "\" oninput=\"startValue.innerText = this.value\">\
                      <br>\
                      <br>\
                      <label for=\"end\">End Hour: <span id=\"endValue\">";
html += String(endHour);
html += "</span></label>\
                      <input type=\"range\" id=\"end\" name=\"end\" min=\"1\" max=\"24\" value=\"";
html += String(endHour);
html += "\" oninput=\"endValue.innerText = this.value\">\
                      <br>\
                      <br>\
                        <label for=\"stripbrightness\">Led Brightness:</label>\
                      <select id=\"stripbrightness\" name=\"stripbrightness\">\
                        <option value=\"20\">Low</option>\
                        <option value=\"130\">Medium</option>\
                        <option value=\"200\">High</option>\
                      </select>\
                      <br>\
                      <br>\
                      <input type=\"submit\" value=\"Submit\">\
                    </form>\
                  </body>\
                </html>";
  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.arg("day") != "") {
    binNight = server.arg("day").toInt();
    EEPROM.write(0, binNight);  // Write to EEPROM at address 0
  }
  if (server.arg("start") != "") {
    startHour = server.arg("start").toInt();
    EEPROM.write(1, startHour);  // Write to EEPROM at address 1
  }
  if (server.arg("end") != "") {
    endHour = server.arg("end").toInt();
    EEPROM.write(2, endHour);  // Write to EEPROM at address 2
  }
    if (server.arg("stripbrightness") != "") {
    ledBrightness = server.arg("stripbrightness").toInt();
    EEPROM.write(3, ledBrightness);  // Write to EEPROM at address 2
  }

  EEPROM.commit();  // Ensure changes are written
  
  server.send(200, "text/html", "Values set successfully. Please reset the Wemos to continue. <a href=\"/\">Go Back</a>");
}
