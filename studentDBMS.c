#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_RECORDS 10
#define NAME_LENGTH 100

typedef struct sturec {
    unsigned int id;

    char fullname[NAME_LENGTH+1];

    float gpa;
}DBMS_RECORD;

void addRecords(DBMS_RECORD ** records, int * numberInArray);
int searchRecords(DBMS_RECORD ** records, int id, int * numberInArray);
void deleteRecords(DBMS_RECORD ** records, int id, int * numberInArray);
void editRecords(DBMS_RECORD ** records, int * numberInArray);
void printRecords(DBMS_RECORD ** records, int numberInArray);
void getInfo(DBMS_RECORD ** records, int numberInArray);
void writeToFile(char * path, DBMS_RECORD ** records, int * numberInArray);
void readFromFile(char * path, DBMS_RECORD ** records, int * numberInArray);
int findFreeSlot(DBMS_RECORD ** records, int * numberInArray);


int main(int argc, char * argv[], char * env[]){
    int status = 1;
    int choice = 0;
    int numberInArray = 0;
    int id = 0;
    char * path = "./testDB.dbm";
    int searchResult;
    DBMS_RECORD ** records = (DBMS_RECORD **)malloc(sizeof(DBMS_RECORD *) * 10);
    //set each pointer to null
    for(int i = 0; i < 10; i++){
        records[i] = NULL;
    }
   
    if(access(path, F_OK) == 0)
        readFromFile(path, records, &numberInArray);    
    printRecords(records, numberInArray);

    //read from and print file
    //Display a menu of choices to user
    while(status){
        printf("Enter 1 to add records, 2 to search for a record, 3 to delete records, 4 to edit, 5 to print records, or 0 to quit: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            addRecords(records, &numberInArray);
            break;
        case 2:
            printf("Enter ID to search for: ");
            scanf("%d", &id);
            searchResult = searchRecords(records, id, &numberInArray);
            break;
        case 3:
            printf("Enter ID to delte: ");
            scanf("%d", &id);
            deleteRecords(records, id, &numberInArray);
            break;
        case 4:
            editRecords(records, &numberInArray);
            break;
        case 5:
            printRecords(records, numberInArray);
            break;
        case 0:
            status = 0;
            break;
        default:
            printf("invalid choice\n");
            break;
        }

    }

    writeToFile(path, records, &numberInArray);
    return 0;
}


void addRecords(DBMS_RECORD ** records, int * numberInArray){
    //TODO: handle the error of trying to add records for the second time.
    int status = 1;
    int freeSlot;
    while(status){
        freeSlot = findFreeSlot(records, numberInArray);
        if(freeSlot == -1){
            return;
        }
        printf("free slot is: %d\n", freeSlot);
        records[freeSlot] = (DBMS_RECORD *) malloc(sizeof(DBMS_RECORD));
        getInfo(records, freeSlot);
        (*numberInArray)++;
        //get status    
        printf("Would you like to add another record? (1/0): ");
        scanf("%d", &status);
    }
}
int searchRecords(DBMS_RECORD ** records, int id, int * numberInArray){
    for(int i = 0; i < (*numberInArray); i++){
        if(records[i] == NULL)
            continue;
        if(records[i]->id == id){
            printf("returned number from search is: %d\n", i);
            return i;
        }
    }
    printf("ID not found\n");
    return -1;
}

void deleteRecords(DBMS_RECORD ** records, int id, int * numberInArray){
    int freeSlot = findFreeSlot(records, numberInArray);
    int slot = searchRecords(records, id, numberInArray);
    if(slot == -1){
        exit(3);
    }
    free(records[slot]);
    records[slot] = NULL;
    (*numberInArray)--;
    return;
}

void editRecords(DBMS_RECORD ** records, int * numberInArray){
    int id = 0;
    printf("what is the id of the record you would like to edit?");
    scanf("%d", &id);
    int position = searchRecords(records, id, numberInArray);
    if(position == -1){
        return;
    }
    getInfo(records, position);
    return;
}

void printRecords(DBMS_RECORD ** records, int numberInArray){
    for(int i = 0; i < numberInArray; i++){
        if(records[i] == NULL){
            continue;
        }
        printf("Student %d's ID is: %d\n", i, records[i]->id);
        printf("Student %d's full name is: %s", i, records[i]->fullname);
        printf("Student %d's GPA is: %f\n", i, records[i]->gpa);
    }
    printf("number in array: %d\n", numberInArray);
    return;
}
void getInfo(DBMS_RECORD ** records, int numberInArray){
    unsigned int id = 0;
    float gpa = 0;
    char name[NAME_LENGTH+1];
    //get id
    printf("Enter id for record number %d: ", numberInArray);
    scanf("%d", &id);
    records[numberInArray]->id = id;
    //get gpa
    printf("Enter gpa for record number %d: ", numberInArray);
    scanf("%f", &gpa);
    records[numberInArray]->gpa = gpa;
    //get fullname
    printf("Enter the student's name for record number %d: ", numberInArray);
    getc(stdin);
    fgets(name, NAME_LENGTH+1, stdin);
    strcpy(records[numberInArray]->fullname, name);
}

void writeToFile(char * path, DBMS_RECORD ** records, int * numberInArray){
    int fileStatus = 1;
    FILE * outputFile = fopen(path, "w");
    for(int j = 0; j < *numberInArray; j++){
        fileStatus = fwrite(records[j], sizeof(DBMS_RECORD), 1, outputFile);
    }
    fclose(outputFile);
}

void readFromFile(char * path, DBMS_RECORD ** records, int * numberInArray){
    DBMS_RECORD buf;
    int fileStatus = 1;
    FILE * inputFile = fopen(path, "r");
    if(access(path, F_OK) == 0){
        while((fileStatus = fread(&buf, sizeof(DBMS_RECORD), 1, inputFile))){
            records[*numberInArray] = (DBMS_RECORD *)malloc(sizeof(DBMS_RECORD));
            records[*numberInArray]->id = buf.id;
            records[*numberInArray]->gpa = buf.gpa;
            strcpy(records[*numberInArray]->fullname, buf.fullname);
            (*numberInArray)++;
        }
    }
    else{
        printf("database not found\n");
        exit(2);
    }
    fclose(inputFile);
    printf("Number in array: %d\n", *numberInArray);
    return;
}
int findFreeSlot(DBMS_RECORD ** records, int * numberInArray){
    for(int i = 0; i <= *numberInArray; i++){
        if(records[i] == NULL){
            return i;
        }
    }
    printf("no null position found\n");
    return -1;
}
