/*
    UDP Server
*/

#include <stdio.h>
#include "UDPServer.h"
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <string>
#include <cmath>

#ifndef _WIN32
using SOCKET = int
#define WSAGetLastError() 1
#else
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

//#define SERVER "127.0.0.1"	//ip address of udp server
#define BUFLEN 	1024		    //Max length of buffer
#define PORT    8888			//The port on which to listen for incoming data

using namespace std;

struct Token {
    int offset;
    int length;
    char nextChar;
};

string lz77_compress(const string& input, int windowSize, int bufferSize) {
    string output;
    int inputSize = input.size();
    int currentIndex = 0;
    while (currentIndex < inputSize) {
        int windowStart = max(0, currentIndex - windowSize);
        int windowEnd = min(currentIndex, inputSize - bufferSize);
        Token bestToken;
        bestToken.offset = 0;
        bestToken.length = 0;
        bestToken.nextChar = input[currentIndex];
        for (int i = windowStart; i < currentIndex; i++) {
            int j = 0;
            while (currentIndex + j < inputSize && j < bufferSize && input[i + j] == input[currentIndex + j]) {
                j++;
            }
            if (j > bestToken.length) {
                bestToken.offset = currentIndex - i;
                bestToken.length = j;
                bestToken.nextChar = input[currentIndex + j];
            }
        }
        output += to_string(bestToken.offset) + "," + to_string(bestToken.length) + "," + bestToken.nextChar;
        currentIndex += bestToken.length + 1;
    }
    return output;
}

string lz77_decompress(const string& input) {
    string output;
    int inputSize = input.size();
    int currentIndex = 0;
    while (currentIndex < inputSize) {
        int comma1 = input.find(",", currentIndex);
        int comma2 = input.find(",", comma1 + 1);
        int offset = stoi(input.substr(currentIndex, comma1 - currentIndex));
        int length = stoi(input.substr(comma1 + 1, comma2 - comma1 - 1));
        char nextChar = input[comma2 + 1];
        for (int i = 0; i < length; i++) {
            output += output[output.size() - offset];
        }
        output += nextChar;
        currentIndex = comma2 + 2;
    }
    return output;
}

int main() {
    struct sockaddr_in si_other;
    //struct sockaddr_in si_other1;
    unsigned short srvport;
    int slen;
    char buf[BUFLEN];
    char msg[BUFLEN];
    int i, k = 0;


    string input = "abracadabra";
    int windowSize = 4;
    int bufferSize = 4;
    string compressed = lz77_compress(input, windowSize, bufferSize);
    cout << "Compressed: " << compressed << endl;
    string decompressed = lz77_decompress(compressed);
    cout << "Decompressed: " << decompressed << endl;

    srvport = (1 == argc) ? PORT : atoi(argv[1]);

    UDPServer server(srvport);
    slen = sizeof(si_other);

    while (1)
    {
        memset(msg, '\0', BUFLEN);
        memset(buf, '\0', BUFLEN);

        printf("Waiting for data...   ");
        server.RecvDatagram(buf, BUFLEN, (struct sockaddr*)&si_other, &slen);
        std::string ncode_string(buf);
        ncode_string = lz77_decompress(ncode_string);
        ncode_string = lz77_compress(ncode_string, windowSize,bufferSize);
        strcpy_s(buf, ncode_string.c_str());
        printf("%s ", buf);
        memset(buf, '\0', BUFLEN);

        printf("\nAnswer : ");
        gets_s(msg, BUFLEN);
        std::string decod_string(msg);
        decod_string = lz77_decompress(decod_string);
        decod_string = lz77_compress (decod_string, windowSize, bufferSize);
        strcpy_s(msg, decod_string.c_str());
        server.SendDatagram(msg, (int)strlen(msg), (struct sockaddr*)&si_other, slen);
        memset(msg, '\0', BUFLEN);
    }
    return 0;
}
