## 1) الملفات التي يجب نسخها

انسخي **مجلد المشروع كامل** كما هو، ويحتوي على:

* `server.js`
* `package.json`
* `package-lock.json`
* مجلد `public/` وفيه:

  * `index.html`
  * `admin.html`
  * `dashboard.html`
  * `cfl-eye-dashboard.html`
  * `reports.html`
  * `style.css`
  * `script.js`

هذا التقسيم متوافق مع اعتماد السيرفر على Express وخدمة ملفات الواجهة من مجلد `public`، ومع وجود ملف السيرفر وملفات الحزم في جذر المشروع.  

---

## 2) البرامج التي يجب تثبيتها على الجهاز الجديد

### أ) Node.js

لازم يكون مثبت **Node.js** لأن المشروع يعتمد على:

* `express`
* `mysql2`
* `cors`
* `body-parser`

وهذه الحزم موجودة داخل `package.json`. 

يفضل تثبيت **Node.js LTS**.

بعد التثبيت، افتحي Terminal واكتبي:

```bash
node -v
npm -v
```

---

### ب) MySQL أو XAMPP

لازم يكون موجود **MySQL Server** أو **XAMPP** مع MySQL، لأن السيرفر يتصل بقاعدة بيانات اسمها:

```text
CFL_Eye
```

وبيانات الاتصال الحالية في `server.js` هي:

* host: `localhost`
* user: `root`
* password: `m7moud13579`
* database: `CFL_Eye` 

إذا على الجهاز الجديد كلمة سر root مختلفة، لازم تعدليها داخل `server.js`.

---

### ج) Arduino IDE

لازم تثبتي **Arduino IDE** لأن كود الـ ESP32 يترفع منه.

---

### د) تعريف ESP32 على Arduino IDE

لازم يكون مثبت **ESP32 Board Package** داخل Arduino IDE، لأن المشروع يعتمد على لوحة ESP32.

---

### هـ) Driver لوحة ESP32

حسب نوع اللوحة، لازم يكون مثبت Driver مثل:

* **CP210x**
  أو
* **CH340**

إذا الجهاز الجديد ما تعرف على منفذ الـ COM، فالمشكلة غالبًا من الـ Driver.

---

## 3) المكتبات المطلوبة في Arduino IDE

على الجهاز الجديد، لازم تثبتي نفس المكتبات التي استخدمتيها مع الحساسات:

* **DHT sensor library**
* **Adafruit Unified Sensor**

وإذا عندك كود ESP32 يستخدم:

* `WiFi.h`
* `HTTPClient.h`

فهذه غالبًا تأتي مع ESP32 package نفسها.

---

## 4) إعداد قاعدة البيانات

على الجهاز الجديد لازم تعملي:

### أ) إنشاء قاعدة البيانات:

```sql
CREATE DATABASE CFL_Eye;
```

### ب) إنشاء الجداول التي يعتمد عليها المشروع

#### جدول users

لأن عندك Login API يفحص:

```sql
SELECT * FROM users WHERE username=? AND password=?
```

فلازم يكون موجود جدول `users`. 

مثال بسيط:

```sql
CREATE TABLE users (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50),
  password VARCHAR(50),
  role VARCHAR(20)
);
```

وأضيفي مستخدمين مثل:

```sql
INSERT INTO users (username, password, role) VALUES
('admin', '1234', 'admin'),
('user', '1234', 'user');
```

#### جدول sensors

لأن الواجهة والسيرفر يقرؤون من:

```sql
SELECT name, value FROM sensors
```

فلازم يكون موجود جدول `sensors`. 

مثال:

```sql
CREATE TABLE sensors (
  name VARCHAR(50) PRIMARY KEY,
  value VARCHAR(50),
  status VARCHAR(20),
  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
```

وأضيفي قيماً أولية:

```sql
INSERT INTO sensors (name, value, status) VALUES
('Fire Sensor', '0', 'Normal'),
('Humidity', '0', 'Normal'),
('Temperature', '0', 'Normal'),
('Gas Level', '0', 'Normal'),
('Lab Door', '0', 'Normal'),
('Evidence Locker', '0', 'Normal');
```

---

## 5) تثبيت حزم المشروع

بعد نقل الملفات إلى الجهاز الجديد، افتحي Terminal داخل مجلد المشروع وشغلي:

```bash
npm install
```

هذا سيثبت الحزم الموجودة في `package.json`:

* express
* mysql2
* cors
* body-parser 

---

## 6) تشغيل السيرفر

بعد تثبيت الحزم:

```bash
node server.js
```

إذا كل شيء صحيح، المفروض يظهر:

* اتصال ناجح مع MySQL
* تشغيل السيرفر على:

```text
http://localhost:3000
```

وهذا يتوافق مع إعدادات `server.js` الحالية. 

---

## 7) تشغيل الموقع

لا تفتحي ملفات HTML مباشرة من الملفات.

الطريقة الصحيحة:

```text
http://localhost:3000
```

لأن صفحاتك تعتمد على `fetch` إلى:

* `/api/auth/login`
* `/api/sensors`

وهي موجودة في `script.js` وفي صفحات الداشبورد.

---

## 8) إذا بدك ربط ESP32 مع الجهاز الجديد

إذا الهدف ليس فقط نقل الموقع بل أيضًا تشغيل الحساسات من خلال الجهاز الجديد، لازم:

### أ) تعرفي IP الجهاز الجديد

من CMD:

```bash
ipconfig
```

وخذي:

```text
IPv4 Address
```

### ب) تعدلي كود ESP32

بدل IP القديم، تحطي IP الجهاز الجديد داخل رابط السيرفر، مثل:

```cpp
http://192.168.x.x:3000/api/ingest
```

### ج) تتأكدي أن:

* اللابتوب وESP32 على نفس شبكة Wi-Fi
* MQ sensor موصول على pin مناسب للـ ADC إذا كنت تستخدمين WiFi مع ESP32

---

## 9) قائمة فحص سريعة قبل التشغيل

على الجهاز الجديد لازم يكون موجود:

* [ ] مجلد المشروع كامل
* [ ] Node.js
* [ ] npm
* [ ] MySQL أو XAMPP
* [ ] قاعدة بيانات `CFL_Eye`
* [ ] جدول `users`
* [ ] جدول `sensors`
* [ ] Arduino IDE
* [ ] ESP32 board package
* [ ] ESP32 USB driver
* [ ] مكتبات DHT
* [ ] تشغيل `npm install`
* [ ] تشغيل `node server.js`

---


