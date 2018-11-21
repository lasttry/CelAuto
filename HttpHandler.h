#ifndef HttpHandler_h
#define HttpHandler_h

#include "Ethernet.h"
#include "Celog.h"
#define RES200OK "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST, GET, PUT, OPTIONS\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n"
#define RES404NF "HTTP/1.1 404 Not Found\r\nContent-Type: text/html; charset=UTF-8\r\nReferrer-Policy: no-referrer\r\nConnection: close\r\n\r\n"

#define MAXHEADERS 10
#define MAXFUNCTIONS 10
#define MAXHEADERNAME 20
#define MAXHEADERVALUE 20
#define MAXURL 20
#define MAXBODY 2000

class HttpHandler {
  public:
    enum HttpMethod {
      GET = 0,
      POST = 1,
      PUT = 2,
      PATCH = 3,
      DELETE = 4,
      OPTIONS = 5,
      HEAD = 6,
      NONE = 7
    };
  
    struct CallFunctions {
      char URL[MAXURL];
      HttpMethod Method;
      String (*Function)(char *, char *);
    };
  
    //struct HttpHeaders {
    //  char[MAXHEADERNAME] Name;
    //  char[MAXHEADERVALUE] Value;
    //};
    
    HttpHandler(EthernetServer* server);
    
    HttpMethod Method;
    char Body[MAXBODY];
    char URL[MAXURL];
    
    void Listen();
    void AddMethod(char * url, HttpMethod method, String (*f)(char *, char *));
  private:

    //HttpHeaders Headers[MAXHEADERS];
    CallFunctions Functions[MAXFUNCTIONS];
    byte _currentFunction;
    EthernetServer* _server;

    void Read(EthernetClient);
    void HandleRequest(String request);
    //int GetHeaders(String request);
};

#endif
