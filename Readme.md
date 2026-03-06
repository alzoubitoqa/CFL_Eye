0) Project Overview (كيف النظام شغال)

Sensors → ESP32 → Backend API → MySQL → Website Dashboard (Live)

ESP32 يقرأ:

DHT11 (Temperature / Humidity)

MQ-2 (Gas Analog)

Flame Sensor (Digital)

IR Sensor (Digital)

ESP32 يرسل القراءات إلى السيرفر:

POST /api/ingest

السيرفر يخزّن/يحدّث القراءات في MySQL داخل جدول:

sensors

الموقع يسحب البيانات من:

GET /api/sensors
ويعرضها مباشرة بالداشبورد (كل 5 ثواني).

1) Project Folder Structure (ترتيب الملفات النهائي)

لازم هيك:

project-folder/
│
├── server.js
├── package.json
├── package-lock.json
│
├── public/
│   ├── index.html
│   ├── admin.html
│   ├── dashboard.html
│   ├── cfl-eye-dashboard.html
│   ├── reports.html
│   ├── style.css
│   └── script.js
│
└── (اختياري: مجلدات الاختبار)
    ├── test_leds/
    ├── test_buzzer/
    └── testing_esp32/

✅ مهم: لا تفتحي HTML من الملفات مباشرة (file://)
افتحي دومًا من السيرفر:

http://localhost:3000

2) البرامج المطلوبة على اللابتوب
A) Node.js

تثبيت Node.js (يفضل LTS)
تحقق:

node -v
npm -v
B) MySQL (XAMPP)

تثبيت XAMPP وتشغيل:

Apache

MySQL

فتح phpMyAdmin:

http://localhost/phpmyadmin

C) Arduino IDE + ESP32

Arduino IDE

ESP32 Board Package

Driver للـ ESP32 (CP210x أو CH340)

3) Backend Setup (Node.js + Express)
تثبيت الحزم

داخل مجلد المشروع:

npm install

إذا بدأتي من الصفر:

npm init -y
npm install express mysql2 cors
تشغيل السيرفر
node server.js

لازم يطلع:

✅ Connected to MySQL DB

🚀 Server running on http://localhost:3000

4) Database Setup (MySQL / phpMyAdmin)
A) اسم قاعدة البيانات

CFL_Eye

B) جدول users (للـ Login)

السيرفر يستخدم:

SELECT role FROM users WHERE username=? AND password=? LIMIT 1

مثال إنشاء:

CREATE TABLE IF NOT EXISTS users (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50),
  password VARCHAR(50),
  role VARCHAR(20)
);

إضافة مستخدمين:

INSERT INTO users (username, password, role) VALUES
('admin', '1234', 'admin'),
('user',  '1234', 'user');
C) جدول sensors (للقيم)

المطلوب أعمدة:

id

name

value

status

مثال:

CREATE TABLE IF NOT EXISTS sensors (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(50),
  value VARCHAR(50),
  status VARCHAR(20) DEFAULT 'Normal'
);
D) إضافة عمود status (إذا ناقص)

phpMyAdmin → sensors → SQL:

ALTER TABLE sensors
ADD COLUMN status VARCHAR(20) DEFAULT 'Normal';

إذا ظهر:

Duplicate column name 'status'
يعني موجود ✅

E) جعل name UNIQUE (مهم جدًا)

عشان ON DUPLICATE KEY UPDATE يشتغل:

ALTER TABLE sensors
ADD UNIQUE KEY uniq_name (name);
F) إضافة أسماء الحساسات الرسمية (Recommended)
INSERT INTO sensors (name, value, status) VALUES
('Gas Level','0','Normal'),
('Humidity','0','Normal'),
('Temperature','0','Normal'),
('Fire Sensor','1','Normal'),
('Lab Door','Closed','Normal'),
('Evidence Locker','Stored','Normal'),
('IR','0','Normal')
ON DUPLICATE KEY UPDATE value=VALUES(value), status=VALUES(status);
G) حذف الأسماء القديمة الصغيرة (تنظيف)

إذا كان عندك:
temperature / humidity / gas
احذفيهم:

DELETE FROM sensors WHERE name IN ('temperature','humidity','gas');
5) Website (Frontend)

افتحي الموقع من السيرفر:

Login: http://localhost:3000

Admin Dashboard: http://localhost:3000/cfl-eye-dashboard.html

User Dashboard: http://localhost:3000/dashboard.html

