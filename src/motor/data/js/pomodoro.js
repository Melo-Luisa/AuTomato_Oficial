let timer;
let remaining = 0;

function startTimer() {
  clearInterval(timer);
  const minutes = parseInt(document.getElementById("minutes").value);
  remaining = minutes * 60;

  // Envia tempo ao ESP
  fetch("/tempo", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ minutos: minutes })
  }).then(res => res.json())
    .then(data => console.log("Tempo enviado ao ESP:", data));

  updateTimer();
  timer = setInterval(updateTimer, 1000);
}

function updateTimer() {
  if (remaining <= 0) {
    clearInterval(timer);
    window.location.href = "pos-questionario.html";
    return;
  }

  let mins = Math.floor(remaining / 60);
  let secs = remaining % 60;
  document.getElementById("timer").innerText =
    \`\${String(mins).padStart(2, '0')}:\${String(secs).padStart(2, '0')}\`;
  remaining--;
}

function resetTimer() {
  clearInterval(timer);
  const minutes = parseInt(document.getElementById("minutes").value);
  remaining = minutes * 60;
  document.getElementById("timer").innerText = \`\${String(minutes).padStart(2, '0')}:00\`;
}