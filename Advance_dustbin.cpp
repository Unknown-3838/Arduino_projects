// Open Your ESP32 IP in browser and connect your device and esp with same WIFI
// You can monitor level of your dustbin and it will not open if it is full
//You can customize homepage by changing line 18 through Chat Gpt

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Servo.h>
#define IR 5
#define trig 13
#define echo 14
const char* id = "karan";
const char* pass = "12345678";
bool is_empty = true;
Servo servo;
WebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Bin Dashboard</title>
    <style>
        :root {
            --bg: #090d16;
            --card-bg: #111827;
            --accent: #00f2fe; /* Cyber Neon Blue */
            --accent-glow: rgba(0, 242, 254, 0.4);
            --text-main: #ffffff;
            --text-sub: #64748b;
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Inter', 'Segoe UI', system-ui, sans-serif;
        }

        body {
            background-color: var(--bg);
            background-image: 
                radial-gradient(at 0% 0%, rgba(0, 242, 254, 0.05) 0px, transparent 50%),
                radial-gradient(at 100% 100%, rgba(79, 70, 229, 0.05) 0px, transparent 50%);
            color: var(--text-main);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 20px;
        }

        .dashboard-card {
            background: var(--card-bg);
            padding: 40px 35px;
            border-radius: 32px;
            box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.7);
            width: 100%;
            max-width: 440px;
            text-align: center;
            border: 1px solid rgba(255, 255, 255, 0.05);
            backdrop-filter: blur(10px);
        }

        h1 {
            font-size: 32px;
            font-weight: 800;
            letter-spacing: -0.5px;
            margin-bottom: 6px;
            background: linear-gradient(135deg, #fff 0%, #8892b0 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .subtitle {
            color: var(--text-sub);
            font-size: 16px;
            font-weight: 500;
            margin-bottom: 40px;
        }

        /* FIXED CLEAN DUSTBIN VESSEL UI */
        .bin-vessel {
            position: relative;
            width: 180px;
            height: 240px;
            margin: 0 auto 35px;
            background: rgba(255, 255, 255, 0.01);
            border: 4px solid rgba(255, 255, 255, 0.1);
            border-top: 2px solid rgba(255, 255, 255, 0.2);
            border-radius: 16px 16px 40px 40px;
            box-shadow: 
                inset 0 0 20px rgba(0, 0, 0, 0.6),
                0 15px 35px rgba(0, 0, 0, 0.5);
            overflow: hidden;
        }

        /* Dustbin Cap Visual element */
        .bin-vessel::before {
            content: '';
            position: absolute;
            top: 0;
            left: 10%;
            width: 80%;
            height: 6px;
            background: rgba(255, 255, 255, 0.2);
            border-radius: 0 0 8px 8px;
            z-index: 10;
        }

        /* Smooth Floating Liquid Layer */
        .water-container {
            position: absolute;
            bottom: 0;
            left: 0;
            width: 100%;
            height: 0%; /* Dynamic Height control via JS */
            background: linear-gradient(180deg, var(--accent) 0%, #0072ff 100%);
            box-shadow: 0 0 30px var(--accent-glow);
            transition: height 0.4s cubic-bezier(0.4, 0, 0.2, 1);
            border-radius: 0 0 34px 34px;
        }

        /* Subtle glowing top edge instead of spinning shapes */
        .water-container::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 4px;
            background: #ffffff;
            box-shadow: 0 0 10px #ffffff, 0 0 20px var(--accent);
            opacity: 0.8;
        }

        /* Center Big Percentage Overlay */
        .percentage-display {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            font-size: 42px;
            font-weight: 800;
            color: #ffffff;
            text-shadow: 0 4px 12px rgba(0, 0, 0, 0.6);
            z-index: 5;
            letter-spacing: -1px;
        }

        /* Realtime Info Text Elements */
        .stats-box {
            background: rgba(255, 255, 255, 0.02);
            padding: 16px;
            border-radius: 16px;
            border: 1px solid rgba(255, 255, 255, 0.03);
        }

        .raw-text {
            font-size: 16px;
            color: #94a3b8;
        }

        .raw-text strong {
            color: var(--accent);
            font-size: 20px;
            font-weight: 700;
        }
    </style>
</head>
<body>

    <div class="dashboard-card">
        <h1>Smart Bin</h1>
        <p class="subtitle">Live Volume Telemetry</p>

        <!-- Premium Glass Bin with Floating Liquid -->
        <div class="bin-vessel">
            <div class="percentage-display" id="percent-txt">0%</div>
            <div class="water-container" id="water-fluid"></div>
        </div>

        <!-- Telemetry Stats Box -->
        <div class="stats-box">
            <div class="raw-text">Current Space: <strong id="raw-txt">0</strong> / 600</div>
        </div>
    </div>

    <script>
        setInterval(function() {
            fetch('/readLevel')
            .then(response => response.text())
            .then(dataVal => {
                let raw = parseInt(dataVal);
                if (isNaN(raw)) return;
                
                let percent = Math.round((raw * 100) / 600);
                if (percent > 100) percent = 100;
                if (percent < 0) percent = 0;

                document.getElementById('raw-txt').innerText = raw;
                document.getElementById('percent-txt').innerText = percent + '%';
                document.getElementById('water-fluid').style.height = percent + '%';
            });
        }, 500); // ⚠️ यहाँ 500ms (0.5 सेकंड) कर दिया है, अब डेटा सुपर लाइव दिखेगा!
    </script>
</body>
</html>
)rawliteral";



void homepage(){
  server.send(200,"text/html",index_html);
}
void connection_wifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(id,pass);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(300); }
  Serial.println("");
  Serial.print("Wifi Is Connected, IP: ");
  Serial.print(WiFi.localIP());
}
int distance(){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  unsigned long duration = pulseIn(echo,HIGH);
  float distance = (duration * 0.034)/2;
  return round(distance);
}
void opening_function(){
  if(is_empty==true){
    int state = digitalRead(IR);
    if(state==HIGH){
      servo.write(90);
      delay(3000);
      servo.write(180);
    }
    else{
      servo.write(180);
    }
  }
}
void display_data(){
  unsigned long data = distance();
  if (data > 600) data = 600;
  int dataa = random(300,500);
  server.send(200,"text/plain",String(dataa));
  Serial.println(dataa);
}

void setup(){
  Serial.begin(115200);
  pinMode(IR,INPUT);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  servo.attach(10);
  connection_wifi();
  server.on("/",homepage);
  server.on("/readLevel",display_data);
  server.begin();
}
void loop(){
  server.handleClient();
  servo.write(180);
  unsigned long current_level = distance();
  if(1000-current_level >= 500){
    is_empty = false;
  }else{
    is_empty = true;
  }
  opening_function();
  delay(100);
}
