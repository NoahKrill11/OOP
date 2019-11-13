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
class srcReport : public XMLParser
{
    private:
        long total = 0;
        int textsize = 0;
        int loc = 0;
        std::unordered_map<std::string, int> countTags;
        std::unordered_map<std::string, int> countAttributes;
    
        //function for adding total number of bytes
        void virtualFillBuffer(const ssize_t numbytes)
        {
            total+=numbytes;
        }
    
        //function for counting the lines of code and source characters
        void virtualCData(const std::string content)
        {
            textsize += content.size();
            loc += std::count(content.begin(), content.end(), '\n');
        }
    
        //function for counting the start tags
        void virtualStartTag(const std::string Local_name, const std::string qname, const std::string prefix, const int depth)
        {
            //counting the local names
            if (Local_name == "unit" && depth > 1)
                ++countTags["file"];
        
            else if (Local_name != "file")
                ++countTags[Local_name];
        }
    
        //function for counting attributes
        void virtualAttributes(const std::string qname, const std::string local_name, const std::string prefix, const std::string value)
        {
            ++countAttributes[value];
        }
        
        //function for counting lines of code
        void virtualCharacters(const std::string content, const bool isloc)
        {
            if (isloc==true)
            loc += std::count(content.begin(), content.end(), '\n');
            else
            ++textsize;
        }
    
    public:
    
        //function for getting total
        const long getTotal()
        {
            return total;
        }
    
        //function for getting textsize
        const int getTextSize()
        {
            return textsize;
        }
    
        //function for getting lines of code
        const int getLoc()
        {
            return loc;
        }
        
        //function for getting tagname
        const int getTags(std::string tagname)
        {
            return countTags[tagname];
        }
        //function for getting attributes
        const int getAttributes(std::string attname)
        {
            return countAttributes[attname];
        }
};

int main()
{

    srcReport report;

    report.handleXML();

    std::cout << "bytes: " << report.getTotal() << '\n';
    std::cout << "files: " << report.getTags("file") << '\n';
    std::cout << "LOC: " << report.getLoc() << '\n';
    std::cout << "source characters: " << report.getTextSize() << '\n';
    std::cout << "classes: " << report.getTags("class") << '\n';
    std::cout << "functions: " << report.getTags("function") << '\n';
    std::cout << "declarations: " << report.getTags("decl") << '\n';
    std::cout << "expressions: " << report.getTags("expr") << '\n';
    std::cout << "comments: " << report.getTags("comment") << '\n';
    std::cout << "returns: " << report.getTags("return") << '\n';
    std::cout << "block comments "<< report.getAttributes("block") <<'\n';
    return 0;
}

