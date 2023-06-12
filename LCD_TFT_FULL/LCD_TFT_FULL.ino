
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//----------------------------------------Link Blynk
#define BLYNK_TEMPLATE_ID "TMPL65cD1TjN_"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "GtNTzrn9yhOlqLVk8Nnpz0AIAsky5VkD"
//----------------------------------------Include Library
#include "SPI.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFiAP.h>
#include <string.h>
#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>
#include <FastLED.h>
#include "bitmap.h"
//----------------------------------------DHT define
#define DHTPIN 32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//----------------------------------------Include pin
#define CS_PIN 13
XPT2046_Touchscreen ts(CS_PIN);
//----------------------------------------Wifi and Blynk
char auth[] = BLYNK_AUTH_TOKEN;
const char *PARAM_INPUT_SSID = "ssid";
const char *PARAM_INPUT_PASSWORD = "password";
String ssid;
String password;
AsyncWebServer server(80);
IPAddress localIP;
//----------------------------------------LED define
#define LED_PIN 15
#define LED_PIN_1 2
#define NUM_LEDS 72
CRGB leds_1[NUM_LEDS];
CRGB leds[NUM_LEDS];
//----------------------------------------BH1750 define
BH1750 lightMeter;
//----------------------------------------Pin configuration and initialization for LCD TFT
#define TFT_DC 17
#define TFT_CS 5
#define led_lcd 16
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//----------------------------------------Cam bien sieu am define
#define trig1 26
#define echo1 27
#define trig2 33
#define echo2 25
//----------------------------------------SR501 define
#define dataSR505 39
//----------------------------------------Cam bien sieu am value
int numHuman;
String queue = "";
int timeoutcounter = 0;
int distanse1;
int distanse2;
int distanseSample = 20;
//----------------------------------------read acs712 (ampe)
#define analogAmpe 34
float amps;
//----------------------------------------read (volt)
#define analogVolt 35
float volt;
// relayLed
#define relayLed 4
int valueRelay;
//----------------------------------------Defines colors
// Assign human-readable names to some common 16-bit color values:
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define DARKORANGE 0xFB60
#define MAROON 0x7800
#define BLACKM 0x18E3
////////////////////////////////////
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
//----------------------------------------Variable for detecting touch screen when touched
#define MINPRESSURE 10
#define MAXPRESSURE 1000
//----------------------------------------Value sample
// value mode button
int mode = 0;       // Start sample value different Normal
int modeBtnOld = 0; // Start sample value different Normal
int modeBtn = 1;    // Start sample value different Normal
// Value for dht
int Humidity;
float Temperature;
float Fahreheit;
float hif;
float hic;
// Value for mode
int brightnessRoom = 250;
int brightnessRoomOld = 0;
int brightnessLed = 30;
//----------------------------------------The x and y points for the Temperature bar
int x_bar_t = 20;
int y_bar_t = 60;
//----------------------------------------The variable to hold the conversion value from the temperature value to the value for the temperature bar
int T_to_Bar;
//----------------------------------------Menu = 0 to display the Main Menu Display, Menu = 1 to control the LED and Menu = 2 to display DHT11 sensor data
int Menu = 0;
//----------------------------------------Variable for the x, y and z points on the touch screen
int TSPointZ;
int x_set_rotatoon_135;
int y_set_rotatoon_135;
//----------------------------------------Millis variable to update the temperature and humidity values
unsigned long previousMillis = 0; //--> will store last time updated
unsigned long previousMillis_Power = 0;
unsigned long previousMillis_WriteDHT = 0;
unsigned long previousMillis_SR505 = 0;

