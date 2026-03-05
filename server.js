const express = require("express");
const mysql = require("mysql2");
const cors = require("cors");
const path = require("path");

const app = express();

// ================= Middleware =================
app.use(cors());
app.use(express.json());

// ⭐ يخلي السيرفر يعرض ملفات الموقع
app.use(express.static(path.join(__dirname, "public")));

// ================= MySQL Connection =================
// ✅ في XAMPP غالبًا root بدون باسورد
const db = mysql.createPool({
  host: "localhost",
  user: "root",
  password: "",
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

// ================= LOGIN API =================
app.post("/api/auth/login", (req, res) => {
  const { username, password } = req.body || {};

  if (!username || !password) {
    return res.json({ success: false });
  }

  db.query(
    "SELECT role FROM users WHERE username=? AND password=? LIMIT 1",
    [username, password],
    (err, results) => {
      if (err) {
        console.error("❌ Login Query Error:", err.message);
        return res.status(500).json({ success: false });
      }

      if (results.length > 0) {
        return res.json({ success: true, role: results[0].role });
      } else {
        return res.json({ success: false });
      }
    }
  );
});

// ================= SENSORS API =================
app.get("/api/sensors", (req, res) => {
  db.query("SELECT name, value FROM sensors", (err, results) => {
    if (err) {
      console.error("❌ Sensors Query Error:", err.message);
      return res.status(500).json({ error: "Database error" });
    }
    res.json(results);
  });
});

// ================= ESP32 INGEST API =================
app.post("/api/ingest", (req, res) => {
  // ✅ لتشخيص أي مشكلة (شوفيها بالترمينال)
  console.log("📩 ingest:", req.body);

  const { gas, humidity, temperature } = req.body || {};

  const gasVal  = Number(gas);
  const humVal  = Number(humidity);
  const tempVal = Number(temperature);

  const updates = [
    ["gas",         Number.isFinite(gasVal)  ? String(gasVal)  : "0"],
    ["humidity",    Number.isFinite(humVal)  ? String(humVal)  : "0"],
    ["temperature", Number.isFinite(tempVal) ? String(tempVal) : "0"],
  ];

  const sql = "UPDATE sensors SET value=? WHERE name=?";

  let done = 0;

  for (const [name, value] of updates) {
    db.query(sql, [value, name], (err) => {
      if (err) {
        console.error("❌ Ingest update error:", err.message);
        return res.status(500).json({ ok: false, error: err.message });
      }

      done++;
      if (done === updates.length) {
        return res.json({ ok: true });
      }
    });
  }
});

// ================= Default Route =================
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "public", "index.html"));
});

// ================= Start Server =================
const PORT = 3000;

// ✅ أهم تعديل: 0.0.0.0 عشان ESP والموبايل يقدروا يوصلوا
app.listen(PORT, "0.0.0.0", () => {
  console.log(`🚀 Server running on http://0.0.0.0:${PORT}`);
  console.log(`➡️ Local:  http://localhost:${PORT}`);
  console.log(`➡️ Network: http://192.168.1.109:${PORT}`);
});