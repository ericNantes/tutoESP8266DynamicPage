#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "InfoConnexion.h"
#include "ESP8266WebServer.h"

// Définition des entrées/sorties
#define PIN_LED_ROUGE 5
#define PIN_LED_VERTE 4

// Page html
const char index_html[] PROGMEM = R"=====(
<!doctype html>
<html lang="fr">
    <head>
        <meta charset="utf-8">
        <title>Commande LED</title>
    </head>
    <body>
        <h1>Etat de la LED</h1>
        <h2 id="etatLED">%LED%</h2>
        <h1>Commande de la LED</h1>
        <button onclick="appelServeur('/switchLedOn', traiteReponse)">Allumer la LED</button>
        <button onclick="appelServeur('/switchLedOff', traiteReponse)">Eteindre la LED</button>

        <script>
            function appelServeur(url, cFonction) {
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                    if (this.readyState == 4 && this.status == 200) {
                        cFonction(this);
                    }
                };
                xhttp.open("GET", url, true);
                xhttp.send();
            }
            function traiteReponse(xhttp) {
                document.getElementById("etatLED").innerHTML = "LED " + xhttp.responseText;
            }
        </script>
    </body>
</html>
)=====";

// Informations de connexion : cachées dans fichier InfoConnexion.h
// Vous pouvez décommenter ici ou créer comme moi un fichier InfoConnexion.h
//const char * SSID = "Votre SSID";
//const char * PASSWORD = "Votre mot de passe";

// Gestion des événements du WiFi
// Lorsque la connexion vient d'aboutir
void onConnected(const WiFiEventStationModeConnected& event);
// Lorsque l'adresse IP est attribuée
void onGotIP(const WiFiEventStationModeGotIP& event);

// Objet WebServer
ESP8266WebServer serverWeb(80);

// Fonctions du serveur Web
void handleRoot() {
  String temp(reinterpret_cast<const __FlashStringHelper *> (index_html));
  if (digitalRead(PIN_LED_VERTE) == HIGH) temp.replace("%LED%", "LED allumée"); else temp.replace("%LED%", "LED éteinte"); 
  serverWeb.send(200, "text/html", temp);
}

void switchLedOn() {
  digitalWrite(PIN_LED_VERTE, HIGH);
  serverWeb.send(200, "text/plain", "allumée");
}

void switchLedOff() {
  digitalWrite(PIN_LED_VERTE, LOW);
  serverWeb.send(200, "text/plain", "éteinte");
}

void setup() {
  // Mise en place d'une liaison série
  Serial.begin(9600L);
  delay(100);

  // Configuration des entrées/sorties
  pinMode(PIN_LED_ROUGE, OUTPUT);
  pinMode(PIN_LED_VERTE, OUTPUT);

  // Mode de connexion
  WiFi.mode(WIFI_STA);
 
  // Démarrage de la connexion
  WiFi.begin(SSID, PASSWORD);

  static WiFiEventHandler onConnectedHandler = WiFi.onStationModeConnected(onConnected);
  static WiFiEventHandler onGotIPHandler = WiFi.onStationModeGotIP(onGotIP);

  // Mise en place du serveur WebServer
  serverWeb.on("/switchLedOn", switchLedOn);
  serverWeb.on("/switchLedOff", switchLedOff);
  serverWeb.on("/", handleRoot);
  serverWeb.on("/index.html", handleRoot);
  serverWeb.begin();
}

void loop() {
  // Si l'objet est connecté au réseau, on effectue les tâches qui doivent l'être dans ce cas
  if (WiFi.isConnected()) {
    digitalWrite(PIN_LED_ROUGE, HIGH);
    serverWeb.handleClient();
  }
  else {
    digitalWrite(PIN_LED_ROUGE, LOW);
  }

}

void onConnected(const WiFiEventStationModeConnected& event) {
  Serial.println("WiFi connecté");
  Serial.println("Adresse IP : " + WiFi.localIP().toString());
}

void onGotIP(const WiFiEventStationModeGotIP& event) {
  Serial.println("Adresse IP : " + WiFi.localIP().toString());
  Serial.println("Passerelle IP : " + WiFi.gatewayIP().toString());
  Serial.println("DNS IP : " + WiFi.dnsIP().toString());
  Serial.print("Puissance de réception : ");
  Serial.println(WiFi.RSSI());
}
