# json_parse

A small, header-only JSON parser for C++;

Include `include/json.hpp` and compile with the `include` directory on your
include path:

See [`examples/parse.cpp`](examples/parse.cpp) for parsing and accessing JSON,
and [`examples/build.cpp`](examples/build.cpp) for constructing and serializing
JSON.

The parser represents JSON objects as `std::map<std::string, Json>`, arrays as
`std::vector<Json>`, integers as `std::int64_t`, and decimals as `double`.
