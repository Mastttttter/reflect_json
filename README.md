# reflect_json

`reflect_json` is a header-only C++ JSON reflection helper built on top of [`nlohmann::json`](https://github.com/nlohmann/json). It uses C++ reflection and field annotations to convert ordinary structs to and from JSON without hand-written `to_json` / `from_json` functions.

## Features

- Serialize structs with `reflect_to_json`.
- Deserialize JSON objects with `from_json_reflect` or `from_json_reflect_into`.
- Rename fields in JSON output/input.
- Ignore fields during serialization and deserialization.
- Apply defaults for missing JSON keys.
- Convert enums to and from enumerator names.
- Handle nested structs.
- Deserialize `std::vector` values recursively.
- Omit empty `std::optional<T>` members during serialization.
- Pass `nlohmann::json` members through as raw JSON values.

## Requirements

- A compiler with experimental C++ reflection support for `<meta>` and annotation syntax.
- C++26 mode.
- `nlohmann_json` available to CMake.
- `-freflection` or the equivalent compiler flag.

The sample `CMakeLists.txt` builds with:

```cmake
set(CMAKE_CXX_STANDARD 26)
find_package(nlohmann_json REQUIRED)
target_compile_options(reflect_json PRIVATE -freflection)
```

## Quick start

Copy `json_helper.hpp` and `enum_helper.hpp` into your project, include `json_helper.hpp`, and call the reflection helpers on ordinary structs.

```cpp
#include <iostream>
#include <optional>
#include <nlohmann/json.hpp>
#include "json_helper.hpp"

struct ServerConfig {
  int port = 8080;
};

struct LoggingConfig {
  std::string file_path = "logs/server.log";
  [[= json_helper::json_meta::default_string("info")]]
  std::string level;
  bool console_output = false;
};

enum class Color {
  red,
  blue,
  green,
};

struct AppConfig {
  [[= json_helper::json_meta::rename("server")]]
  [[= json_helper::json_meta::default_value{ServerConfig{.port = 7890}}]]
  ServerConfig server_config;

  LoggingConfig logging;

  [[= json_helper::json_meta::default_value{Color::green}]]
  Color color;

  std::optional<std::string> environment;
  std::optional<int> timeout_seconds;
  nlohmann::json metadata;
  std::optional<nlohmann::json> inline_metadata;

  [[= json_helper::json_meta::ignore]]
  int runtime_only_value = 0;
};

int main() {
  auto input = nlohmann::json::parse(R"({
    "environment": "production",
    "timeout_seconds": null,
    "metadata": {
      "owner": "ops",
      "tags": ["sample", "raw"]
    },
    "inline_metadata": {
      "priority": 1
    }
  })");

  auto config = json_helper::from_json_reflect<AppConfig>(input);
  auto output = json_helper::reflect_to_json(config);

  std::cout << output.dump(2) << '\n';
}
```

Output:

```json
{
  "color": "green",
  "environment": "production",
  "inline_metadata": {
    "priority": 1
  },
  "logging": {
    "console_output": false,
    "file_path": "logs/server.log",
    "level": "info"
  },
  "metadata": {
    "owner": "ops",
    "tags": [
      "sample",
      "raw"
    ]
  },
  "server": {
    "port": 7890
  }
}
```

## API

### `json_helper::reflect_to_json`

```cpp
template <typename T>
nlohmann::json reflect_to_json(T const& value);
```

Serializes a reflected struct into a JSON object. Field names come from member names unless `rename` is present. Ignored fields are skipped. Enum values are written as enumerator names. Empty optional members are omitted.

### `json_helper::from_json_reflect`

```cpp
template <typename T>
T from_json_reflect(nlohmann::json const& json);
```

Creates a default-initialized `T`, applies annotation defaults, then assigns fields found in the JSON object.

### `json_helper::from_json_reflect_into`

```cpp
template <typename T>
void from_json_reflect_into(nlohmann::json const& json, T& value);
```

Applies JSON values into an existing object after applying annotation defaults.

### `enum_helper::enum_to_string`

```cpp
template <typename E>
constexpr std::string_view enum_to_string(E value);
```

Returns the reflected enumerator name, or `"<unnamed>"` if the enum value has no reflected name.

### `enum_helper::string_to_enum`

```cpp
template <typename E>
constexpr std::optional<E> string_to_enum(std::string_view name);
```

Finds an enum value by enumerator name.

## Annotations

### `rename`

Uses a different JSON key for a field.

```cpp
struct Config {
  [[= json_helper::json_meta::rename("server_port")]]
  int port = 8080;
};
```

### `ignore`

Excludes a field from serialization and deserialization.

```cpp
struct Config {
  [[= json_helper::json_meta::ignore]]
  int runtime_only_value = 0;
};
```

### `default_value`

Sets a default for missing arithmetic, enum, optional, raw JSON, or nested struct fields before JSON assignment.

```cpp
struct Config {
  [[= json_helper::json_meta::default_value{8080}]]
  int port;
};
```

### `default_string`

Sets a default for a missing `std::string` field.

```cpp
struct Config {
  [[= json_helper::json_meta::default_string("info")]]
  std::string log_level;
};
```

## Build and run the sample

```sh
cmake -S . -B out/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/Debug
./out/Debug/reflect_json
```

The sample in `main.cpp` deserializes present optional values, null optional values, and raw JSON payloads, then prints the reflected JSON.

## Behavior notes

- Input to `from_json_reflect` must be a JSON object.
- Missing JSON keys keep constructor defaults unless an annotation default is present.
- Missing optional keys use `default_value<std::optional<T>>` when present, otherwise they reset to empty.
- JSON `null` assigned to `std::optional<T>` resets it to empty.
- `std::optional<nlohmann::json>` treats JSON `null` as empty; use plain `nlohmann::json` when a present null must be preserved.
- Raw `nlohmann::json` members pass objects, arrays, scalars, and null through unchanged.
- JSON keys override annotation defaults when present.
- Invalid enum strings throw at runtime.
- Duplicate annotations of the same kind on one target are rejected.

## Exceptions

`reflect_json` reports conversion failures with `std::runtime_error`, including non-object input, invalid enum strings, and JSON values that do not match the expected container shape.

## License

MIT. See `LICENSE`.
