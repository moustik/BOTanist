# BOTanist

## Conan

### Install conan

See https://docs.conan.io/en/latest/installation.html

### Add new remotes

```sh
conan remote add community https://api.bintray.com/conan/conan-community/conan
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

# Build

```sh
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

# Run

```sh
TOKEN=XXXXXXXX ./build/bin/botanist
```

# Resources

## tgbot-cpp

https://github.com/reo7sp/tgbot-cpp.git

https://reo7sp.github.io/tgbot-cpp/
