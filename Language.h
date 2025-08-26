#pragma once

#include <vector>

class Language {
    private:
        // Variables
        bool comment;
        std::string type;
        std::vector<std::string> data_words;
        std::vector<std::string> keywords;
        std::vector<std::string> methods;
        
        // Methods
        void cplusplus(std::string);
        void java(std::string);
        void python(std::string);
    public:
        // Constructor
        Language();
        
        // Methods
        void set(std::string);
        void display(std::string);
};