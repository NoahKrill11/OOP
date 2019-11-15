/*
    XMLParser.cpp
    Defintions of free functions to be used for XML/srcML Parsing
*/

#include <iostream>
#include <iterator>
#include <string>
#include <unistd.h>
#include <vector>
#include "XMLParser.hpp"
  
/*
 Constructor for calling the parser
 Standard functions and will be called later in the function if need be
*/
XMLParser::XMLParser()
{
    buffer.resize(16 * 1024);
    depth = 0;
    intag=false;
    numbytes = -1;
    pc = buffer.end();
}
//The next 10 functions are used for overloading in another class
void XMLParser::XMLDeclaration(const std::string version, const std::string encoding, const std::string standalone){}

void XMLParser::virtualFillBuffer(const ssize_t numbytes){}

void XMLParser::virtualCData(const std::string characters){}

void XMLParser::virtualStartTag(const std::string local_name, const std::string qname, const std::string prefix, const int depth){}

void XMLParser::virtualEndTag(const std::string Local_name, const std::string qname, const std::string prefix){}

void XMLParser::virtualAttributes(const std::string qname, const std::string local_name, const std::string prefix, const std::string value){}

void XMLParser::XMLComments(const std::string content){}

void XMLParser::XMLNamespace(const std::string prefix, const std::string name, const std::string uri){}

void XMLParser::virtualCharacters(const std::string content, const bool isloc){}

//Function for checking if the conditions are met to fill the buffer
bool XMLParser::bufferCheck()
{
    return (pc == buffer.end() ||
    (*pc == '<' && std::find(pc, buffer.end(), '>') == buffer.end()) ||
    (!intag && *pc == '&' && std::distance(pc, buffer.end()) <= strlen("&amp;")) ||
    (!intag && *pc == '<' && std::distance(pc, buffer.end()) > 1 && *std::next(pc) == '!'));
}

//Function for checking if the conditions are met to parse the declaration
bool XMLParser::declarationCheck()
{
    return (*pc == '<' && *std::next(pc) == '?');
}
//Function for checking if the conditions are met to parse the comments
bool XMLParser::commentCheck()
{
    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '-' && *std::next(pc, 3) == '-');
}

//Function for checking if the conditions are met to parse the cdata
bool XMLParser::cDataCheck()
{
    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '[');
}

//Function for checking if the conditions are met to parse the namespaces
bool XMLParser::namespaceCheck()
{
    return (intag && *pc != '>' && *pc != '/' && std::distance(pc, buffer.end()) > strlen("xmlns") &&
            std::string(pc, std::next(pc, strlen("xmlns"))) == "xmlns" && (*std::next(pc, strlen("xmlns")) == ':' || *std::next(pc, strlen("xmlns")) == '='));
}

//Function for checking if the conditions are met to parse the start tags
bool XMLParser::startTagCheck()
{
    return (*pc == '<' && *std::next(pc) != '/' && *std::next(pc) != '?');
}

//Function for checking if the conditions are met to parse the end tags
bool XMLParser::endTagCheck()
{
    return (*pc == '<' && *std::next(pc) == '/');
}

//Function for checking if the conditions are met to parse the empty elements
bool XMLParser::emptyElementCheck()
{
    return (intag && *pc == '/' && *std::next(pc) == '>');
}

//Function for checking if the conditions are met to parse the attributes
bool XMLParser::attCheck()
{
    return (intag && *pc != '>' && *pc != '/');
}

