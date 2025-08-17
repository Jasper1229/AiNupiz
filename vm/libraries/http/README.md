# HTTP Library for NupizLang

A native HTTP client library that provides HTTP request capabilities to NupizLang programs.

## Features

- **HTTP Methods**: GET, POST, PUT, DELETE, PATCH
- **Header Management**: Custom request and response headers
- **URL Utilities**: Parsing, encoding, and decoding
- **Cross-Platform**: Works on Windows, Linux, and macOS
- **Socket-Based**: Low-level socket implementation for maximum control

## Building

The HTTP library is automatically included when you build the NupizLang VM. The build system will compile both the C wrapper (`httplib.c`) and the C++ implementation (`httpclient.cpp`).

### Prerequisites

- C++20 compatible compiler (GCC, Clang, or MSVC)
- Network libraries (included in standard C/C++ libraries)

### Windows Notes

On Windows, the library automatically links against `ws2_32.lib` for Winsock support.

## Usage

### Basic HTTP Requests

```nupiz
import http

// Simple GET request
response = http.get("https://api.example.com/users")
if response.ok {
    println(response.body)
} else {
    println("Error: " + response.error)
}

// POST with data
data = '{"name": "John", "age": 30}'
headers = {"Content-Type": "application/json"}
response = http.post("https://api.example.com/users", data, headers)
```

### Response Object

All HTTP functions return a response object with these properties:

- `status`: HTTP status code (e.g., 200, 404, 500)
- `body`: Response body as a string
- `headers`: Response headers as a map
- `ok`: Boolean indicating success (2xx status codes)
- `error`: Error message if the request failed

### URL Utilities

```nupiz
// Parse URL into components
url = "https://api.example.com:8080/users?page=1"
components = http.parseUrl(url)
println("Host: " + components.host)
println("Port: " + components.port)
println("Path: " + components.path)

// URL encoding/decoding
encoded = http.encodeUrl("Hello World!")
decoded = http.decodeUrl(encoded)
```

## API Reference

### Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| `http.get(url, headers?)` | Perform GET request | `url`: string, `headers`: optional map |
| `http.post(url, data, headers?)` | Perform POST request | `url`: string, `data`: string, `headers`: optional map |
| `http.put(url, data, headers?)` | Perform PUT request | `url`: string, `data`: string, `headers`: optional map |
| `http.delete(url, headers?)` | Perform DELETE request | `url`: string, `headers`: optional map |
| `http.patch(url, data, headers?)` | Perform PATCH request | `url`: string, `data`: string, `headers`: optional map |
| `http.parseUrl(url)` | Parse URL into components | `url`: string |
| `http.encodeUrl(str)` | URL encode string | `str`: string |
| `http.decodeUrl(str)` | URL decode string | `str`: string |

### Response Object Properties

| Property | Type | Description |
|----------|------|-------------|
| `status` | number | HTTP status code |
| `body` | string | Response body |
| `headers` | map | Response headers |
| `ok` | boolean | Success indicator |
| `error` | string | Error message (if any) |

### URL Components Object

| Property | Type | Description |
|----------|------|-------------|
| `scheme` | string | Protocol (http, https) |
| `host` | string | Hostname |
| `port` | number | Port number |
| `path` | string | URL path |
| `query` | string | Query string |

## Examples

See `npzdemos/http_demo.npz` for comprehensive usage examples.

## Implementation Details

The library consists of:

1. **`httplib.c`**: C wrapper that interfaces with the NupizLang VM
2. **`httpclient.cpp`**: C++ implementation handling socket operations
3. **`httpclient.h`**: Header file with data structures and function declarations

The implementation uses raw sockets for maximum compatibility and control. It handles:
- Cross-platform socket initialization (Winsock on Windows)
- HTTP request/response parsing
- URL parsing and manipulation
- Error handling and reporting

## Limitations

- **No HTTPS Support**: Currently only supports HTTP (no SSL/TLS)
- **Basic Parsing**: HTTP response parsing is simplified
- **No Keep-Alive**: Each request creates a new connection
- **No Compression**: No support for gzip/deflate compression

## Future Enhancements

- HTTPS support with OpenSSL/mbedTLS
- Connection pooling and keep-alive
- Request/response streaming
- Cookie management
- Authentication helpers
- Proxy support

