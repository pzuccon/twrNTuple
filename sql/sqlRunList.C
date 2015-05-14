#include <my_global.h>
#undef max
#undef min
// ^ Keep these lines in; my_global.h unwisely defines "max" and "min" macros
#include <mysql.h>
// ^ need to compile with `mysql_config --cflags --libs`
//#include <climits>
#include <string> // std::string

#include <cstring> // strlen
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <sstream>

//#define TABNAME "twrNTuples_v3"

const int twr_verNo = 3;

	MYSQL *conn;
	MYSQL_RES *result;

int updateDB=0,queryDB=0,addDB=0;
int nFunc=0;
int quiet=1;
char listFile[100]="";
int listSet=0;
// char outFile[100]="";
// int outSet=0;
char fName1[200]="";
int fNameSet=0;
int prefixLength=0;
int nMax=0;
char** extraArgs;
int nExtra=0;
char tableName[100]="twrNTuples_v3";


void _usage(char *progname);
int _proc_args(int argc, char *argv[]);

int _queryDB(const char* line);
int _addDB(const char* line);
int _updateDB(const char* line, int nExtra, char** extraArgs);


void _usage(char *progname)
{
	printf("Usage:\t %s <-AQU> <-l inListFile | -f fName> [-t tablename] [-n nMax]",progname);
	printf("\n\t\t[-p prefixLength] [-hv] [col1 arg1 col2 arg2 ...]\n");
	printf("Options:\n");
	printf("\t-A --add    \t Add rows with given value(s) for fName\n");
	printf("\t-Q --query  \t See whether rows in the table with fName exist\n");
	printf("\t-U --update \t Change given attributes of rows fName\n");
	printf("\t-l          \t Name of the list file\n");
	printf("\t-f          \t Single value of fName\n");
	printf("\t-t --table  \t Name of the SQL table (default=%s)\n",tableName);
	printf("\t-n          \t Maximum number of lines to process (with -l)\n");
	printf("\t-p --prefix \t Length of the prefix to take off the input filename(s)\n");
	printf("\t-h --help   \t Print this message\n");
	printf("\t-v --verbose\t Print executed SQL statements and nominal results to stdout\n");


	exit(0);
}

int _proc_args(int argc, char *argv[])
{
	int c;
	int option_index = 0;
	
// 	struct option
// 	{
// 		const char *name;		Follows -- (or - optionally for getopt_long_only)
// 		int         has_arg;	0: no arg, 1: arg req'd, 2: arg optional
// 		int        *flag;		If flag is NULL, then getopt_long() returns val. (For example, the calling program may set val to the equivalent short option character.) Otherwise, getopt_long() returns 0, and flag points to a variable which is set to val if the option is found, but left unchanged if the option is not found.
// 		int         val;		Value to return, or to load into the variable pointed to by flag.
// 	};
	
	static struct option long_options[] = 
	{
	//  {"longopt",has_arg,flag,retVal}
		{"add",no_argument,0,'A'},
		{"query",no_argument,0,'Q'},
		{"update",no_argument,0,'U'},
		
		{"help",0,0,'h'},
		{"table",required_argument,0,'t'},
		{"verbose",no_argument,0,'v'},
		{"prefix",required_argument,0,'p'},

		{0,0,0,0}
	};

	/* Turn off error check for getopt */
	opterr = 0;

	/* Loop on options */
	while (1) {
		c = getopt_long (argc, argv, "hAQUvt:l:f:p:n:?", long_options, &option_index);
		if (c == -1) break;

		switch (c) 
		{
		case 'A':
		  addDB=1;
		  nFunc++;
		  break;
		case 'Q':
		  queryDB=1;
		  nFunc++;
		  break;
		case 'U':
		  updateDB=1;
		  nFunc++;
		  break;
		case 'l':             /* logdir */
		  sprintf(listFile,"%s",optarg);
		  listSet=1;
		  break;
		case 'f':
		  sprintf(fName1,"%s",optarg);
		  fNameSet=1;
		  break;
// 		case 'o':             /* logdir */
// 		  sprintf(outFile,"%s",optarg);
// 		  outSet=1;
// 		  break;
		case 't':
		  sprintf(tableName,"%s",optarg);
		  break;
		case 'v':
		  quiet=0;
		  break;
		case 'p':
		  if (atoi(optarg)>0) prefixLength=atoi(optarg);
		  break;
		case 'n':
		  nMax=atoi(optarg);
		  break;
		case '?':
		default:            /* help */
		  _usage(argv[0]);
		  break;
		}
	}

	/* No input dir/file from command line */
	if ( argc == 1 ) _usage(argv[0]);
	/* Check path from command line */
	
	if (optind < argc)
	{
		nExtra = argc - optind;
		extraArgs = &argv[optind];
		if (!updateDB)
		{
			printf("Warning: Extra arguments ( ");
			while (optind < argc) printf("%s ", argv[optind++]);
			printf(") will be ignored.  These are only used in update (-U) mode.\n");
        }
        else if (nExtra%2 != 0)
        {
        	printf("Warning: Odd number of extra arguments used.  ");
        	printf("The last argument (%s) will be ignored.\n",argv[argc-1]);
        	if (nExtra > 0) nExtra--;
        }
    }
	
	return 0;
}