//Function for checking if the conditions are met to parse the end of the start tag
bool XMLParser::endStartTagCheck()
{
    return (intag && *pc == '>');
}
void XMLParser::handleXML()
{
    while (numbytes!=0)
    {
        if (bufferCheck())
        {
            // fill the buffer
            fillTheBuffer();
        }

        else if (declarationCheck())
        {
            // parse XML declaration
            declartionParse();
        }

        else if (commentCheck())
        {
            // parse XML comment
            commentParse();
        }

        else if (cDataCheck())
        {
            // parse CDATA
            cDataParse();
        }

        else if (endTagCheck())
        {
            //parse end tag
            endTagParse();
        }

        else if (startTagCheck())
        {
            //parse start tag
            startTagParse();
        }

        else if (endStartTagCheck())
        {
            //end start tag
            endStartTag();
        }

        else if (emptyElementCheck())
        {
            //end empty element
            emptyElement();
        }

        else if (namespaceCheck())
        {
            //parse namespace
            namespaceParse();
        }

        else if (attCheck())
        {
            //parse attribute
            attParse();
        }

        else
        {
            //parse characters
            charcterParse();
        }
    }
}
//Function fills the buffer
void XMLParser::fillTheBuffer()
{
    auto d = std::distance(pc, buffer.end());
    std::copy(pc, buffer.end(), buffer.begin());
    
    while (((numbytes = read(0, buffer.data() + d, buffer.size() - d)) == -1) && (errno == EINTR))
        {}
    //numytes is negitive meaning that file is not properly filled out or made
    if (numbytes == -1)
        exit (1);
    
    numbytes += d;
    
    //Resizes the buffer if the buffer is bigger then the numbytes
    if (numbytes < buffer.size())
        buffer.resize(numbytes);
    
    pc = buffer.begin();
    virtualFillBuffer(numbytes);
}

//Function parses the declarations in the document
void XMLParser::declartionParse()
{
    auto endpc = std::find(pc, buffer.end(), '>');//finds the end tag
    
    pc = std::next(endpc);
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == buffer.end())
        std::exit(1);
    XMLDeclaration("1.0", "UTF-8", "no");
}

//Function parses the comments in the document
void XMLParser::commentParse()
{
    const std::string endcomment = "-->";
    auto endpc = std::search(pc, buffer.end(), endcomment.begin(), endcomment.end());
    
    pc = std::next(endpc, strlen("-->"));
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == buffer.end())
        std::exit(1);
    
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    
    std::string content;
    XMLComments(content);
}

//Function parses the cdata in the document
void XMLParser::cDataParse()
{
    const std::string endcdata = "]]>";
    auto endpc = std::search(pc, buffer.end(), endcdata.begin(), endcdata.end());
    
    std::advance(pc, strlen("<![CDATA["));
    
    std::string characters(pc, endpc);
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == buffer.end())
        std::exit(1);
    
    pc = std::next(endpc, strlen("]]>"));
    
    //calling on the the std::functions that was used in the constructor
    virtualCData(characters);
}

//Function parses the end tags in the document
void XMLParser::endTagParse()
{
    auto endpc = std::find(pc, buffer.end(), '>');
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == buffer.end())
        std::exit(1);
    
    std::advance(pc, 2);
    
    auto pnameend = std::find_if(pc, buffer.end(), [] (char c) { return std::isspace(c) || c == '>' || c == '/'; });
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pnameend == buffer.end())
        std::exit(1);
    
    const std::string qname(pc, pnameend);
    const auto colonpos = qname.find(':');
    const std::string prefix = colonpos != std::string::npos ? qname.substr(0, colonpos) : std::string("");
    const std::string local_name = colonpos != std::string::npos ? qname.substr(colonpos + 1) : qname;
    
    pc = std::next(endpc);
    
    --depth;
    
    virtualEndTag(local_name, qname, prefix);
}

//Function parses start tags in the document
void XMLParser::startTagParse()
{
    auto endpc = std::find(pc, buffer.end(), '>');
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == buffer.end())
        exit (1);
    
    std::advance(pc, 1);
    
    if (*std::prev(endpc) != '/')
    ++depth;
    
    auto pnameend = std::find_if(pc, buffer.end(), [] (char c) { return std::isspace(c) || c == '>' || c == '/'; });
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pnameend == buffer.end())
        exit(1);
    
    const std::string qname(pc, pnameend);
    const auto colonpos = qname.find(':');
    prefix = colonpos != std::string::npos ? qname.substr(0, colonpos) : std::string("");
    local_name = colonpos != std::string::npos ? qname.substr(colonpos + 1) : qname;
    
    pc = pnameend;
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    intag = true;
    
    //calling on the the std::functions that was used in the constructor
    virtualStartTag(local_name, qname, prefix, depth);
}

