FROM alpine:3.14 as builder

RUN apk update
RUN apk add g++ git make cmake json-c-dev openssl-dev

ENV LD_LIBRARY_PATH=/usr/local/lib
WORKDIR /app
COPY . .
WORKDIR /app/build

RUN cmake ..
RUN make
RUN make install

FROM alpine:3.14

RUN apk update
RUN apk add json-c openssl
COPY --from=builder /usr/local/bin/vision /usr/local/bin/vision

CMD ["/usr/local/bin/vision"]
