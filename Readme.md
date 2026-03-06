🛡️ CFL-Eye — Smart Forensic Lab Monitoring System (Full README)

CFL-Eye is an IoT + Web system that monitors a Digital Forensics Laboratory using sensors connected to ESP32, sends readings to a Node.js/Express backend, stores them in MySQL, and visualizes everything in a local web dashboard (Admin/User + Reports).
This README covers everything from zero to the final working live demo, including all commands we used.

✅ Table of Contents

Project Summary

System Architecture

Features

Tech Stack

Requirements (Software & Hardware)

Project Folder Structure

Backend Setup (Node.js + Express) — From Scratch

Database Setup (MySQL/phpMyAdmin) — From Scratch

API Endpoints

Frontend Setup (Website in /public)

ESP32 Setup (Arduino IDE) — From Scratch

Wiring Guide (Sensors + ESP32)

Networking (The Router Isolation Problem) + Hotspot Fix

Windows Firewall Rules (Port 3000)

Validation & Testing Checklist

Troubleshooting (Most common errors)

Demo Script (Doctor Presentation)

Commands Summary (All commands in one place)

1) 📌 Project Summary

Problem: Digital forensic labs contain sensitive devices/evidence and require continuous monitoring for safety and security risks.
Solution: CFL-Eye monitors:

Gas/smoke leakage

Fire/flame

Temperature & humidity

Intrusion (IR movement)
and displays the situation on a web dashboard with Normal/Warning/Alert statuses.

2) 🧠 System Architecture

Data Flow:

Sensors → ESP32 → POST /api/ingest → MySQL → GET /api/sensors → Website Dashboard

Main Components:

ESP32 reads sensors

Backend receives readings, computes status, updates database

Web dashboard fetches readings every few seconds and displays them live

3) ✨ Features
Admin Dashboard

Live sensor cards

System state (OK / WARNING / ALERT)

Logs export

Reports page

Human-friendly values:

Fire: Safe / FIRE DETECTED

IR: Area Clear / INTRUSION

Units: °C / % / ppm

User Dashboard

Simplified live sensor values + status

Backend

Login API for user/admin

Ingest API for ESP32

Sensors API for dashboards

Works on laptop-only demo (local web)

4) 🧰 Tech Stack

Hardware: ESP32 + DHT11 + MQ-2 + Flame + IR + LEDs + Buzzer
Backend: Node.js, Express, MySQL2, CORS
Database: MySQL (via XAMPP/phpMyAdmin)
Frontend: HTML/CSS/JS (Fetch API)

5) ✅ Requirements
Software

Node.js (LTS recommended)

XAMPP (MySQL + phpMyAdmin)

Arduino IDE

ESP32 board package in Arduino IDE

ESP32 USB driver (CP210x or CH340)

Arduino Libraries

DHT sensor library (Adafruit)

Adafruit Unified Sensor

Hardware

ESP32 Dev board

DHT11

MQ-2 (Analog output AO)

Flame sensor (Digital DO)

IR sensor (Digital DO)

LEDs + 220Ω resistors

(Optional) passive buzzer

6) 📁 Project Folder Structure (Final)
project-folder/
│
├── server.js
├── package.json
├── package-lock.json
│
└── public/
    ├── index.html
    ├── admin.html
    ├── dashboard.html
    ├── cfl-eye-dashboard.html
    ├── reports.html
    ├── style.css
    └── script.js

✅ Always run website from server:

http://localhost:3000

❌ Don’t open HTML using file explorer (file://...)

7) 🚀 Backend Setup (Node.js + Express) — From Scratch
7.1 Install Node.js

Check versions:

node -v
npm -v
7.2 Create project folder
mkdir CFL-Eye
cd CFL-Eye
7.3 Initialize + Install dependencies
npm init -y
npm install express mysql2 cors
7.4 Run server
node server.js

Expected:

✅ Connected to MySQL DB

Server running on port 3000

8) 🗄️ Database Setup (MySQL/phpMyAdmin) — From Scratch
8.1 Start XAMPP

Start Apache

Start MySQL

Open:

http://localhost/phpmyadmin

8.2 Create database
CREATE DATABASE CFL_Eye;
8.3 Create users table (Login)
USE CFL_Eye;

CREATE TABLE IF NOT EXISTS users (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50),
  password VARCHAR(50),
  role VARCHAR(20)
);

Insert demo users:

INSERT INTO users (username, password, role) VALUES
('admin', '1234', 'admin'),
('user',  '1234', 'user');
8.4 Create sensors table
CREATE TABLE IF NOT EXISTS sensors (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(50),
  value VARCHAR(50),
  status VARCHAR(20) DEFAULT 'Normal'
);
8.5 Make name UNIQUE (IMPORTANT for UPSERT)
ALTER TABLE sensors
ADD UNIQUE KEY uniq_name (name);
8.6 Insert official sensor names (recommended)
INSERT INTO sensors (name, value, status) VALUES
('Gas Level','0','Normal'),
('Humidity','0','Normal'),
('Temperature','0','Normal'),
('Fire Sensor','1','Normal'),
('Lab Door','Closed','Normal'),
('Evidence Locker','Stored','Normal'),
('IR','0','Normal')
ON DUPLICATE KEY UPDATE value=VALUES(value), status=VALUES(status);
8.7 Cleanup old lowercase duplicates (optional)

If you had old rows like gas, humidity, temperature:

