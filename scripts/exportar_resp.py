import requests
import csv

# IP do seu ESP32 em modo Access Point
URL = "http://192.168.4.1/respostas.json"

response = requests.get(URL)
data = response.json()

# Supondo que cada item é um dicionário com as mesmas chaves
with open('respostas.csv', mode='w', newline='', encoding='utf-8') as file:
    writer = csv.DictWriter(file, fieldnames=data[0].keys())
    writer.writeheader()
    writer.writerows(data)

print("CSV gerado com sucesso!")
