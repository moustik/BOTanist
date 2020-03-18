# BOTanist

## Conan

### Install conan

See https://docs.conan.io/en/latest/installation.html

### Add new remotes

conan remote add community https://api.bintray.com/conan/conan-community/conan
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan

# Build

```sh
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```
