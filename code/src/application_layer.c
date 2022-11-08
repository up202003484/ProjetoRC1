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
    
    int size;


    if(dl_layer.role==LlTx){
        FILE *ptr = fopen(filename,"rb");  // r for read, b for binary  // open ficheiro original do penguim
        if(ptr == NULL)
        {
            //falha a abrir o penguim original
            printf("Error!");   
            exit(1);             
        }

                
        struct stat st;
        stat("penguin.gif", &st);
        size = st.st_size;

        printf("TAMANHO DO FICHEIRO : %d\n", size);

        unsigned char buffer[size];  //buffer with all the file BYTES
        fread(buffer,size,1,ptr); 




      

        llwrite(buffer,size);

        printf("TAMANHO DO FICHEIRO : %d\n", size);
        fseek(ptr, 0, SEEK_END);
        // Printing position of pointer
        printf(" TAMANHO VERDADEIRO %ld", ftell(ptr));

        /*
        for(int i=0; i<size;i++){
            printf("BYTESSS PARA O FILE: %x INDEX: %d \n",buffer[i],i);

        }
        */

        fclose(ptr);// fecha ficheiro

    }

    if(dl_layer.role==LlRx){

        FILE *file;
        if ((file = fopen("new.gif", "wb")) == NULL) {              //criei/abri ficheiro onde vou colocar copia do penguim
            printf("Not possible to create file!\n");
        }

        unsigned char packet[15000];

        printf("CREATING/UPDATING NEW.GIF FILE\n");
     
        llread(packet);

        for(int i=0; i<10968;i++){
            unsigned char value = packet[i];
            //printf("BYTESSS PARA O FILE: %x INDEX: %d \n",value,i);
            fwrite(&value, 1, 1, file);
        }
    
        fclose(file);// fecha ficheiro

    }

    llclose(1);



    
}

