/****************************************************************************************/
/*                                                                                      */
/* serial.c                                                                             */
/* Code to open /proc/cpuinfo on a Raspberry Pi, read it, find the serial number        */
/* and print it out.                                                                    */
/* Inputs: none                                                                         */
/* Assumptions: serial code is within the first 1000 bytes of cpuinfo                   */
/*              serial code is 16 bytes long                                            */
/*              serial code starts after a space after a colon on line starting         */
/*              with the word "Serial"                                                  */
/* Output: if no errors, the serial number of the Raspberry Pi                          */
/* Change history:                                                                      */
/*    Initial version:                                        16 September 2018; PASW   */
/*    for() --> while(); added comments:                      17 September 2018; PASW   */
/*                                                                                      */

#include <stdio.h>

#define CPUINFO "/proc/cpuinfo"
#define BUFSZ 1000
#define SERIAL_NUM_SZ 16

int main(const int argc, const char* const argv[]) {
  FILE* fp = fopen(CPUINFO,"r");
  if (!fp) {
    fprintf(stderr,"%s Error: Unable to open %s; exiting\n", argv[0], CPUINFO);
    return -1;
  }

  char buf[BUFSZ];
  int sz = fread(buf, 1, BUFSZ, fp);
  printf("The size of sz is: %d\n", sz);
  if (sz <= 0) {
    fprintf(stderr,"%s Error: Unable to read from %s; exiting\n", argv[0], CPUINFO);
    return -1;
  }

  /* The Serial number is in cpuinfo in the form
     "Serial      : xxxxxxxxxxxxxxxx"
     The serial number should be 16 bytes
  */
  /* Find "Serial" */
  int serialStartsAt = -1;
  int done = 0;
  int i = 0;
  while (!done && i < sz-5) {           /* -5 because "Serial" has to fit in the buffer */
    if (buf[i] == 'S') {                /* We might have it; check the rest             */
      if ((buf[i+1] == 'e') &&
	  (buf[i+2] == 'r') &&
	  (buf[i+3] == 'i') &&
	  (buf[i+4] == 'a') &&
	  (buf[i+5] == 'l')) {
	serialStartsAt = i;
	done = 1;
      }
    }
    ++i;
  }

  if (serialStartsAt < 0) {
    fprintf(stderr,"%s Error: Unable to find \"Serial\" in %s; exiting\n", argv[0], CPUINFO);
    return -1;
  }

  /* Find the colon */
  int serialNumStartsAt = -1;
  done = 0;
  i = serialStartsAt;
  while (!done && i < sz) {
    if (buf[i] == ':') {
      serialNumStartsAt = i+2;
      done = 1;
    }
    ++i;
  } 

  if (serialNumStartsAt < 0) {
    fprintf(stderr,"%s Error: Unable to find colon after \"Serial\" in %s; exiting\n", argv[0], CPUINFO);
    return -1;
  }
  char serialNumber[SERIAL_NUM_SZ+1];
  serialNumber[SERIAL_NUM_SZ] = 0;
  i = serialNumStartsAt;
  int j = 0;
  while (i < sz && j < SERIAL_NUM_SZ) {
    serialNumber[j] = buf[i];
    ++i;
    ++j;
  }

  printf("The serial number of this Raspberry Pi is: %s\n", serialNumber);
  return 0;

}