Reports: http://localhost:3000/reports.html

API:

http://localhost:3000/api/sensors

6) Backend API Endpoints (النهائية)
Ping (اختبار اتصال)

GET /ping → يرجّع pong

Login

POST /api/auth/login
Body:

{"username":"admin","password":"1234"}
Read Sensors

GET /api/sensors

Ingest from ESP32

POST /api/ingest
Body مثال:

{
  "gas":777,
  "humidity":60,
  "temperature":26,
  "fire":1,
  "door":"Closed",
  "locker":"Stored",
  "ir":0
}
7) مشكلة الشبكة اللي واجهتنا + الحل
المشكلة

ESP32 والموبايل ما كانوا يقدروا يوصلوا للابتوب عبر الراوتر:

ESP32 GET code = -1

TCP connect FAILED

الموبايل ERR_ADDRESS_UNREACHABLE

السبب غالبًا:
✅ Router Isolation (Client/AP Isolation)

الحل المضمون

استخدمنا Windows Mobile Hotspot من اللابتوب.

IP الهوتسبوت

من ipconfig ظهر:

Laptop Hotspot IP = 192.168.137.1

إذن ESP32 لازم يرسل إلى:

http://192.168.137.1:3000/api/ingest

8) أوامر Windows اللي استخدمناها
معرفة IP
ipconfig
فحص المنفذ 3000
netstat -ano | findstr :3000
فتح Port 3000 في Firewall (PowerShell Admin)
netsh advfirewall firewall add rule name="Node Port 3000" dir=in action=allow protocol=TCP localport=3000
9) ESP32 Tests اللي عملناها
A) اختبار WiFi status

Status 3 = Connected ✅

Status 6 = Wrong password ❌

B) اختبار GET /ping

URL:
http://192.168.137.1:3000/ping

Expected:
GET code: 200 + pong

C) اختبار POST /api/ingest

URL:
http://192.168.137.1:3000/api/ingest

Expected:
POST code: 200 + {ok:true}

10) Wiring Notes (مهم جدًا)

✅ كل LED لازم مقاومة 220Ω لحالها.

مخطط pins (مثال شغال):

DHT11 DATA → GPIO33

Flame DO → GPIO32

IR DO → GPIO12

MQ-2 AO → GPIO34 (أفضل مع WiFi)

Buzzer → GPIO25

LEDs → Pins مختلفة

11) Why MQ should be on GPIO34

على ESP32:

ADC2 ممكن يتعطل مع WiFi
فالأفضل MQ analog على ADC1:

GPIO34/35/36/39

12) Dashboard Improvements اللي سويناها

IR: 0/1 → Area Clear / INTRUSION

Fire: 1/0 → Safe / FIRE DETECTED

Units: °C / % / ppm

System status يتغير حسب Warning/Alert

13) How to verify everything is working

شغّلي السيرفر:

node server.js

افتحي:
http://localhost:3000/api/sensors

شغلي Hotspot + وصّلي ESP32 عليه

شغّلي كود ESP32 (يبعث ingest)

افتحي Admin Dashboard وشوفي القيم تتغير Live.

14) نقل المشروع لجهاز ثاني (مختصر)

انسخي مجلد المشروع كامل

نزّلي Node.js + XAMPP + Arduino IDE

npm install

جهزي DB + جدول users + sensors

شغلي node server.js

إذا استخدمتي Hotspot على الجهاز الجديد:

اعملي ipconfig وخذي IP الهوتسبوت

عدّليه داخل ESP32 URL

15) Summary of ALL commands
Node:
npm install
node server.js
Network:
ipconfig
netstat -ano | findstr :3000
Firewall:
netsh advfirewall firewall add rule name="Node Port 3000" dir=in action=allow protocol=TCP localport=3000
MySQL:
ALTER TABLE sensors ADD COLUMN status VARCHAR(20) DEFAULT 'Normal';
ALTER TABLE sensors ADD UNIQUE KEY uniq_name (name);

INSERT INTO sensors (name, value, status) VALUES
('Gas Level','0','Normal'),
('Humidity','0','Normal'),
('Temperature','0','Normal'),
('Fire Sensor','1','Normal'),
('Lab Door','Closed','Normal'),
('Evidence Locker','Stored','Normal'),
('IR','0','Normal')
ON DUPLICATE KEY UPDATE value=VALUES(value), status=VALUES(status);

DELETE FROM sensors WHERE name IN ('temperature','humidity','gas');