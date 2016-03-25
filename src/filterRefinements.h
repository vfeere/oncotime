#ifndef FILTERREFINEMENTS_H
#define FILTERREFINEMENTS_H
#define GROUP_ERROR -2
#include "tree.h"
#include "symtab.h"

/* Basic filter data type that will be used to collect the minimal necessary
values of the three filter types to be used.
*/

typedef struct FILTERS {
	struct POPULATION_FILTER *populationFilter;
	struct PERIOD_FILTER *periodFilter;
	struct EVENT_FILTER *eventFilter;
} FILTERS;

typedef struct POPULATION_FILTER {
	struct INT_VALUE *age;
	struct ID_VALUE *gender;
	struct INT_VALUE *birthyear;
	struct ID_VALUE *diagnosis;
	struct ID_VALUE *postalcode;
	struct INT_VALUE *id;
} POPULATION_FILTER;

typedef struct PERIOD_FILTER {
	struct INT_VALUE *years;
	struct INT_VALUE *months;
	struct INT_VALUE *start;
	struct INT_VALUE *end;
	struct HOUR_VALUE *hours;
	struct ID_VALUE *days;	
} PERIOD_FILTER;

typedef struct EVENT_FILTER {
	struct ID_VALUE *event;
} EVENT_FILTER;

// INT_VALUE --- data type for linking lists of int values or int ranges
typedef struct INT_VALUE {
	enum {rangeK, intK} kind;
	union {
		int i;
		struct {int first; int last;} range;
	} value;
	struct INT_VALUE *next;
} INT_VALUE;

// ID_VALUE --- data type for linking lists of identifiers
typedef struct ID_VALUE {
	char *value;
	struct ID_VALUE *next;
} ID_VALUE;

// HOUR_VALUE -- data type for linking lists of hours and hour ranges
typedef struct HOUR_VALUE {
	enum {hourT, rangeT} type;
	union {
		char *hour;
		struct {char *first; char *last;} range;
	} value;
	struct HOUR_VALUE *next;
} HOUR_VALUE;


FILTERS *performFilterRefinements(PROGRAM_NODE *program, SymbolTable *symbol);
FILTERS *initFilters();
void setEventsFilter(ID_LIST *events, PROGRAM_NODE *program);
void setYearsAttribute(PERIOD_ATTR_LIST *first);
void setMonthsAttribute(PERIOD_ATTR_LIST *first);
void setStartAttribute(PERIOD_ATTR_LIST *first);
void setEndAttribute(PERIOD_ATTR_LIST *first);
void setDaysAttribute(PERIOD_ATTR_LIST *first);
void setHoursAttribute(PERIOD_ATTR_LIST *first);
void setAgeAttribute(POPULATION_ATTR_LIST *first);
void setGenderAttribute(POPULATION_ATTR_LIST *first);
void setBirthYearAttribute(POPULATION_ATTR_LIST *first);
void setDiagnosisAttribute(POPULATION_ATTR_LIST *first);
void setPostalCodeAttribute(POPULATION_ATTR_LIST *first);
void setIDAttribute(POPULATION_ATTR_LIST *first);

INT_VALUE *createIntValue(int i);
INT_VALUE *createIntRangeValue(int start, int end);
ID_VALUE *createIdValue(char *id);
HOUR_VALUE *createHourValue(char *hour);
HOUR_VALUE *createHourRangeValue(char *first, char *last);
#endif