const long interval = 2000; 
// read file
String readFile(fs::FS &fs, const char *path)
{
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("- empty file or failed to open file");
    return String();
  }
  String fileContent;
  while (file.available())
    fileContent = file.readStringUntil('\n');
  return fileContent;
}
// write file
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
    Serial.println("- file written");
  else
    Serial.println("- frite failed");
  file.close();
}
//========================================Connect to wifi
bool initWiFi()
{
  Serial.println("Setting AP (Access Point)");
  WiFi.softAP("Cấu Hình ESP32");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  tft.fillScreen(WHITE);
  tft.drawRGBBitmap(60, 100, loading, 190, 29);
  int col[8];
  col[0] = tft.color565(35, 35, 35);
  col[1] = tft.color565(53, 53, 53);
  col[2] = tft.color565(71, 71, 71);
  col[3] = tft.color565(89, 89, 89);
  col[4] = tft.color565(107, 107, 107);
  col[5] = tft.color565(124, 124, 124);
  col[6] = tft.color565(142, 142, 142);
  col[7] = tft.color565(160, 160, 160);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting to WiFi...");
  int dem = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    for (int i = 8; i > 0; i--)
    {
      tft.fillCircle(150 + 15 * (cos(-(i + 0) * PI / 4)), 160 + 15 * (sin(-(i + 0) * PI / 4)), 3, col[0]);
      delay(10);
      tft.fillCircle(150 + 15 * (cos(-(i + 1) * PI / 4)), 160 + 15 * (sin(-(i + 1) * PI / 4)), 3, col[1]);
      delay(10);
      tft.fillCircle(150 + 15 * (cos(-(i + 2) * PI / 4)), 160 + 15 * (sin(-(i + 2) * PI / 4)), 3, col[2]);
      delay(10);
      tft.fillCircle(150 + 15 * (cos(-(i + 3) * PI / 4)), 160 + 15 * (sin(-(i + 3) * PI / 4)), 3, col[3]);
      delay(10);
      tft.fillCircle(150 + 15 * (cos(-(i + 4) * PI / 4)), 160 + 15 * (sin(-(i + 4) * PI / 4)), 3, col[4]);
      delay(10);
      tft.fillCircle(150 + 15 * (cos(-(i + 5) * PI / 4)), 160 + 15 * (sin(-(i + 5) * PI / 4)), 3, col[5]);
      delay(10);
      tft.fillCircle(150 + 15 * (cos(-(i + 6) * PI / 4)), 160 + 15 * (sin(-(i + 6) * PI / 4)), 3, col[6]);
      delay(10);
      tft.fillCircle(150 + 15 * (cos(-(i + 7) * PI / 4)), 160 + 15 * (sin(-(i + 7) * PI / 4)), 3, col[7]);
      delay(10);
    }
    if (dem > 50)
      break;
    Serial.print(".");
    dem = dem + 1;
  }
  Blynk.begin(auth, ssid.c_str(), password.c_str());
  tft.fillScreen(BLACK);
  Serial.println();
  Serial.print("Successfully connected");
  return true;
}
void updateWifi()
{
  // Upload index2.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index2.html", "text/html"); });
  server.serveStatic("/", SPIFFS, "/");

  // if update ssid , password
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
            { 
      int count = request->params();
      for (int i = 0; i < count; i++)
      {
        AsyncWebParameter* param = request->getParam(i);
        //HTTP POST get value ssid
        if (param->name() == PARAM_INPUT_SSID)
          ssid = param->value();
        writeFile(SPIFFS,"/ssid.txt",ssid.c_str());
        //HTTP POST get value password
        if (param->name() == PARAM_INPUT_PASSWORD)
          password = param->value();
        writeFile(SPIFFS,"/password.txt",password.c_str());
      }
      request->send(200, "text/plain", "Done. ESP will restart.");
      ESP.restart(); });
  server.begin();
}
void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
  else
    Serial.println("SPIFFS mounted successfully");
}
//========================================VOID SETUP()
void setup()
{
  Serial.begin(115200);
  pinMode(led_lcd, OUTPUT);
  pinMode(relayLed, OUTPUT);
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(dataSR505, INPUT);
  digitalWrite(led_lcd, HIGH);
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  FastLED.addLeds<WS2812, LED_PIN, RGB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812, LED_PIN_1, RGB>(leds_1, NUM_LEDS);

  tft.begin();
  tft.setRotation(3);
  ts.begin();
  ts.setRotation(0);

  initSPIFFS();
  // read ssid và pass from file
  ssid = readFile(SPIFFS, "/ssid.txt");
  password = readFile(SPIFFS, "/password.txt");
  Serial.println(ssid);
  Serial.println(password);
  // connect wifi
  initWiFi();
  updateWifi();
  Menu_display();
}
//========================================================================READ BLYNK()
// button brightness room
BLYNK_WRITE(V4)
{
  brightnessRoom = param.asInt();
}
// button mode normal
BLYNK_WRITE(V5)
{
  modeBtn = 1;
}
// button mode read
BLYNK_WRITE(V6)
{
  modeBtn = 2;
}
// button mode sleep
BLYNK_WRITE(V7)
{
  modeBtn = 3;
}
BLYNK_WRITE(V8)
{
  valueRelay = param.asInt();
}
//========================================================================VOID LOOP()
void loop()
{
  // put your main code here, to run repeatedly:
  Blynk.run();

  readNumHuman();
  WriteDHT11Data();
  ShowPower();
  //----------------------------------------Main Menu Display
  if (Menu == 0)
  {
    DrawTempAmountOfPeople(numHuman);
    GetTSPoint();
    //----------------------------------------Conditions for detecting touch screen when touched
    if (TSPointZ > MINPRESSURE && TSPointZ < MAXPRESSURE)
    {

      //----------------------------------------Conditions for detecting when the Button for controlling the LED is touched and its command (Enter the LED controlling menu)
      if (x_set_rotatoon_135 > 17 && x_set_rotatoon_135 < (17 + 280) && y_set_rotatoon_135 > 90 && y_set_rotatoon_135 < (90 + 40))
      {
        Menu = 1;
        DrawButtonControlLEDPress();
        delay(100);
        DrawButtonControlLED();
        delay(100);
        tft.fillScreen(BLACK);
        delay(10);
        DrawTempLux(brightnessRoom);
        DrawButtonTempTru();
        DrawButtonTempCong();
        DrawButtonTempNormal();
        DrawButtonTempRead();
        DrawButtonTempSleep();
        DrawMode();
        DrawButtonBack(10, 200);
      }
      //----------------------------------------Condition to detect when the button to display DHT11 sensor data is touched and the command (Enter the menu displays DHT11 sensor data)
      if (x_set_rotatoon_135 > 17 && x_set_rotatoon_135 < (17 + 280) && y_set_rotatoon_135 > 160 && y_set_rotatoon_135 < (160 + 40))
      {
        Menu = 2;
        DrawButtonTempHumPress();
        delay(100);
        DrawButtonTempHum();
        delay(100);
        tft.fillScreen(BLACK);
        delay(10);
        tft.drawLine(15, 40, 300, 40, MAROON);
        tft.drawLine(15, 39, 300, 39, MAROON);

        tft.setTextSize(2);
        tft.setTextColor(BLUE);
        tft.setCursor(40, 13);
        tft.print("Temperature & Humidity");

        draw_bar(x_bar_t, y_bar_t);

        tft.drawLine(190, 60, 190, 227, MAROON);
        tft.drawLine(190, 127, 300, 127, MAROON);

        tft.fillRect(202, 60, 100, 27, CYAN);
        tft.setTextSize(2);
        tft.setTextColor(BLACK);
        tft.setCursor(205, 65);
        tft.print("Humidity");

        tft.fillRect(202, 140, 100, 43, GREEN);
        tft.setTextSize(2);
        tft.setTextColor(BLACK);
        tft.setCursor(227, 145);
        tft.print("Heat");
        tft.setCursor(220, 165);
        tft.print("Index");

        DrawButtonBack(8, 6);

        GetDHT11Data();
        delay(100);
      }
    }
  }

  //----------------------------------------Menu or Mode to control the LED
  if (Menu == 1)
  {
    ControlTheLED();
  }
  //----------------------------------------

  //----------------------------------------Menu or Mode to display DHT11 sensor data
  if (Menu == 2)
  {
    ShowDHT11Data();
  }
  //----------------------------------------
  if (brightnessRoomOld != brightnessRoom)
  {
    Blynk.virtualWrite(V4, brightnessRoom);
    if (Menu == 1)
      DrawTempLux(brightnessRoom);
    brightnessRoomOld = brightnessRoom;
  }
  Serial.println(modeBtn);
  if (numHuman == 0)
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  else
  {
    brightnessMode(modeBtn);
  }
  SR505();
  ControlButtonMode(modeBtn);
  updateRelayLed(valueRelay);
}
//========================================================================

