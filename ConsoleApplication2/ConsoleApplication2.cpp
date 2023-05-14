#include <iostream>
#include <string>

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
    string input = "abracadabra";
    int windowSize = 4;
    int bufferSize = 4;
    string compressed = lz77_compress(input, windowSize, bufferSize);
    cout << "Compressed: " << compressed << endl;
    string decompressed = lz77_decompress(compressed);
    cout << "Decompressed: " << decompressed << endl;
    return 0;
}
