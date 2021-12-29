FROM alpine:3.14

RUN apk update
RUN apk add g++ git make cmake json-c-dev openssl-dev

ENV LD_LIBRARY_PATH=/usr/local/lib
WORKDIR /app
COPY . .
WORKDIR /app/build

RUN cmake ..
RUN make
RUN make install

WORKDIR /app

CMD ["/usr/local/bin/vision"]
