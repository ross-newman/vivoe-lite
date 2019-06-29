#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include "log_gva.h"

using namespace std;

void
logGva::log (string message, int type)
{
  log(message.c_str(), type);
}

void
logGva::log (char *message, int type)
{
  struct sysinfo info;
  char msgType[4] = "???";

  sysinfo (&info);
  switch (type)
    {
    case LOG_DEBUG:
      strcpy (msgType, "DBG");
      break;
    case LOG_INFO:
      strcpy (msgType, "INF");
      break;
    case LOG_WARNING:
      strcpy (msgType, "WAR");
      break;
    case LOG_ERROR:
      strcpy (msgType, "ERR");
      break;
    }

#if !DEBUG
        /** Discard debug message if DEBUG not enabled */
  if (type == LOG_DEBUG)
    return;
#endif
  if (!m_errorfd)
    {
      m_errorfd = fopen ("log.txt", "w");
    }
  fprintf (m_errorfd, "[%ld] *%s* %s\n", info.uptime, msgType, message);
  fflush(m_errorfd);
}

void
logGva::finish ()
{
  fclose (m_errorfd);
}
