document.getElementById("pre-form").addEventListener("submit", async function (e) {
  e.preventDefault();

  const fatigue = document.querySelector('input[name="fatigue"]:checked').value;
  const motivation = document.querySelector('input[name="motivation"]:checked').value;
  const productivity = document.querySelector('input[name="productivity"]:checked').value;

  const resposta = await fetch("https://script.google.com/macros/s/AKfycbyf4K3u5_Jg3LeEQr41e3W8JWjBo5wKKpEsjBcnmy1H6-ain0iIgIFreFg6k32JK6GZuA/exec", {
    method: "POST",
    body: JSON.stringify({ fatigue, motivation, productivity }),
    headers: {
      "Content-Type": "application/json"
    }
  });

  if (resposta.ok) {
    alert("Dados enviados com sucesso!");
    window.location.href = "pomodoro.html";
  } else {
    alert("Erro ao enviar dados.");
  }
});

