#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRsend.h>
#include <WiFiClient.h>

IRsend irsend(4);  // an IR led is connected to GPIO pin 4 (D2)

const char* ssid = "******";
const char* password = "*******";
MDNSResponder mdns;

ESP8266WebServer server(80);


void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head><title>ESP8266 Demo</title></head>" \
                "<body>" \
                  "<h1>Hello from ESP8266, you can send NEC encoded IR" \
                      "signals from here!</h1>" \
                  "<p><a href=\"ir?code=0xFF827D\">Send ON</a></p>" \
                  "<p><a href=\"ir?code=0xFF02FD\">Send OFF</a></p>" \
                  
                "</body>" \
              "</html>");
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      String code = server.argName(i).substr(1,9);
      //String code = strcpy(server.arg(i).c_str(), NULL, 10);
      Serial.print("code = ");
      Serial.println(code);
      irsend.sendNEC(code, 32);
      delay(300);
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
