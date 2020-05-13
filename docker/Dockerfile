FROM debian:stable-slim as build

RUN apt-get update \
  && apt-get install -y make libc-dev binutils cmake clang git wget \
  && rm -rf /var/lib/apt/lists/*

RUN wget https://dl.bintray.com/conan/installers/conan-ubuntu-64_1_24_0.deb
RUN dpkg -i conan-ubuntu-64_1_24_0.deb

WORKDIR /root/
RUN git clone --recurse-submodules https://gitlab.szchmop.net/moustik/BOTanist.git botanist
# get some conan cache
RUN mkdir -p /root/botanist/build
RUN cd /root/botanist/build && CXX=clang++ cmake ../ -DCMAKE_BUILD_TYPE=Release

WORKDIR /root/botanist
ARG COMMIT=""
ENV COMMIT_SHA=${COMMIT}
RUN git fetch origin ${COMMIT_SHA} && git checkout FETCH_HEAD && git submodule update --init --recursive

WORKDIR /root/botanist/build
RUN CXX=clang++ cmake ../ -DCMAKE_BUILD_TYPE=Release
RUN cmake --build .
RUN cpack .


FROM debian:stable-slim

ARG COMMIT=""
LABEL commit=${COMMIT}

COPY --from=build /root/botanist/build/botanist*tar.gz .
RUN mkdir /code
RUN tar xf botanist*tar.gz -C /code

# import root certificates
COPY cacert.pem /code

WORKDIR /code/
CMD ["./botanist-0.1.1-Linux/bin/botanist"]
