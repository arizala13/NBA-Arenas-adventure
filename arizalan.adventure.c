/******************************************
 ** Program Filename: arizalan.adventure.c
 ** Author: Andres Arizala
 ** Date: 10/23/19
 ** This will be use the rooms from the rooms file to start the game and then play the game.
 * The setup that the most curent rooms file has is the path that is used to play the game,
 * Everytime the buildrooms file is rerun it creates a new randomly generated path. 
 *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>


/*
 8 letters plus a null character = 9 letters for currentRoom
 256 + 1 for currentWorkingDir
 cwd + room = current file that represent's players location
*/
char currentWorkingDir[257];
char currentRoom[9];
int gameOver = 0;
char startRoom[257];
int steps = 0;
char victoryPath[1000];

/*
* Here is the info to get the most recent room files that was created in that directory
* This refrenced from required readings 2.4 The code below is made from that lecture.
* We get the most recent directory and set that to currentWorkingDir which we
* later use.
*/
void recentDir(){
    
  int newestDirTime = -1;
  char targetDirPrefix[32] = "arizalan.rooms.";
  char newestDirName[256];
  memset(newestDirName, '\0', sizeof(newestDirName));

  DIR* dirToCheck;
  struct dirent *fileInDir;
  struct stat dirAttributes;

  dirToCheck = opendir(".");

  if (dirToCheck > 0)
  {
    while ((fileInDir = readdir(dirToCheck)) != NULL)
    {
      if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
      {
        stat(fileInDir->d_name, &dirAttributes);

        if ((int)dirAttributes.st_mtime > newestDirTime)
        {
          newestDirTime = (int)dirAttributes.st_mtime;
          memset(newestDirName, '\0', sizeof(newestDirName));
          strcpy(newestDirName, fileInDir->d_name);
        }
      }
    }
  }

    closedir(dirToCheck);

    /* printf("Newest entry found is: %s\n", newestDirName); -- used for debugging  */

    strcpy(currentWorkingDir, newestDirName);
    
    /* printf("Newest entry found is: %s\n", currentWorkingDir); -- used for debugging */
    return;
}

/*
 * Read thorugh all files in CWD and find one with ROOM TYPE: START_ROOM
 * Once that room is found we use that path to start the game
 */
void findStartRoom() {
    /* printf("we are in findStartRoom \n"); -- used to debug */
    
    DIR* dirToCheck;
    struct dirent *fileInDir;
    struct stat dirAttributes;
    
    /*
    * The code below is similar to what we used to get the startroom().
    * This was refrenced from the from required readings 2.4.
    * The end goal here is to find the file that has the START ROOM in it.
    * Once that is found in the we set that as the startRoom.
    */

    dirToCheck = opendir(currentWorkingDir);
    if (dirToCheck > 0)
    {
      while ((fileInDir = readdir(dirToCheck)) != NULL)
      {
          stat(fileInDir->d_name, &dirAttributes);
          if(fileInDir->d_type == 8) {
              /* open fileInDir->d_name (filename) from CWD */
              char fullPath[265];
              int readIndex = 0;
              int writeIndex = 0;
              while(currentWorkingDir[readIndex] != '\0') {
                  fullPath[writeIndex] = currentWorkingDir[readIndex];
                  writeIndex++;
                  readIndex++;
              }
              fullPath[writeIndex] = '/';
              writeIndex++;
              readIndex = 0;
              while(fileInDir->d_name[readIndex] != '\0') {
                  fullPath[writeIndex] = fileInDir->d_name[readIndex];
                  writeIndex++;
                  readIndex++;
              }
              fullPath[writeIndex] = '\0';
              
              /* printf("OPEN FILE\n"); -- used for debugging */
              const char* mode = "r";
              FILE* f = fopen(fullPath, mode);
              
              /* printf("BUFFER\n"); */
              char *buffer = NULL;
              /* printf("Buffer CREATED\n"); */
              if(f == NULL) {
                  exit(EXIT_FAILURE);
              }

              ssize_t read;
              size_t len = 0;

              while((read = getline(&buffer, &len, f)) != -1) {
                  
                  char searchForRoom[256];
                  
                  memset(searchForRoom, '\0', sizeof(searchForRoom));
                  strcpy(searchForRoom, "ROOM TYPE: START_ROOM\n");
                  
                  /* this is the starting room - we found it!
                   * we also notate the path that got us here so that we can start the game
                   * there!
                   */
                  if(strcmp(buffer, searchForRoom) == 0)
                  {
                      strcpy(startRoom, fullPath);
                      break;
                  }
              }
          }
      }
    }
    else {
        printf("COULD NOT OPEN\n");
    }

      closedir(dirToCheck);
}

