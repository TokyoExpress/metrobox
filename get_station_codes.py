import csv
import requests

api_key = "11d2faeb6b4d4d01a70ef02d8c331258"

r = requests.get('https://api.wmata.com/Rail.svc/json/jStations', headers={"api_key": api_key})

out = []

for i in r.json()['Stations']:
    out.append([i['Name'], i['Code'], ', '.join([i[f'LineCode{z}'] for z in range(1, 5) if i[f'LineCode{z}']])])

header = ["Station", "Code", "Lines"]
with open('wmata_station_codes.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(header)
    for i in sorted(out):
        writer.writerow(i)