//The webpage overview is made through using chat gpt becauase i dont know HTML or JAVA
//you can make it as you want by using gpt.
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTTYPE DHT11   // Inbuilt line 
#define DHTPIN 13  // DHT Sensor Digital Pin 2 par connect karein
#define light 4
#define fans 14
#define Buzz 15

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);
const char* ip = "karan";
const char* pass = "12345678";

const char* wbpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Home Automation</title>
<style>
*{
  box-sizing:border-box;
  margin:0;
  padding:0;
}
body{
  font-family:Arial,sans-serif;
  background:#0b0f14;
  color:#e8edf2;
  padding:20px;
}
.container{
  max-width:900px;
  margin:auto;
}
header{
  display:flex;
  justify-content:space-between;
  align-items:center;
  margin-bottom:25px;
}
h1{font-size:24px;color:#fff}
.subtitle{color:#8995a5;font-size:13px;margin-top:6px}
.status{padding:8px 13px;border-radius:20px;background:#17251c;color:#50e39a;font-size:13px}
.grid{display:grid;grid-template-columns:repeat(2,1fr);gap:15px}
.card{background:#151b23;border:1px solid #252e39;border-radius:16px;padding:20px}
.card-title{font-size:14px;color:#9aa7b7;margin-bottom:15px}
.value{font-size:32px;font-weight:bold;color:#fff}
.unit{font-size:16px;color:#8c99a8}
.icon{font-size:22px;margin-bottom:12px}
.status-value{font-size:22px;font-weight:bold;margin-top:5px}
.safe,.online{color:#4ade80}
.unsafe,.offline{color:#f87171}
.gas-bar{height:8px;background:#28313c;border-radius:10px;margin-top:15px;overflow:hidden}
.gas-fill{height:100%;width:0%;background:#38bdf8;border-radius:10px;transition:width .4s}
.control-card{display:flex;justify-content:space-between;align-items:center;gap:10px}
.control-info{display:flex;align-items:center;gap:12px}
.control-info .icon{margin:0;font-size:26px}
.control-name{font-size:16px;color:#fff;font-weight:bold}
.control-state{font-size:12px;color:#8c99a8;margin-top:5px}
.btn{border:none;border-radius:10px;padding:11px 20px;font-size:14px;font-weight:bold;cursor:pointer;text-decoration:none;display:inline-block;min-width:75px;text-align:center}
.btn-on{background:#166534;color:#bbf7d0}
.btn-off{background:#27313d;color:#e2e8f0}
.footer{margin-top:20px;text-align:center;color:#647181;font-size:12px}
@media(max-width:500px){
  body{padding:14px}
  h1{font-size:21px}
  .grid{gap:10px}
  .card{padding:15px}
  .value{font-size:27px}
  .control-card{padding:14px}
  .btn{padding:10px 15px}
}
</style>
</head>
<body>
<div class="container">
<header>
<div>
<h1>Home Automation</h1>
<div class="subtitle">Smart Home Monitoring System</div>
</div>
<div class="status" id="connection">Connecting...</div>
</header>
<div class="grid">
<div class="card">
<div class="icon">🌡️</div>
<div class="card-title">Temperature</div>
<div class="value"><span id="temp">--</span><span class="unit"> °C</span></div>
</div>
<div class="card">
<div class="icon">💧</div>
<div class="card-title">Humidity</div>
<div class="value"><span id="humidity">--</span><span class="unit"> %</span></div>
</div>
<div class="card">
<div class="icon">☁️</div>
<div class="card-title">Gas Sensor Level</div>
<div class="value" id="gas">--</div>
<div class="gas-bar"><div class="gas-fill" id="gasbar"></div></div>
</div>
<div class="card">
<div class="icon">🛡️</div>
<div class="card-title">System Safety</div>
<div class="status-value" id="safety">--</div>
</div>
<div class="card control-card">
<div class="control-info">
<div class="icon">💡</div>
<div>
<div class="control-name">Light</div>
<div class="control-state" id="light-state">--</div>
</div>
</div>
<a href="/light-on" class="btn btn-off" id="light-btn">--</a>
</div>
<div class="card control-card">
<div class="control-info">
<div class="icon">🌀</div>
<div>
<div class="control-name">Fan</div>
<div class="control-state" id="fan-state">--</div>
</div>
</div>
<a href="/fan-on" class="btn btn-off" id="fan-btn">--</a>
</div>
<div class="card">
<div class="icon">🔔</div>
<div class="card-title">Buzzer</div>
<div class="status-value" id="buzzer">--</div>
</div>
</div>
<div class="footer">Live Sensor Data • ESP32 Home Automation</div>
</div>
<script>
async function updateData(){
  try{
    const response=await fetch('/data');
    const data=await response.json();
    document.getElementById('temp').textContent=data.Temperature;
    document.getElementById('humidity').textContent=data.Humidity;
    document.getElementById('gas').textContent=data['Gas Level'];
    document.getElementById('safety').textContent=data['System Status'];
    document.getElementById('safety').className='status-value '+(data['System Status']==='Safe'?'safe':'unsafe');
    document.getElementById('buzzer').textContent=data.Buzzer;
    document.getElementById('buzzer').className='status-value '+(data.Buzzer==='ON'?'unsafe':'online');
    let gasValue=Math.min((data['Gas Level']/4095)*100,100);
    document.getElementById('gasbar').style.width=gasValue+'%';
    document.getElementById('light-state').textContent=data.Light;
    document.getElementById('light-btn').textContent=data.Light==='ON'?'TURN OFF':'TURN ON';
    document.getElementById('light-btn').className='btn '+(data.Light==='ON'?'btn-on':'btn-off');
    document.getElementById('fan-state').textContent=data.Fan;
    document.getElementById('fan-btn').textContent=data.Fan==='ON'?'TURN OFF':'TURN ON';
    document.getElementById('fan-btn').className='btn '+(data.Fan==='ON'?'btn-on':'btn-off');
    document.getElementById('connection').textContent='● Connected';
    document.getElementById('connection').style.background='#17251c';
    document.getElementById('connection').style.color='#50e39a';
  }catch(error){
    document.getElementById('connection').textContent='● Disconnected';
    document.getElementById('connection').style.background='#301b1b';
    document.getElementById('connection').style.color='#f87171';
  }
}
updateData();
setInterval(updateData,1000);
</script>
</body>
</html>
)rawliteral";

unsigned int temp(){
  float t = dht.readTemperature();
  if(isnan(t)) return 0;
  return round(t);
}
unsigned int humidity(){
  float h = dht.readHumidity();
  if(isnan(h)) return 0;
  return round(h);
}
unsigned int Gas_level(){
  int g = analogRead(12);
  return g;
}

void connection(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ip,pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected, IP: ");
  Serial.println(WiFi.localIP());
}
void homepage(){
  server.send(200,"text/html",wbpage);
}
void sendData(){
  JsonDocument data;
  data["Temperature"] = temp();
  data["Humidity"] = humidity();
  data["Gas Level"] = Gas_level();
  data["Light"] = (digitalRead(light)==HIGH)? "ON" : "OFF";
  data["Buzzer"] = (digitalRead(Buzz)==HIGH)? "ON" : "OFF";
  data["Fan"] = (digitalRead(fans)==HIGH)? "ON" : "OFF";
  if(temp()<50 and Gas_level()<300){
    data["System Status"] = "Safe";
  }else{
    data["System Status"] = "UnSafe";
  }
  String Js;
  serializeJson(data,Js);
  server.send(200,"application/json",Js);
  delay(200);
}

void light_on(){
  if(digitalRead(light)==HIGH){
    digitalWrite(light,LOW);
  }else{
    digitalWrite(light,HIGH);
  }
  server.sendHeader("location","/");
  server.send(303);
}
void fan_on(){
  if(digitalRead(fans)==HIGH){
    digitalWrite(fans,LOW);
  }else{
    digitalWrite(fans,HIGH);
  }
  server.sendHeader("location","/");
  server.send(303);
}

void setup(){
  dht.begin();
  Serial.begin(115200);
  connection();
  pinMode(Buzz,OUTPUT);
  pinMode(light,OUTPUT);
  pinMode(fans,OUTPUT);
  server.on("/",homepage);
  server.on("/data",sendData);
  server.on("/light-on",light_on);
  server.on("/fan-on",fan_on);
  server.begin();
}

void loop(){
  server.handleClient();
}
