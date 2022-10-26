// Link layer protocol implementation

#include "link_layer.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

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

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

int *fd;
struct termios oldtio;

LinkLayer connectionParameters2;

#define FALSE 0
#define TRUE 1

int alarm_enabled;
int timeout_count=0;

void alarmHandler(int signal){
    alarm_enabled =FALSE;
    timeout_count++;
    printf("TIMEMOUT COUNT : %d\n",timeout_count);

}



////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////



int llopen(LinkLayer connectionParameters)
{
    connectionParameters2=connectionParameters;
    (void)signal(SIGALRM,alarmHandler);
    (void)siginterrupt(SIGALRM,TRUE);  //system call interrupt by alarm isnt restarted

    set_serial_port(connectionParameters.serialPort,connectionParameters.baudRate);

    printf("///////////////////////////////////////////////////////////\n");
    printf("ENTERING LLOPEN\n\n");

    if(connectionParameters.role==LlTx ){     ///TRANSMISSOR
    
        printf("TRANSMITOR OPENED\n\n");

        unsigned char frame_SET[5]= {FLAG,COMANDO_EMMISSOR_A,SET_EMISSOR,BCC_EMISSOR,FLAG};

        while(timeout_count<connectionParameters.nRetransmissions){
            if(alarm_enabled == FALSE){
                int res = write(fd,frame_SET,5);
                sleep(1);
                printf("RESPOSTA DO WRITE %d\n",res);
                alarm(connectionParameters.timeout); // 
                alarm_enabled=TRUE;
            }

            unsigned char received_message[5];  //full received trama from receiver
            unsigned char elem;

            int frame_index=0;

            int flag=1;
            printf("TRYING TO READ...\n\n");
            while(frame_index<5){
                //printf("AQUI1\n");
                int res = read(fd,&elem,1);
                if(res==-1){
                    break;
                }
                

                //printf("AQUI2\n");
                printf("RECEIVED UA BYTE-> %x %d\n", elem, res);
                //printf("AQUI3\n");
                received_message[frame_index]=elem;
                frame_index++;
            }

            //printf("AQUI4\n");
         
            sleep(1);

            if(state_machine(&received_message,5,connectionParameters.role)==0){
                break;
            }
            //printf("AQUI5\n");
        }

    }
    
    if(connectionParameters.role==LlRx){     ///TRANSMISSOR
        printf("RECEIVER OPENED\n");

        // CREATES SET AND UA TREAMA FOR LLOPEN
        unsigned char frame_UA[5]= {FLAG,COMANDO_EMMISSOR_A,UA_EMISSOR,BCC_EMISSOR,FLAG};

        printf("RECIVING MESSAGE\n");

        // Wait for UA signal.
        printf("RECIVING MESSAGE\n");

        unsigned char received_message[5];  //full received trama from receiver
        unsigned char elem;

        int frame_index=0;
        while(frame_index<5){
            int res = read(fd,&elem,1);
            printf("RECEIVED SET BYTE->%x %d\n", elem, res);

            received_message[frame_index]=elem;
            frame_index++;
        }

        int ans = state_machine(&received_message,5,connectionParameters.role);
        if(ans == 0){
            printf("NO MISTAKES FOUND BY STATE MACHINE\n");    
            printf("SENDING ANWERS TO TRANSMITER\n");
            int res = write(fd,frame_UA,5);
            sleep(1);
        }

        if(ans==1){
            printf("ERROR RECEIVING PACKAGES!\n");
        }
    }   

    return 1;
}




////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////



