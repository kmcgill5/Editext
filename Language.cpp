#include <algorithm>
#include <iostream>
#include <string>
#include <windows.h>
#include "Language.h"

// Constructor
Language::Language() {
    comment = false;
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
    else if (language == "java") {
        data_words = {"byte", "short", "int", "long", "float", "double", "char",
                      "boolean", "void", "final"};
        keywords = {"abstract", "assert", "break", "case", "catch", "class",
                    "continue", "default", "do", "else", "enum", "extends", "finally",
                    "for", "if", "implements", "instanceof", "interface", "native",
                    "new", "private", "protected", "public", "return", "static",
                    "strictfp", "super", "switch", "synchronized", "this", "throw",
                    "throws", "transient", "try", "volatile", "while"};
    }
    else if (language == "py") {
        data_words = {"str", "int", "float", "complex", "list", "tuple", "range",
                      "dict", "set", "frozenset", "bool", "bytes", "bytearray",
                      "memoryview", "NoneType"};
        keywords = {"False", "True", "None", "and", "or", "as", "assert", "async",
                    "await", "break", "class", "continue", "def", "del", "if",
                    "elif", "else", "except", "finally", "for", "from", "global",
                    "in", "is", "lambda", "nonlocal", "not", "pass", "raise",
                    "return", "try", "while", "with", "yield", "import"};
        methods = {"print", "input", "type"};
    }
}
void Language::display(std::string line) {
    if (type == "cpp" || type == "h")
        cplusplus(line);
    else if (type == "py")
        python(line);
    else if (type == "java")
        java(line);
    else
        std::cout << line;
}
void Language::cplusplus(std::string line) {
    // Checks to manage conflicting colors
    bool sequence = false;
    char string_character;
    
    for (int i = 0; i < line.length(); i++) {
        // Coloring
        if (!comment) {
            if (!sequence) {
                // Comments
                if (line[i] == '/') {
                    if (line[i + 1] == '/') {
                        line = line.substr(0, i) + "\033[97m" + line.substr(i) + "\033[0m";
                        break;
                    }
                    else if (line[i + 1] == '*') {
                        line = line.substr(0, i) + "\033[97m" + line.substr(i);
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
                    break;
                }
                // Starting Strings and Characters
                else if (line[i] == '"' || line[i] == '\'') {
                    line = line.substr(0, i) + "\033[93m" + line.substr(i);
                    i += 5;
                    sequence = true;
                    string_character = line[i];
                }
                // Ending Semicolon
                else if (line[i] == ';') {
                    line = line.substr(0, i) + "\033[90m" + line.substr(i, 1) + "\033[0m" + line.substr(i + 1);
                    i += 10;
                }
                // Data Types and Other Data Words
                for (std::string data_word : data_words) {
                    if (i == 0 && line.find(data_word + " ") == i) {
                        line = line.substr(0, i) + "\033[95m" + line.substr(i, data_word.length()) + "\033[0m" + line.substr(i + data_word.length());
                        i += data_word.length() + 9;
                    }
                    else if (line.find(" " + data_word + " ") == i || line.find(" " + data_word + ",") == i || line.find(" " + data_word + ")") == i
                             || line.find("," + data_word + " ") == i || line.find("," + data_word + ",") == i || line.find("," + data_word + ")") == i
                             || line.find("(" + data_word + " ") == i || line.find("(" + data_word + ",") == i || line.find("(" + data_word + ")") == i) {
                        line = line.substr(0, i + 1) + "\033[95m" + line.substr(i + 1, data_word.length()) + "\033[0m" + line.substr(i + data_word.length() + 1);
                        i += data_word.length() + 10;
                    }
                    else if (i == 0 && (line.find(data_word + "* ") == i || line.find(data_word + "*)") == i)) {
                        line = line.substr(0, i) + "\033[95m" + line.substr(i, data_word.length() + 1) + "\033[0m" + line.substr(i + data_word.length() + 1);
                        i += data_word.length() + 10;
                    }
                    else if (line.find(" " + data_word + "* ") == i || line.find(" " + data_word + "*,") == i || line.find(" " + data_word + "*)") == i
                             || line.find("," + data_word + "*,") == i || line.find("," + data_word + "*,") == i || line.find("," + data_word + "*)") == i
                             || line.find("(" + data_word + "* ") == i || line.find("(" + data_word + "*,") == i || line.find("(" + data_word + "*)") == i) {
                        line = line.substr(0, i + 1) + "\033[95m" + line.substr(i + 1, data_word.length() + 1) + "\033[0m" + line.substr(i + data_word.length() + 2);
                        i += data_word.length() + 11;
                    }
                }
                // Keywords
                for (std::string keyword : keywords) {
                    if (i == 0 && (line.find(" " + keyword) == i && i + keyword.length() == line.length() - 2 || line.find(keyword + " ") == i || line.find(keyword + "(") == i || line.find(keyword + "{") == i || line.find(keyword + ";") == i)) {
                        line = line.substr(0, i) + "\033[94m" + line.substr(i, keyword.length()) + "\033[0m" + line.substr(i + keyword.length());
                        i += keyword.length() + 8;
                    }
                else if (line.find(" " + keyword) == i && i + keyword.length() == line.length() - 2 || line.find(" " + keyword + " ") == i || line.find(" " + keyword + "(") == i || line.find(" " + keyword + "{") == i || line.find(" " + keyword + ";") == i
                         || line.find("(" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("(" + keyword + " ") == i || line.find("(" + keyword + "(") == i || line.find("(" + keyword + "{") == i || line.find("(" + keyword + ";") == i
                         || line.find("{" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("{" + keyword + " ") == i || line.find("{" + keyword + "(") == i || line.find("{" + keyword + "{") == i || line.find("{" + keyword + ";") == i
                         || line.find("}" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("}" + keyword + " ") == i || line.find("}" + keyword + "(") == i || line.find("}" + keyword + "{") == i || line.find("}" + keyword + ";") == i
                         || line.find(";" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find(";" + keyword + " ") == i || line.find(";" + keyword + "(") == i || line.find(";" + keyword + "{") == i || line.find(";" + keyword + ";") == i) {
                        line = line.substr(0, i + 1) + "\033[94m" + line.substr(i + 1, keyword.length()) + "\033[0m" + line.substr(i + keyword.length() + 1);
                        i += keyword.length() + 9;
                    }
                }
            }
            // Escape Sequences in Strings
            else if (line[i] == '\\') {
                if (std::string("abfnrtv\\?\"'").find(line[i + 1]) != std::string::npos || line[i + 1] == '0' && !std::all_of(line.begin() + i + 2, line.begin() + i + 4, ::isdigit)) {
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, 2) + "\033[93m" + line.substr(i + 2);
                    i += 11;
                }
                else if (std::all_of(line.begin() + i + 1, line.begin() + i + 4, ::isdigit)) {
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, 4) + "\033[93m" + line.substr(i + 4);
                    i += 13;
                }
                else if (line[i + 1] == 'x' && std::string("0123456789ABCDEFabcdef").find(line[i + 2]) != std::string::npos) {
                    int k;
                    for (k = 1; k < line.substr(i).length(); k++)
                        if (std::string("0123456789ABCDEFabcdef").find(line[i + k + 2]) == std::string::npos)
                            break;
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, k + 2) + "\033[93m" + line.substr(i + k + 2);
                    i += 11 + k;
                }
                else if (line[i + 1] == 'u') {
                    int k;
                    for (k = 0; k < line.substr(i).length(); k++)
                        if (std::string("0123456789ABCDEFabcdef").find(line[i + k + 2]) == std::string::npos)
                            break;
                    if (k >= 8) {
                        line = line.substr(0, i) + "\033[91m" + line.substr(i, 10) + "\033[93m" + line.substr(i + 10);
                        i += 19;
                    }
                    else if (k >= 4) {
                        line = line.substr(0, i) + "\033[91m" + line.substr(i, 6) + "\033[93m" + line.substr(i + 6);
                        i += 15;
                    }
                }
            }
            // Ending Strings and Characters
            else if (line[i] == string_character) {
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
    std::cout << line << (comment ? "" : "\033[0m");
}
void Language::java(std::string line) {
    // Checks to manage conflicting colors
    bool sequence = false;
    char string_character;
    
    for (int i = 0; i < line.length(); i++) {
        // Coloring
        if (!comment) {
            if (!sequence) {
                // Comments
                if (line[i] == '/') {
                    if (line[i + 1] == '/') {
                        line = line.substr(0, i) + "\033[97m" + line.substr(i) + "\033[0m";
                        break;
                    }
                    else if (line[i + 1] == '*') {
                        line = line.substr(0, i) + "\033[97m" + line.substr(i);
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
                // Packages and Imports
                else if (line.find("package") == i || line.find("import") == i) {
                    line = line.substr(0, i) + "\033[96m" + line.substr(i) + "\033[0m";
                    break;
                }
                // Starting Strings and Characters
                else if (line[i] == '"' || line[i] == '\'') {
                    line = line.substr(0, i) + "\033[93m" + line.substr(i);
                    i += 5;
                    sequence = true;
                    string_character = line[i];
                }
                // Ending Semicolon
                else if (line[i] == ';') {
                    line = line.substr(0, i) + "\033[90m" + line.substr(i, 1) + "\033[0m" + line.substr(i + 1);
                    i += 10;
                }
                // Data Types and Other Data Words
                for (std::string data_word : data_words) {
                    if (i == 0 && line.find(data_word + " ") == i) {
                        line = line.substr(0, i) + "\033[95m" + line.substr(i, data_word.length()) + "\033[0m" + line.substr(i + data_word.length());
                        i += data_word.length() + 9;
                    }
                    else if (line.find(" " + data_word + " ") == i || line.find(" " + data_word + ")") == i
                             || line.find("," + data_word + " ") == i || line.find("," + data_word + ")") == i
                             || line.find("(" + data_word + " ") == i || line.find("(" + data_word + ")") == i) {
                        line = line.substr(0, i + 1) + "\033[95m" + line.substr(i + 1, data_word.length()) + "\033[0m" + line.substr(i + data_word.length() + 1);
                        i += data_word.length() + 10;
                    }
                }
                // Keywords
                for (std::string keyword : keywords) {
                    if (i == 0 && (line.find(keyword) == i && i + keyword.length() == line.length() - 2 || line.find(keyword + " ") == i || line.find(keyword + "(") == i || line.find(keyword + "{") == i || line.find(keyword + ";") == i)) {
                        line = line.substr(0, i) + "\033[94m" + line.substr(i, keyword.length()) + "\033[0m" + line.substr(i + keyword.length());
                        i += keyword.length() + 8;
                    }
                    else if (line.find(" " + keyword) == i && i + keyword.length() == line.length() - 2 || line.find(" " + keyword + " ") == i || line.find(" " + keyword + "(") == i || line.find(" " + keyword + "{") == i || line.find(" " + keyword + ";") == i
                             || line.find("(" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("(" + keyword + " ") == i || line.find("(" + keyword + "(") == i || line.find("(" + keyword + "{") == i || line.find("(" + keyword + ";") == i
                             || line.find("{" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("{" + keyword + " ") == i || line.find("{" + keyword + "(") == i || line.find("{" + keyword + "{") == i || line.find("{" + keyword + ";") == i
                             || line.find("}" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("}" + keyword + " ") == i || line.find("}" + keyword + "(") == i || line.find("}" + keyword + "{") == i || line.find("}" + keyword + ";") == i
                             || line.find(";" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find(";" + keyword + " ") == i || line.find(";" + keyword + "(") == i || line.find(";" + keyword + "{") == i || line.find(";" + keyword + ";") == i) {
                        line = line.substr(0, i + 1) + "\033[94m" + line.substr(i + 1, keyword.length()) + "\033[0m" + line.substr(i + keyword.length() + 1);
                        i += keyword.length() + 9;
                    }
                }
            }
            // Escape Sequences in Strings
            else if (line[i] == '\\') {
                if (std::string("bfnrt\\\"'").find(line[i + 1]) != std::string::npos || line[i + 1] == '0' && !std::all_of(line.begin() + i + 2, line.begin() + i + 4, ::isdigit)) {
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, 2) + "\033[93m" + line.substr(i + 2);
                    i += 11;
                }
                else if (std::all_of(line.begin() + i + 1, line.begin() + i + 4, ::isdigit)) {
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, 4) + "\033[93m" + line.substr(i + 4);
                    i += 13;
                }
                else if (line[i + 1] == 'u') {
                    int k;
                    for (k = 0; k < line.substr(i).length() || k == 4; k++)
                        if (std::string("0123456789ABCDEFabcdef").find(line[i + k + 2]) == std::string::npos)
                            break;
                    if (k == 4) {
                        line = line.substr(0, i) + "\033[91m" + line.substr(i, 6) + "\033[93m" + line.substr(i + 6);
                        i += 15;
                    }
                }
            }
            // Ending Strings and Characters
            else if (line[i] == string_character) {
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
    std::cout << line << (comment ? "" : "\033[0m");
}
void Language::python(std::string line) {
    // Checks to manage conflicting colors
    bool sequence = false;
    char string_character;
    
    for (int i = 0; i < line.length(); i++) {
        // Coloring
        if (!sequence) {
            // Comments
            if (line[i] == '#') {
                line = line.substr(0, i) + "\033[97m" + line.substr(i) + "\033[0m";
                break;
            }
            // Starting Strings and Characters
            else if (line[i] == '"' || line[i] == '\'') {
                line = line.substr(0, i) + "\033[93m" + line.substr(i);
                i += 5;
                sequence = true;
                string_character = line[i];
            }
            // Ending Semicolon
            else if (line[i] == ';') {
                line = line.substr(0, i) + "\033[90m" + line.substr(i, 1) + "\033[0m" + line.substr(i + 1);
                i += 10;
            }
            // Line Continuation
            else if (line[i] == '\\' && i == line.length() - 2) {
                line = line.substr(0, i) + "\033[91m" + line.substr(i, 1) + "\033[0m" + line.substr(i + 1);
                i += 10;
            }
            // Data Types and Other Data Words
            for (std::string data_word : data_words) {
                if (i == 0 && line.find(data_word + " ") == i) {
                    line = line.substr(0, i) + "\033[95m" + line.substr(i, data_word.length()) + "\033[0m" + line.substr(i + data_word.length());
                    i += data_word.length() + 8;
                }
                else if (line.find(" " + data_word + " ") == i || line.find(" " + data_word + ")") == i
                         || line.find("(" + data_word + " ") == i || line.find("(" + data_word + ")") == i) {
                    line = line.substr(0, i + 1) + "\033[95m" + line.substr(i + 1, data_word.length()) + "\033[0m" + line.substr(i + data_word.length() + 1);
                    i += data_word.length() + 9;
                }
            }
            // Keywords
            for (std::string keyword : keywords) {
                if (i == 0 && (line.find(keyword) == i && i + keyword.length() == line.length() - 2 || line.find(keyword + " ") == i || line.find(keyword + "(") == i || line.find(keyword + "{") == i || line.find(keyword + ";") == i)) {
                    line = line.substr(0, i) + "\033[94m" + line.substr(i, keyword.length()) + "\033[0m" + line.substr(i + keyword.length());
                    i += keyword.length() + 8;
                }
                else if (line.find(" " + keyword) == i && i + keyword.length() == line.length() - 2 || line.find(" " + keyword + " ") == i || line.find(" " + keyword + "(") == i || line.find(" " + keyword + "{") == i || line.find(" " + keyword + ";") == i
                         || line.find("(" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("(" + keyword + " ") == i || line.find("(" + keyword + "(") == i || line.find("(" + keyword + "{") == i || line.find("(" + keyword + ";") == i
                         || line.find("{" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("{" + keyword + " ") == i || line.find("{" + keyword + "(") == i || line.find("{" + keyword + "{") == i || line.find("{" + keyword + ";") == i
                         || line.find("}" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find("}" + keyword + " ") == i || line.find("}" + keyword + "(") == i || line.find("}" + keyword + "{") == i || line.find("}" + keyword + ";") == i
                         || line.find(";" + keyword) == i && i + keyword.length() == line.length() - 2 || line.find(";" + keyword + " ") == i || line.find(";" + keyword + "(") == i || line.find(";" + keyword + "{") == i || line.find(";" + keyword + ";") == i) {
                    line = line.substr(0, i + 1) + "\033[94m" + line.substr(i + 1, keyword.length()) + "\033[0m" + line.substr(i + keyword.length() + 1);
                    i += keyword.length() + 9;
                }
            }
            // Special Methods
            for (std::string method : methods) {
                if (i == 0 && (line.find(method + " ") == i || line.find(method + "(") == i || line.find(method + "{") == i || line.find(method + ";") == i)) {
                    line = line.substr(0, i) + "\033[96m" + line.substr(i, method.length()) + "\033[0m" + line.substr(i + method.length());
                    i += method.length() + 8;
                }
                else if (line.find(" " + method + " ") == i || line.find(" " + method + "(") == i || line.find(" " + method + "{") == i || line.find(" " + method + ";") == i
                         || line.find("(" + method + " ") == i || line.find("(" + method + "(") == i || line.find("(" + method + "{") == i || line.find("(" + method + ";") == i
                         || line.find("{" + method + " ") == i || line.find("{" + method + "(") == i || line.find("{" + method + "{") == i || line.find("{" + method + ";") == i
                         || line.find(";" + method + " ") == i || line.find(";" + method + "(") == i || line.find(";" + method + "{") == i || line.find(";" + method + ";") == i) {
                    line = line.substr(0, i + 1) + "\033[96m" + line.substr(i + 1, method.length()) + "\033[0m" + line.substr(i + method.length() + 1);
                    i += method.length() + 9;
                }
            }
        }
        // Escape Sequences in Strings
        else if (line[i] == '\\') {
            if (i == line.length() - 1) {
                line = line.substr(0, i) + "\033[91m" + line.substr(i) + "\033[0m";
                i++;
            }
            else if (std::string("abfnrtv\\\"'").find(line[i + 1]) != std::string::npos || line[i + 1] == '0' && !std::all_of(line.begin() + i + 2, line.begin() + i + 4, ::isdigit)) {
                line = line.substr(0, i) + "\033[91m" + line.substr(i, 2) + "\033[93m" + line.substr(i + 2);
                i += 11;
            }
            else if (std::all_of(line.begin() + i + 1, line.begin() + i + 4, ::isdigit)) {
                line = line.substr(0, i) + "\033[91m" + line.substr(i, 4) + "\033[93m" + line.substr(i + 4);
                i += 13;
            }
            else if (line[i + 1] == 'x') {
                if (std::string("0123456789ABCDEFabcdef").find(line[i + 2]) != std::string::npos && std::string("0123456789ABCDEFabcdef").find(line[i + 3]) != std::string::npos) {
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, 4) + "\033[93m" + line.substr(i + 4);
                    i += 3;
                }
            }
            else if (line[i + 1] == 'N' && line[i + 1] == '{') {
                int k;
                for (k = 0; k < line.length(); k++)
                    if (line[i + k + 2] == '}')
                        break;
                line = line.substr(0, i) + "\033[91m" + line.substr(i, k + 4) + "\033[93m" + line.substr(i + k + 4);
                i += k + 3;
            }
            else if (line[i + 1] == 'u') {
                int k;
                for (k = 0; k < line.substr(i).length(); k++)
                    if (std::string("0123456789ABCDEFabcdef").find(line[i + k + 2]) == std::string::npos)
                        break;
                if (k >= 4) {
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, 6) + "\033[93m" + line.substr(i + 6);
                    i += 15;
                }
            }
            else if (line[i + 1] == 'U') {
                int k;
                for (k = 0; k < line.substr(i).length(); k++)
                    if (std::string("0123456789ABCDEFabcdef").find(line[i + k + 2]) == std::string::npos)
                        break;
                if (k >= 8) {
                    line = line.substr(0, i) + "\033[91m" + line.substr(i, 10) + "\033[93m" + line.substr(i + 10);
                    i += 19;
                }
            }
        }
        // Ending Strings and Characters
        else if (line[i] == string_character) {
            line = line.substr(0, i + 1) + "\033[0m" + line.substr(i + 1);
            i += 4;
            sequence = false;
        }
    }
    
    // Display
    std::cout << line << (comment ? "" : "\033[0m");
}