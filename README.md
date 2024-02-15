# Seseragi

![icon](asset/icon-64.png)

A simple GUI tool for checking the hierarchy information of Alembic files.

## Motivation

Once I wanted to check the object hierarchy of some Alembic files, but existing tools were not easy to build or use. So I created this app.

## Build from source

- Git
- CMake
- Vcpkg
  - Add the directory containing `vcpkg.exe` to your `PATH` and `VCPKG_ROOT` environment variables.
- Rust

```bash
git clone --recursive https://github.com/ugai/seseragi
cd seseragi
vcpkg install
cmake -B build
cmake --build build
```

## Similar apps

- [AbcTree](https://github.com/alembic/alembic/tree/master/bin/AbcTree)
- [abcview](https://github.com/alembic/abcview)
- [Web Alembic Viewer](https://github.com/i-saint/WebAlembicViewer)
- [Alembic USD Plugin](https://openusd.org/release/plugins_alembic.html#alembic-usd-plugin)

## License

This project under the [MIT License](LICENSE).

- This app uses [Slint UI framework](https://slint.dev/) with a [royality free license](https://github.com/slint-ui/slint/blob/d299f0bf3eff7e6b272c3e2331b75596922e102f/LICENSES/LicenseRef-Slint-Royalty-free-1.1.md).

[![madewithslint](asset/MadeWithSlint-logo-dark.svg)](https://madewithslint.com/)
