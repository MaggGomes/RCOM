#include "datalink.h"

struct linkLayer dataLink;
int alarmFlag = 1, count = 0;
volatile int STOP=FALSE;

int llopen(int fd, int flag){

  if (flag == RECEIVER) {
		int i = 0;
		int res;
		char buf[255];
		int counterTrama = 0; // Variable used to verify if the Trama was correctly read
		char temp;
		do {
			res = read(fd,&temp,1);   // returns after 5 chars have been input
			buf[i] = temp;

			if (buf[i] == TRAMA_FLAG)
				counterTrama++;
			i++;
		} while (counterTrama < 2);

		if (verifySET(fd, buf) == 0)
			printf("SET trama successfully received!\n");

		// Creating Trama
    char* UA = (char*) malloc(5 * sizeof(char));
		createUA(UA);

		res = write(fd, UA, 5);
		printf("%d bytes written\n", res);
		printf("UA successfully written!\n");

	}
	else {
		setAlarm();

		while (count < 3 && STOP == FALSE) {
      char* SET = (char*) malloc(5 * sizeof(char));
			createSET(SET);
			write(fd, SET, 5);
			sleep(1);

			if (alarmFlag) {
				printf("Alarm activated.\n");
				alarm(3);
				alarmFlag = 0;
			}

			char* UA = (char*) malloc(5 * sizeof(char));

			while (!alarmFlag) {
				read(fd, UA, 5);
			}

			if (verifyUA(UA) == 0) {
				printf("UA received correctly.\n");
				STOP = TRUE;
			}
		}
	}

	return 0;
}

int llwrite(int fd, char * buffer, int length){

  /* if success
  return numero de caracteres escritos */

  /* if error
  return -1 */

  return 0;
}

int llread(int fd, char * buffer){

  /* if success
  return numero de caracteres lidos
  if fails
  return -1 */

  return 0;
}

int llclose(int fd, int flag){

  // TODO -  FALTA TERMINAR
  // COLOCAR CLOSE PARA O SENDER E O RECEIVER


  sleep(2);
	if ( tcsetattr(fd,TCSANOW,&dataLink.oldtio) == -1) {
		fprintf(stderr, "%s\n", "Error closing link.");
		return 1;
	}

	close(fd);
  return 0;
}

int openSerial(char* port, int type){

  int fd;
  dataLink.port = port;

  if (type == SENDER)
   fd = open(dataLink.port, OPEN_SENDER);
  else
   fd = open(dataLink.port, OPEN_RECEIVER);

	if (fd < 0) {
		perror(dataLink.port);
		exit(-1);
	}

	if (tcgetattr(fd, &dataLink.oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&dataLink.newtio, sizeof(dataLink.newtio));
	dataLink.newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	dataLink.newtio.c_iflag = IGNPAR;
	dataLink.newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	dataLink.newtio.c_lflag = 0;

	dataLink.newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
	dataLink.newtio.c_cc[VMIN] = 1; /* blocking read until 5 chars received */

	tcflush(fd, TCIOFLUSH);

	if (tcsetattr(fd, TCSANOW, &dataLink.newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	printf("New termios structure set.\n");

  return fd;
}

void createSET(char* SET)
{
    SET[0] = TRAMA_FLAG;
    SET[1] = A_SENDER;
    SET[2] = C_SET;
    SET[3] = SET[1] ^ SET[2];
    SET[4] = TRAMA_FLAG;
}

void createUA(char* UA)
{
    UA[0] = TRAMA_FLAG;
    UA[1] = A_SENDER;
    UA[2] = C_UA;
    UA[3] = UA[1] ^ UA[2];
    UA[4] = TRAMA_FLAG;
}

void createDISC(char* DISC)
{
	DISC[0] = TRAMA_FLAG;
	DISC[1] = A_SENDER;
	DISC[2] = C_DISC;
	DISC[3] = DISC[1] ^ DISC[2];
	DISC[4] = TRAMA_FLAG;
}

int verifySET(int fd, char * SET){
    if (SET[0] != TRAMA_FLAG ||
    SET[1] != A_SENDER ||
    SET[2] != C_SET ||
    SET[3] != (SET[1] ^ SET[2]) ||
    SET[4] != TRAMA_FLAG)
    return -1;

    return 0;
}

int verifyUA(char* UA)
{
    if (UA[0] != TRAMA_FLAG ||
	    UA[1] != A_SENDER ||
	    UA[2] != C_UA ||
	    UA[3] != (UA[1] ^ UA[2]) ||
	    UA[4] != TRAMA_FLAG) {
	       return -1;
    }

    return 0;
}

int verifyDISC(int fd, char * SET){

    int i = 0;
    int res;
    char buf[255];
	int counterTrama = 0; // Variable used to verify if the Trama was correctly read
    char temp;
    do {
        res = read(fd,&temp,1);   // returns after 5 chars have been input
        if (res == -1)
          perror("Failing reading DISC.");
	buf[i] = temp;

	if (buf[i] == TRAMA_FLAG)
		counterTrama++;
	i++;
    } while (counterTrama < 2);

    if (SET[0] != TRAMA_FLAG ||
    SET[1] != A_SENDER ||
    SET[2] != C_DISC ||
    SET[3] != (SET[1] ^ SET[2]) ||
    SET[4] != TRAMA_FLAG)
    return -1;

    return 0;
}

void handleAlarm() {
    printf("Read count: %d\n", count);
    alarmFlag= 1;
    count++;
}

void setAlarm() {
    (void) signal(SIGALRM, handleAlarm);
    printf("Alarm set.\n");
}
