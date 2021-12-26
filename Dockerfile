FROM alpine:3.14

RUN apk update
RUN apk add g++ git make cmake yaml-cpp-dev postgresql13-dev nlohmann-json

RUN git clone https://github.com/yhirose/cpp-httplib && cp cpp-httplib/httplib.h /usr/local/include && rm -rf cpp-httplib

ARG REPO=jtv/libpqxx
RUN git clone https://github.com/$REPO && cd $REPO && mkdir build && cd build && cmake .. && make && make install && cd ../ && rm -rf $REPO

ARG REPO=trusch/libbcrypt
RUN git clone https://github.com/$REPO && cd $REPO && mkdir build && cd build && cmake .. && make && make install && cd ../ && rm -rf $REPO

ENV LD_LIBRARY_PATH=/usr/local/lib
WORKDIR /app
COPY . .
WORKDIR /app/build

RUN cmake ..
RUN make
RUN make install

WORKDIR /app

CMD ["/usr/local/bin/vision", "/app/config.yml"]
