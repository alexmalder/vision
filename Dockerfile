FROM alpine:3.16 as builder
RUN apk update
RUN apk add g++ git make cmake nlohmann-json msgpack-c-dev bash librdkafka-dev
ENV LD_LIBRARY_PATH=/usr/lib:/usr/local/lib
WORKDIR /app
COPY . .
WORKDIR /app/build

RUN git clone https://github.com/dpilger26/NumCpp
WORKDIR /app/NumCpp/build
RUN cmake .. -DBUILD_TESTS=ON -DBUILD_MULTIPLE_TEST=ON -DBUILD_EXAMPLE_GAUSS_NEWTON_NLLS=ON -DBUILD_EXAMPLE_README=ON
RUN cmake --build . --config Release 
RUN cmake --build . --config Release --target install

WORKDIR /app
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
