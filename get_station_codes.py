import csv
import requests
import os
from dotenv import load_dotenv

load_dotenv()

api_key = os.getenv("WMATA_API_KEY")
if not api_key:
    raise ValueError("WMATA_API_KEY not found in .env file. Did you properly set it up like .env-example?")

response = requests.get(
    'https://api.wmata.com/Rail.svc/json/jStations',
    headers={"api_key": api_key}
)

out = []

try:
    stations = response.json()['Stations']
except Exception as e:
    raise RuntimeError("Failed to parse stations from WMATA API response.") from e

for i in stations:
    out.append([
        i['Name'],
        i['Code'],
        ', '.join([i[f'LineCode{z}'] for z in range(1, 5) if i.get(f'LineCode{z}')])
    ])

header = ["Station", "Code", "Lines"]
with open('wmata_station_codes.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(header)
    for i in sorted(out):
        writer.writerow(i)