//========================================================================GetTSPoint()
void GetTSPoint()
{
  TS_Point p = ts.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
  y_set_rotatoon_135 = map(p.x, 0, 240, 0, tft.height());
  x_set_rotatoon_135 = map(tft.width() - p.y, 0, 320, 0, tft.width());
  TSPointZ = p.z;
}
//========================================================================DrawButtonTempHum()
void DrawButtonTempHum()
{
  tft.fillRoundRect(17, 160, 280, 40, 10, WHITE);
  tft.fillRoundRect(19, 162, 276, 36, 10, BLUE);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 173);
  tft.print("Temperature & Humidity");
}
//========================================================================

//========================================================================DrawButtonTempHumPress()
void DrawButtonTempHumPress()
{
  tft.fillRoundRect(17, 160, 280, 40, 10, BLACKM);
}
//========================================================================

//========================================================================DrawButtonBack(x, y)
void DrawButtonBack(int x_btn_back, int y_btn_back)
{
  tft.fillRoundRect(x_btn_back, y_btn_back, 30, 30, 5, MAROON);
  tft.fillRoundRect(x_btn_back + 2, y_btn_back + 2, 26, 26, 5, YELLOW);
  tft.setTextSize(2);
  tft.setTextColor(BLACKM);
  tft.setCursor(x_btn_back + 7, y_btn_back + 7);
  tft.print("<");
}
//========================================================================

