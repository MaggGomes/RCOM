#ifndef DATALINK_H
#define DATALINK_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include "utilities.h"

#define OPEN_SENDER O_RDWR | O_NOCTTY | O_NONBLOCK
#define OPEN_RECEIVER O_RDWR | O_NOCTTY

struct linkLayer {
  char* port; /*Device /dev/ttySx, x = 0, 1*/
  unsigned int numTransmissions; /*Number of trys if fails occurs*/
  char frame[MAX_SIZE]; /*Frame*/
  struct termios oldtio;
  struct termios newtio;
};


int llopen(char* port, int flag);
int llwrite(int fd, char * buffer, int length);
int llread(int fd, char * buffer);

// TODO - USAR FLAG AQUI PARA SABER SE É O SENDER OU RECEIVER?
int llclose(int fd);
int openSerial(char* port, int type);
void createSET(char* SET);
void createUA(char* UA);;
void createDISC(char* DISC);
int verifySET(int fd, char * SET);
int verifyUA(char* UA);
int verifyDISC(int fd, char * SET);
void handleAlarm();
void setAlarm();


#endif // DATALINK_H