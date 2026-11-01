// Edit Url according to you your location and try to use the same plaetform
// also You can customize the interface of webpage by changin const char* webpage....Use chat gpt

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

WebServer server(80);
const char* ip = "karan";
const char* pass = "12345678";
const char* url = "https://api.open-meteo.com/v1/forecast?latitude=28.6139&longitude=77.2090&current=temperature_2m,relative_humidity_2m,apparent_temperature,wind_speed_10m,precipitation,weather_code";
HTTPClient http;

const char* webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Shree Weather Data</title>
<style>
*{box-sizing:border-box}
body{margin:0;padding:25px 18px;font-family:Arial,sans-serif;background:#080b12;color:white;min-height:100vh}
.container{max-width:750px;margin:auto}
.header{text-align:center;padding:25px 10px 35px}
.header h1{font-size:30px;margin:0;letter-spacing:1px;background:linear-gradient(90deg,#60a5fa,#c084fc);-webkit-background-clip:text;-webkit-text-fill-color:transparent}
.header p{color:#8994a8;font-size:13px;margin-top:12px;letter-spacing:2px}
.city-box{background:linear-gradient(120deg,#171e30,#111827);border:1px solid #29344a;border-radius:20px;padding:25px;margin-bottom:22px;text-align:center}
.city-label{color:#8994a8;font-size:12px;letter-spacing:2px;text-transform:uppercase}
.city{font-size:27px;font-weight:bold;margin-top:12px;color:#e5edff}
.dashboard{display:grid;grid-template-columns:repeat(2,1fr);gap:15px}
.card{background:linear-gradient(145deg,#171e2d,#101521);border:1px solid #283247;border-radius:20px;padding:24px 15px;min-height:155px;transition:0.3s}
.card:hover{border-color:#6488c9;transform:translateY(-3px)}
.icon{font-size:27px;margin-bottom:15px}
.label{font-size:13px;color:#8994a8;margin-bottom:12px}
.value{font-size:27px;font-weight:bold;color:#f1f5ff}
.unit{font-size:13px;color:#8994a8;font-weight:normal}
.footer{text-align:center;margin-top:35px;color:#58657b;font-size:11px;letter-spacing:1px}
@media(max-width:380px){.value{font-size:23px}.card{padding:20px 12px}}
</style>
</head>
<body>
<div class="container">
<div class="header">
<h1>SHREE WEATHER DATA</h1>
<p>LIVE WEATHER MONITORING</p>
</div>
<div class="city-box">
<div class="city-label">CURRENT LOCATION</div>
<div class="city" id="city">New Delhi</div>
</div>
<div class="dashboard">
<div class="card">
<div class="icon">🌡️</div>
<div class="label">Temperature</div>
<div class="value" id="temperature">-- <span class="unit">°C</span></div>
</div>
<div class="card">
<div class="icon">💨</div>
<div class="label">Wind Speed</div>
<div class="value" id="wind">-- <span class="unit">km/h</span></div>
</div>
<div class="card">
<div class="icon">💧</div>
<div class="label">Humidity</div>
<div class="value" id="humidity">-- <span class="unit">%</span></div>
</div>
<div class="card">
<div class="icon">🌤️</div>
<div class="label">Feels Like</div>
<div class="value" id="feels">--</div>
</div>
</div>
<div class="footer">POWERED BY ESP32 | WEATHER SYSTEM</div>
</div>
<script>
async function updateValue(endpoint,element,unit){
try{
let response=await fetch(endpoint);
if(!response.ok)return;
let data=await response.text();
if(data.trim()!==""){
document.getElementById(element).innerHTML=data+'<span class="unit">'+unit+'</span>';
}
}catch(error){console.log(error);}
}
async function updateData(){
updateValue('/temperature','temperature',' °C');
updateValue('/wind','wind',' km/h');
updateValue('/humidity','humidity',' %');
updateValue('/feels','feels','');
}
updateData();
setInterval(updateData,5000);
</script>
</body>
</html>
)rawliteral";

float temp = 0.0;
int humidity = 0;
float wind = 0.0;

void connection(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ip,pass);
  while(WiFi.status()!=WL_CONNECTED){
  Serial.print(".");
  delay(300);
  }
  Serial.println("");
  Serial.print("Connected IP: ");
  Serial.println(WiFi.localIP());
}
String fetch_data(){
  String respons = " ";
  http.begin(url);
  Serial.println("Gathering Data...");
  int status_code = http.GET();
  if(status_code==200){
    respons = http.getString();
    http.end();
    return respons;
  }
  else{
    Serial.print("Gathering Data Failed..");
  }
  http.end();
  return respons;
}
JsonDocument get_data(String data){
  JsonDocument final_doc;
  DeserializationError error = deserializeJson(final_doc,data);
  if(error){
    Serial.println("Error in parsing");
    return final_doc;
  }
  temp = final_doc["current"]["temperature_2m"];
  humidity = final_doc["current"]["relative_humidity_2m"];
  wind = final_doc["current"]["wind_speed_10m"];
  Serial.println("====== Weather Data ========");
  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("Wind Speed: "); Serial.println(wind);
  return final_doc;
}

void homepage(){
  server.send(200,"text/html; charset=utf-8",webpage);
}
void tempera(){
  server.send(200,"text/plain",String(temp));
}
void windd(){
  server.send(200,"text/plain",String(wind));
}
void humia(){
  server.send(200,"text/plain",String(humidity));
}
void feels(){
  String names[] = {"Shree","Karan","Krishna","Shree Ji"};
  String ran = names[random(0,4)];
  server.send(200,"text/plain",ran);
}

void setup(){
  Serial.begin(115200);
  connection();
  String str_data = fetch_data();
  if(str_data!= " "){
    JsonDocument doc = get_data(str_data);
    if(doc.isNull()){
      server.sendHeader("Location","/");
      server.send(303,"text/plain","None");
      return;
    }
  }else{
    Serial.println("Error in getting String"); 
    return;
  }
  server.on("/",homepage);
  server.on("/temperature",tempera);
  server.on("/wind",windd);
  server.on("/feels",feels);
  server.on("/humidity",humia);
  server.begin();
}
long last_up = 0;
void loop(){
  server.handleClient();
  if(millis()-last_up >= 6000){
    last_up = millis();
    String str_dat = fetch_data();
    if(str_dat!= " "){
      JsonDocument docc = get_data(str_dat);
    }
  }
}
