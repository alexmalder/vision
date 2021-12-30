import os
import csv
import requests
import sys
import getopt
import tarantool
from tarantool.connection import Connection
from tarantool.error import DatabaseError
# import json


def construct_tuples(crypto):
    my_tuples = []
    for row in crypto:
        unix = int(row[0])
        datetime = row[1]
        symbol = row[2]
        _open = float(row[3])
        _high = float(row[4])
        _low = float(row[5]),
        _close = float(row[6])
        volume_original = float(row[7])
        volume_usd = float(row[8])
        if (_open != 0 and _close != 0 and _low != 0):
            my_tuple = (unix, datetime, symbol, _open, _high,
                        _low, _close, volume_original, volume_usd)
            my_tuples.append(my_tuple)
    return my_tuples


def client_create() -> Connection:
    tnt_host = os.getenv("TNT_HOST")
    tnt_port = os.getenv("TNT_PORT")
    tnt_user = os.getenv("TNT_USER")
    tnt_pass = os.getenv("TNT_PASSWORD")
    if (tnt_host is None or tnt_port is None or tnt_user is None or tnt_pass is None):
        sys.exit(1)
    client = tarantool.connect(host=tnt_host, port=int(
        tnt_port), user=tnt_user, password=tnt_pass)
    return client


def upload():
    directory = "data"
    total_length = 0
    client = client_create()
    space = client.space(517)
    filenames = os.listdir(directory)
    for filename in filenames:
        # print(filename.split(".")[-2])
        print(filename)
        with open(directory + "/" + filename, newline='') as csvfile:
            crypto = csv.reader(csvfile, delimiter=',', quotechar='|')
            my_tuples = construct_tuples(crypto)
            for my_tuple in my_tuples:
                try:
                    space.insert(my_tuple)
                    total_length += 1
                except DatabaseError as e:
                    print(e)
    print("total_length: ", total_length)


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
            upload()
        elif stage == "mocks":
            mocks(host, headers)
        else:
            print("python3 api.py -h <host> -s <stage>")
    else:
        print("host is none")
        sys.exit(1)


main()
