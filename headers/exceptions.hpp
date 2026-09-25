#pragma once
#include <string>

class Exception {
    private:
        std::string errorMassage;
    public:
        Exception(const char* massage) {
            if (!massage) {
                errorMassage = "";
            } else {
                errorMassage = massage;
            }
        }

        Exception(const std::string& massage) 
            : errorMassage(massage) {}

        const char* what() const {
            return errorMassage.c_str();
        }
};

class NullPtrException : public Exception {
    public:
        NullPtrException(const char* massage) 
            : Exception(massage) {}
            
        NullPtrException(const std::string& massage) 
            : Exception(massage) {}
};