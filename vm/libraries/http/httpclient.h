#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

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

#include <string>
#include <map>

// Forward declarations for C compatibility
struct ObjMap;

// HTTP response structure
struct HttpResponse {
    int status;
    char* body;
    char* headers;
    char* error;
    
    HttpResponse() : status(0), body(nullptr), headers(nullptr), error(nullptr) {}
    ~HttpResponse() {
        if (body) free(body);
        if (headers) free(headers);
        if (error) free(error);
    }
};

// URL components structure
struct UrlComponents {
    char* scheme;
    char* host;
    int port;
    char* path;
    char* query;
    char* error;
    
    UrlComponents() : scheme(nullptr), host(nullptr), port(0), path(nullptr), query(nullptr), error(nullptr) {}
    ~UrlComponents() {
        if (scheme) free(scheme);
        if (host) free(host);
        if (path) free(path);
        if (query) free(query);
        if (error) free(error);
    }
};

// HTTP client functions
HttpResponse httpGet(const char* url, ObjMap* headers = nullptr);
HttpResponse httpPost(const char* url, const char* data, ObjMap* headers = nullptr);
HttpResponse httpPut(const char* url, const char* data, ObjMap* headers = nullptr);
HttpResponse httpDelete(const char* url, ObjMap* headers = nullptr);
HttpResponse httpPatch(const char* url, const char* data, ObjMap* headers = nullptr);

// URL utility functions
char* urlEncode(const char* str);
char* urlDecode(const char* str);
UrlComponents parseUrl(const char* url);

// Helper functions
std::string buildHttpRequest(const std::string& method, const std::string& url, 
                           const std::string& data, const std::map<std::string, std::string>& headers);
std::string extractHost(const std::string& url);
std::string extractPath(const std::string& url);
int extractPort(const std::string& url);
bool isHttps(const std::string& url);
std::string getDefaultPort(const std::string& scheme);

#endif