//========================================================================DrawButtonBackPress(x, y)
void DrawButtonBackPress(int x_btn_back, int y_btn_back)
{
  tft.fillRoundRect(x_btn_back, y_btn_back, 30, 30, 5, BLACKM);
}
//========================================================================

//========================================================================GetDHT11Data()
void GetDHT11Data()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  Humidity = dht.readHumidity();
  ;
  // Read temperature as Celsius (the default)
  Temperature = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  Fahreheit = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(Humidity) || isnan(Temperature) || isnan(Fahreheit))
  {

    return;
  }

  // Compute heat index in Fahrenheit (the default)
  hif = dht.computeHeatIndex(Fahreheit, Humidity);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(Temperature, Humidity, false);
  Blynk.virtualWrite(V0, Temperature);
  Blynk.virtualWrite(V1, Humidity);
  Serial.print("\n");
  Serial.print("Humidity: " + String(Humidity) + "%");
  Serial.print("\t");
  Serial.print("Temperature:" + String(Temperature) + " C");
}
//========================================================================

//========================================================================draw_bar (Temperature Bar)
void draw_bar(int x_bar, int y_bar)
{
  tft.fillRoundRect(x_bar, y_bar, 35, 120, 5, DARKORANGE);
  tft.fillCircle(x_bar + 17, y_bar + 140, 30, DARKORANGE);
  tft.fillRoundRect(x_bar + 4, y_bar + 4, 27, 120, 2, BLACKM);
  tft.fillCircle(x_bar + 17, y_bar + 140, 25, BLACKM);
  tft.fillRect(x_bar + 8, y_bar + 8, 19, 120, DARKORANGE);
  tft.fillCircle(x_bar + 17, y_bar + 140, 21, DARKORANGE);

  // tft.fillRect(41, 58, 19, 108, RED);

  tft.drawLine(x_bar + 37, y_bar + 8, x_bar + 42, y_bar + 8, RED);
  tft.setTextSize(1);
  tft.setTextColor(RED);
  tft.setCursor(x_bar + 47, y_bar + 4);
  tft.println("50");

  tft.drawLine(x_bar + 37, y_bar + 115, x_bar + 42, y_bar + 115, RED);
  tft.setCursor(x_bar + 47, y_bar + 111);
  tft.println("0");
}
//========================================================================