int main(int argc, char **argv)
{	
	// Process command line input arguments
	nFunc=0;
	int istat = _proc_args(argc, argv);

	// Error from command line 
	if ( istat < 0 )
	{
		printf("Error: from the command line\n");
		exit(-1);
	}
	if (nFunc != 1)
	{
		printf("Error: You must enable ONLY ONE of -A (--add), -Q (--query), and -U (--update) flags.\n");
		exit(-2);
	}
	if ((!listSet)&&(!fNameSet))
	{
		printf("Error: You must set an input list file using flag '-l' or a single value of fName using '-f'.\n");
		exit(-5);
	}
	if (listSet && fNameSet)
	{
		printf("Error: You may only use one of -l and -f flags.\n");
		exit(-6);
	}
	if (updateDB && (nExtra<2))
	{
		printf("Error: Nothing to update; not enough arguments.\n");
		exit(-7);
	}
// 	FILE* pFILE=0;
// 	if (outSet)
// 	{
// 		pFILE = fopen(outFile, "a");
// 		if (!pFILE) 
// 		{
// 			outSet = 0;
// 			printf("Problem opening output file %s\nStandard output will be used.\n",outFile);
// 		}
// 	}

	char const *server = "ams-farm.lns.mit.edu";
	char const *user = "dtuser";
	char const *password = "dtpass";
	char const *database = "dtransfer";

	conn = mysql_init(NULL);
	MYSQL* mm=mysql_real_connect(conn, server, user, password, database, 0, NULL, 0);
	if(!mm)
	{
		printf("ERROR: Problems to connect to the DB sever \nI give up!\n");
		exit(-3);
	}
	
	
//	char tableName[20];
//	sprintf(tableName,"twrNTuples_v%d",twr_verNo);
	
	std::string query;
	
	// QUERY: Check for existence of table
	query+="SELECT * FROM information_schema.tables WHERE table_schema = 'dtransfer' AND table_name = '";
	query+=tableName;
	query+="' LIMIT 1;";
	
	int retQ=mysql_query(conn, query.c_str());
	result = mysql_store_result(conn);
	if(!result)
	{
		printf("Found 0 entries for the requested params\n");
		
		mysql_free_result(result);
		mysql_close(conn);
		return 0;
	}
	int num_rows = mysql_num_rows(result);
	if (num_rows<1)
	{
		printf("The table %s does not exist.\nPlease create it first before populating it.\n",tableName);
		mysql_free_result(result);
		mysql_close(conn);
		return 0;
	}
	mysql_free_result(result);
	
	// Process single fName
	if (fNameSet)
	{
		// Remove a prefix if one exists
		if (prefixLength && strlen(fName1)>prefixLength)
		{
			char *ltmp = fName1;
			ltmp += prefixLength;
			sprintf(fName1,"%s",ltmp);
		}
	
		if (queryDB) retQ = _queryDB(fName1);
		if (addDB) retQ = _addDB(fName1);
		if (updateDB) retQ = _updateDB(fName1,nExtra,extraArgs);
		mysql_close(conn);
		return retQ;
	}
	
	// Process list of fName values
	printf("Proceed to reading list file...\n");
	
	std::ifstream ifs(listFile);
	
	if (!ifs.is_open())
	{
		printf("Error: Problem opening input list file.\n");
		mysql_close(conn);
		exit(-4);
	}
	
	char line[200];

	int nProc=0;
	while (!(ifs.eof())) 
	{
		nProc++;
		if (nMax && (nProc > nMax)) break;
		
		ifs.getline(line,200);
		if (strlen(line)<2) continue;  // Don't use blank line at end of file
		// Remove prefix if requested
		if (prefixLength && strlen(line)>prefixLength)
		{
			char *ltmp = line;
			ltmp += prefixLength;
			sprintf(line,"%s",ltmp);
		}

		if (queryDB) retQ = _queryDB(line);
		if (addDB) retQ = _addDB(line);
		if (updateDB) retQ = _updateDB(line,nExtra,extraArgs);
	}
	
	
	
	
	
/*	
	
	if (queryDB)
	{
		char line[200];
	
		int nProc=0;
		while (!(ifs.eof())) 
		{
			nProc++;
			if (nMax && (nProc > nMax)) break;
			
			ifs.getline(line,200);
			if (strlen(line)<2) continue;  // Don't use blank line at end of file
			if (prefixLength && strlen(line)>prefixLength)
			{
				char *ltmp = line;
				ltmp += prefixLength;
				sprintf(line,"%s",ltmp);
			}
			query ="SELECT * FROM ";
			query+=tableName;
			query+=" WHERE fName='";
			query+=line;
			query+="';";
			
			retQ = mysql_query(conn, query.c_str());
			result = mysql_store_result(conn);
			num_rows = mysql_num_rows(result);
			if (!quiet) printf("> %s\n",query.c_str());
			if (!result || (num_rows<1)) printf("%s NOT in table\n",line);
			else if (num_rows>1) printf("%s MULTIPLY in table\n",line);
			else if (!quiet) printf("%s in table\n",line);
			
			mysql_free_result(result);
		}
	}
	
	if (addDB)
	{	
		char line[200];
		
		int nProc=0;
		while (!(ifs.eof())) 
		{
			nProc++;
			if (nMax && (nProc > nMax)) break;
		
			ifs.getline(line,200);
			if (strlen(line)<2) continue;  // Don't use blank line at end of file
			if (prefixLength && strlen(line)>prefixLength)
			{
				char *ltmp = line;
				ltmp += prefixLength;
				sprintf(line,"%s",ltmp);
			}
			
			query ="INSERT INTO ";
			query+=tableName;
			query+=" (fName,runID) VALUES ('";
			query+=line;
			query+="',";
			
			// extract run number and event number (?), i.e., second int in filename
			long int runno, evno;
			char * pEnd;
			runno = strtol(line,&pEnd,10);
			pEnd++;
			evno = strtol(pEnd,0,10);
			// Convert run number and event number to string via stringstream
			std::ostringstream runSS, evSS;
			runSS << runno;
			evSS << evno;
			
			query+=runSS.str();
			query+=");";
			
			retQ = mysql_query(conn, query.c_str());
			if (!quiet) printf("> %s\n",query.c_str());
			if (retQ) printf("FAILURE: Insert %s\n",line);
			else printf("SUCCESS: Insert %s\n",line);
		}
	}
	
	if (updateDB)
	{	
		char line[200];
		printf("Update procedure not written yet...\n");
		
		for (int j=0; j<nExtra; j+=2)
		{
			printf("Col: %s , Arg: %s\n",extraArgs[j],extraArgs[j+1]);
		}
		
		
		int nProc=0;
		while (!(ifs.eof())) 
		{
			nProc++;
			if (nMax && (nProc > nMax)) break;
		
			ifs.getline(line,200);
			if (strlen(line)<2) continue;  // Don't use blank line at end of file
			if (prefixLength && strlen(line)>prefixLength)
			{
				char *ltmp = line;
				ltmp += prefixLength;
				sprintf(line,"%s",ltmp);
			}
			
			query ="UPDATE ";
			query+=tableName;
			query+=" SET ";
			for (int j=0; j<nExtra; j+=2)
			{
				query+=extraArgs[j];
				query+="=";
				query+=extraArgs[j+1];
				if ((j+2)<nExtra) query+=", ";
			}
			query+=" WHERE fname='";
			query+=line;
			query+="';";
			
			retQ = mysql_query(conn, query.c_str());
			if (!quiet) printf("> %s\n",query.c_str());
			if (retQ) {printf("FAILURE: Update %s\n",line); continue;}
 			num_rows = mysql_affected_rows(conn);
			
			if (num_rows<1) printf("%s NOT in table; Add before updating\n",line);
			else if (num_rows>1) printf("%s MULTIPLY in table; Updated all matching rows.\n",line);
			else if (!quiet) printf("SUCCESS: Update %s\n",line);
		}
		
	}
	*/

	ifs.close();

//	FILE* ff=fopen("lista_run","r");  
// 	char fname[50];
// 	char rname[50];
// 	int runid;
// 	while(1)
// 	{
// 		fscanf(ff,"%s", fname);
// 		if(feof(ff) )break;
// 		int num=0;
// 		for(int ii=0;ii<=strlen(fname);ii++)
// 		if(fname[ii]=='.') {num=ii;break;}
// 		if(num!=0)
// 		{
// 			strncpy(rname,fname,num);
// 			rname[num]='\0';
// 			runid=atoi(rname);
// 			char comm[500];
// 			sprintf(comm, "INSERT INTO twrNTuples_v3 VALUES('%s',%d,0,0,0,0)",fname,runid);
// 			printf("%s\n",comm);
// 
// 			int ret=mysql_query(conn, comm);
// 			if(ret==0) 
// 			printf("INSERT SUCCESSFUL\n");
// 			else
// 			printf("INSERT FAILED\n");
// 		}
// 	}
// 
// 	fclose(ff);

	mysql_close(conn);
	return 0;
}

