FROM alpine:3.16 as builder
RUN apk update
RUN apk add \
    g++ \
    git \
    make \
    cmake \
    nlohmann-json \
    msgpack-c-dev \
    bash \
    librdkafka-dev \
    boost-dev \
    postgresql-dev
ENV LD_LIBRARY_PATH=/usr/lib:/usr/local/lib
WORKDIR /app
RUN git clone https://github.com/dpilger26/NumCpp
RUN cp -r /app/NumCpp/include/* /usr/include/
RUN git clone https://github.com/jtv/libpqxx
RUN cd libpqxx && mkdir build && cd build && cmake .. && make && make install clean

COPY . .
WORKDIR /app/build
RUN cmake ..
RUN make
RUN make install

FROM alpine:3.14
RUN apk update
RUN apk add librdkafka-dev msgpack-c nlohmann-json
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/lib /usr/lib
COPY --from=builder /usr/local/bin/vision /usr/local/bin/vision
CMD ["/usr/local/bin/vision"]
