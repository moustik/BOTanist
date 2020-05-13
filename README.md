# BOTanist

## Conan

### Install conan

See https://docs.conan.io/en/latest/installation.html

# Build

```sh
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

# Run

```sh
TOKEN=XXXXXXXX ./build/bin/botanist
```

# Run in docker for dev


boty image should not be pushed to prod
```sh
# prepare container
docker build -t boty -f Dockerfile.dev .

# start container
docker run --rm -it -e TOKEN=XXXXXXXX boty
```

# Resources

## tgbot-cpp

https://github.com/reo7sp/tgbot-cpp.git

https://reo7sp.github.io/tgbot-cpp/