/*
 * Display all information in cwd/startRoom.
 * Opens the file at startRoom and print its contents.
 */
void printRoom() {
        
    /* printf("We are now in the printRoom() part of the program\n"); */
    
    const char* mode = "r";
    FILE* f = fopen(startRoom, mode);
    
    char *buffer = NULL;

    if(f == NULL) {
        exit(EXIT_FAILURE);
    }

    ssize_t read;
    size_t len = 0;
    /* printf("Reading lines\n"); */
    int itemCount = 0;
    while((read = getline(&buffer, &len, f)) != -1) {
                
        /* The code below searches for the current room name, connections and
          * room type. This info will be displayed to the user.
          */
        
        char currentRoomName[256];
        
        memset(currentRoomName, '\0', sizeof(currentRoomName));
        strcpy(currentRoomName, "ROOM NAME\n");
        
        /* This looks for the CURRENT LOCATION: -- we will save that to output to terminal later
         * THe utilizatipn of strncmp below is refrenced from stackoverflow
         * https://stackoverflow.com/questions/19555434/how-to-extract-a-substring-from-a-string-in-c
         */
        if(strncmp(buffer, currentRoomName, 9) == 0)
        {
            
            const char* lineConst = buffer;
            char line[256];
            char *subString;

            strcpy(line, lineConst);
            
            /* strtok is used to find a specfic text  */
            subString = strtok(line,"ROOM NAME: ");

            printf("CURRENT LOCATION: %s", subString);
            printf("POSSIBLE CONNECTIONS:");

        }

        char connections[256];
        
        memset(connections, '\0', sizeof(connections));
        strcpy(connections, "CONNECTION");
        
        /* This looks for the connecting room -- we will save that to output to terminal later
         * Here again we use the sta stackoverflow post refrenced above
         */
        if(strncmp(buffer, connections, 10) == 0)
        {
            
            const char* lineConst = buffer;
            char line[256];
            char *subString;

            strcpy(line, lineConst);

            subString = strtok(line,":");
            subString=strtok(NULL,"\n");

            if(itemCount > 0) {
                printf(",");
            }
            printf("%s", subString);
            itemCount++;
        }
        
        char roomType[256];
        
        memset(roomType, '\0', sizeof(roomType));
        strcpy(roomType, "ROOM TYPE\n");
        
        char endRoom[256];
        
        memset(endRoom, '\0', sizeof(endRoom));
        strcpy(endRoom, "ROOM TYPE: END_ROOM\n");
        
        /* This looks for the roomType -- we will keep track of this to make sure we have not reached
         * the end of the game. If we have then we will tell the user that.
         */
        if(strncmp(buffer, roomType, 9) == 0)
        {
            if(strcmp(buffer, endRoom) == 0)
            {
                printf("\n");
                printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS! \n");
                printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", steps);
                printf("%s\n", victoryPath);
                
                gameOver = 1;
                break;
            }
        }
    }
        
    
}