//========================================================================Menu_display()
void Menu_display()
{
  tft.fillScreen(BLACK);
  tft.setTextSize(3);
  DrawButtonControlLED();
  DrawButtonTempHum();
}
//========================================================================

//========================================================================ControlTheLED()
void ControlTheLED()
{
  GetTSPoint();

  if (TSPointZ > MINPRESSURE && TSPointZ < MAXPRESSURE)
  {
    if (x_set_rotatoon_135 > 0 && x_set_rotatoon_135 < (0 + 50) && y_set_rotatoon_135 > 26 && y_set_rotatoon_135 < (26 + 60))
    {
      brightnessRoom -= 10;
      DrawButtonTempTruPress();
      delay(100);
      DrawButtonTempTru();
    }
    //----------------------------------------Condition to detect when the Cong Button is touched and the command
    if (x_set_rotatoon_135 > 260 && x_set_rotatoon_135 < (260 + 60) && y_set_rotatoon_135 > 26 && y_set_rotatoon_135 < (26 + 60))
    {
      brightnessRoom += 10;
      DrawButtonTempCongPress();
      delay(100);
      DrawButtonTempCong();
    }
    //----------------------------------------Condition to detect when the Normal Button is touched and the command
    if (x_set_rotatoon_135 > 35 && x_set_rotatoon_135 < (35 + 60) && y_set_rotatoon_135 > 120 && y_set_rotatoon_135 < (120 + 60))
    {
      modeBtn = 1;
      DrawButtonTempNormalPress();
      delay(100);
      DrawButtonTempNormal();
    }
    //----------------------------------------Condition to detect when the Read Button is touched and the command
    if (x_set_rotatoon_135 > 130 && x_set_rotatoon_135 < (130 + 60) && y_set_rotatoon_135 > 120 && y_set_rotatoon_135 < (120 + 60))
    {
      modeBtn = 2;
      DrawButtonTempReadPress();
      delay(100);
      DrawButtonTempRead();
    }
    //----------------------------------------Condition to detect when the Sleep Button is touched and the command
    if (x_set_rotatoon_135 > 225 && x_set_rotatoon_135 < (225 + 60) && y_set_rotatoon_135 > 120 && y_set_rotatoon_135 < (120 + 60))
    {
      modeBtn = 3;
      DrawButtonTempSleepPress();
      delay(100);
      DrawButtonTempSleep();
    }
    //---------------------------------------
  }

  if (mode != modeBtn)
  {
    tft.fillRect(165, 210, 80, 30, BLACK);
    mode = modeBtn;
  }
  if (brightnessRoomOld != brightnessRoom)
  {
    Blynk.virtualWrite(V4, brightnessRoom);
    DrawTempLux(brightnessRoom);
    brightnessRoomOld = brightnessRoom;
  }
  DrawTempMode(modeBtn);
  GetTSPoint();

  if (TSPointZ > MINPRESSURE && TSPointZ < MAXPRESSURE)
  {
    if (x_set_rotatoon_135 > 10 && x_set_rotatoon_135 < (5 + 45) && y_set_rotatoon_135 > 195 && y_set_rotatoon_135 < (195 + 45))
    {
      Menu = 0;
      DrawButtonBackPress(10, 200);
      delay(100);
      DrawButtonBack(10, 200);
      // delay(100);
      Menu_display();
    }
  }
}
//========================================================================

