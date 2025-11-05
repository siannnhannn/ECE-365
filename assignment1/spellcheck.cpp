// spellcheck.cpp
#include "hash.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <ctime>

bool isValidChar(char c);
bool isValidWord(const std::string &word, std::size_t maxLen);
static inline bool hasDigit(const std::string& s);
static inline void toLowerInPlace(std::string& s);
static bool processDocument(hashTable& dict, const std::string& docPath, const std::string& outPath, std::size_t maxLen);

int main() {
	std::string dictionaryFilePath;
	std::string documentFilePath;
	std::string outputFilePath;

	std::cout << "Enter dictionary file: ";
	std::getline(std::cin, dictionaryFilePath);

	std::cout << "Enter document file to be spellchecked: ";
	std::getline(std::cin, documentFilePath);

	std::cout << "Enter output file: ";
	std::getline(std::cin, outputFilePath);

    // open dictionary
    std::ifstream inputFile(dictionaryFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << dictionaryFilePath << '\n';
        return 1;
    }

    const std::size_t MAX_LEN = 20;
    hashTable dict;

    // dictionary load CPU time
    std::clock_t t0 = std::clock();

    std::string word;
    while (std::getline(inputFile, word)) {
        if (!isValidWord(word, MAX_LEN)) continue;  // skip invalid/too-long dict entries
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char ch){ return static_cast<char>(std::tolower(ch)); });
        dict.insert(word);
    }

    std::clock_t t1 = std::clock();
    double dictSecs = double(t1 - t0) / double(CLOCKS_PER_SEC);

    // spellcheck document CPU time
    std::clock_t t2 = std::clock();

    if (!processDocument(dict, documentFilePath, outputFilePath, MAX_LEN)) {
        std::cerr << "Error: Could not open document or output file.\n";
        return 1;
    }

    std::clock_t t3 = std::clock();
    double docSecs = double(t3 - t2) / double(CLOCKS_PER_SEC);

    // print timing to stdout
    std::cout.setf(std::ios::fixed);
    std::cout.precision(2);
    std::cout << "Time to load dictionary: " << dictSecs << " seconds\n";
    std::cout << "Time to check document: "  << docSecs  << " seconds\n";

    return 0;
}

bool isValidChar(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) ||
           std::isdigit(static_cast<unsigned char>(c)) ||
           c == '-' || c == '\'';
}

bool isValidWord(const std::string &word, std::size_t maxLen) {
    if (word.empty() || word.size() > maxLen) return false;
    for (char c : word) if (!isValidChar(c)) return false;
    return true;
}

static inline bool hasDigit(const std::string& s) {
    for (unsigned char ch : s) if (std::isdigit(ch)) return true;
    return false;
}

static inline void toLowerInPlace(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char ch){ return static_cast<char>(std::tolower(ch)); });
}

// spellcheck the document
static bool processDocument(hashTable& dict, const std::string& docPath, const std::string& outPath, std::size_t maxLen)
{
    std::ifstream in(docPath);
    if (!in) return false;

    std::ofstream out(outPath);
    if (!out) return false;

    std::string line;
    std::size_t lineNo = 0;

    while (std::getline(in, line)) {
        ++lineNo;

        std::string token;
        token.reserve(32);

        auto flushToken = [&](){
            if (token.empty()) return;

            if (token.size() > maxLen) {
                out << "Long word at line " << lineNo
                    << ", starts: " << token.substr(0, maxLen) << '\n';
            } else {
                if (!hasDigit(token)) {
                    std::string probe = token;
                    toLowerInPlace(probe);
                    if (!dict.contains(probe)) {
                        out << "Unknown word at line " << lineNo
                            << ": " << token << '\n';
                    }
                }
            }
            token.clear();
        };

	for (char c : line) {
	    if (isValidChar(c)) {
		token.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
	    } else {
		flushToken();
	    }
	}
        flushToken();
    }

    return true;
}

