#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "telenet-34090"; //embed    //Verander
const char* password = "BfBv60WUspH3";  // weareincontrol //verander

const char* mqttServer = "192.168.0.235";  //verander
const int mqttPort = 1883;
const char* mqttUser = "robinpi";
const char* mqttPassword = "robindevoecht";
const char* clientID = "client_livingroom";  // MQTT client ID

const char* moisture_topic = "home/livingroom/moisture";
const char* groundTemp_topic = "home/livingroom/bodemTemp";
const char* Temp_topic = "home/livingroom/Temp";
const char* Light_topic = "home/livingroom/Light";
const char* waterLevel_topic = "home/livingroom/waterLevel";

WiFiClient espClient;
PubSubClient client(espClient);


// ds18b20
const int oneWireBus = 5;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

//BME280
#define BME_SCK 12
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

Adafruit_BME280 bme;
// Led Strip
#define pin 18
#define NUM_leds 30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_leds, pin, NEO_GRB + NEO_KHZ800);
//LCD
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
//WaterLevelSensor
const int sensorPin = 27;
const int waterLevelThreshold = 800;
const int lowLevelThreshold = 500;
const int ledPin = 14;

//LDR
const int ldrPin = 34;

// Bodemvochtsensor
const int Bodemvocht = 35;
int bodemValue = 0;

//relay
const int VentilatorPin = 25;
const int WaterPompPin = 26;
const int BodemvochtTreshold = 1400;
const int TemperatuurTreshold = 21;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientID, mqttUser, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);

  Serial.println("Initializing BME280...");
  Wire.begin();
  bool status;
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("could not find BME280");
    while (1)
      ;
  }
  Serial.println("Default test");
  Serial.println();

  //Initialisatie
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);

  //LCD initialiseren
  lcd.clear();
  lcd.init();
  lcd.backlight();

  //LED initialisatie
  strip.begin();
  strip.show();

  //ds18b20
  sensors.begin();
  // Relay
  pinMode(VentilatorPin, OUTPUT);
  pinMode(WaterPompPin, OUTPUT);

  digitalWrite(VentilatorPin, LOW);
  digitalWrite(WaterPompPin, LOW);
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  //Call all functions
  client.loop();
  delay(1000);
  ReadWaterLevel();
  ReadLDR();
  delay(2000);
  ReadBME280();
  delay(2000);
  ReadDS18B20();
  delay(2000);
  ReadBodemVocht();
  delay(2000);
}

void ReadWaterLevel() {
  int sensorValue = analogRead(sensorPin);  //


  if (sensorValue >= waterLevelThreshold) {
    Serial.println("Water detected!");  //
    //stuur waarde door naar database
    client.publish(waterLevel_topic, String(waterLevelThreshold).c_str());
    // Water wordt getedecteer en ledlampje zal uitgaan
    digitalWrite(ledPin, LOW);
  } else if (sensorValue < lowLevelThreshold) {
    Serial.println("Warning: Low water level detected!");
    // LEDlamp aan wanneer water level laag is
    digitalWrite(ledPin, HIGH);
    //stuur waarde door naar database
    client.publish(waterLevel_topic, String(lowLevelThreshold).c_str());
  }

  delay(1000);
}
void ReadLDR() {
  // Waarde wordt opgemeten en geprint op lcd scherm.
  int ldrValue = analogRead(ldrPin);
  lcd.clear();
  Serial.print("Lichtintensiteit: ");
  Serial.println(ldrValue);
  lcd.setCursor(0, 0);
  lcd.print("Licht: ");
  lcd.print(ldrValue);
  //Waarde wordt doorgestuurd naar database
  client.publish(Light_topic, String(ldrValue).c_str());



//Wanneer er te weinig licht is, zal de ledstrip aan springen
  if (ldrValue < 1500) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(246, 205, 139));  // stel een bepaalde kleur in.
    }
    strip.show();
  } else {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));  // Zet alle LED's uit
    }
    strip.show();
  }
  delay(1000);
}
void ReadBME280() {
  //lees de waarde van de bme280, print deze op het lcd scherm.
  Serial.print("Temp: ");
  Serial.println(bme.readTemperature());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(bme.readTemperature());
  lcd.print(" *C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(bme.readHumidity());
  // Stuur waarde door naar database
  client.publish(Temp_topic, String(bme.readTemperature()).c_str());

// Indien de opgemeten temperatuur te hoog staat, zal de ventilator aan springen. Dit gebeurd via de relay
  if (bme.readTemperature() < TemperatuurTreshold) {
    digitalWrite(VentilatorPin, HIGH);
  } else {
    digitalWrite(VentilatorPin, LOW);
  }

  delay(1000);
}
void ReadDS18B20() {
  // Print de opgemeten waarde naar het lcd scherm
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC != -127.00) {
    Serial.print("Grond Temperatuur: ");
    Serial.print(tempC);
    Serial.print(" *C");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp Grond:");
    lcd.setCursor(0, 1);
    lcd.print(tempC);
    lcd.print(" *C");
    // Waarde wordt doorgestuurd naar grafana.
    client.publish(groundTemp_topic, String(tempC).c_str());

  } else {
    Serial.print("Fout bij lezen van temp");
  }
  delay(1000);
}
void ReadBodemVocht() {
  // print de opgemeten waarde op het lcd scherm
  bodemValue = analogRead(Bodemvocht);

  
  Serial.print("Soil Moisture Value: ");
  Serial.println(bodemValue);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bodemvocht: ");
  lcd.setCursor(0, 1);
  lcd.print(bodemValue);
  // Waarde wordt doorgestuurd naar database
  client.publish(moisture_topic, String(bodemValue).c_str());

// Indien de gemeten waarde van de bodemvochtsensor lager is dan de meegegeven treshold (1400). 
// dan zal de relay pin, waar de waterpomp aan vast hangt, voor 2 seconden aan gaan.
  if (bodemValue > BodemvochtTreshold) {
    digitalWrite(WaterPompPin, LOW);
    delay(2000);
    digitalWrite(WaterPompPin, HIGH);
  } else {
    digitalWrite(WaterPompPin, HIGH);
  }

  // Wait 1 second before taking another reading
  delay(1000);
}