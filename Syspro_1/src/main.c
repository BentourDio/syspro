#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "app.h"
#include "functions.h"
//./vaccineMonitor -c citizenRecordsFile –b bloomSize

int main(int argc, char *argv[]) {
  FILE *fp;
  int bloomSize;
  srand(time(NULL));

  if (argc != 5) {
    printf("[Error] Wrong arguments Given\nExiting\n");
    return -1;
  }

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-c") == 0) {
      if (access(argv[i + 1], F_OK) != 0) {
        printf("File doesn't exitst\nExiting\n");
        exit(EXIT_FAILURE);
      }

      fp = fopen(argv[i + 1], "r");

      if (fp == NULL)
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[i], "-b") == 0) {
      bloomSize = atoi(argv[i + 1]);

      if(bloomSize == 666){
        printf("Number of the Beast\n");
      }
    }
  }

  
  Create_Htable(fp);
  Load_File(fp);
  Application();
  fclose(fp);
  Purge();

  printf("EXITING MAIN\n");

  return 0;
}