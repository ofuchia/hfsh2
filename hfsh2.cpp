//*********************************************************
//
// Ofuchinyelu Akpom
// Operating Systems
// Writing Your Own Shell: hfsh2
// Feb 20, 2023
// Instructor: Dr. Scherger
//*********************************************************


//*********************************************************
//
// Includes and Defines
//
//*********************************************************
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "csapp.h"


//string definitions
#define STR_EXIT "exit"
#define STR_CD "cd"
#define STR_PATH "path"


//*********************************************************
//
// Extern Declarations
//
//*********************************************************
// Buffer state. This is used to parse string in memory...
// Leave this alone.
extern "C"{
    extern char **gettoks();
    typedef struct yy_buffer_state * YY_BUFFER_STATE;
    extern YY_BUFFER_STATE yy_scan_string(const char * str);
    extern YY_BUFFER_STATE yy_scan_buffer(char *, size_t);
    extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
    extern void yy_switch_to_buffer(YY_BUFFER_STATE buffer);
}


//*********************************************************
//
// Namespace
//
// If you are new to C++, uncommment the `using namespace std` line.
// It will make your life a little easier.  However, all true
// C++ developers will leave this commented.  ;^)
//
//*********************************************************
// using namespace std;


//*********************************************************
//
// Function Prototypes
//
//*********************************************************
bool processBuiltinCommands(char **toks, int num_toks);
void notBuiltin(char **toks, int num_toks, int num_redirects);
char *h_redirections(char **toks, int num_redirect);
void runCmd(char **toks, int position, int num_redirects);
void exitMsg();

//GLOBAL VARIABLES
char *is_path[1000];
char prompt[] = "hfsh2> ";
//*********************************************************
//
// Main Function
//
//*********************************************************
int main(int argc, char *argv[])
{
    /* local variables */
    int ii;
    char **toks;        //an array of strings
    int retval;
    char linestr[256];
    YY_BUFFER_STATE buffer;
    int num_toks;
    int num_redirects;
    

    /* initialize local variables */
    ii = 0;
    toks = NULL;
    retval = 0;
    num_toks = 0;
    num_redirects = 0;
    is_path[0] = strdup("/bin"); 
    char line[256]; 
    FILE *batch;
    /* main loop */
    if(argc == 2){
        //batch mode: reading and executing from file and running without prompt 
        
        //opening the batch file in read mode.
        batch = fopen(argv[1], "r");

        //checking to see if the file opened successfully
        if(batch == NULL){
            exitMsg();
            exit(1);
        }

        //processing the batch file like regular
        while(fgets(linestr, 256, batch)){    //while there's still stuff to read from the file
            // make sure line has a '\n' at the end of it
            if(!strstr(linestr, "\n"))
                strcat(linestr, "\n");

            /* get arguments */
            buffer = yy_scan_string(linestr);
            yy_switch_to_buffer(buffer);
            toks = gettoks();       //parsing the command line
            yy_delete_buffer(buffer);
            
            if(toks[0] != NULL){    //if there's something in the command line, aka the first entry of argv[0] isn't empty
                /* simple loop to echo all arguments */
                //echo out the 
                num_toks = 0;
                for(ii=0; toks[ii] != NULL; ii++){

                    //keeping count of number of arguments passed to the command line
                    num_toks++;

                    //checking to see how many number of > were passed in by user
                    if(!strcmp(toks[ii], ">")){
                        num_redirects++;
                    }
                }
                //check to see if the commands are built in commands 
                notBuiltin(toks, num_toks, num_redirects);
            }
           
        }
        fclose(batch);
        return( retval );
    }
    else if(argc == 1) {
        //interactive mode running
        while(1){
            if(prompt) {
                printf("%s", prompt);
            }
            while(fgets(linestr, 256, stdin)){ //change to if statement.
                // make sure line has a '\n' at the end of it
                if(!strstr(linestr, "\n"))
                    strcat(linestr, "\n");

                /* get arguments */
                buffer = yy_scan_string(linestr);
                yy_switch_to_buffer(buffer);
                toks = gettoks();       //parsing the command line
                yy_delete_buffer(buffer);

                if(toks[0] != NULL){    //if there's something in the command line, aka the first entry of argv[0] isn't empty
                    /* simple loop to echo all arguments */
                    //echo out the 
                    num_toks = 0;

                    for(ii=0; toks[ii] != NULL; ii++){
                        // printf( "Argument %d: %s\n", ii, toks[ii] ); 
                        num_toks++;

                        //checking to see how many number of > were passed in by user
                        if(!strcmp(toks[ii], ">")){
                            num_redirects++;
                        }
                    }
                    //check to see if the commands are built in commands 
                    // processBuiltinCommands(num_toks, toks);
                    notBuiltin(toks, num_toks, num_redirects);
                    
                }
                printf("%s", prompt);
            }
        }

        /* return to calling environment */
        return( retval );
    }
    else{ //incorrect number of arguments
        exitMsg();
        exit(1);
    }

}

