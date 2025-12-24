#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <portaudio.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "10.124.53.29"   
#define SERVER_PORT 7077
#define SAMPLE_RATE 16000
#define FRAMES_PER_BUFFER 512
#define NUM_CHANNELS 1
#define SAMPLE_TYPE paInt16
#define SAMPLE_SIZE (2)
#define BUFFER_SIZE (FRAMES_PER_BUFFER * SAMPLE_SIZE * NUM_CHANNELS)

int main(void)
{
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    PaStream *stream;
    PaError err;

    setvbuf(stdout, NULL, _IONBF, 0); // disable printf buffering
    printf("[Client] Starting up...\n");

    // --- Initialize Winsock ---
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }
    printf("WSAStartup -> OK\n");

    // --- Create UDP socket ---
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // --- Server address ---
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // --- Initialize PortAudio ---
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio init failed: %s\n", Pa_GetErrorText(err));
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // --- Open microphone stream ---
    err = Pa_OpenDefaultStream(&stream, NUM_CHANNELS, 0, SAMPLE_TYPE, SAMPLE_RATE,
                               FRAMES_PER_BUFFER, NULL, NULL);
    if (err != paNoError) {
        printf("Failed to open input stream: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    Pa_StartStream(stream);
    printf("[Client] Sending microphone audio to %s:%d ...\n", SERVER_IP, SERVER_PORT);

    // --- Main loop ---
    while (1) {
        Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);
        sendto(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    // --- Cleanup ---
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    closesocket(sock);
    WSACleanup();
    return 0;
}
