#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
#endif

#include "httplib.h"

// Simple HTTP GET implementation in pure C
static char* performHttpGet(const char* url) {
    // Initialize Winsock on Windows
    #ifdef _WIN32
    static bool winsockInitialized = false;
    if (!winsockInitialized) {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        winsockInitialized = true;
    }
    #endif
    
    // Parse URL to extract host and path
    char* host = NULL;
    char* path = NULL;
    int port = 80;
    
    // Find scheme
    char* schemeEnd = strstr(url, "://");
    if (schemeEnd) {
        if (strncmp(url, "https", 5) == 0) {
            port = 443;
        }
        url = schemeEnd + 3;
    }
    
    // Find host and path
    char* pathStart = strchr(url, '/');
    char* portStart = strchr(url, ':');
    
    if (portStart && (!pathStart || portStart < pathStart)) {
        // Port is specified
        size_t hostLen = portStart - url;
        host = malloc(hostLen + 1);
        strncpy(host, url, hostLen);
        host[hostLen] = '\0';
        port = atoi(portStart + 1);
        url = pathStart ? pathStart : url + strlen(url);
    } else {
        // No port specified
        size_t hostLen = pathStart ? (pathStart - url) : strlen(url);
        host = malloc(hostLen + 1);
        strncpy(host, url, hostLen);
        host[hostLen] = '\0';
        url = pathStart ? pathStart : url + strlen(url);
    }
    
    // Set path
    if (*url == '/') {
        path = strdup(url);
    } else {
        path = strdup("/");
    }
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        free(host);
        free(path);
        return strdup("Failed to create socket");
    }
    
    // Resolve hostname
    struct hostent* server = gethostbyname(host);
    if (!server) {
        closesocket(sock);
        free(host);
        free(path);
        return strdup("Failed to resolve hostname");
    }
    
    // Setup server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        closesocket(sock);
        free(host);
        free(path);
        return strdup("Failed to connect to server");
    }
    
    // Build HTTP request
    char request[2048];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: NupizLang/1.0\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host);
    
    // Send request
    if (send(sock, request, strlen(request), 0) < 0) {
        closesocket(sock);
        free(host);
        free(path);
        return strdup("Failed to send request");
    }
    
    // Receive response
    char buffer[4096];
    char* response = malloc(1);
    response[0] = '\0';
    size_t responseLen = 0;
    
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        
        // Reallocate response buffer
        char* newResponse = realloc(response, responseLen + bytesReceived + 1);
        if (!newResponse) {
            free(response);
            closesocket(sock);
            free(host);
            free(path);
            return strdup("Failed to allocate memory for response");
        }
        response = newResponse;
        
        // Append to response
        strcpy(response + responseLen, buffer);
        responseLen += bytesReceived;
    }
    
    // Close socket
    closesocket(sock);
    free(host);
    free(path);
    
    if (responseLen == 0) {
        free(response);
        return strdup("No response received");
    }
    
    // Extract body from response (skip headers)
    char* bodyStart = strstr(response, "\r\n\r\n");
    if (bodyStart) {
        bodyStart += 4; // Skip \r\n\r\n
        char* body = strdup(bodyStart);
        free(response);
        return body;
    } else {
        // No body separator found, return full response
        return response;
    }
}

