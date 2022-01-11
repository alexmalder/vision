FROM alpine:3.14 as builder

RUN apk update
RUN apk add \
    g++ \
    git \
    make \
    cmake \
    zeromq-dev \
    bash
    #openssl-dev 
COPY ./cmake_install /cmake_install
RUN /cmake_install rtsisyk/msgpuck master
RUN /cmake_install tarantool/tarantool-c master

ENV LD_LIBRARY_PATH=/usr/local/lib
WORKDIR /app
COPY . .
WORKDIR /app/build

RUN cmake ..
RUN make
RUN make install

FROM alpine:3.14
RUN apk update
RUN apk add zeromq 
#openssl
COPY --from=builder /usr/local/lib /usr/local/lib
#COPY --from=builder /usr/lib /usr/lib
COPY --from=builder /usr/local/bin/vision /usr/local/bin/vision
CMD ["/usr/local/bin/vision"]
