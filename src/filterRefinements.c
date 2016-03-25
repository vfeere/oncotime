#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "filterRefinements.h"

FILTERS *filters;
PROGRAM_NODE *theprogram;
SymbolTable *st;
/* Performs filter reduction to simplified final revision and stores
it in it's own new data type FILTERS with 3 simple filters : population, 
period & events with their final attributes
*/
FILTERS *performFilterRefinements(PROGRAM_NODE *program, SymbolTable *symbols) {
	filters = initFilters();
	theprogram = program;
	st = symbols;
	SECTION_NODE *optionalSection = program->opt_sections;
	while (optionalSection) {

		// If section is filter section then look through it constantly
		// re-writing former attributes -- NOTE: if an attribute is null 
		// it defaults to * --- this will be implied by a null attribute
		if (optionalSection->kind == filterK) {
			FILTER_NODE *filter = optionalSection->val.filterList->head;
			while (filter) {
				switch(filter->kind) {
					case popK: {
						POPULATION_ATTR_LIST *attribute = filter->val.popAttrList;
						while (attribute) {
							switch(attribute->kind) {
								case DiagK: 
									setDiagnosisAttribute(attribute);
									break;
								case GenderK:
									setGenderAttribute(attribute);
									break;
								case AgeK:
									setAgeAttribute(attribute);
									break;
								case BirthYK:
									setBirthYearAttribute(attribute);
									break;
								case PstlCodeK:
									setPostalCodeAttribute(attribute);
									break;
								case PopIdK:
									setIDAttribute(attribute);
									break;
							}
							attribute = attribute->next;
						}
						break;
					} 
					case periodK: {
						PERIOD_ATTR_LIST *attribute = filter->val.periodAttrList;
						while (attribute) {
							switch(attribute->kind) {
								case YearK:
									setYearsAttribute(attribute);
									break;
								case MonthK:
									setMonthsAttribute(attribute);
									break;
								case DaysK:
									setDaysAttribute(attribute);
									break;
								case HoursK:
									setHoursAttribute(attribute);
									break;
								case StartK:
									setStartAttribute(attribute);
									break;
								case EndK:
									setEndAttribute(attribute);
									break;
							}

							attribute = attribute->next;
						}
						break;
					}
					case eventsK: {
						ID_LIST *events = filter->val.events;
						setEventsFilter(events, program);
						break;
					}
				}

				filter = filter->next;
			}
		}

		optionalSection = optionalSection->next;
	}

	return filters;
}

FILTERS *initFilters() {
	FILTERS *filters = malloc(sizeof(FILTERS));
	POPULATION_FILTER *population_filter = malloc(sizeof(POPULATION_FILTER));
	PERIOD_FILTER *period_filter = malloc(sizeof(PERIOD_FILTER));
	EVENT_FILTER *event_filter = malloc(sizeof(EVENT_FILTER));
	filters->populationFilter = population_filter;
	filters->periodFilter = period_filter;
	filters->eventFilter = event_filter;
	return filters;
}

/*****************************/
/*      EVENTS FILTER        */
/*****************************/

void setEventsFilter(ID_LIST *events, PROGRAM_NODE *program) {

	// fetch the groups section node
	SECTION_NODE* optionalSections = program->opt_sections;
	GROUP_LIST *groups;
	while (optionalSections) {
		if (optionalSections->kind == groupK) {
			groups = optionalSections->val.groupList;
		}
		optionalSections = optionalSections->next;
	}


	ID_NODE *event = events->head;
	ID_VALUE *head = NULL;
	ID_VALUE *current;
	while (event) {
		switch(event->kind) {
			case stringK: {
				ID_VALUE *nextNode = createIdValue(event->val);  
				if (head == NULL) {
					head = nextNode;
					current = head;
				} else {
					current->next = nextNode;
					current = nextNode;
				}
				break;
			}
			case idK: {
				int foundGroup = 0;
				if (groups) {
					GROUP_NODE *currentGroup = groups->head;
					while(currentGroup) {
						if (strcmp(currentGroup->id, event->val) == 0) {
							foundGroup = 1;
							ID_NODE *strings = currentGroup->paramList->head; 
							while(strings) {
								ID_VALUE *nextNode = createIdValue(strings->val);
								if (head == NULL) {
									head = nextNode;
									current = head;
								} else {
									current->next = nextNode;
									current = nextNode;
								}
								strings = strings->next;
							}
						}
						currentGroup = currentGroup->next;
					}
				}
				if (!foundGroup) {
					printf("Invalid Group Error: %s is not the name of a group and is an invalid event filter value\n", event->val);
					exit(GROUP_ERROR);
				}
				break;
			}
		}

		event = event->next;
	}

	filters->eventFilter->event = head;
}

