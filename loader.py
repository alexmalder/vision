import os
import csv
import requests
import json


def main():
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
            "authorization": "vnmntn@gmail.com"
        }
        resp = requests.post("http://localhost:5000/crypto",
                             json=items, headers=headers)
        print(resp.status_code)
        print(resp.text)


main()
