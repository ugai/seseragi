# Seseragi

![icon](asset/icon-64.png)

A simple GUI tool for checking the hierarchy information of Alembic files.

## Build from source

### Prerequisites

- Git
- CMake
- Vcpkg
  - Add the directory containing `vcpkg` executable to your `PATH` and `VCPKG_ROOT` environment variables.
- Rust

### Build

```bash
# Ubuntu 22.04 {{{
sudo apt install build-essentials cargo cmake curl git libgtk-3-dev pkg-config rustc tar unzip zip
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update && sudo apt install g++-13
export CXX=/usr/bin/g++-13
# }}}

git clone --recursive https://github.com/ugai/seseragi
cd seseragi
vcpkg install
cmake -B build
cmake --build build
```

## License

This project under the [MIT License](LICENSE).

- [Third Party Notices](ThirdPartyNotices.md)
- This app uses [Slint UI framework](https://slint.dev/) with a [royality free license](https://github.com/slint-ui/slint/blob/d299f0bf3eff7e6b272c3e2331b75596922e102f/LICENSES/LicenseRef-Slint-Royalty-free-1.1.md).

[![madewithslint](asset/MadeWithSlint-logo-dark.svg)](https://madewithslint.com/)

## Similar apps

- [AbcTree](https://github.com/alembic/alembic/tree/master/bin/AbcTree)
- [ogawa-rs](https://github.com/Traverse-Research/ogawa-rs)