int _queryDB(const char* line)
{	
	std::string query;
	
	query ="SELECT * FROM ";
	query+=tableName;
	query+=" WHERE fName='";
	query+=line;
	query+="';";
	
	int retQ = mysql_query(conn, query.c_str());
	result = mysql_store_result(conn);
	int num_rows = mysql_num_rows(result);
	if (!quiet) printf("> %s\n",query.c_str());
	if (!result || (num_rows<1))
	{
		printf("%s NOT in table\n",line);
		mysql_free_result(result);
		return -1;
	}
	else if (num_rows>1) printf("%s MULTIPLY in table\n",line);
	else if (!quiet) printf("%s in table\n",line);
	
	mysql_free_result(result);

	return num_rows;
}

int _addDB(const char* line)
{
	std::string query;
	
	query ="INSERT INTO ";
	query+=tableName;
	query+=" (fName,runID) VALUES ('";
	query+=line;
	query+="',";
	
	// extract run number and event number (?), i.e., second int in filename
	long int runno, evno;
	char * pEnd;
	runno = strtol(line,&pEnd,10);
	pEnd++;
	evno = strtol(pEnd,0,10);
	// Convert run number and event number to string via stringstream
	std::ostringstream runSS, evSS;
	runSS << runno;
	evSS << evno;
	
	query+=runSS.str();
	query+=");";
	
	int retQ = mysql_query(conn, query.c_str());
	if (!quiet) printf("> %s\n",query.c_str());
	if (retQ)
	{
		printf("FAILURE: Insert %s\n",line);
		return -1;
	}
	else
	{
		printf("SUCCESS: Insert %s\n",line);
		return 0;
	}
}

