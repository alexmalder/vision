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

```lua
box.schema.space.create('crypto')

box.space.crypto:format({
  {name='unix', type='number'}, 
  {name='datetime', type='string'},
  {name='symbol', type='string'}, 
  {name='open', type='number'}, 
  {name='high', type='number'},
  {name='low', type='number'},
  {name='close', type='number'},
  {name='volume_original', type='number'},
  {name='volume_usd', type='number'}
})

box.space.crypto:create_index('primary', { 
  unique = true, 
  parts = { 
    {field=1, type='number'} 
  } 
})
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
