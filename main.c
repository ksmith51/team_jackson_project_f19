/*********************************************************************************
 * This is a simple roll call system with console-based interface
 * Must be able to:
 *      - Add/remove students
 *      - Read/write student data
 *      - Retrieve student data by Name, USF ID, or Email
 *      - Update all pieces of data
 * Each student must include:
 *      - Name (up to 40 char)
 *      - USF ID (up to 10 char)
 *      - Email (up to 40 char)
 *      - Presentation grade (0(F) to 4(A))
 *      - Essay grade (same as above)
 *      - Term Project grade (same as above)
*********************************************************************************/

#include "main.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* global constants / definitions */
#define BUFFER 10000                        //large integer
#define buff 100000000
#define MAX_ID 10
#define MAX_STRING 40

/* boolean type because C doesn't have one */
#define true 1
#define false 0
typedef int bool;

/* Enumeration of grades (F=0...A=4) */
typedef enum gradeType{
    F, D, C, B, A, ERR
} grade;

/* Structure that holds student information inside
*  the global array */
typedef struct studentInfo{
    char name[MAX_STRING + 1];              //40 char long, +1 for null char
    char email[MAX_STRING + 1];             //40 char long, +1 for null char
    char id[MAX_ID + 1];                    //10 char long, +1 for null char
    grade presentation;                     //enum value
    grade essay;                            //enum value
    grade project;                          //enum value
} student;

/* global variables */
student *Students;                          //holds all student information
int count = 0;                              //index of Students, initially 0
int max = 2;                                //number of student structs allocated to Students, initially 2

/* ================================================================================================================== */
/* HELPER FUNCTIONS */

/*
    reallocates memory to Students if not sufficient
*/
void add_student_memory(){
    //if # of filled students equals amount allocated,
    //reallocate Students with power 2 of current number
    if(count == max){
        max *= max;
        Students = calloc(Students, sizeof(student)^(max*buff));
    }
}

/* 
    trims string of beginning and ending spaces
    removes tabs from inside string, replaced with space
    removes newline from end of string
*/
void trim_string(char *str){
    char *start = str;
    char *end = str + strlen(str);
    char *ptr;

    //skip if given string is empty or non-existent
    if(*str == '\0' || str == NULL){ return; }

    //finds first non-whitespace character
    while((isspace(*start) || *start == '\n' || *start == '\t') && start != end){
        *start = '\0';
        start++;
    }

    //finds last non-whitespace character
    while ((isspace(*end) || *end == '\n' || *end == '\t') && end != start){
        *end  = '\0';
        end--;
    }

    //replaces tabs in string with single spaces
    if(start < end){
        ptr = start;
        while(ptr != end){
            if(*ptr == '\t'){
                *ptr = ' ';
            }
            ptr++;
        }
    }
    
    //moves trimmed string to front
    if(start != str){
        ptr = str;
        while(*start != '\0'){
            *ptr = *start;
            ptr++;
            start++;
        }
        *ptr = '\0';
    }
}

/*
    function to convert character input into enum value
    returns ERR if character given is not a, b, c, d, or f
*/
grade convert_char_to_grade(char c){
    grade ret;
    switch(c){
        case 'A': case 'a': case '4':   ret = A;
            break;
        case 'B': case 'b': case '3':   ret = B;

        case 'C': case 'c': case '2':   ret = C;
            break;
        case 'D': case 'd': case '1':   ret = D;
            break;
        case 'F': case 'f': case '0':   ret = F;
            break;
        default:              ret = ERR;
            break;
    }
    return ret;
}



/*
    function to convert enum input into char
    returns ? if character given is not A, B, C, D, or F
*/
char convert_grade_to_char(grade g){
    char ret;
    switch(g){
        case A:     ret = 'A';
            break;
        case B:     ret = 'B';
            break;
        case C:     ret = 'C';
            break;
        case D:     ret = 'D';
            break;
        case F:     ret = 'F';
            break;
        default:    ret = '?';      //should never be returned
            break;
    }
    return ret;
}

