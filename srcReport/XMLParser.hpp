/*
    XMLParser.hpp
    
    Defining the the free functions for use in other cpp files
*/


#ifndef XMLPARSER
#define XMLPARSER

class xmlparser
{
private:
    int depth = 0;
    bool intag = false;
    ssize_t numbytes = -1;
    std::vector<char>::iterator pc;
    std::vector<char> buffer;
    
public:
    /*Constructor for calling the parser
     @param BUFFERSIZE is the constant value of how big the buffer is
     */
    xmlparser();
    
    /* Deconstructor*/
    ~xmlparser();
    
    //Function for getting pc value
    //This is an inline function so its best to keep it in hpp file
    std::vector<char>::iterator getpc()
    {return pc;}
    
    //Function for getting the depth value
    int getdepth()
    {return depth;}
    
    //Function for returning the numbytes value
    ssize_t getNumbytes()
    {return numbytes;}
    
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

    //Function for filling the buffer
    void fillTheBuffer();

    //Function for parsing the declarations in the document
    void declartionParse();

    //Function for parsing the comments in the document
    void commentParse();

    //Function for parsing the cdata in the document
    void cDataParse(std::string&);

    //Function for parsing the end tags in the document
    void endTagParse(std::string&, std::string&, std::string&);

    //Function for parsing the start tags in the document
    void startTagParse(std::string&, std::string&, std::string&);

    //Function for parsing the atributes in the document
    void attParse(std::string&,std::string&,std::string&,std::string&);

    //Function for parsing the characters in the document
    void charcterParse(std::vector<char>::iterator&);

    //Function for parsing the namespace in the document
    void namespaceParse(std::string&, std::string&);

    //Function for parsing the empty elements in the document
    void emptyElement();

    //Function for parsing the end of start tags in the document
    void endStartTag();
};
#endif//XMLPARSER

