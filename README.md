# BinDayIndicator_V2

V2 of the BinDayIndicator - Wemos D1 Mini and RGB Shield to show which bins to be put out.

Using a:

WemosD1 Mini: https://www.aliexpress.com/item/32529101036.html

WS2812B Shield: https://www.aliexpress.com/item/1005003449908799.html

Red, Yellow, Green Bin Indicator.

Essentially the idea is that the night before the LED starts to flash either red/green or red/yellow to indicate which bins need to go out that night. Flashing only takes place between the hours you set in the webpage.

Some default values are set via EEPROM, or just save them via the webpage for the first time.

  // These lines are used to set initial values in the EEPROM.
  //EEPROM.write(0, 2); // Bin Night- 1 - Sun, 2 - Mon, 3 - Tue...
  //EEPROM.write(1, 16); // Start blinking at 4p
  //EEPROM.write(2, 22); // Stop blinking at 10p
  //EEPROM.write(3, 20); // Some hardcoded value as per the web page - essentially - Low, Medium, High
  //EEPROM.write(4, 23); // referenceGreenWeek - This is used to calculate if its a Yellow or Green Week - Set once via the web page and essentially calcs if its an odd or even week, and hence if its yellow or green bin night.
  //EEPROM.commit();  // Ensure changes are written

Todo: Add OTA updater...