/*
    function to print a student struct
*/
void print_student(student s, bool printOptions){

    if(printOptions){
        printf("A: Name: %s\n", s.name);
        printf("B: Email: %s\n", s.email);
        printf("C: UID: %s\n", s.id);
        printf("D: Presentation Grade: %c\n", convert_grade_to_char(s.presentation));
        printf("E: Essay Grade: %c\n", convert_grade_to_char(s.essay));
        printf("F: Project Grade: %c\n", convert_grade_to_char(s.project));
        printf("G: Exit Updating Student\n");
    }
    else{
        printf("Name: %s\n", s.name);
        printf("Email: %s\n", s.email);
        printf("UID: %s\n", s.id);
        printf("Presentation Grade: %c\n", convert_grade_to_char(s.presentation));
        printf("Essay Grade: %c\n", convert_grade_to_char(s.essay));
        printf("Project Grade: %c\n", convert_grade_to_char(s.project));
    }

}

/*
    function to determine if user option chosen is valid
    used mostly for find function
*/
bool valid_option(char *input){
    switch(input[0]){
        case 'A': case 'a':
            return true;
        case 'B': case 'b':
            return true;
        case 'C': case 'c':
            return true;
        case 'D': case 'd':
            return true;
    }
    return false;
}

/* 
    helper function for getting user input
    used any time a string is read in from a source
*/
void get_input(char *input){
    fgets(input, BUFFER, stdin);
    trim_string(input);
}

/*
    checks format of number, returns true if correct
*/
bool id_check(char *str){
    while(*str != '\0'){
        if(!isxdigit(*str)){
            return false;
        }
        str++;
    }
    return true;
}

/*
    prints all available commands for user
*/
void print_commands(){
    printf("********************************************\n");
    printf("* COMMANDS:                                *\n");
    printf("* a: Add Student         r: Remove Student *\n");
    printf("* p: Show Students       u: Update Student *\n");
    printf("* f: Find Student        q: Quit Program   *\n");
    printf("********************************************\n");
    printf("Enter \"h\" for options menu\n");
}

/*
    function to open save file for reading/writing
    also ensures save file exists, creates one if not
*/
void open_student_file(FILE **file){
    *file = fopen("students.txt", "w+");
    if(*file == NULL){
        printf("...students.txt does not exist. Creating new file\n");
            *file = fopen("students.txt", "w");
            if(*file == NULL){
                printf("...Unable to open file\n");
            return;
            }
        printf("...students.txt created successfully\n");
        return;
    }
}

/*
    function to open save file for rewriting
*/
void write_student_file(FILE **file){
    *file = fopen("students.txt", "w+");
    if(*file == NULL){
        printf("Unable to open file..\n");
        return;
    }
}

/*
    function to close the save file
*/
void close_student_file(FILE **file){
    fclose(*file);
}

/* ================================================================================================================== */
/* MAIN FUNCTIONS */

/*
    function to save the student file
*/
void save_student_file(){
    FILE *file;
    int i;
    char str[BUFFER];

    //open student file
    write_student_file(&file);

    /* loops thru student array, adds all info to save file */
    for (i = 0; i <= count; i++){
        //writes students name
        strcpy(str, Students[i].name);
        fprintf(file, "%s\n", str);
        //writes students email
        strcpy(str, Students[i].email);
        fprintf(file, "%s\n", str);
        //writes students uid
        strcpy(str, Students[i].id);
        fprintf(file, "%s\n", str);
        //writes students presentation grade
        fprintf(file, "%c\n", convert_grade_to_char(Students[i].presentation));
        //writes students essay grade
        fprintf(file, "%c\n", convert_grade_to_char(Students[i].essay));
        //writes students project grade
        fprintf(file, "%c\n", convert_grade_to_char(Students[i].project));
    }

    //close student file
    close_student_file(&file);
}

