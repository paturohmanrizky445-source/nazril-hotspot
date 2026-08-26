#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

WebServer server(80);
DNSServer dnsServer;

const char* apSSID = "NAZRIL_HOTSPOT";
const char* apPass = "12345678";
const char* adminPass = "admin123";

bool loggedIn = false;

String loginPage(String msg = "") {
  String h = "<!DOCTYPE html><html><head><meta name=viewport content=width=device-width,initial-scale=1>";
  h += "<title>NAZRIL</title><style>body{background:#111;color:#fff;font-family:Arial;text-align:center;padding:40px}";
  h += "input,button{width:90%;padding:14px;margin:10px 0;border-radius:8px;border:none;font-size:16px}";
  h += "button{background:#00bcd4;color:white} .msg{color:yellow}</style></head><body>";
  h += "<h2>NAZRIL HOTSPOT</h2>";
  if(msg != "") h += "<p class=msg>" + msg + "</p>";
  h += "<form action=/login method=POST>";
  h += "<input type=password name=password placeholder='Password' required>";
  h += "<button>LOGIN</button></form></body></html>";
  return h;
}

String adminPage() {
  String h = "<!DOCTYPE html><html><head><meta name=viewport content=width=device-width,initial-scale=1>";
  h += "<title>Admin</title><style>body{background:#111;color:#fff;font-family:Arial;padding:20px}";
  h += ".card{background:#222;padding:15px;margin:10px 0;border-radius:10px}</style></head><body>";
  h += "<h2>NAZRIL ADMIN</h2>";
  h += "<div class=card>SSID: <b>" + String(apSSID) + "</b></div>";
  h += "<div class=card>Client: <b>" + String(WiFi.softAPgetStationNum()) + "</b></div>";
  h += "<div class=card>IP: <b>" + WiFi.softAPIP().toString() + "</b></div>";
  h += "<br><a href=/logout><button style='padding:12px 25px;background:#00bcd4;color:white;border:none;border-radius:8px'>LOGOUT</button></a>";
  h += "</body></html>";
  return h;
}

void handleRoot() {
  if (loggedIn) server.send(200, "text/html", adminPage());
  else server.send(200, "text/html", loginPage());
}

void handleLogin() {
  if (server.hasArg("password") && server.arg("password") == adminPass) {
    loggedIn = true;
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  } else {
    server.send(200, "text/html", loginPage("Password salah!"));
  }
}

void handleLogout() {
  loggedIn = false;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleNotFound() {
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPass);

  IPAddress ip = WiFi.softAPIP();
  Serial.println("Hotspot aktif");
  Serial.println(ip);

  dnsServer.start(53, "*", ip);

  server.on("/", handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/logout", handleLogout);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Server siap");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}