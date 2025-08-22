#include <algorithm>
#include <iostream>
#include <string>
#include <windows.h>
#include "Language.h"

// Constructor
Language::Language() {
    color = 0;         // Starting color of text
    comment = false;   // Used to tell if you are inside a comment
}

// Methods
void Language::set(std::string language) {
    type = language;   // File type / coding language
    
    // Makes up list of primitive data types for each language
    if (language == "cpp" || language == "h") {
        data_words = {"int", "void", "bool", "char", "long", "short", "float",
                      "double", "signed", "unsigned", "size_t", "wchar_t",
                      "char8_t", "char16_t", "char32", "const", "class", "struct",
                      "public", "private", "protected", "virtual", "friend",
                      "this", "new", "delete", "static", "typedef", "extern",
                      "auto", "null", "nullptr"};
        keywords = {"if", "else", "switch", "case", "default", "for", "while", "do",
                    "break", "continue", "goto", "try", "catch", "throw", "return"};
     }
    else if (language == "py") {
        data_words = {""};
        keywords = {""};
    }
}
void Language::display(std::string line) {
    if (type == "cpp" || type == "h")
        cplusplus(line);
    else if (type == "py")
        python(line);
    else
        std::cout << line;
}
void Language::cplusplus(std::string line) {
    // Boolean to check if we started or are ending a string/character
    bool sequence = false;
    
    for (int i = 0; i < line.length(); i++) {
        // Coloring
        if (!comment) {
            if (!sequence) {
                // Comments
                if (line[i] == '/') {
                    if (line[i + 1] == '/') {
                        line = line.substr(0, i) + "\033[97m" + line.substr(i) + "\033[0m";
                        color = 0;
                        break;
                    }
                    else if (line[i + 1] == '*') {
                        line = line.substr(0, i) + "\033[97m" + line.substr(i);
                        color = 97;
                        if (line.find("*/", i + 2) != std::string::npos) {
                            line = line.substr(0, line.find("*/", i + 2) + 2) + "\033[0m" + line.substr(line.find("*/", i + 2) + 2);
                            i = line.find("*/", i + 2) + 1;
                        }
                        else {
                            comment = true;
                            break;
                        }
                    }
                }
                // Pre-processors
                else if (line[i] == '#') {
                    line = line.substr(0, i) + "\033[96m" + line.substr(i) + "\033[0m";
                    color = 0;
                    break;
                }
                // Starting Strings and Characters
                else if (line[i] == '"' || line[i] == '\'') {
                    line = line.substr(0, i) + "\033[93m" + line.substr(i);
                    color = 93;
                    i += 5;
                    sequence = true;
                }
                else if (line[i] == ';') {
                    line = line.substr(0, i) + "\033[90m" + line.substr(i) + "\033[0m";
                    i += 10;
                }
                // Data Types and Other Data Words
                for (std::string data_word : data_words) {
                    if (i == 0 && (line.find(data_word + " ") == i || line.find(data_word + ")") == i)
                        || line.find(" " + data_word + " ") == i || line.find(" " + data_word + ")") == i
                        || line.find("(" + data_word + " ") == i || line.find("(" + data_word + ")") == i) {
                        line = line.substr(0, i) + "\033[95m" + line.substr(i, data_word.length() + 1) + "\033[0m" + line.substr(i + data_word.length() + 1);
                        i += data_word.length() + 9;
                    }
                    else if (i == 0 && (line.find(data_word + "* ") == i || line.find(data_word + "*)") == i)
                        || line.find(" " + data_word + "* ") == i || line.find(" " + data_word + "*)") == i
                        || line.find("(" + data_word + "* ") == i || line.find("(" + data_word + "*)") == i) {
                        line = line.substr(0, i) + "\033[95m" + line.substr(i, data_word.length() + 2) + "\033[0m" + line.substr(i + data_word.length() + 2);
                        i += data_word.length() + 10;
                    }
                }
                // Keywords
                for (std::string keyword : keywords) {
                    if (i == 0 && (line.find(keyword + " ") == i || line.find(keyword + "(") == i || line.find(keyword + "{") == i || line.find(keyword + ";") == i)
                        || line.find(" " + keyword + " ") == i || line.find(" " + keyword + "(") == i || line.find(" " + keyword + "{") == i || line.find(" " + keyword + ";") == i
                        || line.find("(" + keyword + " ") == i || line.find("(" + keyword + "(") == i || line.find("(" + keyword + "{") == i || line.find("(" + keyword + ";") == i
                        || line.find("{" + keyword + " ") == i || line.find("{" + keyword + "(") == i || line.find("{" + keyword + "{") == i || line.find("{" + keyword + ";") == i
                        || line.find(";" + keyword + " ") == i || line.find(";" + keyword + "(") == i || line.find(";" + keyword + "{") == i || line.find(";" + keyword + ";") == i) {
                        line = line.substr(0, i) + "\033[94m" + line.substr(i, keyword.length() + 1) + "\033[0m" + line.substr(i + keyword.length() + 1);
                        i += keyword.length() + 9;
                    }
                }
            }
            // Escape Sequences in Strings
            else if (line[i] == '\\' && std::string("abfnrtv0\\?\"'").find(line[i + 1]) != std::string::npos) {
                line = line.substr(0, i) + "\033[91m" + line.substr(i, 2) + "\033[" + std::to_string(color) + "m" + line.substr(i + 2);
                i += (color == 0) ? 10 : 11;
            }
            // Ending Strings and Characters
            else if (line[i] == '"' || line[i] == '\'') {
                line = line.substr(0, i + 1) + "\033[0m" + line.substr(i + 1);
                i += 4;
                sequence = false;
            }
        }
        // Ending Comments
        else if (line[i] == '*' && line[i + 1] == '/' && !sequence) {
            line = line.substr(0, i + 2) + "\033[0m" + line.substr(i + 2);
            i += 5;
            comment = false;
        }
    }
    
    // Display
    std::cout << line;
}
void Language::python(std::string line) {
    // Display
    std::cout << line;
}