int llclose(int showStatistic){
    printf("\n\n\n///////////////////////////////////////////////////////////\n");
    printf("ENTERED LLCOSE\n");

    (void)signal(SIGALRM,alarmHandler);
    (void)siginterrupt(SIGALRM,TRUE);  //system call interrupt by alarm isnt restarted

  
    unsigned char frame_DISC[5]= {FLAG,COMANDO_EMMISSOR_A,DISC,COMANDO_EMMISSOR_A^DISC,FLAG};
    unsigned char frame_UA[5]= {FLAG,COMANDO_EMMISSOR_A,UA_EMISSOR,BCC_EMISSOR,FLAG};

    alarm_enabled=FALSE;
    timeout_count=0;



    if(connectionParameters2.role==LlTx ){     ///TRANSMISSOR
    
        printf("TRANSMITOR OPENED\n\n");
        printf("SENDING DISC TRAMA\n");

        
        while(timeout_count<connectionParameters2.nRetransmissions){
            if(alarm_enabled == FALSE){
                int res = write(fd,frame_DISC,5);
                sleep(1);
                printf("RESPOSTA DO WRITE %d\n",res);
                alarm(connectionParameters2.timeout); // 
                alarm_enabled=TRUE;
            }

            unsigned char received_message[5];  //full received trama from receiver
            unsigned char elem;

            int frame_index=0;

            int flag=1;
            printf("TRYING TO READ...\n\n");
            while(frame_index<5){
                //printf("AQUI1\n");
                int res = read(fd,&elem,1);
                if(res==-1){
                    break;
                }
                

                //printf("AQUI2\n");
                printf("RECEIVED DISC BYTE-> %x %d\n", elem, res);
                //printf("AQUI3\n");
                received_message[frame_index]=elem;
                frame_index++;
            }

            //printf("AQUI4\n");
         
            sleep(1);

            if(state_machine_close(&received_message,5,connectionParameters2.role)==0){
                break;
            }
            //printf("AQUI5\n");
        }

        sleep(1);

        printf("TRANSMITOR OPENED\n\n");
        printf("SENDING UA TRAMA\n");

        int res = write(fd,frame_UA,5);
        sleep(1);
        printf("RESPOSTA DO WRITE %d\n",res);
        if(res==-1){
            printf("ERROR SENDING UA TRAMA\n");
        }
        



    }
    
    if(connectionParameters2.role==LlRx){   

        printf("RECEIVER OPENED\n");
        // Wait for UA signal.
        printf("RECIVING MESSAGE\n");

        unsigned char received_message[5];  //full received trama from receiver
        unsigned char elem;

        int frame_index=0;
        while(frame_index<5){
            int res = read(fd,&elem,1);
            printf("RECEIVED DISC BYTE->%x %d\n", elem, res);

            received_message[frame_index]=elem;
            frame_index++;
        }

        int ans = state_machine_close(&received_message,5,connectionParameters2.role);
        if(ans == 0){
            printf("NO MISTAKES FOUND BY STATE MACHINE\n");    
            printf("SENDING ANWERS TO TRANSMITER\n");
            int res = write(fd,frame_DISC,5);
            sleep(1);
        }

        if(ans==1){
            printf("ERROR RECEIVING PACKAGES!\n");
            return -1;
        }

        sleep(1);

        printf("RECIVING MESSAGE\n\n");


        frame_index=0;
        while(frame_index<5){
            int res = read(fd,&elem,1);
            printf("RECEIVED UA BYTE->%x %d\n", elem, res);

            received_message[frame_index]=elem;
            frame_index++;
        }


        ans = state_machine_close(&received_message,5,connectionParameters2.role);
        if(ans == 0){
            printf("NO MISTAKES FOUND BY STATE MACHINE\n");    
         
        }

        if(ans==1){
            printf("ERROR RECEIVING PACKAGES!\n");
            return -1;
        }

  
    }
 
    close_serial_port();
   
    return 0;        
}



unsigned char* byte_stuffing(  unsigned char* msg[], int length){
    printf("SIZE OF dentro DO BUFFING: %x\n",msg[0]);

	unsigned char* str;
	int i=0;
	int j=0;
	unsigned int array_length = length;
	str = (unsigned char *) malloc(array_length);
	for(; i < length; i++, j++){

		if(j >= array_length){
			array_length = array_length+(array_length/2);
			str = (unsigned char*) realloc(str, array_length);

		}
		if(msg[i] ==  0x7e){
			str[j] = 0x7d;
			str[j+1] = 0x5e;
			j++;
		}
		else if(msg[i] == 0x7d){
			str[j] = 0x7d;
			str[j+1]= 0x5d;
			j++;
		}
		else{
			str[j] = msg[i];
		}
	}
	length = j;
	//free(msg);

	return str;
}




