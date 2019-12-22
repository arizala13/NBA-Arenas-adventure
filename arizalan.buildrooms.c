/******************************************
 ** Program Filename: arizalan.buildrooms.c
 ** Author: Andres Arizala
 ** Date: 10/16/19
 ** This will be used to create the rooms for program 2
 ** This will tie into the adventure code to give it rooms
 ** The headers below are used to allow the program to work properly including time, mkdir
 ** and other functions
 *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#define MAXROOM 7
#define MAXROOM_CONNECTIONS 6
#define MINROOM_CONNECTIONS 3
#define BUFFER_SIZE 256


/* Declaring an array of pointers
 * Here below we hard code the room names and the room types
 */
char* roomType[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
char* roomName[10] = {"toyota", "staples", "att", "pepsi", "fedex", "american", "chase", "amway", "target", "spectrum"};
int chosen[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*
 * refreneced from https://stackoverflow.com/questions/4159713/how-to-use-boolean-datatype-in-c
 */
typedef enum {false = 0, true = 1} bool;

/*
 * Here is the structs for each room
 * This refrenced from lecture "2.1 C structs"
 */
struct room
{
    char roomName[100];
    char roomType[12];
    struct room * outboundConnections[6];
    int numConnections;
};

struct room currentList[MAXROOM];
int contains(const int *arr, int count, int val);

bool IsGraphFull(struct room currentList[]){
    int i;
    for(i = 0; i <7; i++){
        if(currentList[i].numConnections < 3){
            return false;
        }
    }
    return true;
}

/*
* The functions below here are from the notes "Program 2.2 outline" that were
* were provided as notes.
*/

bool CanAddConnectionFrom(struct room* x)
{
  if(x->numConnections < 6){
      return true;
      
  }
  else {
      return false;
  }
}

/* This ses if that connection has already been made. If it has true if not false. */
bool ConnectionAlreadyExists(struct room* x,struct room* y)
{
    int i;
    for(i = 0; i< x->numConnections; i++){
        if(strcmp(x->outboundConnections[i]->roomName,y->roomName) == 0)
            return true;
    }
    return false;
}

/* Connects the rooms that we need to connect */
void ConnectRoom(struct room* x,struct room* y){
    x->outboundConnections[x->numConnections] = y;
    x->numConnections++;
}

/* makes sure we do not connect the same room to itself */
bool IsSameRoom(struct room* x, struct room* y){
    if(strcmp(x->roomName, y->roomName) == 0)
        return true;
    else
        return false;
}

/* randomly generates a random room */
struct room* GetRandomRoom(struct room* currentList){
    int random_room = rand() % 7;
    struct room* pointer;
    pointer = &currentList[random_room];
    return pointer;
}

/* main part of connecting all rooms
 * here we create the structs then after that do all the needed work to make sure
 * the rooms meet the standard of the rules that were given
 */
void AddRandomConnection(struct room* currentList)
{
  struct room * A ;
  struct room * B ;

  while(true)
  {
    A = GetRandomRoom(currentList);

    if (CanAddConnectionFrom(A))
      break;
  }

  do
  {
    B = GetRandomRoom(currentList);
  }
  while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

  ConnectRoom(A, B);
  ConnectRoom(B, A);
}


/*
 * Here we make the directory.
 * Then it gets the PID and adds the PID to the end of that specefic directory.
 * This is setup is refrenced from the lecture code "2.4 Manipulating direcories"
 * The PID portion was refrenced from a piazza post which helped clear up how to get the PID.
 */

/*
 * Here we get 7 random rooms from the given array
 * Rand() is used to get a random number then we step through the array one by one
 * the result is 7 random rooms generated. If we get to 10 then we start at the begining of
 * the array.
 */


void pick() {
    int getRandomNum;
    
    FILE* roomFile;
    
    int i;
    int j;
    
    char newestDirName[256];
    
    char* directoryNamePrefix = "arizalan.rooms.";
    int pid = getpid();
    
    char* randomRoom;

    memset(newestDirName,'\0',sizeof(newestDirName));
    sprintf(newestDirName,"%s%d",directoryNamePrefix,pid);

    mkdir(newestDirName, 0755);
    
    chdir(newestDirName);
    
    /* choose our 7 rooms; first room can be start_room, last can be end_room, fill up currentList */
    for(i=0;i<MAXROOM;i++) {
        /* pick a random room number 0 through 9 */
        getRandomNum = rand() % 10;
        while(chosen[getRandomNum] == 1) { /* as long as that room was previously chosen */
            /* then choose another random room */
            getRandomNum = rand() % 10;
        }
        /* finally mark that room as having been chosen (so it won't repeat) */
        chosen[getRandomNum] = 1;
        
        randomRoom = roomName[getRandomNum];
        /* printf("%s\n", randomRoom); */
        /* printf("open file!\n"); */
        
        sprintf(currentList[i].roomName, "%s", randomRoom);
        if(i == 0) {
            /* printf("I AM BEING SET TO START_ROOM"); */
            sprintf(currentList[i].roomType, "%s", roomType[0]);
        }
        else if(i == MAXROOM-1) {
            /* printf("I AM BEING SET TO END_ROOM"); */
            sprintf(currentList[i].roomType, "%s", roomType[1]);
        }
        else {
            /* printf("I AM BEING SET TO MID_ROOM"); */
            sprintf(currentList[i].roomType, "%s", roomType[2]);
        }
        
        currentList[i].numConnections = 0;
    }
    
    /* After we get all rooms we see if those given rooms are full with connections.
     * If they are not full they will continue to look for connections thaat they have not used
     * before hand. This is from "Program 2.2 outline" given in the notes.
     */
    
    while (IsGraphFull(currentList) == false)
    {
      AddRandomConnection(currentList);
    }
    
    
    /* for each of 7 rooms (or whatever maxroom is) we write to them what info is needed */
    for(i=0;i<MAXROOM;i++) {
        
        roomFile = fopen(currentList[i].roomName, "w");
        
        fprintf(roomFile, "ROOM NAME: %s\n", currentList[i].roomName);
        
        for(j=0;j<currentList[i].numConnections;j++) {
            fprintf(roomFile, "CONNECTION %d: %s\n", j+1, currentList[i].outboundConnections[j]->roomName);
        }
        
        fprintf(roomFile, "ROOM TYPE: %s\n", currentList[i].roomType);
        
        fclose(roomFile);
    }

}


/* main function that calls everything to run. */
int main(void){
    srand( (unsigned int) time(NULL) );
    pick();
    return 0;
}

