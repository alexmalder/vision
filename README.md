# VISION

## Motivation

We have a lot of cryptocurrency services, but this is stupidly. No linear conditions in you'r trading strategies...

Get tranparency of trading platform with computer vision technologies now!

## Stack

-   msgpuck[serialization]
-   tarantool[database]
-   zmq[communication]

## Service result

### Workflow

1. client request

    - `symbol`
    - `start_date`
    - `end_date`

2. gateway request

    - `symbol`
    - `start_date`
    - `end_date`
    - `user_id`: extracted from json web token

3. zmq wait msgpack tuple forever

    - example of tuple: `[1630454400, 1638316800, 2, 1]`

    - human readable fields
        - `start_date`: start date unix format
        - `end_date`: end day unix format, completed interval
        - `searchio`: symbol of cryptocurrency type
        - `user_id`: user unique identifier in database

4. write data to tarantool

-   RAM

    -   `ssize`: uint64_t
    -   `slide`: uint64_t
    -   `distance`: double
    -   `x`: uint64_t
    -   `y`: uint64_t
    -   `similarity`: double
    -   `source`: double[]
    -   `target`: double[]

-   TNT

    -   `user_id`: uint64_t
    -   `request_id`: uint64_t
    -   `unix`: uint64_t
    -   `symbol`: uint64_t
    -   `ssize`: uint64_t
    -   `slide`: uint64_t
    -   `distance`: double
    -   `x`: uint64_t
    -   `y`: uint64_t
    -   `similarity`: double
    -   `value`: double

5. zmq sending callback for a gateway...

-   example of tuple: `[1, 1]`

-   human readable fields
    -   `user_id`: uint64_t
    -   `request_id`: uint64_t

> Hardcoded values in runtime

> `thresh`: minimal of similarity

> `resolution`: step for increment size of read head

## Implementation specific cases

- array_t
    - array: array with `double` type like `open` or `close` or etc...
    - unix: array with `uint64_t` type, unix timestamp: not implemented
    - used: actual memory usage
    - size: allocated memory

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

```bash
tarantool
```

```lua
--
console = require('console')
console.connect('tnt_user:tnt_password@127.0.0.1:3301')
--
```

## TODO

- [ ] request_id in high level
- [ ] result_t initialization
- [ ] result_t in search engine
- [ ] result_t callback

## Functions

- `vec_similarity`
- `vec_distance`
- `vec_stabilization`
- `vec_merge`: not implemented
- `vec_slide`: not implemented

## Workflow

-   [x] user sign up and user sign in
-   [x] show similarity by latest items with range
-   [x] client-side configuration of search parameters
-   [x] similar vector stabilization
-   [ ] service result callback
-   [ ] double-line chart
-   [ ] virtual contribution
-   [ ] debug visioner algorithm
-   [ ] auto purchases and auto sales
