import os
import csv
import requests
import sys
import getopt
import time
# import json


def upload(host, headers):
    directory = "data"
    filenames = os.listdir(directory)

    for filename in filenames:
        # print(filename.split(".")[-2])
        with open(directory + "/" + filename, newline='') as csvfile:
            request = {}
            items = []
            spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
            for i, row in enumerate(spamreader):
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
                if (item["open"] != 0):
                    items.append(item)
                if (i % 100 == 0):
                    request["length"] = len(items)
                    request["items"] = items
                    resp = requests.post(
                        host + "/v1/crypto", json=request, headers=headers)
                    print(resp.status_code)
                    print(resp.text)
                    items = []


def mocks(host, headers):
    routes = ["crypto", "workflow"]
    resp = requests.get(host + "/v1/workflow/fields", headers=headers)
    if resp.status_code == 200:
        print(resp.text)
        resp_json = resp.json()
        for field_name in resp_json:
            params = {
                "symbol": "BTC/USD",
                "start_date": "2021-01-01",
                "end_date": "2021-03-01",
                "field_name": field_name["column_name"]
            }
            for route in routes:
                endpoint = host + '/v1/' + route
                resp = requests.get(endpoint, params=params, headers=headers)
                if resp.status_code != 200:
                    print(resp.status_code)
                    sys.exit(1)
                print(resp.text)
    else:
        print("bad status code", resp.status_code, resp.text)
        sys.exit(1)


def main():
    headers = {"authorization": "vnmntn"}
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:s:")
        print(opts, args)
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)
    host = str()
    stage = str()
    for o, a in opts:
        if o in ("-h", "--host"):
            host = a
        elif o in ("-s", "--stage"):
            stage = a
        else:
            assert False, "unhandled option : getopt"
    if host is not None:
        if stage == "upload":
            upload(host, headers)
        elif stage == "mocks":
            mocks(host, headers)
        else:
            print("python3 api.py -h <host> -s <stage>")
    else:
        print("host is none")
        sys.exit(1)


main()