/********************************/
/*  GET POP/PER ATTTRIBUTE HEAD */
/********************************/

INT_VALUE *getListOfIntValues(INT_NODE *intNode) {
	INT_VALUE *head;

	switch (intNode->kind) {

		case rangeK:
			head = createIntValue(intNode->val.intE);
			break;
		
		case intK:
			head = createIntRangeValue(intNode->val.rangeE.beginning, intNode->val.rangeE.end);
			break;
	}

	intNode = intNode->next;
	INT_VALUE *current = head;
		
	while (intNode) {
		switch (intNode->kind) {

			case rangeK: {
				INT_VALUE *nextNode = createIntValue(intNode->val.intE);
				current->next = nextNode;
				current = nextNode;
				break;
			}
			case intK: { 
				INT_VALUE *nextNode = createIntRangeValue(intNode->val.rangeE.beginning, intNode->val.rangeE.end);
				current->next = nextNode;
				current = nextNode;
				break;
			}
		}

		intNode = intNode->next;
	}

	return head;

}

ID_VALUE *getListOfIdValues(ID_NODE *idNode, int containsIdentifiers) {

	if (!containsIdentifiers) {
		ID_VALUE *head = NULL;
		ID_VALUE *current;
		while (idNode) {
			if (head == NULL) {
				head = createIdValue(idNode->val);
				current = head;
			}  else {
				ID_VALUE *nextNode = createIdValue(idNode->val);
				current->next = nextNode;
				current = nextNode;
			}

			idNode = idNode->next;
		}

		return head;
	}

	// fetch the groups section node
	SECTION_NODE* optionalSections = theprogram->opt_sections;
	GROUP_LIST *groups;
	while (optionalSections) {
		if (optionalSections->kind == groupK) {
			groups = optionalSections->val.groupList;
		}
		optionalSections = optionalSections->next;
	}

	ID_VALUE *head = NULL;
	ID_VALUE *current;
	while (idNode) {
		switch(idNode->kind) {
			case stringK: {
				ID_VALUE *nextNode = createIdValue(idNode->val);  
				if (head == NULL) {
					head = nextNode;
					current = head;
				} else {
					current->next = nextNode;
					current = nextNode;
				}
				break;
			}
			case idK: {
				int foundGroup = 0;
				if (groups) {
					GROUP_NODE *currentGroup = groups->head;
					while(currentGroup) {
						if (strcmp(currentGroup->id, idNode->val) == 0) {
							foundGroup = 1;
							ID_NODE *strings = currentGroup->paramList->head; 
							while(strings) {
								ID_VALUE *nextNode = createIdValue(strings->val);
								if (head == NULL) {
									head = nextNode;
									current = head;
								} else {
									current->next = nextNode;
									current = nextNode;
								}
								strings = strings->next;
							}
						}
						currentGroup = currentGroup->next;
					}
				}

				if (!foundGroup) {
					SYMBOL *symbol = getSymbol(st, idNode->val);
					if (symbol && symbol->scope == TopLevelS) {
						ID_VALUE *nextNode = createIdValue(idNode->val);
						if (head == NULL) {
							head = nextNode;
							current = head;
						} else {
							current->next = nextNode;
							current = nextNode;
						}
						break;
					}

					printf("Invalid Identifier Error: %s is not the name of a group or script argument and is an invalid filter value\n", idNode->val);
					exit(GROUP_ERROR);
				}
				break;
			}
		}

		idNode = idNode->next;
	}

	return head;

}

