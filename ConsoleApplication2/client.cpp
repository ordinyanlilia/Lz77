#include "UDPSocket.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <string>
#include <cmath>


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
    //	SOCKET s;
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);

    //struct sockaddr_in si_other1;
    //int slen1 = sizeof(si_other1);

    char buf[BUFLEN];
    char message[BUFLEN];

    string input = "abracadabra";
    int windowSize = 4;
    int bufferSize = 4;
    string compressed = lz77_compress(input, windowSize, bufferSize);
    cout << "Compressed: " << compressed << endl;
    string decompressed = lz77_decompress(compressed);
    cout << "Decompressed: " << decompressed << endl;
    unsigned short srv_port = 0;
    char srv_ip_addr[40];
    memset(srv_ip_addr, 0, 40);


    //create socket
    UDPSocket client_sock;

    //setup address structure
    memset((char*)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;

    if (1 == argc)
    {
        si_other.sin_port = htons(PORT);
        si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
        printf("1: Server - addr=%s , port=%d\n", SERVER, PORT);
    }
    else if (2 == argc)
    {
        si_other.sin_port = htons(atoi(argv[1]));
        si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
        printf("2: argv[0]: Server - addr=%s , port=%d\n", SERVER, atoi(argv[1]));
    }
    else
    {
        si_other.sin_port = htons(atoi(argv[2]));
        si_other.sin_addr.S_un.S_addr = inet_addr(argv[1]);
        printf("3: Server - addr=%s , port=%d\n", argv[1], atoi(argv[2]));
    }

    while (1)
    {
        memset(buf, '\0', BUFLEN);
        memset(message, '\0', BUFLEN);

        printf("Enter message: ");
        gets_s(message, BUFLEN);
        std::string decod_string(message);
        decod_string = lz77_decompress(decod_string);
        decod_string = lz77_compress(decod_string, windowSize, bufferSize);
        strcpy_s(message, decod_string.c_str());
        client_sock.SendDatagram(message, (int)strlen(message), (struct sockaddr*)&si_other, slen);
        memset(message, '\0', BUFLEN);

        client_sock.RecvDatagram(buf, BUFLEN, (struct sockaddr*)&si_other, &slen);
        std::string ncode_string(buf);
        ncode_string = lz77_decompress(ncode_string);
        ncode_string = lz77_compress(ncode_string, windowSize, bufferSize);
        strcpy_s(buf, ncode_string.c_str());
        printf("Answer: %s\n", buf);
        memset(buf, '\0', BUFLEN);
    }
    return 0;

}
