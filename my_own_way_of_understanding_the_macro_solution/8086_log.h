#ifndef __8086_LOG_H__
#define __8086_LOG_H__
#include <stdarg.h>

#define UNUSED(value) (void)(value)
#define TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

typedef enum{
  INFO,
  WARNING,
  ERROR,
  NO_LOGS,
} logLevel_t;

logLevel_t MinimalLogLevel = INFO;

void Log( logLevel_t LogLevel, const char *Format, ...)
{
  if(LogLevel < MinimalLogLevel){
    return;
  }
  switch(LogLevel) {
    case INFO:{
      fprintf(stderr, "[INFO]:");
    }break;

    case WARNING:{
      fprintf(stderr, "[WARNING]:");
    }break;

    case ERROR:{
      fprintf(stderr, "[ERROR]:");
    }break;

    case NO_LOGS:{
      return;
    }

    default:{
      UNREACHABLE("Log");
    }
  }

  va_list Arguments;
  va_start(Arguments, Format);
  vfprintf(stderr, Format, Arguments);
  va_end(Arguments);
  fprintf(stderr,"\n");
}

void LogFatal( logLevel_t LogLevel, const char *Format, ...)
{
  if(LogLevel < MinimalLogLevel){
    return;
  }
  switch(LogLevel) {
    case INFO:{
      fprintf(stderr, "[INFO]:");
    }break;

    case WARNING:{
      fprintf(stderr, "[WARNING]:");
    }break;

    case ERROR:{
      fprintf(stderr, "[ERROR]:");
    }break;

    case NO_LOGS:{
      return;
    }

    default:{
      UNREACHABLE("Log");
    }
  }

  va_list Arguments;
  va_start(Arguments, Format);
  vfprintf(stderr, Format, Arguments);
  va_end(Arguments);
  fprintf(stderr,"\n");
  exit(21);
}

#endif
