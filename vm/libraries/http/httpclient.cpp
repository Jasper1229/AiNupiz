#include "httpclient.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cctype>
#include <algorithm>

#ifdef _WIN32
    static bool winsockInitialized = false;
    static void initWinsock() {
        if (!winsockInitialized) {
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            winsockInitialized = true;
        }
    }
#endif

// Helper function to convert ObjMap to std::map
std::map<std::string, std::string> objMapToStdMap(ObjMap* objMap) {
    std::map<std::string, std::string> result;
    if (!objMap) return result;
    
    // This is a simplified conversion - you'd need to implement proper iteration
    // over the ObjMap structure based on your VM's implementation
    return result;
}

// Extract host from URL
std::string extractHost(const std::string& url) {
    size_t schemeEnd = url.find("://");
    if (schemeEnd == std::string::npos) return "";
    
    size_t hostStart = schemeEnd + 3;
    size_t hostEnd = url.find('/', hostStart);
    if (hostEnd == std::string::npos) hostEnd = url.length();
    
    size_t portStart = url.find(':', hostStart);
    if (portStart != std::string::npos && portStart < hostEnd) {
        hostEnd = portStart;
    }
    
    return url.substr(hostStart, hostEnd - hostStart);
}

// Extract path from URL
std::string extractPath(const std::string& url) {
    size_t schemeEnd = url.find("://");
    if (schemeEnd == std::string::npos) return "/";
    
    size_t hostStart = schemeEnd + 3;
    size_t pathStart = url.find('/', hostStart);
    if (pathStart == std::string::npos) return "/";
    
    size_t queryStart = url.find('?', pathStart);
    if (queryStart == std::string::npos) {
        return url.substr(pathStart);
    }
    
    return url.substr(pathStart, queryStart - pathStart);
}

// Extract port from URL
int extractPort(const std::string& url) {
    size_t schemeEnd = url.find("://");
    if (schemeEnd == std::string::npos) return 80;
    
    size_t hostStart = schemeEnd + 3;
    size_t portStart = url.find(':', hostStart);
    if (portStart == std::string::npos) return 80;
    
    size_t pathStart = url.find('/', hostStart);
    if (pathStart == std::string::npos) pathStart = url.length();
    
    if (portStart >= pathStart) return 80;
    
    std::string portStr = url.substr(portStart + 1, pathStart - portStart - 1);
    return std::stoi(portStr);
}

// Check if URL uses HTTPS
bool isHttps(const std::string& url) {
    return url.substr(0, 5) == "https";
}

// Get default port for scheme
std::string getDefaultPort(const std::string& scheme) {
    if (scheme == "https") return "443";
    return "80";
}

// Build HTTP request string
std::string buildHttpRequest(const std::string& method, const std::string& url, 
                           const std::string& data, const std::map<std::string, std::string>& headers) {
    std::string path = extractPath(url);
    if (path.empty()) path = "/";
    
    std::ostringstream request;
    request << method << " " << path << " HTTP/1.1\r\n";
    request << "Host: " << extractHost(url) << "\r\n";
    
    // Add custom headers
    for (const auto& header : headers) {
        request << header.first << ": " << header.second << "\r\n";
    }
    
    // Add content length if there's data
    if (!data.empty()) {
        request << "Content-Length: " << data.length() << "\r\n";
    }
    
    request << "\r\n";
    
    if (!data.empty()) {
        request << data;
    }
    
    return request.str();
}

// URL encoding
char* urlEncode(const char* str) {
    if (!str) return nullptr;
    
    std::string result;
    for (int i = 0; str[i]; i++) {
        char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02X", (unsigned char)c);
            result += hex;
        }
    }
    
    char* encoded = (char*)malloc(result.length() + 1);
    if (encoded) {
        strcpy(encoded, result.c_str());
    }
    return encoded;
}

