// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <extra_functions.h>


void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{

    LinkLayer dl_layer;
    printf("BUILDING CONTRUCTOR in APPLICATION LAYER\n");

    
   

    if(strcmp(role,"tx")){
        dl_layer.role=LlRx;
    }

    if(strcmp(role,"rx")){
        dl_layer.role= LlTx;
    }

    strcpy(dl_layer.serialPort, serialPort); 
    dl_layer.baudRate=baudRate;
    dl_layer.nRetransmissions= 3;
    dl_layer.timeout=timeout;

    
    
    
    printf("Starting link-layer protocol application\n"
            "  - Serial port: %s\n"
            "  - Baudrate: %d\n"
            "  - Number of tries: %d\n"
            "  - Timeout: %d\n",
            dl_layer.serialPort,
            dl_layer.baudRate,
            dl_layer.nRetransmissions,
            dl_layer.timeout
            );



  
    llopen(dl_layer);
    sleep(3);
    printf("SLEEP 3 SECOND\n\n");
    sending_reading(dl_layer);
    sleep(1);
    printf("SLEEP 3 SECOND\n\n");
    llclose(1);

    

    
}

