#pragma once

#include <exception>
#include <string>

namespace sylvanmats::antlr4::parse{

    class Exception :  public std::exception{
        private:
        std::string message{};
        public:
        Exception()=delete;
        Exception(std::string message) : message (message){};
        Exception(const Exception& orig)=delete;
        virtual ~Exception() throw(){};
        virtual const char* what()const noexcept{return message.c_str();};
    };
}