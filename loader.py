import os
import csv
import requests
import json
import sys

host = sys.argv[1]

def loader():
    items = []
    directory = "data"
    filenames = os.listdir(directory)

    for filename in filenames:
        # print(filename.split(".")[-2])
        with open(directory + "/" + filename, newline='') as csvfile:
            spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
            for row in spamreader:
                item = {
                    "unix": int(row[0]),
                    "datetime": row[1],
                    "symbol": row[2],
                    "open": float(row[3]),
                    "high": float(row[4]),
                    "low": float(row[5]),
                    "close": float(row[6]),
                    "volume_original": float(row[7]),
                    "volume_usd": float(row[8])
                }
                items.append(item)

        headers = {
            "authorization": "vnmntn"
        }
        resp = requests.post(host, json=items, headers=headers)
        print(resp.status_code)
        print(resp.text)


def mocks():
    headers={"authorization": "vnmntn"}
    params = {
        "symbol": "BTC/USD",
        "start_date": "2020-01-01",
        "end_date": "2021-01-01"
    }
    for v in range(1024):
        resp = requests.get(host, params=params, headers=headers)
        print(resp.text, v)

mocks()
# loader()