//========================================================================ShowDHT11Data()
void ShowDHT11Data()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    GetDHT11Data();
  }

  T_to_Bar = map(Temperature, 0.0, 50.0, 108, 0);

  tft.fillRect(x_bar_t + 8, (y_bar_t + 8) + T_to_Bar, 19, 108 - T_to_Bar, ORANGE);
  tft.fillRect(x_bar_t + 8, y_bar_t + 8, 19, T_to_Bar, BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ORANGE, BLACK);
  tft.setCursor(75, 100);
  tft.print(Temperature);
  if (Temperature < 10)
    tft.print(" ");
  tft.setCursor(160, 100);
  tft.print((char)247);
  tft.println("C");

  tft.setCursor(75, 135);
  tft.print(Fahreheit);
  if (Fahreheit < 100)
    tft.print(" ");
  tft.setCursor(160, 135);
  tft.print((char)247);
  tft.println("F");

  tft.setTextSize(3);
  tft.setTextColor(CYAN, BLACK);
  tft.setCursor(205, 95);
  tft.print(Humidity);
  tft.print(" %");

  tft.setTextSize(1);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(205, 200);
  tft.print(hic);
  tft.print(" ");
  tft.print((char)247);
  tft.print("C");
  if (hic < 10)
    tft.print(" ");

  tft.setTextSize(1);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(205, 220);
  tft.print(hif);
  tft.print(" ");
  tft.print((char)247);
  tft.print("F");
  if (hif < 100)
    tft.print(" ");

  GetTSPoint();

  if (TSPointZ > MINPRESSURE && TSPointZ < MAXPRESSURE)
  {
    if (x_set_rotatoon_135 > 3 && x_set_rotatoon_135 < (3 + 50) && y_set_rotatoon_135 > 1 && y_set_rotatoon_135 < (1 + 50))
    {
      Menu = 0;
      DrawButtonBackPress(8, 6);
      delay(100);
      DrawButtonBack(8, 6);
      // delay(100);
      Menu_display();
    }
  }
}
void WriteDHT11Data()
{
  unsigned long currentMillis_WriteDHT = millis();
  if (currentMillis_WriteDHT - previousMillis_WriteDHT >= interval)
  {
    // save the last time you blinked the LED
    previousMillis_WriteDHT = currentMillis_WriteDHT;
    GetDHT11Data();
  }
}
//========================================================================
void DrawTempLux(int lux)
{
  int px_x = map(lux, 0, 1000, 0, 216);
  tft.fillRoundRect(50, 36, 220, 40, 10, WHITE);
  tft.fillRoundRect(52, 38, px_x, 36, 10, BLUE);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.setCursor(120, 49);
  tft.print(lux);
  tft.setCursor(170, 49);
  tft.print("lux");
}
void DrawButtonTempTru()
{
  tft.fillRoundRect(10, 41, 30, 30, 5, YELLOW);
  // tft.fillRoundRect(52, 38, 216, 36, 10, BLUE);
  tft.setTextSize(2);
  tft.setTextColor(BLACKM);
  tft.setCursor(20, 50);
  tft.print("-");
}
void DrawButtonTempCong()
{
  tft.fillRoundRect(280, 41, 30, 30, 5, YELLOW);
  // tft.fillRoundRect(52, 38, 216, 36, 10, BLUE);
  tft.setTextSize(3);
  tft.setTextColor(BLACKM);
  tft.setCursor(288, 46);
  tft.print("+");
}
void DrawButtonTempNormal()
{
  tft.fillRoundRect(35, 120, 60, 60, 10, WHITE);
  tft.fillRoundRect(37, 122, 56, 56, 10, RED);
  tft.setTextSize(1);
  tft.setTextColor(BLACKM);
  tft.setCursor(47, 146);
  tft.print("Normal");
}
void DrawButtonTempRead()
{
  tft.fillRoundRect(130, 120, 60, 60, 10, WHITE);
  tft.fillRoundRect(132, 122, 56, 56, 10, YELLOW);
  tft.setTextSize(1);
  tft.setTextColor(BLACKM);
  tft.setCursor(145, 146);
  tft.print("Read");
}
void DrawButtonTempSleep()
{
  tft.fillRoundRect(225, 120, 60, 60, 10, WHITE);
  tft.fillRoundRect(227, 122, 56, 56, 10, BLUE);
  tft.setTextSize(1);
  tft.setTextColor(BLACKM);
  tft.setCursor(240, 146);
  tft.print("Sleep");
}
void DrawMode()
{
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(95, 210);
  tft.print("Mode:");
}
void DrawTempMode(int mode)
{
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  if (mode == 1)
  {
    tft.setCursor(165, 210);
    tft.print("Normal");
    brightnessMode(1);
    Blynk.virtualWrite(V5, 1);
  }
  if (mode == 2)
  {
    tft.setCursor(165, 210);
    tft.print("Read");
    brightnessMode(2);
    Blynk.virtualWrite(V6, 1);
  }
  if (mode == 3)
  {
    tft.setCursor(165, 210);
    tft.print("Sleep");
    brightnessMode(3);
    Blynk.virtualWrite(V7, 1);
  }
}
void DrawTempAmountOfPeople(int numHuman)
{
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(45, 20);
  tft.print("Amount of people: ");
  if (numHuman < 10)
    tft.print(" ");
  tft.print(numHuman);
}
void DrawButtonControlLED()
{
  tft.fillRoundRect(17, 90, 280, 40, 10, WHITE);
  tft.fillRoundRect(19, 92, 276, 36, 10, GREEN);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(65, 103);
  tft.print("Control the LED");
}
//========================================================================

