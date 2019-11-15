/*
srcReport.cpp

Produces a report that counts the number of statements, declarations, etc. of C++ programs
*/

#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "XMLParser.hpp"
#include <unordered_map>

int main()
{
    long total = 0;
    int textsize = 0;
    int loc = 0;
    std::unordered_map<std::string, int> countTags;
    std::unordered_map<std::string, int> countAttributes;
    
    /*
    creating an object with the use of Lambdas
    Lambdas are anonymous fuctions that will be called if the functions in the other files need them
    They do all the parsing outside of srcreport while keeping the srcML varibles inside
    */
    XMLParser parser(
    //creating a lambda function for the buffer
    [&total](const ssize_t numbytes) -> void
    {
        total+=numbytes;
    },
    //lambda for c-data
    [&textsize, &loc](const std::string characters) -> void
    {
        //counting the source characters and the lines of codes
        textsize += characters.size();
        loc += std::count(characters.begin(), characters.end(), '\n');
    },
    //lambda for  local_names
    [&countTags](const std::string Local_name, const int depth) -> void
    {
    //counting the local names
        if (Local_name == "unit" && depth > 1)
            ++countTags["file"];
        
        else if (Local_name != "file")
            ++countTags[Local_name];
    },
    //lambda for attributes
    [&countAttributes](const std::string value) -> void
    {
        //counting the attributes names
        ++countAttributes[value];
    },
     //lambda for lines of code
    [&loc](const std::string characters) -> void
    {
        loc += std::count(characters.begin(), characters.end(), '\n');
    },
    //lambda for source characters 
    [&textsize]() -> void
    {
        //counting the source characters
        ++textsize;
    }
);

    parser.handleXML();

    std::cout << "bytes: " << total << '\n';
    std::cout << "files: " << countTags["file"] << '\n';
    std::cout << "LOC: " << loc << '\n';
    std::cout << "source characters: " << textsize << '\n';
    std::cout << "classes: " << countTags["class"] << '\n';
    std::cout << "functions: " << countTags["function"] << '\n';
    std::cout << "declarations: " << countTags["decl"] << '\n';
    std::cout << "expressions: " << countTags["expr"] << '\n';
    std::cout << "comments: " << countTags["comment"] << '\n';
    std::cout << "returns: " << countTags["return"] << '\n';
    std::cout << "block comments "<< countAttributes["block"] <<'\n';
    return 0;
}

