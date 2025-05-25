document.getElementById('wifi-form').addEventListener('submit', async function (e) {
  e.preventDefault();

  const ssid = document.getElementById('ssid').value;
  const password = document.getElementById('password').value;

  const response = await fetch("/connect", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ ssid, password })
  });

  const result = await response.json();

  if (result.success) {
    window.location.href = "pre-questionario.html";
  } else {
    document.getElementById("status").innerText = "❌ Falha ao conectar.";
  }
});