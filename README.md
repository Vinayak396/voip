# Voice over IP (VoIP) Communication System

This project implements a **one-way Voice over Internet Protocol (VoIP) system**
using the **C programming language**. It enables real-time audio transmission
from a **client** to a **server** over a local area network (LAN) using **UDP
socket communication**.

The client captures audio input using the **PortAudio library** and sends the
audio data as UDP packets to the server. The server receives the packets and
plays the audio in real time through its speakers.

---

## Features
- One-way real-time audio streaming (Client → Server)
- Low-latency UDP-based communication
- Audio capture and playback using PortAudio
- Lightweight and efficient C implementation
- Tested on Windows using MinGW

---

## Technologies Used
- C Programming Language
- PortAudio Library
- Winsock (UDP Socket Programming)
- LAN / Wi-Fi Networking

---

---

## Requirements
- Windows Operating System
- MinGW (GCC Compiler)
- PortAudio library
- Two computers connected to the same network

---

## Compilation

## Compile Server
gcc server.c -I../include -L../build_mingw -lportaudio -lws2_32 -o server.exe

## Compile Client
gcc client.c -I../include -L../build_mingw -lportaudio -lws2_32 -o client.exe

---

## How to Run
1. Run server.exe on the server machine
2. Run client.exe on the client machine
3. Ensure both systems are connected to the same Wi-Fi or LAN

---

## Network Configuration
The client uses the server’s IPv4 address to send audio
UDP communication is used for low latency
Firewall should allow UDP traffic on the selected port

---

## Limitations
1. One-way audio communication only
2. No audio compression
3. No encryption
4. No echo cancellation
