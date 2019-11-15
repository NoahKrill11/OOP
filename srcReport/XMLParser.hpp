/*
    XMLParser.hpp
    
    Defining the the free functions for use in other cpp files
*/


#ifndef XMLPARSER
#define XMLPARSER

class XMLParser
{
private:
    int depth;
    bool intag;
    ssize_t numbytes;
    std::vector<char>::iterator pc;
    std::vector<char>::iterator pcur;
    std::vector<char> buffer;
    std::string qname;
    std::string prefix;
    std::string local_name;
    std::string characters;
    std::string value;
    std::string uri;
    std::function<void(const ssize_t)> parsetotal;
    std::function<void(const std::string)> cData;
    std::function<void(const std::string, const int)> local;
    std::function<void(const std::string)> attribute;
    std::function<void(const std::string)> loc;
    std::function<void()> sourceChar;

public:
    
     //Constructor for calling the parser with std::functions as variables
    XMLParser(std::function<void(const ssize_t)>, std::function<void(const std::string)>, std::function<void(const std::string, const int)>,
    std::function<void(const std::string)>, std::function<void(const std::string)>, std::function<void()>);
    
    //Function for checking if the conditions are met to fill the buffer
    bool bufferCheck();

    //Function for checking if the conditions are met parse the declarations
    bool declarationCheck();

    //Function for checking if the conditions are met parse the comments
    bool commentCheck();

    //Function for checking if the conditions are met parse the cdata
    bool cDataCheck();

    //Function for checking if the conditions are met parse the end tags
    bool endTagCheck();

    //Function for checking if the conditions are met parse the namespace
    bool namespaceCheck();

    //Function for checking if the conditions are met parse the start tag
    bool startTagCheck();

    //Function for checking if the conditions are met parse the end of the start tag
    bool endStartTagCheck();

    //Function for checking if the conditions are met parse the empty element
    bool emptyElementCheck();

    //Function for checking if the conditions are met parse the attributes
    bool attCheck();
    
    void handleXML();
    
    //Function for filling the buffer
    void fillTheBuffer();

    //Function for parsing the declarations in the document
    void declartionParse();

    //Function for parsing the comments in the document
    void commentParse();

    //Function for parsing the cdata in the document
    void cDataParse();

    //Function for parsing the end tags in the document
    void endTagParse();

    //Function for parsing the start tags in the document
    void startTagParse();

    //Function for parsing the atributes in the document
    void attParse();

    //Function for parsing the characters in the document
    void charcterParse();

    //Function for parsing the namespace in the document
    void namespaceParse();

    //Function for parsing the empty elements in the document
    void emptyElement();

    //Function for parsing the end of start tags in the document
    void endStartTag();
};
#endif//XMLPARSER

