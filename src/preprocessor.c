#include <string.h>
#include "tree.h"
#include "memory.h"
void preprocessSECTION_NODE(SECTION_NODE *s, ID_LIST *l) {
  ID_NODE *cur;
  ID_LIST *extList;
  ID_NODE *newNode;
  
  switch(s->kind) {
  case headerK:
    break;
  case useK:
    extList = s->val.groupFiles;
    cur = extList->head;

    
    while (cur!=NULL) {
      
      if (l->head==NULL) {
	newNode = NEW(ID_NODE);
	newNode->val = cur->val;
	l->head = newNode;
	l->tail = newNode;
      }
      
      else {
	newNode = NEW(ID_NODE);
	newNode->val = cur->val;
	l->tail->next = newNode;
	l->tail = l->tail->next;
	l->tail->next = NULL;
      }
      
      cur = cur->next;
      
    }
    break;
  case groupK:
    break;
  case filterK:
    break;
  case computationK:
    break;
  }
}

ID_LIST *preprocessPROGRAM(PROGRAM_NODE *program) {
  // Loop through printing the sections until there are none
  SECTION_NODE *sections = program->opt_sections;
  ID_LIST *l;
  l = NEW(ID_LIST);
  
  while (sections) {
    preprocessSECTION_NODE(sections, l);
    sections = sections->next;
  }
  
  return l;

}




void addGroupsSECTION_NODE(SECTION_NODE *s, GROUP_LIST *l) {
  ID_NODE *cur;
  ID_LIST *extList;
  ID_NODE *newNode;
  
  switch(s->kind) {
  case headerK:
    break;
  case useK:

    break;
  case groupK:
    s->val.groupList->tail->next = l->head;
    s->val.groupList->tail = l->tail;
    break;
  case filterK:
    break;
  case computationK:
    break;
  }
}

void addGroupsPROGRAM(PROGRAM_NODE *program, GROUP_LIST *glist) {
  // Loop through printing the sections until there are none
  SECTION_NODE *sections = program->opt_sections;

  
  while (sections) {
    addGroupsSECTION_NODE(sections, glist);
    sections = sections->next;
  }
  


}




