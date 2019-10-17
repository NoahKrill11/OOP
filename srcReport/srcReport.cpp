/*
    srcReport.cpp

    Produces a report that counts the number of statements, declarations, etc. of C++ programs
*/

#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "XMLParser.hpp"

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
    long total = 0;
    int return_count=0;
    int block_count=0;
    xmlparser parser;
    while (parser.getNumbytes() !=0)
    {
        if (parser.bufferCheck())
        {
            // fill the buffer
           parser.fillTheBuffer();
            total+=parser.getNumbytes();
        }
        
        else if (parser.declarationCheck())
        {
            // parse XML declaration
            parser.declartionParse();
        }
        
        else if (parser.commentCheck())
        {
            // parse XML comment
            parser.commentParse();
        }
        
        else if (parser.cDataCheck())
        {
            // parse CDATA
            std::string characters;
            parser.cDataParse(characters);
            loc += std::count(characters.begin(), characters.end(), '\n');
            textsize += characters.size();
        }
        
        else if (parser.endTagCheck())
        {
            // parse end tag
            std::string qname, prefix, local_name;
            parser.endTagParse(qname, prefix, local_name);
        }
        
        else if (parser.startTagCheck())
        {
            // parse start tag
            std::string local_name, prefix, qname;
            parser.startTagParse(local_name, prefix, qname);
            if (local_name == "expr")
                ++expr_count;
            else if (local_name == "function")
                ++function_count;
            else if (local_name == "decl")
                ++decl_count;
            else if (local_name == "class")
                ++class_count;
            else if (local_name == "unit" && parser.getdepth() > 1)
                ++file_count;
            else if (local_name == "comment")
                ++comment_count;
            else if (local_name == "return" )
                ++return_count;
        }
       
        else if (parser.endStartTagCheck())
        {
            // end start tag
            parser.endStartTag();
        }
        
        else if (parser.emptyElementCheck())
        {
            // end empty element
            parser.emptyElement();
        }
        
        else if (parser.namespaceCheck())
        {
            // parse namespace
            std::string uri, prefix;
            parser.namespaceParse(uri, prefix);
        }
        
        else if (parser.attCheck())
        {
            // parse attribute
            std::string local_name, prefix, value, qname;
            parser.attParse(local_name, prefix, value, qname);
           if (value == "block")
               ++block_count;
        }
        
        else
        {
             // parse characters
       std::vector<char>::iterator pcur;
       parser.charcterParse(pcur);
       //adjust values, this if/else is for the sole purpose of decoupling
       if (*pcur != '&')
           {
               std::string characters(pcur, parser.getpc());
               loc += std::count(characters.begin(), characters.end(), '\n');
           }
       else
           ++textsize;
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
    std::cout << "block comments "<< block_count <<'\n';
    
    return 0;
}
