function loginUser(event) {
  event.preventDefault();

  const username = document.getElementById("username")?.value?.trim();
  const password = document.getElementById("password")?.value;

  if (!username || !password) {
    alert("⚠️ يرجى إدخال اسم المستخدم وكلمة المرور");
    return;
  }

  const API_BASE = window.location.origin; // ✅ بدل localhost

  fetch(`${API_BASE}/api/auth/login`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ username, password })
  })
    .then(res => res.json())
    .then(data => {
      if (data.success) {
        if (data.role === "admin") {
          window.location.href = "cfl-eye-dashboard.html";
        } else {
          window.location.href = "dashboard.html";
        }
      } else {
        alert("❌ اسم المستخدم أو كلمة المرور غير صحيحة");
      }
    })
    .catch(err => {
      console.error(err);
      alert("❌ مشكلة اتصال بالسيرفر");
    });
}

// ✅ تحديث بيانات الحساسات فقط إذا الصفحة فيها sensors
function updateUserSensors() {
  const sensorEls = document.querySelectorAll(".sensor");
  if (!sensorEls || sensorEls.length === 0) return; // ✅ مو صفحة داشبورد

  const API_BASE = window.location.origin;

  fetch(`${API_BASE}/api/sensors`)
    .then(res => res.json())
    .then(data => {
      const idMap = {
        "Fire Sensor": 0,
        "Humidity": 1,
        "Temperature": 2,
        "Gas Level": 3,
        "Lab Door": 4,
        "Evidence Locker": 5
      };

      data.forEach(sensor => {
        const idx = idMap[sensor.name];
        if (idx === undefined) return;

        const sensorDiv = sensorEls[idx];
        if (!sensorDiv) return;

        const span = sensorDiv.querySelector("span");
        if (span) span.textContent = sensor.value;
      });
    })
    .catch(err => console.error(err));
}

// ✅ شغل التحديث فقط إذا الصفحة فيها sensors
document.addEventListener("DOMContentLoaded", () => {
  const sensorEls = document.querySelectorAll(".sensor");
  if (sensorEls && sensorEls.length > 0) {
    updateUserSensors();
    setInterval(updateUserSensors, 5000);
  }
});