/////////////////////////////////////////////
// Function Definitions
/////////////////////////////////////////////

/*
This function to processes the built in commands inputted by the user.
If the user inputted a built-in command, it will be executed and the function will return true.
Else, the function returns false.
bool True if a builtin command was entered, false if otherwise.

Parameters:
toks        char pointer    tokenized user input array
num_toks    int             number of tokens entered by user
*/

bool processBuiltinCommands(char **toks, int num_toks){
    
    //builtin EXIT: exiting the program
    if(!strcmp(toks[0], STR_EXIT)){  
        if(num_toks > 1){
            return false;
        }
        else{
            exit(0);
        }  
    }

    //builtin CD: changing directories
    else if (!strcmp(toks[0], STR_CD)) {  // cd is first command entered

        //right number of arguments passed in
        if(num_toks == 2){
            if(chdir(toks[1]) != 0){
                return false;
            }
        }
        else{
            // exitMsg();    //this exit message is correct.
            return false;
        } 
        return true;
    }

    //builtin PATH: updating the shell path
    else if(!strcmp(toks[0], STR_PATH)){        //path was entered by user
       
        //initialize local variable dd
        int dd = 1;

        //cleaning the is_path array 
        for(int ii = 0; ii <= 50; ii++){
        is_path[ii] = NULL;
        }

        //filling the user specified path
        while(toks[dd] !=NULL && dd <=256+1){
            is_path[dd-1] = toks[dd];
            dd++;
        }
        return true;
    }
    
    return false; //false bc not builtin cmd
}
    
/*
This function to processes the non-builtin commands inputted by the user.
It forks a child to carry out the processes and returns nothing upon completion.
--------------------------------------------
Parameters:
--------------------------------------------

toks        char pointer    tokenized user input array
num_redirects    int             number of '>' symbols entered by user
-------------------------------------------
Local Variables:
--------------------------------------------
working     char[]     concatinated user input that'll be passed into access()
pid         pid_t      process ID
status      int       
output      char*      name of user-specified output file
fd          FILE*      file descripter of output file

*/

void notBuiltin(char **toks, int num_toks, int num_redirects){
    
    //initializing and declaring vairables
    // char working[60]; //what we will pass into access
    pid_t pid;
    int status;
    int cmdCount = 0;
    int next_empty = 0;

    //handling of children processes
    pid_t children[num_toks];
    int num_children = 0;
   
   //looping through user entry string to count &
   for(int dd = 0; toks[dd] != NULL; dd++){
    
    //count encountered ampersand symbols
    if(strcmp(toks[dd], "&") == 0){
        cmdCount++;
    }
   }
   // if there are no &s entered:
   if(cmdCount == 0){
        //if it isnt a built-in command:
        if(!(processBuiltinCommands(toks, num_toks))){

            //fork a child to carry out the command
            pid = fork();
            if(pid == 0){  
                //process that &'s command
                runCmd(toks, 0, num_redirects);
                exitMsg();
                exit(1);       
            } 
        else if(pid > 0){    //parent reaping the children
            waitpid(pid, &status, 0); 
        } 
        else{
            return;
        }
        }
    }
    else{

        //keeps track of position in user-input string
        int position = 0;

        //looping through user input
        for(int dd = 0; toks[dd] != NULL; dd++){
            //process each ampersand in the user input string
            if(strcmp(toks[dd], "&") == 0 || dd == num_toks){
                //removing the & from the path
                toks[dd] = NULL;

                //fork a child to carry out the job
                pid = fork();
                if(pid == 0){ 
                    runCmd(toks, position, num_redirects);
                    exit(1);
                }
                else{
                    //adding pid to our children array to keep track of the children processes
                    children[num_children++] = pid;
                    //moving position tracker to after &
                    position = dd+1;
                }   
            }
        }
    //if we encountered a & but we're not done running through the user input string
    if(position < num_toks){
        //fork a child to carry out the job
        pid = fork();
        if(pid == 0){ 
            runCmd(toks, position, num_redirects);
            exit(1);
        }
        else{
            //parent reaping child
            waitpid(pid, &status, 0);
        }
    }

        //while we still have children executing
        // for(int ii = 0; num_children > 0; ii++){
            while(num_children > 0){
            //reaping all children processes and getting their PID
            pid_t cps = waitpid(-1, &status, 0);
            for(int dd = 0; dd < num_children; dd++){
                if(children[dd] == cps){
                    //remove the completed child process
                    for(int ee= 0; ee < num_children-1; ee++){
                        children[ee] = children[ee+1];
                    }
                    num_children--;
                    break;

                }
            }

        }
    }
    // }
    return;
}

