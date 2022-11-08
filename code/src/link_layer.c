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
#include <extra_functions.h>

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
#define TYPE_SIZE 0x00
#define CONTROL_START 0x02

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

int fd;
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
                int res=write(fd,frame_SET,5);
                sleep(1);
                printf("RESPOSTA DO WRITE %d\n",res);
                alarm(connectionParameters.timeout); // 
                alarm_enabled=TRUE;
            }

            unsigned char received_message[5];  //full received trama from receiver
            unsigned char elem;

            int frame_index=0;

            //int flag=1;
            printf("TRYING TO READ...\n\n");
            while(frame_index<5){
                //printf("AQUI1\n");
                int res=read(fd,&elem,1);
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

            if(state_machine(received_message,5,connectionParameters.role)==0){
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
            int res= read(fd,&elem,1);
            printf("RECEIVED SET BYTE->%x %d\n", elem, res);

            received_message[frame_index]=elem;
            frame_index++;
        }

        int ans = state_machine(received_message,5,connectionParameters.role);
        if(ans == 0){
            printf("NO MISTAKES FOUND BY STATE MACHINE\n");    
            printf("SENDING ANWERS TO TRANSMITER\n");
            write(fd,frame_UA,5);
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

            //int flag=1;
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

            if(state_machine_close(received_message,5,connectionParameters2.role)==0){
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

        int ans = state_machine_close(received_message,5,connectionParameters2.role);
        if(ans == 0){
            printf("NO MISTAKES FOUND BY STATE MACHINE\n");    
            printf("SENDING ANWERS TO TRANSMITER\n");
            write(fd,frame_DISC,5);
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


        ans = state_machine_close(received_message,5,connectionParameters2.role);
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




int llwrite(const unsigned char *buf, int bufSize){
    printf("\n");
    printf("///////////////////////////   LLWIRTE    /////////////////////////\n");
    printf("\n");


    printf("READING FILE\n\n");

    int c=0;
    int contador_de_info_enviada=0;

    int x=0;
    while(c!=457){

        int data_size_each_packet = bufSize/457;
        printf("DATA PACKET SIZE INFORMATION WITHOUT STUFFING-> %d\n", data_size_each_packet);


        unsigned char data_to_send_each_interection[data_size_each_packet];

        for(int i=0 ;i< data_size_each_packet;i++){
            data_to_send_each_interection[i]=buf[contador_de_info_enviada];
            printf("BYTES NOT STUFFED:  %x INDEX: %d\n",data_to_send_each_interection[i],x);
            x+=1;
            if(data_to_send_each_interection[i]==  0x7e){
                printf("\nAQUIIIIIIIIIIIIIIII FAZ STUFFING!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            }
            contador_de_info_enviada++;
        }

        printf("\n\nESTADO do contador GLOBAL %d\n",contador_de_info_enviada);


        unsigned char control_packet[10];
        unsigned char data_packet[2000];

        data_packet[0]=FLAG;
        data_packet[1]=A;
        data_packet[2]=0x00;
        data_packet[3]=BCC;

        printf("AQUI1\n");
        


        unsigned char bcc2 = 0x00;      //criou bbc2 para a primeira trama de informacao
        for(int i=0; i < data_size_each_packet; i++){
            bcc2 ^=data_to_send_each_interection[i];
        }


        printf("BBC2 CURRENT: %x \n",bcc2);


        int j=0;
        for(int i=0;i<data_size_each_packet;i++){
            if(data_to_send_each_interection[i] ==  0x7e){
                data_packet[j+4] = 0x7d;
                data_packet[j+5] = 0x5e;
                j+=2;
                
                
            }

            else if(data_to_send_each_interection[i] == 0x7d){
                data_packet[j+4] = 0x7d;
                data_packet[j+5]= 0x5d;
                j+=2;
                
        
            }

            else{
                data_packet[j+4] = data_to_send_each_interection[i];
                j++;
            }  
        }



        data_packet[j+4]=bcc2;
        data_packet[j+5]=FLAG;

        /*
        for(int i=0 ; i<j+6 ;i++){
            printf("BYTES STUFFED:  %x INDEX: %d\n",data_packet[i],i);
        }
        */

        printf("CURRENT J VALUE: %d\n", j+6);

        control_packet[0] = CONTROL_START;      // INDICA INICO DO PACOTE DE CONTROLO
        control_packet[1] = TYPE_SIZE;          //INDICA QUE VOU MANDAR UM TAMANHO
        control_packet[2] = j+6;
        printf("AQUI4\n");


        int res;
        //res = write(fd,control_packet,3);
        //res = write(fd,data_packet,j+6);

        (void)signal(SIGALRM,alarmHandler);
        (void)siginterrupt(SIGALRM,TRUE);  //system call interrupt by alarm isnt restarted

        alarm_enabled=FALSE;
        timeout_count=0;


        //////////////////////////////////////////////////////////////////
        while(timeout_count<connectionParameters2.nRetransmissions){
            if(alarm_enabled == FALSE){
                res = write(fd,control_packet,3);
                res = write(fd,data_packet,j+6);

                printf("RESPOSTA DO WRITE %d\n",res);
                alarm(connectionParameters2.timeout); // 
                alarm_enabled=TRUE;
            }

            unsigned char received_message[5];  //full received trama from receiver
            unsigned char elem;

            int frame_index=0;

            printf("TRYING TO READ...\n\n");
            while(frame_index<5){
                //printf("AQUI1\n");
                res = read(fd,&elem,1);
                if(res==-1){
                    break;
                }
                

                //printf("AQUI2\n");
                printf("RECEIVED RR BYTE-> %x %d\n", elem, res);
                //printf("AQUI3\n");
                received_message[frame_index]=elem;
                frame_index++;
            }

            //printf("AQUI4\n");
         

            if(state_machineRR(received_message,5)==0){
                break;
            }
            //printf("AQUI5\n");
        }


        c++;
    }

    //fclose(ptr);// fecha ficheiro
    return 0;
}


////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{

    //unsigned char frame_RR1[5]= {FLAG,COMANDO_EMMISSOR_A,RR1,BCC_EMISSOR,FLAG};
    unsigned char frame_RR0[5]= {FLAG,COMANDO_EMMISSOR_A,RR0,BCC_EMISSOR,FLAG};

    printf("\n");
    printf("///////////////////////////   LLREAD   /////////////////////////\n");
    printf("\n");

    
    /*
    FILE *file;
    if ((file = fopen("new.gif", "wb")) == NULL) {              //criei/abri ficheiro onde vou colocar copia do penguim
        printf("Not possible to create file!\n");
        return -1;
    }

    printf("CREATING/UPDATING NEW.GIF FILE\n");

    */
    int contador_final=0;

    int count=0;
    while(count!=457){

        unsigned char control[3];
        
        read(fd,&control,3);  //ler um bit
        int size = control[2];
        printf("SIZE: %d\n", size);

        int contador=0;

        int state = 0 ;
        int flag1 = 0;  //encontrar os 7D coloca a flag a 1
        int flagD = 0;
        int flagE = 0;
        int aux = 0;

        int count_distuff=0;

        printf("\n\n\nSENDING NEW TRAMA!!!!!!!!\n\n\n");
        printf("SIZEEEEEE-> %d\n",size);
        unsigned char bcc2 = 0x00;      //criou bbc2 para a primeira trama de informacao
     
        while(contador!=size){

            if(contador==30){
                count_distuff=0;
            }
            
            printf("CONTADOR-> %d\n",contador);
            unsigned char byte;
            read(fd,&byte,1);  //ler um bit
            printf("BYTE-> %x\n",byte);

     
            
            //printf("STATE: %d\n", state);

            states(byte,&state,&flag1,&flagD,&flagE,&aux );

            //printf("STATE: %d\n", state);

            if((state==5 && flag1== 0 && flagD==0 && flagE==0 && contador<size-2  )|| (state==5 && flag1== 0  && flagE==1  && contador<size-2 )|| (state==5 && flag1== 0  && flagD==1 && contador<size-2 )){
                printf("ESTE BYTE PODE SER ESCRITO\n");
                //fwrite(&byte, 1, 1, file);            ////////////586
                packet[contador_final]=byte;
                bcc2 ^= byte;
                printf("CONTADOR: %d\n",contador_final);
                contador_final++;
                flag1 = 0;  
                flagD = 0;
                flagE = 0;
                count_distuff=0;

            }

            if(state>=4 && flag1== 1 && flagD==0 && flagE==0){
                printf("ECONTROU O 7D MAS NAO ESCREVE NADA\n");
              
                count_distuff++;
                if(count_distuff==2){
                    flag1 = 0;  
                    flagD = 0; //10373
                    flagE = 0;
                    unsigned char value = 0x7D;
                    //write(&value, 1, 1, file);
                    //fwrite(&value, 1, 1, file);
                    packet[contador_final]=value;
                    bcc2 ^= value;
                    printf("CONTADOR: %d\n",contador_final);
                    printf("BYTE DISTUFED: %x\n",value);
                    contador_final++;
                    count_distuff=0;

                }
            }

            printf("COUNT_DISTUFF: %d\n",count_distuff);

            if(contador==28){
                count_distuff=0; //8828
                if(bcc2==byte){
                    printf("ENCONTREI O BCC2 IGUAL\n");
                }
            }

            if(state>=4 && flag1== 1 && flagD==1 && flagE==0){
                printf("ENCONTROU UM 5D->fazer distuff\n");
                unsigned char value = 0x7d;
                flag1 = 0;  
                flagD = 0;
                flagE = 0;
                //fwrite(&value, 1, 1, file);
                packet[contador_final]=value;
                bcc2 ^= value;
                printf("CONTADOR: %d\n",contador_final);
                printf("BYTE DISTUFED: %x\n",value);
                contador_final++;
                count_distuff=0;


            }


            if(state>=4 && flag1== 1 && flagD==0 && flagE==1){
                printf("ENCONTROU UM 5E>fazer distuff\n");
                unsigned char value = 0x7E;
                flag1 = 0;  
                flagD = 0;
                flagE = 0;
                //fwrite(&value, 1, 1, file);
                packet[contador_final]=value;
                bcc2 ^= value;
                printf("CONTADOR: %d\n",contador_final);
                printf("BYTE DISTUFED: %x\n",value);
                contador_final++;

            }


            printf("STATE: %d\n", state);
            printf(" \n");
   
            contador++;

            if(aux==1){
            state=5;
            printf("INCREMETOU O STATE\n");
            aux=0;
            }
        }


        write(fd,frame_RR0,5);
        printf("SENDING TRAMA TO LLWRITE, ALL BYTES WERE PASSED!\n");
        count++;
    }

    /*
    for(int i=0; i<300 ;i++){
        printf("BYTESSS PARA O PACKET: %x\n",packet[i]);
    }
    */


    return 1;
 
}










