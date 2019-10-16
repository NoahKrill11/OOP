/*
    XMLParser.hpp
    
    Defining the the free functions for use in other cpp files
*/


#ifndef XMLPARSER
#define XMLPARSER

class xmlparser
{
private:
    //Private variables for use in functions
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
    std::function<void()> statements;
    std::function<void()> statements2;
    std::function<void()> statements3;
    

    
public:
    
     //Constructor for calling the parser with std::functions as variables
    xmlparser(std::function<void()>, std::function<void()>,std::function<void()>);

     //Deconstructor
    ~xmlparser();
    
    //Function for getting pc value
    //This is an inline function so its best to keep it in hpp file to get the pc
    std::vector<char>::iterator getpc()
    {return pc;}
     
    //This is an inline function so its best to keep it in hpp file to get the buffer
    std::vector<char>getbuffer()
    {return buffer;}
    
    size_t getsize() { return characters.size();}
  
    //Function for getting the depth value
    int getdepth()
    {return depth;}
    
    //Function for returning the numbytes value
    ssize_t getNumbytes()
    
    {return numbytes;}
      //Function for returning the local name
    std::string getLocal_name()
    {
        return local_name;
    }
      //Function for returning the characters
    std::string getCharacters()
    {
        return characters;
    }
      //Function for returning the pcur
   std::vector<char>::iterator getpcur()
    {
        return pcur;
    }
      //Function for returning the value
    std::string getvalue()
    {
        return value;
    }
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

