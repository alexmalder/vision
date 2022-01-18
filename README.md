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

1. client request

    - `symbol`
    - `start_date`
    - `end_date`

2. gateway request

    - `symbol`
    - `start_date`
    - `end_date`
    - `user_id`: extracted from json web token

3. zmq wait msgpack tuple with fields

    - `start_date`: start date unix format
    - `end_date`: end day unix format, completed interval
    - `searchio`: symbol of cryptocurrency type
    - `user_id`: user unique identifier in database

> hardcoded values

> `thresh`: minimal of similarity
> `resolution`: step for increment size of read head

4. write data to tarantool

-   data in RAM

    -   ssize: uint64_t
    -   slide: uint64_t
    -   distance: double
    -   x: uint64_t
    -   y: uint64_t
    -   similarity: double
    -   source: double[]
    -   target: double[]

-   data for FILTER
    -   result
        -   ssize: uint64_t
        -   slide: uint64_t
        -   distance: double
        -   x: uint64_t
        -   y: uint64_t
        -   similarity: double
        -   sim_id: uint64_t
    -   similarity
        -   id: uint64_t
        -   sim_id: uint64_t
        -   value: double

5. Callback for a gateway

-   show data by `request_id` as `uint64_t` value from callback

## Note

-   symbols map
    -   `BCH/USD`: 1
    -   `BTC/USD`: 2
    -   `ETH/USD`: 3
    -   `LTC/USD`: 4
    -   `XRP/USD`: 5

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
