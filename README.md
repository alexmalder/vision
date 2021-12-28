# VISION

## Workflow

- [ ] User sign-in
- [ ] Daily filtered data visualization
- [ ] Similarity vectors visualization
- [ ] Prediction interface
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
  `POSTGRES_CONN` : postgresql connection string

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
