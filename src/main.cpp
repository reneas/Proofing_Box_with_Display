// Geschrieben für Arduino Uno Rev3 SMD
#include <Arduino.h>          // Standard Arduino library 
#include <Adafruit_GFX.h>     // Core graphics library for displays (used for text, shapes, etc.)
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735 (128x160 pixel TFT display)
#include <DS18B20.h>          // Temperature sensor library for DS18B20 temperature sensor (1-Wire)

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

// Display Verkabelung
// CLK/SCL 13                                   // CLK != SCLK = SCK = SCL (Serial) Clock
// SDA 11                                       // SDA (Serial Data)
#define TFT_RS 5                                // RS = DC = A0
#define TFT_RST 6                               // RST = Reset 
#define TFT_CS 7                                // CS = ? 

// Gerät "tft" dem angeschlossenen Bildschirm mit den entsprchenden Pins zuweisen. CLK/SCL und SDA werden automatisch zugewiesen
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_RS, TFT_RST);

#define UpButtonPin 19                          // Knopf zum Hochschalten 
#define DownButtonPin 18                        // Knopf zum Runterschalten 

// Input/Output
#define TEMPPIN 17                              // Eingang vom DS18B20 Temperatursensor 
#define HEATPIN 16                              // Ausgang zum Relais / PIN 1 geht nicht aus Gründen 

OneWire oneWire(TEMPPIN);                       // Create a OneWire object with the TEMPPIN 
DS18B20 ds(&oneWire);                           // Pass the OneWire object to the DS18B20 constructor
                                                // ds ruft Werte vom Temperatursensor ab


// Variablen definieren
float t;                                        // die ausgelesene Temperatur 
float target_t = 28.0;                          // Ausgangs-Zieltemperatur 
float target_max = 33.0;                        // Maximaltemperatur
float target_min = 23.0;                        // Minimaltemperatur
float target_range = 1.1;                         // Temperaturbereich um die Zieltemperatur
float step_size = 0.5;                          // Schrittgröße zum umschalten

void setup()                                    // SETUP-START
{
  Serial.begin(9600);                           // Bitrate festlegen, 9600 ist Standard
  ds.begin();                                   // Initialisierung des Temperatursensors

  pinMode(TEMPPIN, INPUT);                      // TEMPPIN als Eingang definiert
  pinMode(HEATPIN, OUTPUT);                     // HEATPIN als Ausgang definiert
  pinMode(UpButtonPin, INPUT_PULLUP);           // UpButtonPin als Eingang mit Pullup-Widerstand definiert
  pinMode(DownButtonPin, INPUT_PULLUP);         // DownButtonPin als Eingang mit Pullup-Widerstand definiert
                                                // INPUT_PULLUP setzt den Pin standardmäßig auf HIGH -> zum betätigen muss er LOW gesetzt werden über Verbindung nach GND 
                                                // Dies sorgt für weniger/keine Störungen im Button-Signal, weil diese bei kleinen Spannungen schnell "falsch an" sein können
                                                // siehe Funktion "isUpButtonPressed()" und "isDownButtonPressed()" -> schalten auf LOW

  tft.initR(INITR_BLACKTAB);                    // ST7735S chip initialisieren, Farbthema schwarz
  tft.setTextSize(2);                           // Textgröße von 1 - 4
  tft.setRotation(3);                           // Rotation von 1 - 4 in 90° Schritten
  tft.fillScreen(BLACK);                        // Hintergrund schwarz
  tft.drawRect(5, 5, 150, 59, RED);             // zeichne ein rotes Rechteck
  tft.setCursor(54, 15);                        // Anfangspunkt zum Schreiben definieren (oberster linkster Punkt)
  tft.setTextColor(RED);                        // Textfarbe rot
  tft.setTextSize(1);                           
  tft.println("Richtwert");                     // schreibe "Richtwert" mit Zeilenumbruch
  tft.drawRect(5, 65, 150, 59, GREEN);          // zeichne ein grünes Rechteck
  tft.setCursor(22, 80);
  tft.setTextColor(GREEN);
  tft.setTextSize(1);
  tft.println("aktuelle Temperatur");
}

// Funktionen definieren

bool isUpButtonPressed() {                      // Abfrage, ob der UpButton gedrückt ist
  return digitalRead(UpButtonPin) == LOW;       // Wenn er gedrückt ist, gibt digitalRead(UpButtonPin) ein LOW zurück -> return true / wenn nicht, dann HIGH -> return false
}

bool isDownButtonPressed() {                    // Abfrage, ob der DownButton gedrückt ist
  return digitalRead(DownButtonPin) == LOW;     // siehe oben. Diese Logik verhindert ein "if Bedingung = true, return true / if Bedingung = false, return false"
}

void increaseTargetTemp() {                     // Erhöhen der Zieltemperatur
  if(target_t < target_max){                    // Wenn Zieltemperatur kleiner als Maximaltemperatur
    target_t += step_size;                      // erhöhe Zieltemperatur um vorgegebene Schrittgröße
  }
}

void decreaseTargetTemp() {                     // Verringern der Zieltemperatur
  if(target_t > target_min){                    // Wenn Zieltemperatur größer als Minimaltemperatur
    target_t -= step_size;                      // verringere Zieltemperatur um vorgegebene Schrittgröße
  }
}

void loop()                                     // LOOP-START
{
  ds.requestTemperatures();                     // Temperaturmessung anfordern         
  delay(750);                                   // Wartezeit für die Messung   
  t = ds.getTempC();                            // Temperatur auslesen

  tft.fillRect(45, 90, 80, 25, BLACK);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.setCursor(50, 95);
  tft.print(t);                                 // zeige aktuelle gemessene Temperatur an

  tft.setCursor(50, 28);                        // Überschreiben der vorherigen Ergebnisse mit schwarz
  tft.setTextColor(BLACK);
  tft.print(target_t );
  

  if(isUpButtonPressed()) {                     // Wenn der UpButton gedrückt ist
    increaseTargetTemp();                       // erhöhe Zieltemperatur
  } else if(isDownButtonPressed()) {            // Wenn der DownButton gedrückt ist
    decreaseTargetTemp();                       // verringere Zieltemperatur
  }

  tft.setTextColor(RED);
  tft.setTextSize(2);                           // Textgröße erhöhen
  tft.setCursor(50, 28);                        // Anfangspunkt zum Schreiben
  tft.print(target_t);                          // zeige aktuelle Zieltemperatur an

  delay(100);

  if (t < (target_t - (target_range / 2))) {    // Wenn aktuelle Temperatur kleiner als Zieltemperatur - halber Temperaturbereich
    digitalWrite(HEATPIN, HIGH);                // schalte den HEATPIN ein
  }
  else if (t > (target_t + (target_range / 2))) {   // Wenn aktuelle Temperatur größer als Zieltemperatur + halber Temperaturbereich
    digitalWrite(HEATPIN, LOW);                     // schalte den HEATPIN aus
  }

}