int llwrite(const unsigned char *buf, int bufSize){

    printf("\n");
    printf("///////////////////////////   LLWIRTE    /////////////////////////\n");
    printf("\n");

    (void)signal(SIGALRM,alarmHandler);
    (void)siginterrupt(SIGALRM,TRUE);  //system call interrupt by alarm isnt restarted

    printf("READING FILE\n\n");
    unsigned char trama[100];      //10000 bytes de tamanho;

    FILE *ptr = fopen("penguin.gif","rb");  // r for read, b for binary  // open ficheiro original do penguim
    if(ptr == NULL)
    {
        //falha a abrir o penguim original
        printf("Error!");   
        exit(1);             
    }

    struct stat st;
    stat("penguin.gif", &st);
    int size = st.st_size;
    printf("TAMANHO: %d\n", size);


    
    unsigned char buffer[94];  //////////change 100
    int count=0;

    trama[0]=FLAG;
    trama[1]=A;
    trama[2]=0x00;
    trama[3]=BCC;
    printf("BUILDING TRAMA TO WITH FILE INFO\n");

    (void)signal(SIGALRM,alarmHandler);
    (void)siginterrupt(SIGALRM,TRUE);  //system call interrupt by alarm isnt restarted

    int contador=0;
    while(TRUE){
        
        unsigned char bcc2 = 0x00;
        //printf("COUNT->\n %d ",count);
        fread(buffer,sizeof(buffer),1,ptr); // reads 94 bytes to our buffer

        ///////////////////////////////////////////////////////////////////////

        int length=94;
        unsigned char* str;
        int i=0;
        int j=0;
        unsigned int array_length = length;
        str = (unsigned char *) malloc(array_length);
        for(; i < length; i++, j++){

            if(j >= array_length){
                array_length = array_length+(array_length/2);
                str = (unsigned char*) realloc(str, array_length);

            }
            if(buffer[i] ==  0x7e){
                str[j] = 0x7d;
                str[j+1] = 0x5e;
                j++;
            }
            else if(buffer[i] == 0x7d){
                str[j] = 0x7d;
                str[j+1]= 0x5d;
                j++;
            }
            else{
                str[j] = buffer[i];
            }
        }
        length = j;

        unsigned char final[length+6];  //////////change 100
        final[0]=FLAG;
        final[1]=A;
        final[2]=0x00;
        final[3]=BCC;

        for(int i=0;i<length;i++){
            printf("BYTE STUFFED-> %x INDEX: %d \n",str[i],i );  //str é informacao stuffed
            final[i+4]=str[i];      // final e a ttrama que vou mandar
        }
        unsigned char bcc2_2 = 0x00;
        for(int i=0; i<94; i++){
		    bcc2_2 ^=buffer[i];
	    }
        printf("BCC2:  %x \n",bcc2_2);
        printf("FLAG:  %x \n",FLAG);
        printf("LENGTH: %d\n",length);

        final[length+4]=bcc2_2;
        final[length+5]=FLAG;

        printf(" 7777777777777777777777777777\n");
        for(int i=0;i<length+6;i++){
            printf("BYTE TRAMA FINAL-> %x INDEX: %d \n",final[i],i );  //str é informacao stuffed
        }



        ///////////////////////////////////////////////////////////////////////

        for(int i=0; i<94; i++){
		    bcc2 ^=buffer[i];
	    }

        
        printf("CREATING BCC2->%x\n" ,bcc2);
        
        for(int i=0;i<94;i++){
            trama[i+4]=buffer[i];
            
        }

        trama[98]=bcc2;
        trama[99]=FLAG;


        alarm_enabled=FALSE;
        timeout_count=0;

        while(timeout_count<3){

            if(alarm_enabled == FALSE){
                printf("SENDING TRAMA TO READER\n");
                int res = write(fd,trama,100);  /////mudar aquiiiiiiii
                //sleep(1);
                printf("RESPOSTA DO WRITE %d\n",res);
                alarm(3); // 
                alarm_enabled=TRUE;
            }

            unsigned char received_message[5];  //full received trama from receiver
            unsigned char elem;

            int frame_index=0;

            int flag=1;
            printf("TRYING TO READ...\n\n");
            while(frame_index<5){
                //printf("AQUI1\n");
                int res = read(fd,&elem,1);
                if(res==-1){
                    break;
            }   
    
            printf("RECEIVED RR1 BYTE-> %x %d\n", elem, res);

            received_message[frame_index]=elem;
            frame_index++;
            
            }

            if(state_machineRR(&received_message,5)==0){
                break;
            }

            if(state_machineRR(&received_message,5)==-1){
                printf("ERROR IN STATE MACHINE , SENDING AND READING AGAIN READING AGAIN!\n");
            
            }
      
          
           
        }

        //sleep(1);

        count+=94;
        if(count>10900){
            break;
        }
        contador++;
        printf("CONTADOR DE INTERACOES: %d\n\n",contador);
    }

 
    fclose(ptr);// fecha ficheiro

}


