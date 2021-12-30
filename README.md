# VISION

## Motivation

We have a lot of cryptocurrency services, but this is stupid. no linear conditions in you'r trading strategies...

Get tranparency of trading platform with computer vision technologies now!

## Migration

- msgpuck
- tarantool-c
- httpserver.h

## Research

- zmq/msgpuck for microservices

## Requirements

- environment
  - `POSTGRES_CONN` : postgresql connection string

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

## Todo

- [x] User sign-in
- [x] Daily filtered data visualization
- [ ] Similarity vectors visualization
  - [x] search similarity in many by one
  - [ ] vector metadata for client result[bug]
  - [ ] high resolution with smaller steps
  - [ ] multiple fields eye
  - [ ] visualize
- [ ] Workflow of prediction
- [ ] Contribution managment
- [ ] Finance managment
