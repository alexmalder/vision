# VISION

## Motivation

We have a lot of cryptocurrency services, but this is stupid. No linear conditions in you'r trading strategies...

Get tranparency of trading platform with computer vision technologies now!

## Migration

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

-   tuples with fields
    -   `start_date` : `uint64_t`
    -   `end_date` : `uint64_t`

2. Callback on gateway

-   show data by unique identifier as `uint64_t` value from callback

## Requirements

-   environment
    - tarantool database

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

## How to create space and configure it

> interactive

```lua
--
console = require('console')
console.connect('tnt_user:tnt_password@127.0.0.1:3301')
--
```

## Workflow

- show similarity by latest items with range
- client-side configuration of search parameters
- double-line chart
- virtual contribution web view
