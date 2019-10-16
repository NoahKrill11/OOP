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
    //xmlparser parser;
    std::unordered_map<std::string,int> count{};
    xmlparser parser([&]() -> void {
    if (parser.getLocal_name() == "unit" && parser.getdepth() > 1)
        ++count["file"];
    else if (parser.getLocal_name() != "block" && parser.getLocal_name() != "file")
        ++count[parser.getLocal_name()];
    }
    );
    
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
            
            parser.cDataParse();
            count["textsize"] += parser.getsize();
            count["loc"] += std::count(parser.getCharacters().begin(),parser.getCharacters().end(), '\n');
           
        }
        
        else if (parser.endTagCheck())
        {
            // parse end tag
   
            parser.endTagParse();
        }
        
        else if (parser.startTagCheck())
        {
            // parse start tag
        
            parser.startTagParse();
            
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
           
            parser.namespaceParse();
        }
        
        else if (parser.attCheck())
        {
            // parse attribute
            parser.attParse();
           if (parser.getvalue() == "block")
               ++count["block"];
            
        }
        
        else
        {
             // parse characters
       parser.charcterParse();
       //adjust values, this if/else is for the sole purpose of decoupling
       if (*parser.getpcur() != '&')
           {
               std::string characters(parser.getpcur(), parser.getpc());
               count["loc"] += std::count(characters.begin(), characters.end(), '\n');
           }
       else
           ++count["textsize"];
        }
         
    }
    
    std::cout << "bytes: " << total << '\n';
    std::cout << "files: " << count["file"] << '\n';
    std::cout << "LOC: " << count["loc"] << '\n';
    std::cout << "source characters: " << count["textsize"] << '\n';
    std::cout << "classes: " << count["class"] << '\n';
    std::cout << "functions: " << count["function"] << '\n';
    std::cout << "declarations: " << count["decl"] << '\n';
    std::cout << "expressions: " << count["expr"] << '\n';
    std::cout << "comments: " << count["comment"] << '\n';
    std::cout << "returns: " << count["return"] << '\n';
    std::cout << "block comments "<< count["block"] <<'\n';
    return 0;
}