/*
    function to load the student file
*/
void load_student_file(){
    FILE *file;
    char str[BUFFER];

    //open student file
    open_student_file(&file);
    if(fscanf(file, "%s", str) == EOF){
        return;
    } else {
        rewind(file);
    }

    //read in file text
    while(!feof(file)){
        student s;

        //scan students name
        fgets(str, BUFFER, file);
        trim_string(str);
        strcpy(s.name, str);

        //scan students email
        fgets(str, BUFFER, file);
        trim_string(str);
        strcpy(s.email, str);

        //scan students id
        fgets(str, BUFFER, file);
        trim_string(str);
        strcpy(s.id, str);

        //scan students presentation grade
        fgets(str, BUFFER, file);
        trim_string(str);
        s.presentation = convert_char_to_grade(str[0]);

        //scan students presentation grade
        fgets(str, BUFFER, file);
        trim_string(str);
        s.essay = convert_char_to_grade(str[0]);

        //scan students presentation grade
        fgets(str, BUFFER, file);
        trim_string(str);
        s.project = convert_char_to_grade(str[0]);

        Students[count] = s;
        count++;
        add_student_memory();

        if(fscanf(file, "\n", str) == EOF) { break; }
    }

    //close student file
    close_student_file(&file);
    return;
}

/* 
    create and return new student
    ADD FORMAT CHECKER FOR EMAIL AND ID
*/
student create_student(){
    student s;
    char input[BUFFER];

    //get name
    printf("Enter student name (40 char max): ");
    get_input(input);
    while(strlen(input) == 0 || strlen(input) > MAX_STRING){
        printf("Invalid name. Re-enter student name (40 char max): ");
        get_input(input);
    }
    strcpy(s.name, input);

    //get email
    printf("Enter student email: ");
    get_input(input);
    while(strlen(input) == 0 || strlen(input) > MAX_STRING){
        printf("Invalid name. Re-enter student email (40 char max): ");
        get_input(input);
    }
    strcpy(s.email, input);

    //get id
    printf("Enter student UID: ");
    get_input(input);
    while(strlen(input) == 0 || strlen(input) > MAX_ID || id_check(input) == false){
        printf("Invalid name. Re-enter student UID (10 char max): ");
        get_input(input);
    }
    //check format
    strcpy(s.id, input);

    //get presentation grade
    printf("Enter presentation grade (A, B, C, D, F): ");
    get_input(input);
    while(strlen(input) != 1 || convert_char_to_grade(input[0]) == ERR){
        printf("Invalid grade. Re-enter presentation grade (A, B, C, D, F): ");
        get_input(input);
    }
    s.presentation = convert_char_to_grade(input[0]);

    //get essay grade
    printf("Enter essay grade (A, B, C, D, F): ");
    get_input(input);
    while(strlen(input) != 1 || convert_char_to_grade(input[0]) == ERR){
        printf("Invalid grade. Re-enter essay grade (A, B, C, D, F): ");
        get_input(input);
    }
    s.essay = convert_char_to_grade(input[0]);

    //get project grade
    printf("Enter project grade (A, B, C, D, F): ");
    get_input(input);
    while(strlen(input) != 1 || convert_char_to_grade(input[0]) == ERR){
        printf("Invalid grade. Re-enter project grade (A, B, C, D, F): ");
        get_input(input);
    }
    s.project = convert_char_to_grade(input[0]);
    return s;
}