//Function parses the attributes in the document
void XMLParser::attParse()
{
    auto pnameend = std::find(pc, buffer.end(), '=');
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pnameend == buffer.end())
        std::exit(1);
    
    const std::string qname(pc, pnameend);
    const auto colonpos = qname.find(':');
    prefix = colonpos != std::string::npos ? qname.substr(0, colonpos) : std::string("");
    local_name = colonpos != std::string::npos ? qname.substr(colonpos + 1) : qname;
    
    pc = std::next(pnameend);
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pc == buffer.end())
        std::exit(1);
    
    char delim = *pc;
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 2
    */
    if (delim != '"' && delim != '\'')
        std::exit(2);
    
    std::advance(pc, 1);
    
    auto pvalueend = std::find(pc, buffer.end(), delim);
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pvalueend == buffer.end())
        std::exit(1);
    
    const std::string temp2(pc, pvalueend);
    value=temp2;
    
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    
    //calling on the the std::functions that was used in the constructor
    virtualAttributes(qname, local_name, prefix, value);
}

//Function parses the namespace in the document
void XMLParser::namespaceParse()
{
    
    std::advance(pc, strlen("xmlns"));
    
    auto pnameend = std::find(pc, buffer.end(), '=');
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pnameend == buffer.end())
        std::exit(1);
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pc == buffer.end())
        std::exit(1);
    
    pc = pnameend;
    
    if (*pc == ':')
        {
            std::advance(pc, 1);
            prefix.assign(pc, pnameend);
        }
    
    pc = std::next(pnameend);
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pc == buffer.end())
        std::exit(1);
    
    char delim = *pc;
    
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 2
    */
    if (delim != '"' && delim != '\'')
        std::exit(2);
    
    std::advance(pc, 1);
    
    auto pvalueend = std::find(pc, buffer.end(), delim);
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pvalueend == buffer.end())
        std::exit(1);
    
    const std::string uri(pc, pvalueend);
    
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    
    std::string name;
    XMLNamespace(prefix, name, uri);
}

//Function parses the characters in the document
void XMLParser::charcterParse()
{
    bool isloc = false;
    pcur = pc;
    pc = std::find_if(pc, buffer.end(), [](char c) { return c == '<' || c == '&'; });
    
    if (depth > 0)
    {
        if (*pcur != '&')
            {
                std::string characters(pcur,pc);
                isloc=true;
                //calling on the the std::functions that was used in the constructor
                virtualCharacters(characters, isloc);
                return;
            }
        else
        {
            if (*std::next(pc) == 'l' && *std::next(pc, 2) == 't' && *std::next(pc, 3) == ';')
                {
                characters += '<';
                    
                std::advance(pc, strlen("&lt;"));
                }
            else if (*std::next(pc) == 'g' && *std::next(pc, 2) == 't' && *std::next(pc, 3) == ';')
                {
                characters += '>';
                    
                std::advance(pc, strlen("&gt;"));
                }
            else if (*std::next(pc) == 'a' && *std::next(pc, 2) == 'm' && *std::next(pc, 3) == 'p' && *std::next(pc, 4) == ';')
                {
                characters += '&';
                    
                std::advance(pc, strlen("&amp;"));
                }
            else
                {
                characters += '&';
                    
                std::advance(pc, 1);
                }
        
            //calling on the the std::functions that was used in the constructor
            virtualCharacters(characters, isloc);
        }
    }
}

//Function parses the empty elements in the document
void XMLParser::emptyElement()
    {
    std::advance(pc, 2);
        
    intag=false;
    }

//Function parses the end of the start tags in the document
void XMLParser::endStartTag()
    {
    std::advance(pc, 1);
        
    intag=false;
    }
//decontructor
XMLParser::~XMLParser()
    {
    
    }
