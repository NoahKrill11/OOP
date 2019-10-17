/*
    srcReport.cpp

    Produces a report that counts the number of statements, declarations, etc. of C++ programs

    Input is a srcML form of the code.

    Note: Does not handle XML comments
*/

#include <iostream>
#include <iterator>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include "XMLParser.hpp"

const int BUFFER_SIZE = 16 * 1024;

int main()
{
    int textsize = 0;
    int loc = 0;
    int expr_count = 0;
    int function_count = 0;
    int class_count = 0;
    int file_count = 0;
    int decl_count = 0;
    int comment_count = 0;
    int depth = 0;
    long total = 0;
    int return_count=0;
    //int block_count=0;
    bool intag = false;
    std::vector<char> buffer(BUFFER_SIZE);
    std::vector<char>::iterator pc = buffer.end();
    ssize_t numbytes = -1;
    while (numbytes !=0)
    {
        if (bufferCheck(buffer.end(),pc,intag))
        {
            // fill the buffer
            fillTheBuffer(buffer, pc, numbytes, BUFFER_SIZE,total);
        }
        else if (declarationCheck(pc))
        {
            // parse XML declaration
            declartionParse(buffer.end(), pc);
        }
        else if (commentCheck(pc))
        {
            // parse XML comment
            commentParse(buffer.end(), pc);
        }
        else if (cDataCheck(pc))
        {
            // parse CDATA
            cDataParse(buffer.end(), pc,textsize,loc);
        }
        else if (endTagCheck(pc))
        {
            // parse end tag
            std::string qname, prefix, local_name;
            endTagParse(pc, buffer.end(), depth, qname, prefix, local_name);
        }
        else if (startTagCheck(pc))
        {
            // parse start tag
            std::string local_name;
            std::string prefix, qname;
            startTagParse(buffer.end(),pc, depth, local_name, prefix,qname);
            if (local_name == "expr")
                ++expr_count;
            else if (local_name == "function")
                ++function_count;
            else if (local_name == "decl")
                ++decl_count;
            else if (local_name == "class")
                ++class_count;
            else if (local_name == "unit" && depth > 1)
                ++file_count;
            else if (local_name == "comment")
                ++comment_count;
            else if (local_name == "return" )
                ++return_count;
            intag = true;
        }
        else if (endStartTagCheck(pc, intag))
        {
            // end start tag
            endStartTag(pc, intag);
        }
        else if (emptyElementCheck(pc,intag))
        {
            // end empty element
            emptyElement(pc, intag);
        }
        else if (namespaceCheck(intag, buffer.end(),pc))
        {
            // parse namespace
            std::string uri,prefix;
            namespaceParse(pc, buffer.end(),uri,prefix);
        }
        else if (attCheck(intag, pc))
        {
            // parse attribute
            std::string local_name, prefix, value, qname ;
            
            attParse(buffer.end(), pc, local_name, prefix, value, qname);
           // if (value == "block")
              //  ++block_count;
        }
        else
        {
            //parse the rest of the characters
            charcterParse(pc, buffer.end(), depth, loc, textsize);
        }
    }
    std::cout << "bytes: " << total << '\n';
    std::cout << "files: " << file_count << '\n';
    std::cout << "LOC: " << loc << '\n';
    std::cout << "source characters: " << textsize << '\n';
    std::cout << "classes: " << class_count << '\n';
    std::cout << "functions: " << function_count << '\n';
    std::cout << "declarations: " << decl_count << '\n';
    std::cout << "expressions: " << expr_count << '\n';
    std::cout << "comments: " << comment_count << '\n';
    std::cout << "returns: " << return_count << '\n';
   // std::cout << "block comments "<< block_count <<'\n';
    return 0;
}