HOUR_VALUE *getListOfHourValues(HOUR_NODE *hourNode) {
	
	HOUR_VALUE *head;
	if (hourNode->kind == hourK) {
		head = createHourValue(hourNode->val.hour);
	} else {
		head = createHourRangeValue(hourNode->val.rangeH.begin, hourNode->val.rangeH.end);
	}
	hourNode = hourNode->next;
	HOUR_VALUE *current = head;
	
	while (hourNode) {
		switch(hourNode->kind) {
			case hourK: {
				HOUR_VALUE *nextNode = createHourValue(hourNode->val.hour);
				current->next = nextNode;
				current = nextNode;
				break;
			}
			case hourRangeK: {
				HOUR_VALUE *nextNode = createHourRangeValue(hourNode->val.rangeH.begin, hourNode->val.rangeH.end);
				current->next = nextNode;
				current = nextNode;
				break;
			}
		}

		hourNode = hourNode->next;
	}

	return head;

}

/*****************************/
/*  PERIOD FILTER ATTRIBUTES */
/*****************************/

void setYearsAttribute(PERIOD_ATTR_LIST *first) {
	filters->periodFilter->years = getListOfIntValues(first->val.intList->head);
}

void setMonthsAttribute(PERIOD_ATTR_LIST *first){
	filters->periodFilter->months = getListOfIntValues(first->val.intList->head);
}

void setStartAttribute(PERIOD_ATTR_LIST *first) {
	filters->periodFilter->start = malloc(sizeof(INT_VALUE));
	filters->periodFilter->start->value.i = first->val.num;
}

void setEndAttribute(PERIOD_ATTR_LIST *first) {
	filters->periodFilter->end = malloc(sizeof(INT_VALUE));
	filters->periodFilter->end->value.i = first->val.num;
}

void setDaysAttribute(PERIOD_ATTR_LIST *first) {
	filters->periodFilter->days = getListOfIdValues(first->val.idList->head, 0);
}

void setHoursAttribute(PERIOD_ATTR_LIST *first) {
	filters->periodFilter->hours = getListOfHourValues(first->val.hList->head);
}

/********************************/
/* POPULATION FILTER ATTRIBUTES */
/********************************/

void setAgeAttribute(POPULATION_ATTR_LIST *first) {
	filters->populationFilter->age = getListOfIntValues(first->val.intList->head);
}

void setGenderAttribute(POPULATION_ATTR_LIST *first) {
	filters->populationFilter->gender = getListOfIdValues(first->val.idList->head, 0);
}

void setBirthYearAttribute(POPULATION_ATTR_LIST *first) {
	filters->populationFilter->birthyear = getListOfIntValues(first->val.intList->head);
}

void setDiagnosisAttribute(POPULATION_ATTR_LIST *first) {
	filters->populationFilter->diagnosis = getListOfIdValues(first->val.idList->head, 1);
}

void setPostalCodeAttribute(POPULATION_ATTR_LIST *first) {
	filters->populationFilter->postalcode = getListOfIdValues(first->val.idList->head, 0);
}

void setIDAttribute(POPULATION_ATTR_LIST *first) {
	filters->populationFilter->id = getListOfIntValues(first->val.intList->head);
}

/**********************/
/* CREATE VALUE TYPES */
/**********************/

INT_VALUE *createIntValue(int i) {
	INT_VALUE *intValue = malloc(sizeof(INT_VALUE));
	intValue->value.i = i;
	intValue->kind = intK;
	return intValue;
}

INT_VALUE *createIntRangeValue(int start, int end) {
	INT_VALUE *intRangeValue = malloc(sizeof(INT_VALUE));
	intRangeValue->value.range.first = start;
	intRangeValue->value.range.last = end;
	intRangeValue->kind = rangeK;
	return intRangeValue;
}

ID_VALUE *createIdValue(char *id) {
	ID_VALUE *idValue = malloc(sizeof(ID_VALUE));
	idValue->value = id;
	return idValue;
}

HOUR_VALUE *createHourValue(char *hour) {
	HOUR_VALUE *hourValue = malloc(sizeof(HOUR_VALUE));
	hourValue->value.hour = hour;
	hourValue->type = hourT;
	return hourValue;
}

HOUR_VALUE *createHourRangeValue(char *first, char *last) {
	HOUR_VALUE *hourValue = malloc(sizeof(HOUR_VALUE));
	hourValue->value.range.first = first;
	hourValue->value.range.last = last;
	hourValue->type = rangeT;
	return hourValue;
}
