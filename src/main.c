#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "pretty.h"
#include "symtab.h"
#include "type.h"
#include "filterRefinements.h"
#include "importFiles.h"
#include "preprocessor.h"
#include "codegen.h"

int line;
FILE *output;
FILE *source;
FILE *yyin;


/* Compiler Flags */
int DUMPSYMBOLTABLE = 0;
int DUMPSYMBOLTABLEALL = 0;
int PRETTYPRINTTYPE = 0;
int VERSION = 0;
int HELP = 0;

/* changes the file extension froom foo.onc to foo.extension*/
static char *changeFileExtension(char *extension, char *oncofile);
static char *pathToDir(char *fullPath);

int main(int argc, char** argv){

  DUMPSYMBOLTABLE = 0; 
  if(argc < 2)
    {
      fprintf(stderr,"usage: <sourcefile> <flags>\n");
      exit(EXIT_FAILURE);
    }
  
  /* line number needs to be manually set to 0 */
  line=1;
  /* char *tmp; */
  /* tmp = malloc(strlen(argv[1]) * sizeof(char)); */
  /* strcpy(tmp, argv[1]); */
  
  /* char *newFile; */
  /* newFile = convertFileToStr(tmp); */

  /* char *tmpFile; */
  /* tmpFile = changeFileExtension("tmp", tmp); */
  

  /* FILE *f; */

  /* int k; */
  /* f = fopen (tmpFile, "w"); */
  /* for (k=0; k<strlen(newFile); k++) { */
  /*   fprintf(f, "%c", newFile[k]); */
  /* } */


  /* fclose (f); */

    /* set lexer to read from source file rather then default STDIN */
    yyin = fopen(argv[1],"r");

    if(yyin == NULL)
    {
      fprintf(stderr,"source file %s not found\n",argv[1]);
      exit(EXIT_FAILURE);
    }


    /* Check for compiler flags */
    int i;
    for(i = 2; i < argc; i++)
    {

      if(!strcmp(argv[i],"-dumpsymtab")) 
        DUMPSYMBOLTABLE = 1;
      else if(!strcmp(argv[i],"-dumpsymtaball"))
        DUMPSYMBOLTABLEALL = 1;
      else if(!strcmp(argv[i],"-pptype"))
        PRETTYPRINTTYPE = 1;
      else if(!strcmp(argv[i],"-v"))
        VERSION = 1;
      else if(!strcmp(argv[i],"-h"))
        HELP = 1;
      else
      {
        fprintf(stderr,"uknown compiler flag, possible flags are: -dumpsymtab -pptype -dumpsymtaball -h -v\n");
        exit(EXIT_FAILURE);
      }
    }


    /* store the program node in parse */
    PROGRAM_NODE *theprogram = parse();
    SymbolTable *t;
    PROGRAM_NODE *theGroup; //Holds the group list from each imported file
    theGroup = NULL;

    GROUP_LIST *addedGroups = NEW(GROUP_LIST); // Holds the final group list which will be attached to the original one
    addedGroups->head = NULL;

    
    ID_LIST *l;
    char *fileGet;
    char *directory = pathToDir(argv[1]);

    l = preprocessPROGRAM(theprogram); // Gives us a list of files we need to import
    ID_NODE *cur;
    cur = l->head;


    // Import each file and parse it into a list of groups. Then attach all these lists together
    while (cur!=NULL) {

      fileGet = malloc((strlen(cur->val) + strlen(directory))*sizeof(char));
      strcpy(fileGet, directory);
      strcat(fileGet, cur->val);


      yyin = fopen(fileGet, "r");

            
      if (yyin==NULL) {
	      fprintf(stderr, "external file %s not found\n", fileGet);
	      exit(EXIT_FAILURE);
      }


      // If we have an empty file, ignore it
      fseek(yyin, 0L, SEEK_END);
      int sz = ftell(yyin);
      fseek(yyin, 0L, SEEK_SET);
      if (sz==0) {
	       cur = cur->next;
	       continue;
      }

      theGroup = groupParse();

      if (addedGroups->head==NULL) {
	      addedGroups = theGroup->group_list;
      } else{
	      addedGroups->tail->next = theGroup->group_list->head;
	      addedGroups->tail = theGroup->group_list->tail;
      }
      
	    fclose(yyin);
      cur = cur->next;
    }


    addGroupsPROGRAM(theprogram, addedGroups);

    /* build the symbol table: if dumpsymtab flag is on dump the top level frame
     * during printing */
    if(DUMPSYMBOLTABLE)
    {
      char *filename = changeFileExtension("symtab", argv[1]);
      output = fopen(filename,"w");
      t = symProgram(theprogram);
      fclose(output); 
    }
    else
      t = symProgram(theprogram); 

    SymbolTable *r  = symReverse(t);

    /* perform filter refinement to know exactly which filters will apply in code generation */
    FILTERS *filters = performFilterRefinements(theprogram, r);

    typeImplementationPROGRAM_NODE(theprogram, r);

    
    if(DUMPSYMBOLTABLEALL)
    {

      char *filename = changeFileExtension("symtab", argv[1]);
      output = fopen(filename,"w");
      printSymTab(output,r);
      fclose(output);

    }
    if(PRETTYPRINTTYPE)
    {
      char *filename = changeFileExtension("pptype.onc", argv[1]);
      output = fopen(filename,"w");
      prettyPROGRAM(theprogram, filters, r);
      fclose(output);
    }
    if(VERSION)
    { 
      output = stdout;
      fprintf(output,"OncoTime Compiler Version: %d\n", VERSION);
    }
    if(HELP)
    {
      output = stdout;
      fprintf(output,"\n---------\nONCOTIME\n--------\n");
      fprintf(output,"-dumpsymtab: ouputs the symbol table to the the ouput file at every scope entrance.\n");
      fprintf(output,"-dumpsymtaball: ouputs the symbol table to the output file at the end of the pass\n");
      fprintf(output,"-pptype: pretty prints the program with the type information to the ouput file.\n");
      fprintf(output,"-v: prints the version number of the compiler.\n-h: prints the flags for the compiler\n\n");
    }

    /* Generate python script
    */

    char *filename = changeFileExtension("py", argv[1]);
    output = fopen(filename, "w");
    codegenPROGRAM(theprogram, filters);
    fclose(output);
    
}


static char *changeFileExtension(char *extension, char *oncofile)
{
  size_t length = strlen(oncofile);
  char *new_name = (char*) malloc(sizeof(char) * (length + 1 + strlen(extension)));
  memcpy(new_name,oncofile, length - 3);
  new_name[length -3] = 0;
  strcat(new_name,extension);
  return new_name;
}


static char *pathToDir(char *fullPath) {
  char *retPath = (char*) malloc(sizeof(char) * strlen(fullPath));
  int i, slashPoint=0;
  for (i=0; i<strlen(fullPath); i++) {
    if (fullPath[i]=='/')
      slashPoint = i;
  }
  if (slashPoint>0) {
    slashPoint++;
    memcpy(retPath, fullPath, slashPoint);
    retPath[slashPoint] = '\0';
    
  }
  else {
    strcpy(retPath, "");
  }
  return retPath;
}

