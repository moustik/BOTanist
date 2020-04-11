FROM debian:stable-slim as build

RUN apt-get update \
  && apt-get install -y make libc-dev binutils cmake clang git wget \
  && rm -rf /var/lib/apt/lists/*

RUN wget https://dl.bintray.com/conan/installers/conan-ubuntu-64_1_24_0.deb
RUN dpkg -i conan-ubuntu-64_1_24_0.deb

WORKDIR /root/
RUN git clone --recurse-submodules https://github.com/moustik/BOTanist.git botanist

WORKDIR /root/botanist
RUN mkdir build

WORKDIR /root/botanist/build
RUN CXX=clang++ cmake ../ -DCMAKE_BUILD_TYPE=Release
RUN cmake --build .
RUN cpack .


FROM debian:stable-slim

WORKDIR /root/
COPY --from=build /root/botanist/build/botanist*tar.gz .
RUN tar xf botanist*tar.gz

CMD ["./botanist-0.1.1-Linux/bin/botanist"]
