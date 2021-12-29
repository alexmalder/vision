import os
import csv
import requests
import json
import sys
import getopt


def upload(host):
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
        resp = requests.post(host + "/v1/crypto", json=items, headers=headers)
        print(resp.status_code)
        print(resp.text)


def mocks(host):
    headers={"authorization": "vnmntn"}
    params = {
        "symbol": "BTC/USD",
        "start_date": "2021-01-01",
        "end_date": "2021-01-03"
    }
    resp = requests.get(host + "/v1/crypto", params=params, headers=headers)
    print(resp.text)

    resp = requests.get(host + "/v1/fields", headers=headers)
    print(resp.text)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:s:")
        # print(opts, args)
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)
    stage = str()
    for o, a in opts:
        if o in ("-h", "--host"):
            host = a
        elif o in ("-s", "--stage"):
            stage = a
        else:
            assert False, "unhandled option : getopt"

    if stage == "upload":
        upload(host)
    elif stage == "mocks":
        mocks(host)
    else:
        print("python3 api.py -h <host> -s <stage>")

main()
