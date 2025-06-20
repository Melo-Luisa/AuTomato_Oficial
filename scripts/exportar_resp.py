import requests
import csv
import json

ESP_IP = "http://192.168.4.1"
url = f"{ESP_IP}/respostas.json"

try:
    response = requests.get(url)
    response.raise_for_status()

    # Aqui pegamos o texto da resposta
    lines = response.text.strip().splitlines()

    # Convertendo cada linha em um dicionário
    data = [json.loads(line) for line in lines if line.strip()]

    # Salva em CSV
    with open("respostas_exportadas.csv", mode="w", newline="", encoding="utf-8") as csv_file:
        if data:
            writer = csv.DictWriter(csv_file, fieldnames=data[0].keys())
            writer.writeheader()
            writer.writerows(data)
            print("✅ Respostas exportadas para 'respostas_exportadas.csv'")
        else:
            print("⚠️ Nenhuma resposta para exportar.")

except requests.exceptions.RequestException as e:
    print("❌ Erro ao acessar o ESP:", e)
except json.JSONDecodeError as e:
    print("❌ Erro ao decodificar o JSON:", e)
