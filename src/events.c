#include <string.h>
#include "events.h"
#include "tree.h"
#include <stdlib.h>
#include <stdio.h>

int isEvent(char *str) {
  int i=0;
  for (i=0; i<NUM_EVENTS; i++) {
    if (strcmp(str, events[i])==0)
      return 1;
  }
  return 0;
}

int countArgs(char *str) {
  int i=0;
  for (i=0; i<NUM_EVENTS; i++) {
    if (strcmp(str, events[i])==0)
      break;
  }

  return numArgs[i];
}


SymType typeAt(char *name, int place) {
  int count = countArgs(name);

  int i, k;
  for (i=0; i<NUM_EVENTS; i++) {
    if (strcmp(name, events[i])==0)
      break;
  }


  if (argTypes[i][place] == 1)
    return PatientT;
  else if (argTypes[i][place] == 2)
    return DoctorT;
  
  return PatientT;
}

const char *eventTypeToString(EventType type){
  return events[type];
}

/* Given an Even Type returns the correspoinding query 
 * if the event type doesn't exist returns null */
static char *getEventQuery(EventType type)
{
  int i;
  for(i = 0; i < NUM_EVENTS; i++)
  {
    if(eventQueries[i].event == type)
      return eventQueries[i].str;
  }
  return NULL;
}

/* Given the Event type with the formating arguments returns the formatted query
 * if the even doesn't exist returns null */
char *buildEventQuery(EventType type, ...)
{
  char *query = getEventQuery(type);
  if(query)
  {
    char *full_query = (char *) malloc(2 * (strlen(query)+1));
    va_list va;
    va_start(va,type);
    vsnprintf(full_query,2*strlen(query), query,va);
    full_query = realloc(full_query, strlen(full_query) + 1);
    va_end(va);
    return full_query;
  }
  return  NULL;
}

