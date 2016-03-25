# Oncotime
A domain specific language compiler for radio-oncology data
## Members
Victoria Feere - 260405806 <br />
Jonathan Eidelman - 260291198 <br />
Guilherme Raposo Pereira Mansur - 260508923

## Milestone 1
To build the scanner \ parser \ pretty-printer go to directory /src and call **make** from the command-line. This will generate a program called *oncotime* which, from the main directory can be called as **run foo.onc** and will run the oncotime scanner/parser as well as generate the pretty-printed program *foo.pretty.onc* in the same directory as the specified path in the input param.

## Milestone 2
To **build** the compiler see instructions above. To **run** oncotime we no longer use our run script.  You will now need to navigate to \ src and type:

 ./oncotime ../path/to/program [arguments]

The arguments are optional and can be one of 3 options:
-pptype
-dumpsymtab
-dumpsymtaball

## Milestone 3

No logistics changed, now outputs .py program by default when you run compiler given foo.onc

milestone3.pdf in doc/

codegen.c, codegen.h added --- started to implement code gen, will output (invalid and partial) python program if you run the compiler

2 benchmark programs added 

TODO: 
- SQL fetch optimizations
- SQL events/sequences to python
- print timeline/barchart 

## Milestone 4

### Dependencies
1. Open ssh tunnel >> in shell use command: ssh cs520@hig.cs.mcgill.ca -L 3306:127.0.0.1:3306
2. To run the output foo.py program you need:
 - python 2.7.6+
 - mysqlconnector >> to install:  pip install mysql-connector-python --allow-external mysql-connector-python
 - sqlalchemy  >> to install: pip install SQLAlchemy

### Logistics 
run **make** to build and **./oncotime ../programs/path/to/program** to compile program

### Program Args (*appear after path to program argument*)
-h (help) <br />
-v (version) <br />
-dumpsymtab <br />
-dumpsymtaball <br />
-pptype

**Output**: foo.onc >> foo.py in same directory as input onco script

### Test Programs (output valid python script)
- programs/valid/(valid1.onc | valid2.onc | valid3.onc | valid4.onc) // Good for testing key functionality (filters, if-checks on attributes, sequences, tables, print & barchart statements and for loops)

- programs/benchmarks/(benchmark1.onc | benchmark2.onc) // Good for testing long computations over many DB results

**to run**: 

- *valid1.py*: <br />
	>> python <br />
	>> import valid1 <br />
	>> valid1.printTimeline()

**Comment Description:**

/*  This script will print the gender and timeline of a patient over 60 with some form of lung cancer and if they are over 30 will print just their age.
	It will then loop through the doctors printing their doctor ids.
*/

- *valid2.py*: <br />
	>> python <br />
	>> import valid2 <br />
	>> valid2.visualizePatientsBetween15And35()

**Comment Description:**

/*
	prints all tuples of patients between the ages of 15 and 35
	both in the console and in the form of a plot chart
*/ 

- *valid3.py*: <br />
	>> python <br />
	>> import valid3 <br />
	>> valid3.sortGenderAndPrintSequencesInRange()

**Comment Description:**

/* Script which prints a plot describing number of male vs. female patients born between 1980 and 2000.  Given these patients it will then print the instances of event sequences [(event name, timestamp), (event2 name, timestamp2)]
 */

- *valid4.py*: <br />
	>> python <br />
	>> import valid4 <br />
	>> valid4.ageGenderInfo({'SerNum' : '1'}]

**Comment Description:**

/* This script first counts patients by their birthyear, prints the number of different birthyears (given the population filter).  Loops through all of these patients, prints their birthyear and gender if male or diagnosis and gender if female.  It finally, takes the input parameters c and uses it to find and print a sequence matching CT_sim_booked -> CT_sim_completed -> appointment_scheduled for a patient c (if it exists). */

- *benchmark1.py*: <br />
	>> python <br />
	>> import benchmark1 <br />
	>> benchmark1.printMultipleSequences()

**Comment Description:**

/*  This program looks for all possible combinations of 3 patients, and prints the possible sequences
   they could have booked a CT, and then completed the CT. There is no filter so all patients are checked
*/

- *benchmark2.py*: <br />
	>> python <br />
	>> import benchmark2 <br />
	>> benchmark2.printTimeline()

**Comment Description:**

/*  This program looks for all possible combinations of 3 patients, and prints the possible sequences
   they could have booked a CT, and then completed the CT. There is no filter so all patients are checked
*/
