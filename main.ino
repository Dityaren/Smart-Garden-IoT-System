#include <ESP8266WiFi.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

char auth[] = "enter-auth-key-here";
char ssid[] = "enter-wifi-name-here";
char pass[] = "enter-wifi-password-here";

#define DHTPIN D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int analog = A0;
int Pin_Tangki = D5;
int Pin_Kipas = D3;
int Relay_Pompa = D4;
int Pin_Hujan = D2;
float panjangSensor = 4.0;
WidgetLCD LCDWaterV(V1);

void setup() {
    Serial.begin(9600); // 115200
    dht.begin();
    Blynk.begin(auth, ssid, pass);
    pinMode(Pin_Tangki, OUTPUT);
    pinMode(Pin_Kipas, OUTPUT);
    pinMode(Relay_Pompa, OUTPUT);
    pinMode(Pin_Hujan, INPUT);
    pinMode(analog, INPUT);
    digitalWrite(Relay_Pompa, HIGH);
}

int cekTangki() {
    digitalWrite(Pin_Tangki, HIGH);
    return analogRead(analog);
}

int cekTanah() {
    digitalWrite(Pin_Tangki, LOW);
    return analogRead(analog);
}

void sensorTangki() {
    int nilai = cekTangki();
    float tinggiAir = nilai * panjangSensor / 1024;
    if (tinggiAir < 0.3) {
        LCDWaterV.clear();
        LCDWaterV.print(0, 0, "TANGKI KOSONG!");
        Serial.println("TANGKI KOSONG!"); Serial.println(tinggiAir);
    } else if (tinggiAir > 0.2 && tinggiAir < 3.8) {
        LCDWaterV.clear();
        LCDWaterV.print(0, 0, "Kapasitas Air: ");
        LCDWaterV.print(1, 1, " cm");
        LCDWaterV.print(0, 1, tinggiAir);
        Serial.print("Kapasitas Air = ");
        Serial.print(tinggiAir);
        Serial.println(" cm");
    } else if (tinggiAir > 3.7) {
        LCDWaterV.clear();
        LCDWaterV.print(0, 0, "TANGKI PENUH!");
        Serial.println("TANGKI PENUH!");
    }
}

void sensorSuhu() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("\n");
    Serial.print(" Temperature: ");
    Serial.print(t);
    Serial.println("°C ");
    Blynk.virtualWrite(V3, t);
    if (t > 27) {
        digitalWrite(Pin_Kipas, HIGH);
        Blynk.notify("Temperatur Panas, Kipas Hidup!");
    } else {
        digitalWrite(Pin_Kipas, LOW);
    }
    delay(2000);
}

void sensorTanah() {
    int NilaiTanah = cekTanah();
    Serial.println("Kelembapan Tanah : ");
    Serial.println(NilaiTanah);
    if (NilaiTanah > 20) {
        Serial.println("Kelemmbapan Tanah Rendah ");
        Blynk.notify("Tanah Kering, Pompa Hidup!");
        digitalWrite(Relay_Pompa, LOW);
    } else {
        digitalWrite(Relay_Pompa, HIGH);
    }
    Blynk.virtualWrite(V4, NilaiTanah);
}

void loop() {
    Blynk.run();
    int NilaiHujan = digitalRead(Pin_Hujan);
    if (NilaiHujan == 1) {
        Serial.println("Tidak Hujan | sensor
        jalan ");
        sensorTangki(); delay(200); sensorSuhu(); delay(200); sensorTanah(); delay(200);
    } else {
        Serial.println("Hujan! | Semmua Sensor Mati ");
        Blynk.notify("Hujan Dideteksi, semua
        sensor dimatikan selama 5 detik!");
        delay(2000);
    }
    Serial.println(" "); delay(1000);
}