// URL decoding
char* urlDecode(const char* str) {
    if (!str) return nullptr;
    
    std::string result;
    for (int i = 0; str[i]; i++) {
        if (str[i] == '%' && i + 2 < strlen(str)) {
            char hex[3] = {str[i+1], str[i+2], 0};
            int val;
            if (sscanf(hex, "%x", &val) == 1) {
                result += (char)val;
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    
    char* decoded = (char*)malloc(result.length() + 1);
    if (decoded) {
        strcpy(decoded, result.c_str());
    }
    return decoded;
}

// Parse URL into components
UrlComponents parseUrl(const char* url) {
    UrlComponents components;
    if (!url) {
        components.error = strdup("URL is null");
        return components;
    }
    
    std::string urlStr(url);
    
    // Extract scheme
    size_t schemeEnd = urlStr.find("://");
    if (schemeEnd != std::string::npos) {
        std::string scheme = urlStr.substr(0, schemeEnd);
        components.scheme = strdup(scheme.c_str());
    } else {
        components.scheme = strdup("http");
    }
    
    // Extract host
    size_t hostStart = (schemeEnd != std::string::npos) ? schemeEnd + 3 : 0;
    size_t hostEnd = urlStr.find('/', hostStart);
    if (hostEnd == std::string::npos) hostEnd = urlStr.length();
    
    size_t portStart = urlStr.find(':', hostStart);
    if (portStart != std::string::npos && portStart < hostEnd) {
        components.host = strdup(urlStr.substr(hostStart, portStart - hostStart).c_str());
        std::string portStr = urlStr.substr(portStart + 1, hostEnd - portStart - 1);
        components.port = std::stoi(portStr);
    } else {
        components.host = strdup(urlStr.substr(hostStart, hostEnd - hostStart).c_str());
        components.port = (strcmp(components.scheme, "https") == 0) ? 443 : 80;
    }
    
    // Extract path
    if (hostEnd < urlStr.length()) {
        size_t queryStart = urlStr.find('?', hostEnd);
        if (queryStart != std::string::npos) {
            components.path = strdup(urlStr.substr(hostEnd, queryStart - hostEnd).c_str());
            components.query = strdup(urlStr.substr(queryStart + 1).c_str());
        } else {
            components.path = strdup(urlStr.substr(hostEnd).c_str());
        }
    } else {
        components.path = strdup("/");
    }
    
    return components;
}

// Perform HTTP request
HttpResponse performHttpRequest(const std::string& method, const std::string& url, 
                              const std::string& data, const std::map<std::string, std::string>& headers) {
    HttpResponse response;
    
#ifdef _WIN32
    initWinsock();
#endif
    
    std::string host = extractHost(url);
    int port = extractPort(url);
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        response.error = strdup("Failed to create socket");
        return response;
    }
    
    // Resolve hostname
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        response.error = strdup("Failed to resolve hostname");
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return response;
    }
    
    // Setup server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        response.error = strdup("Failed to connect to server");
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return response;
    }
    
    // Build and send request
    std::string request = buildHttpRequest(method, url, data, headers);
    if (send(sock, request.c_str(), request.length(), 0) < 0) {
        response.error = strdup("Failed to send request");
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return response;
    }
    
    // Receive response
    std::string responseData;
    char buffer[4096];
    int bytesReceived;
    
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        responseData += buffer;
    }
    
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    
    if (responseData.empty()) {
        response.error = strdup("No response received");
        return response;
    }
    
    // Parse response
    size_t headerEnd = responseData.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        response.error = strdup("Invalid response format");
        return response;
    }
    
    std::string responseHeaders = responseData.substr(0, headerEnd);
    std::string body = responseData.substr(headerEnd + 4);
    
    // Extract status code
    size_t statusStart = responseHeaders.find("HTTP/1.1 ");
    if (statusStart != std::string::npos) {
        statusStart += 9;
        size_t statusEnd = responseHeaders.find(' ', statusStart);
        if (statusEnd != std::string::npos) {
            std::string statusStr = responseHeaders.substr(statusStart, statusEnd - statusStart);
            response.status = std::stoi(statusStr);
        }
    }
    
    response.headers = strdup(responseHeaders.c_str());
    response.body = strdup(body.c_str());
    
    return response;
}

// HTTP GET request
HttpResponse httpGet(const char* url, ObjMap* headers) {
    std::map<std::string, std::string> stdHeaders = objMapToStdMap(headers);
    return performHttpRequest("GET", url, "", stdHeaders);
}

// HTTP POST request
HttpResponse httpPost(const char* url, const char* data, ObjMap* headers) {
    std::map<std::string, std::string> stdHeaders = objMapToStdMap(headers);
    if (!data) data = "";
    return performHttpRequest("POST", url, data, stdHeaders);
}

// HTTP PUT request
HttpResponse httpPut(const char* url, const char* data, ObjMap* headers) {
    std::map<std::string, std::string> stdHeaders = objMapToStdMap(headers);
    if (!data) data = "";
    return performHttpRequest("PUT", url, data, stdHeaders);
}

// HTTP DELETE request
HttpResponse httpDelete(const char* url, ObjMap* headers) {
    std::map<std::string, std::string> stdHeaders = objMapToStdMap(headers);
    return performHttpRequest("DELETE", url, "", stdHeaders);
}

// HTTP PATCH request
HttpResponse httpPatch(const char* url, const char* data, ObjMap* headers) {
    std::map<std::string, std::string> stdHeaders = objMapToStdMap(headers);
    if (!data) data = "";
    return performHttpRequest("PATCH", url, data, stdHeaders);
}

