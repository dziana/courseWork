#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#define MESSAGE_SIZE 50
#define SAMPLE_LENGTH 16384
void writeIntoFile (float * array);
void generatePulse (float sampleRate, float waveFrequency, float waveVolume);
void generateSine(float sampleRate, float waveFrequency, float waveVolume);
void generateSawtooth(float sampleRate, float waveFrequency, float waveVolume);
void generateTriangle(float sampleRate, float waveFrequency, float waveVolume);
void generateSquare (float sampleRate, float waveFrequency, float waveVolume);
void callFunction(char * buf, int sock);
void *connectionHandler(void * socket_desc);
int fileNumber = 0;
int main()
{
    int sock, listener;
    struct sockaddr_in addr;


    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3422);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 3);
    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }
        pthread_t thread_id;
        puts("connection accepted");

        if( pthread_create( &thread_id , NULL, connectionHandler, (void*) &sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    }
}
void *connectionHandler(void * socket_desc)
{
    int sock = *(int*)socket_desc;
    char buf[MESSAGE_SIZE];
    int bytes_read;
    while(1)
    {
        bytes_read = recv(sock, buf, MESSAGE_SIZE, 0);
        if(bytes_read <= 0) break;
        printf("%s",buf);
        printf("\n");
        callFunction(buf, sock);
    }
}
void callFunction(char * buf, int sock)
{
    float sampleRate = 44100, waveFrequency = 1000,  waveVolume = 1000;
    if(strcmp(buf,"pulse")==0)
    {
        generatePulse (sampleRate, waveFrequency, waveVolume);
        send(sock, "OK", MESSAGE_SIZE, 0);
    }
    else if(strcmp(buf,"sine")==0)
    {
        generateSine(sampleRate, waveFrequency, waveVolume);
        send(sock, "OK", MESSAGE_SIZE, 0);
    }
    else if(strcmp(buf,"sawtooth")==0)
    {
        generateSawtooth(sampleRate, waveFrequency, waveVolume);
        send(sock, "OK", MESSAGE_SIZE, 0);
    }
    else if(strcmp(buf,"triangle")==0)
    {
        generateTriangle(sampleRate, waveFrequency, waveVolume);
        send(sock, "OK", MESSAGE_SIZE, 0);
    }
    else if(strcmp(buf,"square")==0)
    {
        generateSquare(sampleRate, waveFrequency, waveVolume);
        send(sock, "OK", MESSAGE_SIZE, 0);
    }
    else
    {
        printf("error recognizing command");
        send(sock, "FAIL", MESSAGE_SIZE, 0);
    }
}

void generateSine(float sampleRate, float waveFrequency, float waveVolume)
{
    float * bufferSine = (float*) calloc(SAMPLE_LENGTH,sizeof(float));
    float period=sampleRate/(waveFrequency*2);

    for(int i=0; i < SAMPLE_LENGTH; i++)
    {
        bufferSine[i]=waveVolume * sin(i*M_PI/period);
    }
    writeIntoFile(bufferSine);
    free(bufferSine);
}

void generateSawtooth(float sampleRate, float waveFrequency, float waveVolume)
{
    float * bufferSawtooth = (float*) calloc(SAMPLE_LENGTH,sizeof(float));
    float period = sampleRate/(waveFrequency*2);
    float sr=sampleRate/waveFrequency;
    int c=0;

    for(int i = 0; i < SAMPLE_LENGTH; i++)
    {
        if(c >= sr)
            c=0;
        bufferSawtooth[i] = waveVolume*(c/period)-waveVolume;
        c++;
    }
    writeIntoFile(bufferSawtooth);
    free(bufferSawtooth);
}
void generateTriangle(float sampleRate, float waveFrequency, float waveVolume)
{
    float * bufferTriangle = (float*) calloc(SAMPLE_LENGTH,sizeof(float));
    float period = sampleRate/(waveFrequency*2);
    float sr=period/2;
    int c = (int)period;
    int c2=-1;
    for(int i = 0; i < SAMPLE_LENGTH; i++)
    {
        if(c > period)
        {
            c=(int)period;
            c2=-1;
        }
        if(c < 0)
        {
            c=0;
            c2=1;
        }
        bufferTriangle[i]=waveVolume*(c/sr)-waveVolume;
        c+=c2;
    }
    writeIntoFile(bufferTriangle);
    free(bufferTriangle);
}
void generateSquare (float sampleRate, float waveFrequency, float waveVolume)
{
    float * bufferSquare = (float*) calloc(SAMPLE_LENGTH,sizeof(float));;
    float period = sampleRate/waveFrequency;
    float sr = period/2;
    int c = 0;
    int c2 = 1;

    for(int i=0; i < SAMPLE_LENGTH; i++)
    {
        if(c >= sr)
        {
            c = 0;
            c2 = c2 * -1;
        }
        bufferSquare[i]= waveVolume*c2;
        c = c+1;
    }
    writeIntoFile(bufferSquare);
    free(bufferSquare);
}
void generatePulse (float sampleRate, float waveFrequency, float waveVolume)
{
    float * bufferPulse = (float*) calloc(SAMPLE_LENGTH,sizeof(float));
    float period = sampleRate/waveFrequency;
    int c = 0;
    int c2 = 1;

    for(int i=0; i < SAMPLE_LENGTH; i++)
    {
        if (c >= 4 / 5 * period)
            c2 = 1;
        if (c >= period)
        {
            c = 0;
            c2 = -1;
        }
        bufferPulse[i] = waveVolume * c2;
        c = c + 1;
    }
    writeIntoFile(bufferPulse);
    free(bufferPulse);

}
void writeIntoFile(float * array) {
    FILE *file;

    char filename[50];
    strcat(filename, "file");

    char fileNumberStr[10];
    sprintf(fileNumberStr, "%d", fileNumber);
    fileNumber++;
    strcat(filename, fileNumberStr);
    strcat(filename, ".txt");
    file = fopen(filename, "w");

    for (int i = 0; i < SAMPLE_LENGTH; i++)
        fprintf(file, "%f", array[i]);

    fclose(file);
}

