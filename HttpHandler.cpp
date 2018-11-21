#include "HttpHandler.h"

HttpHandler::HttpHandler(EthernetServer* server){
  _server = server;
}

void HttpHandler::Listen(){
  EthernetClient client = _server->available();
  if(client.available())
    Read(client);
}

void HttpHandler::Read(EthernetClient client){
  if(!client.available())
    return;

  bool isRequest = true;
  bool isHeaders = false;
  bool isBody = false;
  bool isLineBlank = false;

  String request = "";
  String headers = "";
  Body[0] = '\0';
  URL[0] = '\0';
  unsigned int currentRead = 0;
  
  while(client.available()){
    char in = client.read();
    if(in == '\n'){
      // move next line;
      if(isRequest){
        isRequest = false;
        isHeaders = true;
        request[currentRead] = '\0';
        currentRead = 0;
        continue;
      }
      if(isHeaders){
        if(isLineBlank){
          isHeaders = false;
          isBody = true;
          headers[currentRead] = '\0';
          currentRead = 0;

          //unsigned int indexOfStartSize = headers.indexOf("Content-Length: ");
          //unsigned int indexOfEndSize = headers.indexOf('\n', indexOfStartSize);
          //String cl = headers.substring(indexOfStartSize+16, indexOfEndSize-1);
          //int contentLength = cl.toInt();
          continue;
        } else {
          isLineBlank = true;
        }
      }
    }
    else if(in != '\r'){
      isLineBlank = false;
    }
    
    if(isRequest){
      request += in;
    } else if(isHeaders){
      headers += in;
           
    } else if(isBody){
      Body[currentRead] = in;
    }
    currentRead++;
  }
  Body[currentRead] = '\0';
  HandleRequest(request);
  
  LOG("Available Method: ");LOGLN(Method);
  LOG("Available URL: ");LOGLN(URL);
  LOG("Available Body: ");LOGLN(Body);

  bool hasMethods = false;
  for(byte f = 0;f<MAXFUNCTIONS;f++){
    if(strlen(Functions[f].URL) == 0){
      LOGLN("There are no functions to handle. breaking cycle");
      break;
    }
    if(((String)URL).startsWith(Functions[f].URL) && Functions[f].Method == Method){
      client.print(RES200OK);
      String res = Functions[f].Function(URL, Body);
      client.print(res);
      hasMethods = true;
      break;
    }
  }
  if(!hasMethods)
    client.print(RES404NF);
    
  client.flush();
  delay(1);
  client.stop();
}

void HttpHandler::HandleRequest(String request) {
  unsigned int endString = 0;
  unsigned int startString = 0;
  
  // let's find the first index of ' ' to retrieve the Method;
  endString = request.indexOf(' ', 1) + 1;
  LOG("First index of ' ': ");LOGLN(endString);
  char method[endString];
  request.substring(0, endString).toCharArray(method, endString);

  if(strcmp(method, "GET") == 0)
    Method = GET;
  else if(strcmp(method, "POST") == 0)
    Method = POST;
  else if(strcmp(method, "PUT") == 0)
    Method = PUT;
  else if(strcmp(method, "PATCH") == 0)
    Method = PATCH;
  else if(strcmp(method, "DELETE") == 0)
    Method = DELETE;
  else if(strcmp(method, "OPTIONS") == 0)
    Method = OPTIONS;
  else if(strcmp(method, "HEAD") == 0)
    Method = HEAD;

  // lets get the url
  startString = endString;
  endString = request.indexOf(' ', startString) + 1;

  request.substring(startString, endString).toCharArray(URL, endString-startString);

}

/*int HttpHandler::GetHeaders(String request){
  bool headers = true;
  byte currentHeader = 0;
  int startString = 0;
  int endString;
  int middleString;
  
  while(headers){
    // let's get the headers
    // first we get the end of line since it's 2 characters we need to add them to the start string
    endString = request.indexOf('\n', startString);
    //LOG("startofline: ");LOG(startString);LOG("; endofline: ");LOGLN(endString);
    byte l = endString-startString;
    if(l == 1)
      break;
    //LOG("length: ");LOGLN(l);
    char header[l];
    String headerComplete = request.substring(startString, endString);
    middleString = headerComplete.indexOf(':') + 1;
    //LOG("middleString: ");LOGLN(middleString);

    char name[middleString];
    headerComplete.substring(0, middleString).toCharArray(name, middleString);
    Headers[currentHeader].Name = name;
    
    char value[sizeof(headerComplete)-middleString];
    headerComplete.substring(middleString, sizeof(value)).toCharArray(value, sizeof(value));
    Headers[currentHeader].Value = value;    
    //LOG("headerName: ");LOGLN(Headers[currentHeader].Name);
    //LOG("headerValue: ");LOGLN(Headers[currentHeader].Value);

    startString = endString + 1;
    currentHeader++;
  }
  // we need to take in consideration the line ends
  return startString+2;
}*/

void HttpHandler::AddMethod(char * url, HttpMethod method, String (*f)(char *, char *)){
  //LOG("Adding function to method: ");LOGLN(url);
  if(MAXFUNCTIONS == _currentFunction){
    LOGLN("CAN'T ADD MORE FUNCTIONS");
    return;
  }
  strcpy(Functions[_currentFunction].URL, url);
  Functions[_currentFunction].Method = method;
  Functions[_currentFunction].Function = f;
  _currentFunction++;
}
