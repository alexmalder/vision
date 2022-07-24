FROM archlinux as builder
RUN pacman -Syyu --noconfirm
RUN pacman -S g++ git make cmake nlohmann-json msgpack-c-dev bash librdkafka boost \
    --noconfirm
ENV LD_LIBRARY_PATH=/usr/lib:/usr/local/lib
WORKDIR /app
COPY . .

RUN git clone https://github.com/dpilger26/NumCpp /app/NumCpp
RUN cp -r /app/NumCpp/include/* /usr/include/

WORKDIR /app/build
RUN cmake ..
RUN make
RUN make install

FROM archlinux
RUN pacman -Syyu --noconfirm
RUN pacman -S librdkafka msgpack-c nlohmann-json --noconfirm
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/lib /usr/lib
COPY --from=builder /usr/local/bin/vision /usr/local/bin/vision
CMD ["/usr/local/bin/vision"]
