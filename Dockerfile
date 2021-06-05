FROM gcc:10.3 as build

RUN apt-get update && \
    apt-get install -y \
      cmake \
      qt5-default \
      libqt5network5 \
      qtbase5-dev \
    && rm -rf /var/lib/apt/lists/*

ADD ./secret_sharing /app/secret_sharing

WORKDIR /app/build

RUN cmake ../secret_sharing && \
    make

ENTRYPOINT ["./secret_sharing"]