////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{

    unsigned char frame_RR1[5]= {FLAG,COMANDO_EMMISSOR_A,RR1,BCC_EMISSOR,FLAG};
    unsigned char frame_RR0[5]= {FLAG,COMANDO_EMMISSOR_A,RR0,BCC_EMISSOR,FLAG};

    printf("\n");
    printf("///////////////////////////   LLREAD   /////////////////////////\n");
    printf("\n");

    
    
    FILE *file;
    if ((file = fopen("new.gif", "wb")) == NULL) {              //criei/abri ficheiro onde vou colocar copia do penguim
        printf("Not possible to create file!\n");
        return -1;
    }

    printf("CREATING/UPDATING NEW.GIF FILE\n");


 
    int count=0;


    while(TRUE){
        
        //////////////////////////////////////7
        int cont=0;
        unsigned char trama[200];
        unsigned char buffer2[94];

        unsigned char buffer_maquina_de_estados[6];
        int count_maquinadeestados=0;

        int cont_aux=0;

        unsigned char final[200];
        printf("///////////////////////////////////////////////////\n\n");
        while(TRUE){

            unsigned char bit;
            int res = read(fd,&bit,1);  //ler um bit
          
            printf("BYTE QUE SAI DO READ: %x\n",bit);
            trama[cont]=bit;             //coloco esste bit na trama
            final[cont]=bit;
            printf("BYTE QUE FICA NA TRAMA COM INDEX CORRETO: %x\n", trama[cont]);

       
            printf("CURRENT COUNT: %d \n", cont);
            if(cont<98 && cont>3){          // se nao forem flags colocamos no buffer2
                printf("QUERO ESTE BIT\n\n");
                buffer2[cont_aux]=bit;  //buffer com informacao
                cont_aux++;

            }
            else{
                buffer_maquina_de_estados[count_maquinadeestados]=bit;      // se forem flags colocamos no buffer da maquina de estados
                count_maquinadeestados++;
                printf("NAO QUERO ESTE BIT\n\n");

            }

       
            cont++;
            if(cont>99 && bit == 0x7e){    // se encontrar a FLAG DA BRAKE   //tenho no final a trama todo ainda por da disstuffe a informcao

                /////////////////////////////////////////////////////////////////
                break;
            }

        }


        ////////////////////////////////////////////
        printf("TERMINEI DE CONTRUIR A TRAMA\n");

        for(int i=0;i<94;i++){
            //buffer2[i]=trama[4+i];
            printf("BYTE-> %x\n",buffer2[i]);
        }

        for(int i=0;i<6;i++){
            //buffer2[i]=trama[4+i];
            printf("BYTE da MQUINA DE ESTADOS-> %x\n",buffer_maquina_de_estados[i]);
        }
        
        /*
        int x = info_state_machine(buffer_maquina_de_estados,6);
        if(x==-1){
            printf("ERROR IN STATE MACHINE\n");
            //break;
        }
        */

        fwrite(buffer2, 94, 1, file);


        //if(res!=-1){
        printf("SENDING TO ANSWER TO EMISSOR\n");
        int response = write(fd,frame_RR1,5);
            //sleep(1);
        //}

        /*
    
        printf("BUFFER QUE VOU USAR PARA CONTRUIR O NOVO GIF");
        for(int i=0;i<100;i++){
            //buffer2[i]=trama[4+i];
            printf("BYTE-> %x\n",trama[i]);
        }
        printf("WRITING TO NEW FILE\n\n");
        fwrite(buffer2, 94, 1, file);
        //sleep(1);
        */

        count+=94;
        if(count>10900){
            break;
        }
    }

    fclose(file); 
    return 0;
}