int _updateDB(const char* line, int nExtra, char** extraArgs)
{	
	std::string query;
	
	query ="SELECT * FROM ";
	query+=tableName;
	query+=" WHERE fName='";
	query+=line;
	query+="'";
	for (int j=0; j<nExtra; j+=2)
	{
		query+=" AND ";
		query+=extraArgs[j];
		query+="=";
		query+=extraArgs[j+1];
	}
	query+=";";
	int retQ = mysql_query(conn, query.c_str());
	result = mysql_store_result(conn);
	int num_rows = mysql_num_rows(result);
	if ((num_rows > 0)&& result)
	{
		if (!quiet) printf("%s already has these values in table. No update performed \n",line);
		mysql_free_result(result);
		return -1;
	}
	mysql_free_result(result);
		
	
	query ="UPDATE ";
	query+=tableName;
	query+=" SET ";
	for (int j=0; j<nExtra; j+=2)
	{
		query+=extraArgs[j];
		query+="=";
		query+=extraArgs[j+1];
//		query+="'";
		if ((j+2)<nExtra) query+=", ";
	}
	query+=" WHERE fname='";
	query+=line;
	query+="';";
	
	retQ = mysql_query(conn, query.c_str());
	if (!quiet) printf("> %s\n",query.c_str());
	if (retQ) {printf("FAILURE: Update %s\n",line); return -1;}
	num_rows = mysql_affected_rows(conn);
	
	if (num_rows<1) {printf("%s NOT in table; Add before updating\n",line); return -2;}
	else if (num_rows>1) {printf("%s MULTIPLY in table; Updated all matching rows.\n",line); return num_rows;}
	else
	{
		if (!quiet) printf("SUCCESS: Update %s\n",line);
		return 0;
	}
}
