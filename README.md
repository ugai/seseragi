# Seseragi

A simple tool for checking the hierarchy information of Alembic files.

## Motivation

Once I wanted to check the object hierarchy of some Alembic files, but existing tools were not easy to build or use. So I created this app.

## Build from source

- CMake
- Vcpkg
- Rust
- Qt (for Windows)

```bash
git clone --recursive https://github.com/ugai/seseragi
cd seseragi
vcpkg install
cmake -B build
cmake --build build
```

## License

MIT Licene

- Icon from [いらすとや](https://www.irasutoya.com/)

## Related Apps

- [abcview](https://github.com/alembic/abcview)
- [Web Alembic Viewer](https://github.com/i-saint/WebAlembicViewer)
- [Alembic USD Plugin](https://openusd.org/release/plugins_alembic.html#alembic-usd-plugin)
