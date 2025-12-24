#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <portaudio.h>

#pragma comment(lib, "ws2_32.lib")

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
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    int recvLen;
    PaStream *stream;
    PaError err;

    setvbuf(stdout, NULL, _IONBF, 0); // disable printf buffering
    printf("[Server] Starting up...\n");

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

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    // --- Bind socket ---
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Bind successful on port %d\n", SERVER_PORT);

    // --- Initialize PortAudio ---
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio init failed: %s\n", Pa_GetErrorText(err));
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // --- Open playback stream ---
    err = Pa_OpenDefaultStream(&stream, 0, NUM_CHANNELS, SAMPLE_TYPE, SAMPLE_RATE,
                               FRAMES_PER_BUFFER, NULL, NULL);
    if (err != paNoError) {
        printf("Failed to open playback stream: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    Pa_StartStream(stream);
    printf("[Server] Ready to receive audio...\n");

    // --- Receive loop ---
    while (1) {
        recvLen = recvfrom(sock, buffer, BUFFER_SIZE, 0,
                           (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (recvLen > 0) {
            Pa_WriteStream(stream, buffer, FRAMES_PER_BUFFER);
        }
    }

    // --- Cleanup ---
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    closesocket(sock);
    WSACleanup();
    return 0;
}
