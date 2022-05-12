FROM alpine:3.14 as builder

RUN apk update
RUN apk add \
    g++ \
    git \
    make \
    cmake \
    nlohmann-json \
    msgpack-c-dev \
    #zeromq-dev \
    bash \
    librdkafka-dev
    #openssl-dev 
#COPY ./cmake_install /cmake_install
#RUN /cmake_install rtsisyk/msgpuck master
#RUN /cmake_install tarantool/tarantool-c master

RUN git clone https://github.com/yhirose/cpp-httplib /cpp-httplib
RUN mkdir -p /usr/local/include
RUN cp /cpp-httplib/httplib.h /usr/local/include/httplib.h
RUN rm -rf /cpp-httplib

ENV LD_LIBRARY_PATH=/usr/lib:/usr/local/lib
WORKDIR /app
COPY . .
WORKDIR /app/build

RUN cmake ..
RUN make
RUN make install

FROM alpine:3.14
RUN apk update
RUN apk add \
    #zeromq \
    librdkafka-dev \
    msgpack-c \
    nlohmann-json
COPY ./data /data
#openssl
COPY --from=builder /usr/local/include/httplib.h /usr/local/include/httplib.h
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/lib /usr/lib
COPY --from=builder /usr/local/bin/vision /usr/local/bin/vision
CMD ["/usr/local/bin/vision"]