/*
This function to handle the improper use of redirections inputted by the user.
It returns a char pointer named output upon successful completion
--------------------------------------------
Parameters:
--------------------------------------------
toks        char pointer    tokenized user input array
num_redirects    int             number of '>' symbols entered by user
-------------------------------------------

*/
char *h_redirections(char **toks, int num_redirect){
    //initilization of varibles
    char *output = NULL;
    int num_redirects =0;

       for (int ii = 0; toks[ii] != NULL; ii++){
        if(!strcmp(toks[ii], ">")){
            num_redirects++;
        }
       }
    
    //looping through user input and checking number of > operators
    for (int ii = 0; toks[ii] != NULL; ii++){
        
        //if user tries to use more than one redirect
        if(num_redirects > 1){
            exitMsg();
            exit(1);
        }

        //if an outfile is not specified:
        if(strcmp(toks[ii], ">") == 0 && toks[ii+1] == NULL){
            exitMsg();
            exit(1);
        }

        //if second redirect file is specified:
        if(strcmp(toks[ii], ">") == 0 && toks[ii+2] != NULL){
            exitMsg();
            exit(1);
        }
    }

        //opening user specified output file
        for (int ii = 0; toks[ii] != NULL; ii++){
        if((strcmp(toks[ii], ">"))== 0){
            output = toks[ii+1];    //inputting output file name specified by user into variable output

            ///replacing the > with a NULL so reading the input will work correctly
            toks[ii] = NULL;   
            toks[ii+1] = NULL; //removing the file name 
            break;  //breaking once it hits the first >
        }
    }
    return output;
}

/*
This function to run the command pasesed in by user.
It returns nothing upon successful completion
--------------------------------------------
Parameters:
--------------------------------------------
toks        char pointer    tokenized user input array
position    int             keeping position in user input array
num_redirects    int             number of '>' symbols entered by user
-------------------------------------------

*/
void runCmd(char **toks, int position, int num_redirects){
    char *output;
    FILE* fd;
    char working[256]; //what we will pass into access

    //file name passed into 'output' from redirection method
    output = h_redirections(&toks[position], num_redirects);
    
    //looping through to work on carrying out path command
    for(int ii = 0; is_path[ii] != NULL; ii++){
        
        //concatinating path, /, and cmd to pass into access()
        snprintf(working, sizeof(working), "%s/%s", is_path[ii], toks[position]);
        
        //if executable is located under that path
        if(access(working, X_OK) == 0){

            //if an output file was specified
            if(output != NULL){
                
                //attempting to open a file with specified name 
                if((fd = fopen(output, "w")) == NULL){
                    // printf("Line 442\n");
                    return;
                }

                //copying contents from stdout into file
                dup2(fileno(fd), STDOUT_FILENO);
                dup2(fileno(fd), STDERR_FILENO);
                
                //closing the file
                fclose(fd); 
            }
            execv(working, &toks[position]);
        }
    }
}
/*
This function to output error messages to stderr.
Returns nothing upon successful completion.
--------------------------------------------
*/
void exitMsg(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}
