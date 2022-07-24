FROM archlinux as builder
RUN pacman -Syyu --noconfirm
RUN pacman -S \
    gcc \
    git \
    make \
    cmake \
    nlohmann-json \
    msgpack-c \
    bash \
    librdkafka \
    boost \
    libpqxx \
    --noconfirm
ENV LD_LIBRARY_PATH=/usr/lib:/usr/local/lib
WORKDIR /app
RUN git clone https://github.com/dpilger26/NumCpp
RUN cp -r /app/NumCpp/include/* /usr/include/
#RUN git clone https://github.com/jtv/libpqxx
#RUN cd libpqxx && mkdir build && cd build && cmake .. && make && make install clean

COPY . .
WORKDIR /app/build
RUN cmake ..
RUN make
RUN make install

FROM archlinux
RUN pacman -Syyu --noconfirm
RUN pacman -Syyu librdkafka msgpack-c nlohmann-json --noconfirm
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/lib /usr/lib
COPY --from=builder /usr/local/bin/vision /usr/local/bin/vision
CMD ["/usr/local/bin/vision"]