int roomHasExit(char userType[9]) {
    
        const char* mode = "r";
        FILE* f = fopen(startRoom, mode);
        
        char *buffer = NULL;

        if(f == NULL) {
            exit(EXIT_FAILURE);
        }

        ssize_t read;
        size_t len = 0;

        int numRoomms = 0;
        int count = 1;
        while((read = getline(&buffer, &len, f)) != -1) {
                
            
            
            /* The code below searches for the current room name, connections and
              * room type. This info will be displayed to the user.
              */
            
            char currentRoomName[256];
            
            memset(currentRoomName, '\0', sizeof(currentRoomName));
            strcpy(currentRoomName, "ROOM NAME\n");
            
            /* This looks for the CURRENT LOCATION: -- we will save that to output to terminal later
            * The code below was refrenced from the stackoverflow post below
            * https://stackoverflow.com/questions/19555434/how-to-extract-a-substring-from-a-string-in-c
             */
            if(strncmp(buffer, currentRoomName, 9) == 0)
            {
                /* printf("CURRENT LOCATION: %s\n", buffer); */
                
                const char* lineConst = buffer;
                char line[256];
                char *subString;

                strcpy(line, lineConst);

                /* find the text ROOM NAME: */
                subString = strtok(line,"ROOM NAME: ");

            }

            char connections[256];
            
            memset(connections, '\0', sizeof(connections));
            strcpy(connections, "CONNECTION");
            
            /* This looks for the connecting room -- we will save that to output to terminal later */
            if(strncmp(buffer, connections, 10) == 0)
            {
                
                const char* lineConst = buffer;
                char line[256];
                char *subString;

                strcpy(line, lineConst);

                subString = strtok(line,":");
                subString=strtok(NULL,"\n");

                
                /* need to get space in front deleted to comapre properly
                 * That is were the sscanf comes in hand. It deletes the
                 * begining space that the string has to compare properly
                 */
                
                sscanf(subString, "%s", subString);
            
                
                /* Here we take the user to a different room
                 * This is done by calling recentDir() again then
                 * concatinating to the end "/" and what the user typed
                 * We only get here if it is a valid room
                 */

                if(strcmp(subString, userType) == 0){
                    
                    steps++;
                    /* printf("This is how many steps the user has taken so far %d \n", steps); */
                    strcat(victoryPath, userType);
                    strcat(victoryPath, "\n");
                    
                    recentDir();
                    strcat(currentWorkingDir,"/");
                    strcat(currentWorkingDir,userType);
                    
                    strcpy(startRoom, currentWorkingDir);

                    gameOver = 0;
                }
                
                /* Here we track what the user has enetered to see if that matches
                 * one of the rooms. If all rooms have been compared and
                 * they are not a match we later towards the end of this function
                 * tell the user that is not a room.
                 */
                 
                if((strcmp(subString, userType) != 0)) {
                    /* printf("This is numRoomms %d\n", numRoomms); */
                    /* printf("This is COUNT %d\n", count); */
                    count++;
                }
            numRoomms++;
            }
        }
                /* Here we tell the user that what they enetered is not a match
                 */
                    if(numRoomms  < count){
                        printf("\n");
                        printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN. \n");
                        printf("\n");
                    }
    }

/* Call to mutex time function
* This is refrenced from lecture 2.3 Concurrency at min 18:54 "Creating a thread"
*/

/* creates mutex function
 * The below website and TAs helped me get the mutex portion to work correctly
 * https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
 */
pthread_t thread[2];
int counter;
pthread_mutex_t lock;
  
/* Function to get time */
void* getTime()
{
    /* MUTEX IS locked */
    pthread_mutex_lock(&lock);

    char timeStamp[256];
    time_t t;
    struct tm *tempTime;

    t = time(NULL);
    
    /* current time is beging set  */
    tempTime = localtime(&t);
    
    /* formatting correctly to show time  */
    strftime(timeStamp, sizeof(timeStamp), "%l:%M%P, %A, %B %d, %Y",tempTime);
 
    printf("\n%s\n", timeStamp);
    
    printf("\n");

    /* now we get the file we want to write to and open it and write to it */
    FILE *timeFile;
    
    /* NOTE that we use "w+" here to take the file and set that tiem
    * if the file exsist it is overwritten with the most recent time.
    */
    timeFile = fopen("currentTime.txt", "w+");
    
    
    fprintf(timeFile, timeStamp);
    fclose(timeFile);

    /* MUTEX IS unlocked */
    pthread_mutex_unlock(&lock);
    return NULL;
}

/* This runs the entire game together
 *  gameOver is used to make the game running
 * if gameOver is 1 the game is over. If it is 0 we continue playing
 */
void *runEntireGame(){
    
    /* We find the most recent directory  */
    recentDir();
    
    /* We find the Start Room   */
    findStartRoom();
    
    /* printf("This is within main ------ Newest entry found is: %s\n", currentWorkingDir); */
    
    /* We begin playing the game and show the user the interface  */
    while(gameOver == 0) {
        printRoom();
        if (gameOver == 1){
            exit (0);
        }
        printf("\n");
        printf("WHERE TO > ");
        char userType[9];
        scanf("%s", userType, stdin);
        printf("\n");
        
        /* If the user asks for time we give that to them*/
        if(strcmp(userType, "time") == 0) {
            
            /* MUTEX IS USED */
            pthread_mutex_unlock(&lock);
            getTime();

        }
        
        /* If we get to the room that is NOT the start room we run  */
        else if(roomHasExit(userType)) {
            strcpy(currentRoom, userType);
        }
        
    }
}

/* Main to run the entire adventure game
 */

int main(){

    /* MUTEX IS USED */
    pthread_create(&(thread[0]), NULL, &runEntireGame, NULL);
    
    /* join is used as notaed by Tyler Lawson in message to entire class */
    pthread_join(thread[0], NULL);
    pthread_mutex_destroy(&lock);
}
