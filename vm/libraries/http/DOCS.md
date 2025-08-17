# HTTP Library

A comprehensive HTTP client library for NupizLang that provides:

- HTTP GET, POST, PUT, DELETE, PATCH requests
- Request/response headers management
- JSON payload support
- URL parsing and validation
- Response status codes and body handling
- Cookie management
- Basic authentication

## Usage Examples

```nupiz
import http

// Simple GET request
response = http.get("https://api.example.com/users")
println(response.body)

// POST with JSON
headers = {"Content-Type": "application/json"}
data = {"name": "John", "age": 30}
response = http.post("https://api.example.com/users", data, headers)

// Custom request
request = http.Request()
request.url = "https://api.example.com/data"
request.method = "PUT"
request.headers = {"Authorization": "Bearer token123"}
request.body = "custom data"
response = http.send(request)
```

## Functions

- `http.get(url, headers?)` - Perform GET request
- `http.post(url, data?, headers?)` - Perform POST request
- `http.put(url, data?, headers?)` - Perform PUT request
- `http.delete(url, headers?)` - Perform DELETE request
- `http.patch(url, data?, headers?)` - Perform PATCH request
- `http.send(request)` - Send custom request object
- `http.parseUrl(url)` - Parse URL into components
- `http.encodeUrl(data)` - URL encode data
- `http.decodeUrl(data)` - URL decode data

## Response Object

Response objects contain:
- `status` - HTTP status code
- `body` - Response body as string
- `headers` - Response headers as map
- `ok` - Boolean indicating success (2xx status)
- `error` - Error message if request failed

