FROM alpine:latest AS builder

RUN apk update && \
    apk add gcc && \
    apk add musl-dev

WORKDIR /usr/src/http-server

COPY ./src/server.c ./

RUN mkdir -p build && \
    gcc -static -o build/HttpServer server.c

FROM busybox:uclibc

LABEL org.opencontainers.image.authors="Michal Reszka @ github.com/rreszka86"
LABEL org.opencontainers.image.url="https://github.com/rreszka86/PAwChO-project"

WORKDIR /srv/HttpServer/

COPY --from=builder /usr/src/http-server/build/HttpServer ./HttpServer
COPY ./src/*.html ./

EXPOSE 8080

HEALTHCHECK --interval=5s --timeout=1s --start-period=3s --retries=3 \
    CMD wget -nv -O /dev/null localhost:8080/ || exit 1

CMD ["./HttpServer"]