//========================================================================DrawButtonControlLEDPress()
void DrawButtonControlLEDPress()
{
  tft.fillRoundRect(17, 90, 280, 40, 10, BLACKM);
}
void DrawButtonTempTruPress()
{
  tft.fillRoundRect(10, 41, 30, 30, 5, BLACKM);
}
void DrawButtonTempCongPress()
{
  tft.fillRoundRect(280, 41, 30, 30, 5, BLACKM);
}
void DrawButtonTempNormalPress()
{
  tft.fillRoundRect(35, 120, 60, 60, 10, BLACKM);
}
void DrawButtonTempReadPress()
{
  tft.fillRoundRect(130, 120, 60, 60, 10, BLACKM);
}
void DrawButtonTempSleepPress()
{
  tft.fillRoundRect(225, 120, 60, 60, 10, BLACKM);
}
void readNumHuman()
{
  distanse1 = 0;
  distanse2 = 0;
  measureDistanse1();
  measureDistanse2();
  if (distanse1 < distanseSample && queue.charAt(0) != '1')
  {
    queue += "1";
  }
  else if (distanse2 < distanseSample && queue.charAt(0) != '2')
  {
    queue += "2";
  }
  if (queue.equals("12"))
  {
    numHuman++;
    Serial.print("queue: ");
    Serial.println(queue);
    queue = "";
  }
  else if (queue.equals("21") && numHuman > 0)
  {
    numHuman--;
    Serial.print("Hang doi: ");
    Serial.println(queue);
    queue = "";
  }
  if (queue.length() > 2 || queue.equals("11") || queue.equals("22") || timeoutcounter > 100)
  {
    queue = "";
  }
  if (queue.length() == 1)
  {
    timeoutcounter++;
  }
  else
  {
    timeoutcounter = 0;
  }
  Blynk.virtualWrite(V3, numHuman);
}
void measureDistanse1()
{
  digitalWrite(trig1, LOW);
  delayMicroseconds(2);
  digitalWrite(trig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1, LOW);
  unsigned long time1 = pulseIn(echo1, HIGH);
  distanse1 = int(time1 / 2 / 29.412);
}
void measureDistanse2()
{
  digitalWrite(trig2, LOW);
  delayMicroseconds(2);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);
  unsigned long time2 = pulseIn(echo2, HIGH);
  distanse2 = int(time2 / 2 / 29.412);
}
void ShowPower()
{
  unsigned long currentMillis_Power = millis();
  if (currentMillis_Power - previousMillis_Power >= interval)
  {
    previousMillis_Power = currentMillis_Power;
    readPower();
  }
  if (Menu == 0)
  {
    DrawTempPower(amps, volt);
  }
  Blynk.virtualWrite(V2, amps * volt);
}
void DrawTempPower(float amps, float volt)
{
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(15, 57);
  tft.print("Power: ");
  tft.print(volt);
  tft.print("V");
  tft.print(" ");
  tft.print(amps);
  tft.print("A");
  tft.print(" ");
  tft.print(amps * volt);
  tft.print("W");
}
void readPower()
{
  int rawValue = analogRead(analogAmpe);
  float voltage = (rawValue / 4096.0) * 3300;
  amps = ((2500 - voltage) / 66);
  int aVolt = analogRead(analogVolt);
  volt = (((aVolt - 1024) * 3.3) * 2) / 4096.0;
  if (amps < 0)
  {
    amps = 0;
  }
  if (volt < 0)
  {
    volt = 0;
  }
}
void ledColor(int modeColor, int Brightness)
{
  if (modeColor == 1)
  {
    FastLED.setBrightness(Brightness);
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
  }
  if (modeColor == 2)
  {
    FastLED.setBrightness(Brightness);
    fill_solid(leds, NUM_LEDS, CRGB::SaddleBrown);
    FastLED.show();
  }
  if (modeColor == 3)
  {
    FastLED.setBrightness(Brightness);
    fill_solid(leds, NUM_LEDS, CRGB::Goldenrod1);
    FastLED.show();
  }
}
void brightnessMode(int numColor)
{
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  if (lux < (brightnessRoom - 10) || lux > (brightnessRoom + 10))
  {

    if (brightnessLed == 0)
      brightnessLed = 1;
    if (brightnessLed == 255)
      brightnessLed = 254;
    if (lux < brightnessRoom)
    {
      brightnessLed++;
    }
    if (lux > brightnessRoom)
    {
      brightnessLed--;
    }
    Serial.println(brightnessLed);
  }
  ledColor(numColor, brightnessLed);
  Serial.println(brightnessRoom);
}
void ControlButtonMode(int modeBtn)
{
  if (modeBtnOld != modeBtn)
  {
    if (modeBtn == 1)
    {
      Blynk.virtualWrite(V5, 1);
      Blynk.virtualWrite(V6, 0);
      Blynk.virtualWrite(V7, 0);
    }
    if (modeBtn == 2)
    {
      Blynk.virtualWrite(V5, 0);
      Blynk.virtualWrite(V7, 0);
    }
    if (modeBtn == 3)
    {
      Blynk.virtualWrite(V5, 0);
      Blynk.virtualWrite(V6, 0);
    }
    modeBtnOld = modeBtn;
  }
}
void SR505()
{
  int readDataSR505 = 0;
  unsigned long currentMillis_SR505 = millis();
  if (digitalRead(dataSR505) == 1)
  {
    readDataSR505 = 1;
    previousMillis_SR505 = millis();
  }
  if (currentMillis_SR505 - previousMillis_SR505 >= 10000)
  {
    previousMillis_SR505 = currentMillis_SR505;
    readDataSR505 = 0;
  }
  if (readDataSR505 == 1)
  {
    fill_solid(leds_1, NUM_LEDS, CRGB::White);
    FastLED.show();
    Serial.println("ON");
  }
  else
  {
    fill_solid(leds_1, NUM_LEDS, CRGB::Black);
    FastLED.show();
    Serial.println("OFF");
  }
}
void updateRelayLed(int valueRelay)
{
  if (valueRelay == 1)
  {
    digitalWrite(relayLed, HIGH);
    Serial.println("ONLED");
  }
  else
  {
    digitalWrite(relayLed, LOW);
    Serial.println("OFFLED");
  }
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<