/*
    function to search for student***
    RETURN POINTER OR INDEX
*/
int find_student(){
    char input[BUFFER];
    int parameter, valid = 0;

    //get search parameter
    printf("A: Name\nB: Email\nC: UID\nD: Exit\n");
    printf("How would you like to search for the student?\n");
    while(valid == 0){
        printf("Enter letter of search parameter: ");
        get_input(input);
        if(strlen(input) == 1){
            switch(input[0]){
                case 'A': case 'a':
                    parameter = 0;
                    valid = 1;
                    break;
                case 'B': case 'b':
                    parameter = 1;
                    valid = 1;
                    break;
                case 'C': case 'c':
                    parameter = 2;
                    valid = 1;
                    break;
                case 'D': case 'd':
                    valid = 1;
                    return -1;
                    break;
                default:
                    valid = 0;
                    break;
            }
        }
        if (valid == 0){
            printf("Unknown parameter. Please try again.\n");
        }
    }

    //get students info to search with
    printf("Enter students ");
    switch(parameter){
        case 0:
            printf("name: ");
            get_input(input);
            while(strlen(input) == 0 || strlen(input) > MAX_STRING){
                printf("Invalid name. Re-enter student name (40 char max): ");
                get_input(input);
            }
            break;
        case 1:
            printf("email: ");
            get_input(input);
            while(strlen(input) == 0 || strlen(input) > MAX_STRING){
                printf("Invalid email. Re-enter student email (40 char max): ");
                get_input(input);
            }
            break;
        case 2:
            printf("UID: ");
            get_input(input);
            while(strlen(input) == 0 || strlen(input) > MAX_ID){
                printf("Invalid UID. Re-enter student UID (10 char max): ");
                get_input(input);
            }
            break;
        default:
            printf("ERROR with parameter\n");
            return -1;
    }
    
    //search for student using given parameter and information
    for(int i = 0; i < count; i++){
        switch(parameter){
            case 0:
                if (strcmp(Students[i].name, input) == 0){
                    return i;
                }
                break;
            case 1:
                if (strcmp(Students[i].email, input) == 0){
                    return i;
                }
                break;
            case 2:
                if (strcmp(Students[i].id, input) == 0){
                    return i;
                }
                break;
            default:
                printf("ERROR searching\n");
                return -1;
        }
    }
    printf("Student does not exist\n");
    return -1;
}


/*
    function to remove student from save file***
*/
void remove_student(){
    int i, j;                                   //ADDED DECLARATION OF j
    //find student(get pointer to student)
    i = find_student();
    if(i == -1){
        return;
    }
    //remove from array by moving all students after selected student forward once
    for(int i = j; j < count; i++){
        strcpy(Students[i].name, Students[j].name);
        strcpy(Students[i].email, Students[j].email);
        strcpy(Students[i].id, Students[j].id);
        Students[i].presentation = Students[j].presentation;
        Students[i].essay = Students[j].essay;
        Students[i].project = Students[j].project;
        j++;
    }
    count = count - 1;

    //rewrite save file
    save_student_file();
}




/*
    function to update specific field of student
*/

student update_student_field(student s, int fieldToChange){
    char input[BUFFER];

    switch(fieldToChange){
        case 1:
            printf("Enter student name (40 char max): ");
            get_input(input);
            while(strlen(input) == 0 || strlen(input) > MAX_STRING){
                printf("Invalid name. Re-enter student name (40 char max): ");
                get_input(input);
            }
            strcpy(s.name, input);
            break;
        case 2:
            printf("Enter student email: ");
            get_input(input);
            while(strlen(input) == 0 || strlen(input) > MAX_STRING){
                printf("Invalid name. Re-enter student email (40 char max): ");
                get_input(input);
            }
            strcpy(s.email, input);
            break;
        case 3:
            printf("Enter student UID: ");
            get_input(input);
            while(strlen(input) == 0 || strlen(input) > MAX_ID || id_check(input) == false){
                printf("Invalid name. Re-enter student UID (10 char max): ");
                get_input(input);
            }
            //check format
            strcpy(s.id, input);
            break;
        case 4:
            printf("Enter presentation grade (A, B, C, D, F): ");
            get_input(input);
            while(strlen(input) != 1 || convert_char_to_grade(input[0]) == ERR){
                printf("Invalid grade. Re-enter presentation grade (A, B, C, D, F): ");
                get_input(input);
            }
            s.presentation = convert_char_to_grade(input[0]);
            break;
        case 5:
            printf("Enter essay grade (A, B, C, D, F): ");
            get_input(input);
            while(strlen(input) != 1 || convert_char_to_grade(input[0]) == ERR){
                printf("Invalid grade. Re-enter essay grade (A, B, C, D, F): ");
                get_input(input);
            }
            s.essay = convert_char_to_grade(input[0]);
            break;
        case 6:
            printf("Enter project grade (A, B, C, D, F): ");
            get_input(input);
            while(strlen(input) != 1 || convert_char_to_grade(input[0]) == ERR){
                printf("Invalid grade. Re-enter project grade (A, B, C, D, F): ");
                get_input(input);
            }
            s.project = convert_char_to_grade(input[0]);
            break;
        default:
            printf("Error");
    }

    return s;
}

