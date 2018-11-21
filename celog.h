#ifndef CelaLog_h
#define CelaLog_h

#ifndef DEBUG
#define LOG_SETUP() Serial.begin(250000);
#define LOG(var) Serial.print(var);
#define LOGLN(var) Serial.println(var);
#else
#define LOG_SETUP()
#define LOG(var)
#define LOGLN(var)
#endif

#endif
