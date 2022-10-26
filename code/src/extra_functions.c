#include <extra_functions.h>
#include "link_layer.h"

extern int *fd;
extern struct termios oldtio;

void set_serial_port(char *port, int baudRate){

    printf(" \n");
    //printf("ENTERING SET SERIAL PORT \n");

    //printf("PORTA: %s\n FD%d\n baudarate%d\n", port,fd,baudRate );
    
    fd = open(port, O_RDWR | O_NOCTTY);
    //printf("FDDDDDD=%d",fd);

    if (fd < 0)
    {
        perror(port);
        exit(-1);
    }

  
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;  /////DUVIDAAAAAAAAAAA PQ TEM DEFINEE?????????????
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 1;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("PORT IS OPEN\n\n");
}

int sending_reading(LinkLayer connectionParameters){
    if(connectionParameters.role==LlTx){
        llwrite("ola",4);
    }

    if(connectionParameters.role==LlRx){
        llread("ola");
    }

}

int state_machine(unsigned char *double_word,int size,LinkLayerRole role){
    printf("\nENTROU NA MAQUINA DE ESTADOS\n");
    int state=0;
    
    for(int i=0; i<size;i++){
        switch(state){
            case 0:
                //printf("state 0\n");
                if(double_word[i]==FLAG){
                    state=1;
                    break;
                }
                else{
                    state=0;
                    break;
                }
                break;

            case 1:
                if(double_word[i]==A ){
                
                    state=2;
                    break;
                }
                else{
                    state=0;
                }
                break;
                

            case 2:
                //printf("state 2\n");
                if(role==LlTx ){
                    if(double_word[i]=C ){
                    state=3;
                    break;
                    }
                }

                if(role==LlRx ){
                    if(double_word[i]=U){
                    state=3;
                    break;
                    }
                    
                }

                else{
                    state=0;
                    break;
                }
                break;

            case 3:
                //printf("state 3\n");
                if(double_word[i]==BCC){
                    state=4;
                    break;
                }
                else{
                    state=0;
                    break;
                }
                break;

            case 4:
                //printf("state 4\n");
                if(double_word[i]==FLAG ){
                    state=5;
                    break;
                }
        }
    }

    printf("CURRENT STATE ON STATE MACHINE: %d\n", state);

    if(state!=5){
        printf("\nSTATE MACHINE ERROR -1\n");
        return -1;
    }

    printf("STATE MACHINE SEQUENE ACCEPTED\n");
    return 0;
}


int state_machineRR(unsigned char *double_word,int size){
    printf("\nENTROU NA MAQUINA DE ESTADOS\n");
    int state=0;
    for(int i=0; i<size;i++){
        switch(state){
            case 0:
                //printf("state 0\n");
                if(double_word[i]==FLAG){
                    state=1;
                    break;
                }
                else{
                    state=0;
                    break;
                }
                break;

            case 1:
                if(double_word[i]==A ){
                
                    state=2;
                    break;
                }
                else{
                    state=0;
                }
                break;
                

            case 2:
                //printf("state 2\n");
                if(double_word[i]==RR1 || double_word[i]==RR0 ){
                state=3;
                break;
                }
                
                else{
                    state=0;
                    break;
                }
                break;

            case 3:
                if(double_word[i]==0x00){
                    state=4;
                    break;
                }
                else{
                    state=0;
                    break;
                }
                break;

            case 4:
                //printf("state 4\n");
                if(double_word[i]==FLAG ){
                    state=5;
                    break;
                }
        }
    }

    printf("CURRENT STATE ON STATE MACHINE: %d\n", state);

    if(state!=5){
        printf("\nSTATE MACHINE ERROR -1\n");
        return -1;
    }

    printf("STATE MACHINE SEQUENE ACCEPTED\n");
    return 0;

}


int state_machine_close(unsigned char *double_word,int size,LinkLayerRole role){
    printf("\nENTROU NA MAQUINA DE ESTADOS\n");
    int state=0;
    for(int i=0; i<size;i++){
        switch(state){
            case 0:
                //printf("state 0\n");
                if(double_word[i]==FLAG){
                    state=1;
                    break;
                }
                else{
                    state=0;
                    break;
                }
                break;

            case 1:
                if(double_word[i]==A ){
                
                    state=2;
                    break;
                }
                else{
                    state=0;
                }
                break;
                

            case 2:
                //printf("state 2\n");
                if(role==LlTx ){
                    if(double_word[i]=DISC ){
                    state=3;
                    break;
                    }
                }

                if(role==LlRx ){
                    if(double_word[i]=DISC){
                    state=3;
                    break;
                    }
                    if(double_word[i]=UA_EMISSOR){
                    state=3;
                    break;
                    }
                }

                else{
                    state=0;
                    break;
                }
                break;

            case 3:
                //printf("state 3\n");
                if(double_word[i]==COMANDO_EMMISSOR_A^DISC){
                    state=4;
                    break;
                }
                if(double_word[i]==0x00){
                    state=4;
                    break;
                }
                else{
                    state=0;
                    break;
                }
                break;

            case 4:
                //printf("state 4\n");
                if(double_word[i]==FLAG ){
                    state=5;
                    break;
                }
        }
    }

    printf("CURRENT STATE ON STATE MACHINE: %d\n", state);

    if(state!=5){
        printf("\nSTATE MACHINE ERROR -1\n");
        return -1;
    }

    printf("STATE MACHINE SEQUENE ACCEPTED\n");
    return 0;

}


int close_serial_port(){

	if ( tcsetattr(fd,TCSANOW, &oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);

    printf("\nCLOSING SERIAL PORT\n");
    return 0;
}



int info_state_machine(unsigned char *double_word,int size){
    printf("\nENTROU NA MAQUINA DE ESTADOS\n");
    int state=0;
    for(int i=0; i<size;i++){
        switch(state){
            case 0:
                //printf("state 0\n");
                if(double_word[i]==FLAG){
                    printf("BYTE: %x\n", double_word[i]);
                    printf("state: %d\n",state);
                    state=1;
                    break;
                }
                else{
                    state=0;
                    break;
                }
                break;

            case 1:
                if(double_word[i]==0x03 ){
                      printf("BYTE: %x\n", double_word[i]);
                         printf("state: %d\n",state);
                    state=2;
                    break;
                }
                else{
                    state=0;
                }
                break;
                

            case 2:
                printf("state 2\n");
              
                if(double_word[i]==0x00 ){
                      printf("BYTE: %x\n", double_word[i]);
                         printf("state: %d\n",state);
                state=3;
                break;
                }
                

        
                else{
                    state=0;
                    break;
                }
                break;

            case 3:
                //printf("state 3\n");
                if(double_word[i]==0x00){
                      printf("BYTE: %x\n", double_word[i]);
                         printf("state: %d\n",state);
                    state=4;
                    break;
                }
               
                else{
                    state=0;
                    break;
                }
                break;

            case 4:
                //printf("state 4\n");
                    printf("BYTE: %x\n", double_word[i]);
                    printf("state: %d\n",state);
                    state=5;
                    break;
            case  5:
                if(double_word[i]==FLAG){
                        printf("BYTE: %x\n", double_word[i]);
                        printf("state: %d\n",state);
                        state=6;
                        break;
                    }

                else{

                    state=0;
                    break;
                }
                break;

                
        }
    }

    printf("CURRENT STATE ON STATE MACHINE: %d\n", state);

    if(state!=6){
        printf("\nSTATE MACHINE ERROR -1\n");
        return -1;
    }

    printf("STATE MACHINE SEQUENE ACCEPTED\n");
    return 0;

}

