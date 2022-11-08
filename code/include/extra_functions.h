
#ifndef _EXTRA_FUNCTIONS_H_
#define _EXTRA_FUNCTIONS_H_

#include <link_layer.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define COMANDO_EMMISSOR_A 0x03
#define SET_EMISSOR 0x03
#define BCC_EMISSOR 0x00
#define UA_EMISSOR 0x07
#define DISC 0x0B
#define A    0x03
#define C    0x03
#define U    0x07
#define BCC  (A^C)
#define RR1  0x85
#define RR0  0x05
#define Ns   0x40
#define FALSE 0
#define TRUE 1



void set_serial_port(char *port, int baudRate);

int close_serial_port();

int state_machine_close(unsigned char *double_word,int size,LinkLayerRole role);

int state_machine(unsigned char *double_word,int size,LinkLayerRole role);

int state_machineRR(unsigned char *double_word,int size);

int info_state_machine(unsigned char *double_word,int size);

void states( unsigned char byte, int *state, int *flag1, int *flagD, int *flagE,int *aux);

#endif // _APPLICATION_LAYER_H_