// Simple URL parsing that returns individual components
static NativeResult parseUrlNative(VM* vm, int argc, Value* args) {
    if (!expectArgs(vm, argc, 1)) {
        return NATIVE_FAIL;
    }
    
    if (!IS_STRING(args[0])) {
        runtimeError(vm, "Expected string as argument for parseUrl.");
        return NATIVE_FAIL;
    }
    
    ObjString* urlStr = AS_STRING(args[0]);
    const char* url = urlStr->chars;
    
    // Simple URL parsing
    char* scheme = NULL;
    char* host = NULL;
    int port = 80;
    char* path = NULL;
    char* query = NULL;
    
    // Find scheme
    char* schemeEnd = strstr(url, "://");
    if (schemeEnd) {
        size_t schemeLen = schemeEnd - url;
        scheme = malloc(schemeLen + 1);
        strncpy(scheme, url, schemeLen);
        scheme[schemeLen] = '\0';
        url = schemeEnd + 3;
        
        // Set default port based on scheme
        if (strcmp(scheme, "https") == 0) {
            port = 443;
        }
    }
    
    // Find host and port
    char* pathStart = strchr(url, '/');
    char* portStart = strchr(url, ':');
    
    if (portStart && (!pathStart || portStart < pathStart)) {
        // Port is specified
        size_t hostLen = portStart - url;
        host = malloc(hostLen + 1);
        strncpy(host, url, hostLen);
        host[hostLen] = '\0';
        
        port = atoi(portStart + 1);
        url = pathStart ? pathStart : url + strlen(url);
    } else {
        // No port specified
        size_t hostLen = pathStart ? (pathStart - url) : strlen(url);
        host = malloc(hostLen + 1);
        strncpy(host, url, hostLen);
        host[hostLen] = '\0';
        url = pathStart ? pathStart : url + strlen(url);
    }
    
    // Find path and query
    if (*url == '/') {
        char* queryStart = strchr(url, '?');
        if (queryStart) {
            size_t pathLen = queryStart - url;
            path = malloc(pathLen + 1);
            strncpy(path, url, pathLen);
            path[pathLen] = '\0';
            
            query = strdup(queryStart + 1);
        } else {
            path = strdup(url);
        }
    } else {
        path = strdup("/");
    }
    
    // For now, just return the host as a string to test functionality
    ObjString* hostStr = copyString(vm, host ? host : "localhost", host ? strlen(host) : 9);
    
    // Clean up allocated memory
    if (scheme) free(scheme);
    if (host) free(host);
    if (path) free(path);
    if (query) free(query);
    
    return NATIVE_VAL(OBJ_VAL(hostStr));
}

// Real HTTP GET implementation in pure C
static NativeResult httpGetNative(VM* vm, int argc, Value* args) {
    if (!expectArgs(vm, argc, 1)) {
        return NATIVE_FAIL;
    }
    
    if (!IS_STRING(args[0])) {
        runtimeError(vm, "Expected string URL as first argument for http.get.");
        return NATIVE_FAIL;
    }
    
    ObjString* urlStr = AS_STRING(args[0]);
    const char* url = urlStr->chars;
    
    // Perform the actual HTTP GET request
    char* response = performHttpGet(url);
    
    if (!response) {
        ObjString* errorStr = copyString(vm, "Unknown error occurred", 22);
        return NATIVE_VAL(OBJ_VAL(errorStr));
    }
    
    // Check if response is an error message
    if (strncmp(response, "Failed", 6) == 0 || strncmp(response, "No response", 11) == 0) {
        ObjString* errorStr = copyString(vm, response, strlen(response));
        free(response);
        return NATIVE_VAL(OBJ_VAL(errorStr));
    }
    
    // Return the successful response
    ObjString* responseStr = copyString(vm, response, strlen(response));
    free(response);
    return NATIVE_VAL(OBJ_VAL(responseStr));
}

// Temporary stub functions for other HTTP operations
static NativeResult httpPostNative(VM* vm, int argc, Value* args) {
    // TODO: Implement actual HTTP POST
    return NATIVE_FAIL;
}

static NativeResult urlEncodeNative(VM* vm, int argc, Value* args) {
    // TODO: Implement URL encoding
    return NATIVE_FAIL;
}

static NativeResult urlDecodeNative(VM* vm, int argc, Value* args) {
    // TODO: Implement URL decoding
    return NATIVE_FAIL;
}

bool importHttpLib(VM* vm, ObjString* lib) {
    LIBFUNC("get", httpGetNative);
    LIBFUNC("post", httpPostNative);
    LIBFUNC("encodeUrl", urlEncodeNative);
    LIBFUNC("decodeUrl", urlDecodeNative);
    LIBFUNC("parseUrl", parseUrlNative);
    
    return true;
}

