# VISION

## Todo

- [x] User sign-in
- [x] Daily filtered data visualization
- [ ] Similarity vectors visualization
  - [x] search similarity in many by one
  - [ ] vector metadata for client result
  - [ ] high resolution with smaller steps
  - [ ] multiple fields eye
  - [ ] visualize
- [ ] Workflow of prediction
- [ ] Contribution managment
- [ ] Finance managment

## Dependencies

- pq
- pqxx
- yaml-cpp
- bcrypt
- cpp-httplib

## Research

- msgpack
- zmq
- rbtree

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
