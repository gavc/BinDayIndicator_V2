# BinDayIndicator_V2

V2 of the BinDayIndicator - Wemos D1 Mini and RGB Shield to show which bins to be put out.

Using a:

WemosD1 Mini: https://www.aliexpress.com/item/32529101036.html

WS2812B Shield: https://www.aliexpress.com/item/1005003449908799.html

Red, Yellow, Green Bin Indicator.

Essentially the idea is that the night before the LED starts to flash either red/green or red/yellow to indicate which bins need to go out that night. Flashing only takes place between the hours you set in the webpage.

Some default values are set, but you can just update them via the webpage.

binNight = 2; // 1 - Sunday, 2 - Monday...
int startHour = 17; // Start blinking at 4p
int endHour = 20; // End blinking at 10p
int ledBrightness = 20 // strip.brightness

Set the reference week to calculate if its a Green or Yellow week, its currently not set via the webpage and is still hardcoded...

int referenceGreenWeek = 23; // Green bin reference week.
