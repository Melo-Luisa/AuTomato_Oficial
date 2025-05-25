document.getElementById("pos-form").addEventListener("submit", async function (e) {
  e.preventDefault();

  const focoFinal = document.getElementById("focoFinal").value;
  const comentarios = document.getElementById("comentarios").value;

  const task1 = document.getElementById("task1").value;
  const task2 = document.getElementById("task2").value;
  const task3 = document.getElementById("task3").value;
  const task4 = document.getElementById("task4").value;
  const task5 = document.getElementById("task5").value;
  const task6 = document.getElementById("task6").value;
  const task7 = document.getElementById("task7").value;
  const task8 = document.getElementById("task8").value;

  const resposta = await fetch("https://script.google.com/macros/s/AKfycbxQmSl1qUDc3QCYeoJCkorbKjYJ9RM7-XptwYOw_z9BW0mQ4fOr3E8-TqQZLh07fDnX/exec", {
    method: "POST",
    body: JSON.stringify({
      focoFinal,
      comentarios,
      task1, task2, task3, task4, task5, task6, task7, task8
    }),
    headers: {
      "Content-Type": "application/json"
    }
  });

  if (resposta.ok) {
    alert("Dados enviados com sucesso!");
    // Aqui você pode redirecionar ou exibir uma mensagem final
  } else {
    alert("Erro ao enviar os dados.");
  }
});