/*
    function to update student***
*/
void update_student(){
    char c, input[BUFFER];
    printf("...updating student\n");

    //Access array to edit student
    int arrayIndex = find_student();
    if (arrayIndex == -1){
        return;
    }

    //Initial Student found from find_student to update
    student studentToUpdate = Students[arrayIndex];

    //Student After Update
    student updatedStudent = studentToUpdate;

    //Keep Asking for updates before exiting
    bool keepRunning = true;
    while(keepRunning){

        //Pass in True since it will show A, B, C, .. next to entries
        print_student(updatedStudent, true);

        printf("Select an option for which field to update\n");
        get_input(input);
        c = input[0];
        switch(c){
            case 'A':
            case 'a':
                updatedStudent = update_student_field(studentToUpdate, 1);
                break;
            case 'B':
            case 'b':
                updatedStudent = update_student_field(studentToUpdate, 2);
                break;
            case 'C':
            case 'c':
                updatedStudent = update_student_field(studentToUpdate, 3);
                break;
            case 'D':
            case 'd':
                updatedStudent = update_student_field(studentToUpdate, 4);
                break;
            case 'E':
            case 'e':
                updatedStudent = update_student_field(studentToUpdate, 5);
                break;
            case 'F':
            case 'f':
                updatedStudent = update_student_field(studentToUpdate, 6);
                break;
            case 'G':
            case 'g':
                keepRunning = false;
                break;
        }
    }

    Students[arrayIndex] = updatedStudent;
    save_student_file();

}





/* MAIN FUNCTION */
int main() {
    //allocate memory
    Students = (student*)calloc(1, sizeof(student)*max);
    if (Students == NULL){
        printf("...memory not allocated\n");
        return 0;
    }

    //load students from save file
    load_student_file();

    //display commands initially
    print_commands();
    printf("\n");

    int end = 0;
    //while user has not entered command to quit program:
    while(end != 1){
        int valid = 0;

        //repeats prompt until user enters valid command
        do{
            char c, input[BUFFER];

            //prompt user for input
            printf("Enter command: ");
            get_input(input);

            while(strlen(input) > 1){
                printf("Unknown command. Enter \"h\" to see all commands.\nEnter command: ");
                get_input(input);
            }
            c = input[0];

            //determines command
            switch (c){

                //add student
                case 'A':
                case 'a': valid = 1;
                    printf("*****Adding student*****\n");
                    Students[count] = create_student();
                    count++;
                    //reduces amount of reallocations
                    add_student_memory();
                    printf("\n");
                    break;
                
                //remove student
                case 'R':
                case 'r': valid = 1;
                    printf("*****Removing student*****\n");
                    remove_student();
                    printf("\n");
                    break;
                
                //print students
                case 'P':
                case 'p': valid = 1;
                    printf("*****Printing students*****\n");
                    if (count == 0){
                        printf("ERR: No students exist. Enter \"a\" to add a new student.\n");
                        break;
                    }
                    for (int i = 0; i < count; i++){
                        print_student(Students[i],false);
                        printf("\n");
                    }
                    break;
                
                //update student
                case 'U':
                case 'u': valid = 1;
                    printf("*****Updating student*****\n");
                    update_student();
                    printf("\n");
                    break;
                
                //find student
                case 'F':
                case 'f': valid = 1;
                    int index;
                    printf("*****Finding student*****\n");
                    index = find_student();
                    if(index != -1){
                        print_student(Students[index],false);
                    }
                    printf("\n");
                    break;

                //quit program
                case 'Q':
                case 'q': valid = 1;
                    printf("*****Quitting program*****\n");
                    save_student_file();
                    end = 1;
                    printf("\n");
                    break;

                //show commands
                case 'H':
                case 'h': valid = 1;
                    print_commands();
                    printf("\n");
                    break;

                default:
                    printf("Unknown command. Enter \"h\" for commands.\n");
                    break;
            }

        }while (valid != 1);
    }
    free(Students);
    return 0;
}