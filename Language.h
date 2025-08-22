#pragma once

#include <vector>

class Language {
    private:
        // Variables
        std::string type;
        short color;
        bool comment;
        std::vector<std::string> data_words;
        std::vector<std::string> keywords;
        
        // Methods
        void cplusplus(std::string);
        void python(std::string);
    public:
        Language();
        void set(std::string);
        void display(std::string);
};