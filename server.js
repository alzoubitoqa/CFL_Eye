const express = require("express");
const mysql = require("mysql2");
const cors = require("cors");
const path = require("path");

const app = express();

// ================= Middleware =================
app.use(cors());
app.use(express.json());

// ✅ Log requests (للتشخيص)
app.use((req, res, next) => {
  console.log(`➡️ ${req.method} ${req.url}`);
  next();
});

// ⭐ يخلي السيرفر يعرض ملفات الموقع
app.use(express.static(path.join(__dirname, "public")));

// ================= MySQL Connection =================
const db = mysql.createPool({
  host: "localhost",
  user: "root",
  password: "",          // ✅ XAMPP غالبًا فاضي
  database: "CFL_Eye",
  port: 3306,
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0
});

// ✅ اختبار اتصال سريع
db.getConnection((err, connection) => {
  if (err) {
    console.log("❌ DB Connection Error:", err.message);
    console.log("➡️ تأكدي أن MySQL شغال على 3306 وأن بيانات الدخول صحيحة.");
  } else {
    console.log("✅ Connected to MySQL DB");
    connection.release();
  }
});

// ================= Ping (مهم جدًا للاختبار) =================
app.get("/ping", (req, res) => {
  res.status(200).send("pong");
});

// ================= Helpers =================
function computeStatus(name, value) {
  const GAS_WARNING = 500;
  const GAS_ALERT = 800;
  const TEMP_WARNING = 35;
  const TEMP_ALERT = 50;

  const v = (value ?? "").toString().trim();

  if (name === "Gas Level") {
    const n = Number(v);
    if (isNaN(n)) return "Normal";
    if (n >= GAS_ALERT) return "Alert";
    if (n >= GAS_WARNING) return "Warning";
    return "Normal";
  }

  if (name === "Temperature") {
    const n = Number(v);
    if (isNaN(n)) return "Normal";
    if (n >= TEMP_ALERT) return "Alert";
    if (n >= TEMP_WARNING) return "Warning";
    return "Normal";
  }

  if (name === "Fire Sensor") {
    if (v === "0" || v.toLowerCase().includes("fire") || v.toLowerCase().includes("detected")) {
      return "Alert";
    }
    return "Normal";
  }

  if (name === "Lab Door") {
    if (v.toLowerCase() === "open" || v === "1") return "Warning";
    return "Normal";
  }

  if (name === "Evidence Locker") {
    if (v.toLowerCase() === "open" || v === "1") return "Warning";
    return "Normal";
  }

  if (name === "IR") {
    if (v === "1") return "Alert";
    return "Normal";
  }

  return "Normal";
}

// ================= LOGIN API =================
app.post("/api/auth/login", (req, res) => {
  const { username, password } = req.body || {};

  if (!username || !password) return res.json({ success: false });

  db.query(
    "SELECT role FROM users WHERE username=? AND password=? LIMIT 1",
    [username, password],
    (err, results) => {
      if (err) return res.status(500).json({ success: false });
      if (results.length > 0) return res.json({ success: true, role: results[0].role });
      return res.json({ success: false });
    }
  );
});

// ================= SENSORS API =================
app.get("/api/sensors", (req, res) => {
  db.query("SELECT name, value, status FROM sensors", (err, results) => {
    if (err) return res.status(500).json({ error: "Database error" });
    res.json(results);
  });
});

// ================= ESP32 INGEST API =================
app.post("/api/ingest", (req, res) => {
  console.log("📩 ingest body:", req.body);

  const { gas, humidity, temperature, fire, door, locker, ir } = req.body || {};

  const rows = [
    ["Gas Level", gas ?? 0],
    ["Humidity", humidity ?? 0],
    ["Temperature", temperature ?? 0],
    ["Fire Sensor", fire ?? 1],
    ["Lab Door", door ?? "Closed"],
    ["Evidence Locker", locker ?? "Stored"],
    ["IR", ir ?? 0]
  ].map(([name, value]) => {
    const v = String(value);
    const st = computeStatus(name, v);
    return [name, v, st];
  });

  const sql = `
    INSERT INTO sensors (name, value, status)
    VALUES ?
    ON DUPLICATE KEY UPDATE
      value = VALUES(value),
      status = VALUES(status)
  `;

  db.query(sql, [rows], (err) => {
    if (err) return res.status(500).json({ ok: false, error: err.message });
    res.json({ ok: true });
  });
});

// ================= Default Route =================
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "public", "index.html"));
});

// ================= Start Server =================
const PORT = 3000;

app.listen(PORT, "0.0.0.0", () => {
  console.log(`🚀 Server running on http://0.0.0.0:${PORT}`);
  console.log(`➡️ Local:   http://localhost:${PORT}`);
  console.log(`➡️ Network: http://192.168.1.109:${PORT}`);
  console.log(`➡️ Test:    http://192.168.1.109:${PORT}/ping`);
});