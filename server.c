#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MESSAGE_SIZE 50
size_t samplelength = 16384;
void writeIntoFile (float * array);
void generatePulse (float sampleRate, float waveFrequency, float waveVolume);
void generateSine(float sampleRate, float waveFrequency, float waveVolume);
void generateSawtooth(float sampleRate, float waveFrequency, float waveVolume);
void generateTriangle(float sampleRate, float waveFrequency, float waveVolume);
void generateSquare (float sampleRate, float waveFrequency, float waveVolume);
int main()
{
    int sock, listener;
    struct sockaddr_in addr;
    char buf[MESSAGE_SIZE];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    float sampleRate = 44100, waveFrequency = 1000,  waveVolume = 1000;
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);

    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }

        while(1)
        {
            bytes_read = recv(sock, buf, MESSAGE_SIZE, 0);
            if(bytes_read <= 0) break;
            printf(buf);
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
            else if(strcmp(buf," square")==0)
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
        close(sock);
    }
}
void generateSine(float sampleRate, float waveFrequency, float waveVolume)
{
    double * buffer = (double*) malloc(samplelength);
    float period=sampleRate/(waveFrequency*2);

    for(int i=0; i < samplelength; i++)
    {
        buffer[i]=waveVolume * sin(i*M_PI/period);
    }
    writeIntoFile(buffer);
    free(buffer);
}

void generateSawtooth(float sampleRate, float waveFrequency, float waveVolume)
{
    float * buffer = (float*) malloc(samplelength);
    float period = sampleRate/(waveFrequency*2);
    float sr=sampleRate/waveFrequency;
    int c=0;

    for(int i = 0; i < samplelength; i++)
    {
        if(c >= sr)
            c=0;
        buffer[i] = waveVolume*(c/period)-waveVolume;
        c++;
    }
    writeIntoFile(buffer);
    free(buffer);
}
void generateTriangle(float sampleRate, float waveFrequency, float waveVolume)
{
    float * buffer = (float*) malloc(samplelength);
    float period = sampleRate/(waveFrequency*2);
    float sr=period/2;
    int c = (int)period;
    int c2=-1;

    for(int i = 0; i < samplelength; i++)
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
        buffer[i]=waveVolume*(c/sr)-waveVolume;
        c+=c2;
    }
    writeIntoFile(buffer);
    free(buffer);
}
void generateSquare (float sampleRate, float waveFrequency, float waveVolume)
{
    float * buffer = (float*) malloc(samplelength);
    float period = sampleRate/waveFrequency;
    float sr = period/2;
    int c = 0;
    int c2 = 1;

    for(int i=0; i < samplelength; i++)
    {
        if(c >= sr)
        {
            c = 0;
            c2 = c2 * -1;
        }
        buffer[i]= waveVolume*c2;
        c = c+1;
    }
    writeIntoFile(buffer);
    free(buffer);
}
void generatePulse (float sampleRate, float waveFrequency, float waveVolume)
{

    float * buffer = (float*) malloc(samplelength);
    float period = sampleRate/waveFrequency;
    int c = 0;
    int c2 = 1;

    for(int i=0; i < samplelength; i++)
    {
        if (c >= 4 / 5 * period)
            c2 = 1;
        if (c >= period)
        {
            c = 0;
            c2 = -1;
        }
        buffer[i] = waveVolume * c2;
        c = c + 1;
    }
    writeIntoFile(buffer);
    free(buffer);

}
void writeIntoFile(float * array) {
    FILE *file;
    file = fopen("fprintf.txt", "w");

    for (int i = 0; i < samplelength; i++)
        fprintf(file, "%f", array[i]);

    fclose(file);
}

