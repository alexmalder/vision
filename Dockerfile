FROM gcc:10

RUN apt update
RUN apt install git make cmake libmicrohttpd-dev libyaml-cpp-dev libpqxx-dev nlohmann-json-dev libtool autoconf -y

RUN git clone https://github.com/etr/libhttpserver
RUN cd libhttpserver && ./bootstrap && mkdir build && cd build && ../configure && make && make install && cd ../ && rm -rf libhttpserver

RUN git clone https://github.com/trusch/libbcrypt && cd libbcrypt && mkdir build && cd build && cmake .. && make && make install && cd ../ && rm -rf libbcrypt

ENV LD_LIBRARY_PATH=/usr/local/lib
WORKDIR /app
COPY . .
WORKDIR /app/build

RUN cmake ..
RUN make
RUN make install

WORKDIR /app

CMD ["/usr/local/bin/vision", "/app/config.yml"]
