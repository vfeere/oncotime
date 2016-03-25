#ifndef EVENTS_H_
#define EVENTS_H_
#include "tree.h"
#include <stdarg.h>
static const int NUM_EVENTS = 5;

typedef enum {CT_SIM_BOOKED, CT_SIM_COMPLETED, READY_FOR_PHYSICS_QA, APPOINTMENT_SCHEDULED, 
  CONSULT_REFERRAL_RECEIVED} EventType;


static int hasCT_sim_booked = 0, hasCT_sim_completed = 0, hasReady_for_physics = 0, hasAppointment_scheduled = 0, hasConsult_referral_received = 0;

static struct
{
  char* str;
  EventType event;
}eventQueries[NUM_EVENTS]
={
  {

    "Select p.PatientSerNum, ap.LastUpdated as TimeStamp from Patient p, Diagnosis d, Appointment ap, Task t, Alias a where a.AliasName = \"Ct-sim\" and a.AliasSerNum = ap.AliasSerNum  and p.PatientSerNum = ap.PatientSerNum limit 10;", 

    CT_SIM_BOOKED
  },
  {
    "Select ap.ScheduledStartTime as Start, ap.ScheduledEndTime as End, ap.LastUpdated as Timestamp,  p.PatientSerNum from Patient p, Diagnosis d, Appointment ap, Alias a where a.AliasName=\"Ct-sim\" and a.AliasSerNum = ap.AliasSerNum and p.PatientSerNum = ap.PatientSerNum and ap.Status = \"Manually Completed\" limit 10;" , 
      CT_SIM_COMPLETED
  },
  {
     "Select p.PatientSerNum, t.CreationDate, t.DueDateTime, t.CompletionDate, t.LastUpdated as TimeStamp, pr.PrioritySerNum from Patient p, Diagnosis d, Task t, Priority pr, Alias a where a.AliasName = \"Ready for Physics QA\" and a.AliasSerNum = t.AliasSerNum and t.PatientSerNum = p.PatientSerNum and pr.PrioritySerNum = t.PrioritySerNum limit 10" ,
     READY_FOR_PHYSICS_QA
  },
  {"Select p.PatientSerNum, pl.PatientLocationID, ap.ScheduledStartTime as \"Scheduled For\", ap.LastUpdated as TimeStamp, from Patient p, Diagnosis d, PatientLocation pl, Appointment ap where p.PatientSerNum = ap.PatientSerNum and ap.AppointmentSerNum = pl.AppointmentSerNum limit 10;",
    APPOINTMENT_SCHEDULED
  },
  {"Select p.PatientSerNum, t.CreationDate as TimeStamp, t.DueDateTime, pr.PriorityCode, t.CompletionDate from Patient p, Diagnosis d, Task t, Priority pr, Alias a where a.AliasName = \"CRR with Origin\" and a.AliasSerNum = t.AliasSerNum and t.PatientSerNum = p.PatientSerNum and Status= \"Open\" and pr.PrioritySerNum = t.PrioritySerNum limit 10;",
   CONSULT_REFERRAL_RECEIVED
  }
};



static const char *events[] = { "CT_sim_booked", "CT_sim_completed", "ready_for_physics_QA",
  "appointment_scheduled", "consult_refferal_received"};

static const int numArgs[] = {1, 1, 1, 1};
static const int argTypes[4][2] = {{1, -1}, {1, -1}, {1, -1}, {1, -1}};

int countArgs(char *str);
SymType typeAt(char *name, int place);
static char *getEventQuery(EventType type);
char *buildEventQuery(EventType type, ...);
#endif