DELETE FROM sensors WHERE name IN ('temperature','humidity','gas');
9) 🔌 API Endpoints (Backend)
9.1 Ping

GET /ping → pong

Example:

http://localhost:3000/ping

9.2 Login

POST /api/auth/login

Body:

{"username":"admin","password":"1234"}

Response:

{"success":true,"role":"admin"}
9.3 Read sensors (Dashboard)

GET /api/sensors

Returns:

[
  {"name":"Gas Level","value":"777","status":"Warning"},
  {"name":"Temperature","value":"26","status":"Normal"}
]
9.4 Ingest from ESP32

POST /api/ingest

Body example:

{
  "gas":777,
  "humidity":60,
  "temperature":26,
  "fire":1,
  "door":"Closed",
  "locker":"Stored",
  "ir":0
}

Response:

{"ok":true}
10) 🖥️ Frontend Setup (Website)

Files are served from /public via:

app.use(express.static(path.join(__dirname, "public")));

Open:

http://localhost:3000 (Login)

http://localhost:3000/cfl-eye-dashboard.html (Admin)

http://localhost:3000/dashboard.html (User)

http://localhost:3000/reports.html (Reports)

11) 🤖 ESP32 Setup (Arduino IDE) — From Scratch
11.1 Install ESP32 Board Package

Arduino IDE → Preferences → Additional Boards Manager URLs
Add ESP32 URL, then:
Tools → Board → Boards Manager → install ESP32

11.2 Install Libraries

Arduino IDE → Library Manager:

DHT sensor library (Adafruit)

Adafruit Unified Sensor

11.3 Driver / COM Port

If COM port not detected:

Install CP210x / CH340 driver (depends on board)

12) 🔧 Wiring Guide (Typical Working Pins)

Your dashboard is already working with these sensor names in DB.
Keep wiring consistent with ESP32 code.

Recommended pins:

DHT11 DATA → GPIO33

Flame DO → GPIO32

IR DO → GPIO12

MQ-2 AO → GPIO34 (IMPORTANT: ADC1 works with WiFi)

Buzzer → GPIO25 (optional)

LEDs on free GPIOs + each LED has 220Ω resistor

⚠️ MQ on ADC2 pins (like GPIO13) may fail when WiFi is enabled. Use GPIO34 instead.

13) 🌐 Networking Problem + Hotspot Fix (What happened)
Issue we faced

ESP32 could connect to WiFi, but could not reach laptop server:

GET code = -1

TCP connect FAILED
This happens when router blocks device-to-device communication (Client Isolation/AP Isolation).

Guaranteed solution used

✅ Windows Mobile Hotspot

Laptop becomes the “router” for ESP32

Laptop hotspot IP becomes: 192.168.137.1

Confirm hotspot IP

Command:

ipconfig

Look for:

Local Area Connection* 10 (Mobile Hotspot)

IPv4: 192.168.137.1

ESP32 sends to:

http://192.168.137.1:3000/ping

http://192.168.137.1:3000/api/ingest

14) 🧱 Windows Firewall (Port 3000)

To allow inbound connections on port 3000:

Run PowerShell as Administrator:

netsh advfirewall firewall add rule name="Node Port 3000" dir=in action=allow protocol=TCP localport=3000

Check listening port:

netstat -ano | findstr :3000
15) ✅ Validation & Testing Checklist
Backend

 node server.js runs

 http://localhost:3000/ping returns pong

 http://localhost:3000/api/sensors returns JSON

Database

 sensors table contains official names

 name is UNIQUE

ESP32

 WiFi status = 3 (connected)

 GET /ping returns 200 + pong

 POST /api/ingest returns 200 + {ok:true}

Dashboard

 Values update every 5 seconds

 Fire/IR show human friendly text

 System status shows OK/WARNING/ALERT

16) 🧯 Troubleshooting
A) ESP32 POST code = -1

Wrong server IP

Router isolation
✅ Use Hotspot (192.168.137.1)

B) Server error: “Unknown column status”

Add status column or update schema:

ALTER TABLE sensors ADD COLUMN status VARCHAR(20) DEFAULT 'Normal';
C) Upsert not working (duplicates)

Ensure UNIQUE name:

ALTER TABLE sensors ADD UNIQUE KEY uniq_name (name);
D) Website shows same values always

ESP32 still sending test payload

ESP32 not reading real sensor pins

MQ connected to wrong analog pin (ADC2 conflict)

17) 🎤 Demo Script (Doctor Presentation)

Run MySQL (XAMPP)

Run server:

node server.js

Open Admin dashboard:

http://localhost:3000/cfl-eye-dashboard.html

Turn on Hotspot → connect ESP32

Show live updates:

Change gas/temperature/IR/flame and show changes + status

Export logs + show Reports page

18) 📌 Commands Summary (ALL commands we used)
Node / NPM
npm install
node server.js
Network
ipconfig
netstat -ano | findstr :3000
Firewall
netsh advfirewall firewall add rule name="Node Port 3000" dir=in action=allow protocol=TCP localport=3000
MySQL / phpMyAdmin
CREATE DATABASE CFL_Eye;

CREATE TABLE IF NOT EXISTS users (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50),
  password VARCHAR(50),
  role VARCHAR(20)
);

CREATE TABLE IF NOT EXISTS sensors (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(50),
  value VARCHAR(50),
  status VARCHAR(20) DEFAULT 'Normal'
);

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
