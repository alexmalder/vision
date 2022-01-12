# VISION

## Motivation

We have a lot of cryptocurrency services, but this is stupidly. No linear conditions in you'r trading strategies...

Get tranparency of trading platform with computer vision technologies now!

## Stack

-   msgpuck
-   tarantool
-   zmq

## Research

-   zmq/msgpuck for microservices
    -   socket bind
    -   simple deserialization
    -   service result structure

## Service result

Description

-   simple `uint64_t` value is an unique identifier in tarantool database

1. Step one

-   msgpack tuple with fields

    -   `start_date`: start date unix format
    -   `end_date`: end day unix format, completed interval
    -   `searchio`: symbol of cryptocurrency
    -   `user_id`: user unique identifier in database

-   symbols map
    -   `BCH/USD`: 1
    -   `BTC/USD`: 2
    -   `ETH/USD`: 3
    -   `LTC/USD`: 4
    -   `XRP/USD`: 5

2. Callback for a gateway

-   show data by `request_id` as `uint64_t` value from callback

## Environment

-   tarantool

## How to build

```bash
#/bin/bash
set -eo pipefail

mkdir -p build
cd build
cmake ..
make
make install #optional
```

## How to connect to tarantool

> interactive

```lua
--
console = require('console')
console.connect('tnt_user:tnt_password@127.0.0.1:3301')
--
```

## Workflow

-   [x] user sign up and user sign in
-   [x] show similarity by latest items with range
-   [x] client-side configuration of search parameters
-   [ ] similar vector stabilization
-   [ ] double-line chart
-   [ ] virtual contribution
-   [ ] debug visioner algorithm ...
-   [ ] find beta testers
-   [ ] auto purchase and auto sale
