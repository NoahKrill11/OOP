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
 set up vaiables for use at beggining of the program
*/


xmlparser::xmlparser(std::function<void()> count) : statements(count)
{
    const int BUFFER_SIZE = 16 * 1024;
    buffer.resize(BUFFER_SIZE);
    depth = 0;
    intag=false;
    numbytes = -1;
    pc=buffer.end();
}

/*
 Deconstructor to deconstruct the parser
 */
  xmlparser::~xmlparser()
{}

/*
 Function for checking if the conditions are met to fill the buffer
 */
bool xmlparser::bufferCheck()
{
    return (pc == buffer.end() ||
    (*pc == '<' && std::find(pc, buffer.end(), '>') == buffer.end()) ||
    (!intag && *pc == '&' && std::distance(pc, buffer.end()) <= strlen("&amp;")) ||
            (!intag && *pc == '<' && std::distance(pc, buffer.end()) > 1 && *std::next(pc) == '!'));
}

/*
Function for checking if the conditions are met to parse the declaration
*/
bool xmlparser::declarationCheck()
{
    return (*pc == '<' && *std::next(pc) == '?');
}

/*
Function for checking if the conditions are met to parse the comments
*/
bool xmlparser::commentCheck()
{
    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '-' && *std::next(pc, 3) == '-');
}

/*
Function for checking if the conditions are met to parse the cdata
*/
bool xmlparser::cDataCheck()
{
    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '[');
}

/*
Function for checking if the conditions are met to parse the namespaces
*/
bool xmlparser::namespaceCheck()
{
   return (intag && *pc != '>' && *pc != '/' && std::distance(pc, buffer.end()) > strlen("xmlns") && std::string(pc, std::next(pc, strlen("xmlns"))) == "xmlns"
      && (*std::next(pc, strlen("xmlns")) == ':' || *std::next(pc, strlen("xmlns")) == '='));
}

/*
Function for checking if the conditions are met to parse the start tags
*/
bool xmlparser::startTagCheck()
{
    return (*pc == '<' && *std::next(pc) != '/' && *std::next(pc) != '?');
}

/*
Function for checking if the conditions are met to parse the end tags
*/
bool xmlparser::endTagCheck()
{
    return (*pc == '<' && *std::next(pc) == '/');
    
}

/*
Function for checking if the conditions are met to parse the empty elements
*/
bool xmlparser::emptyElementCheck()
{
    return (intag && *pc == '/' && *std::next(pc) == '>');
}

/*
Function for checking if the conditions are met to parse the attributes
*/
bool xmlparser::attCheck()
{
    return (intag && *pc != '>' && *pc != '/');
}

/*
Function for checking if the conditions are met to parse the end of the start tag
*/
bool xmlparser::endStartTagCheck()
{
    return (intag && *pc == '>');
}

/*
 Function fills the buffer
*/
void xmlparser::fillTheBuffer()
{
    auto d = std::distance(pc, buffer.end());
    std::copy(pc, buffer.end(), buffer.begin());
    while (((numbytes = read(0, buffer.data() + d, buffer.size() - d)) == -1) && (errno == EINTR))
    { }
    //numytes is negitive meaning that file is not properly filled out or made
    if (numbytes == -1)
        exit (1);
    numbytes += d;
   //Resizes the buffer if the buffer is bigger then the numbytes
    if (numbytes < buffer.size())
        buffer.resize(numbytes);
    pc = buffer.begin();
    
}

/*
Function parses the declarations in the document
*/
void xmlparser::declartionParse()
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
}

/*
Function parses the comments in the document
*/
void xmlparser::commentParse()
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
}

/*
Function parses the cdata in the document
*/
void xmlparser::cDataParse()
{
    const std::string endcdata = "]]>";
    const std::string endcomment = "-->";
    auto endpc = std::search(pc, buffer.end(), endcdata.begin(), endcdata.end());
    std::advance(pc, strlen("<![CDATA["));
    std::string temp(pc, endpc);
    characters=temp;
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == buffer.end())
        std::exit(1);
    pc = std::next(endpc, strlen("]]>"));
}

/*
Function parses the end tags in the document
*/
void xmlparser::endTagParse ()
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
    const std::string temp(pc, pnameend);
    qname=temp;
    const auto colonpos = qname.find(':');
    const std::string temp2 = colonpos != std::string::npos ? qname.substr(0, colonpos) : std::string("");
    prefix=temp2;
    const std::string temp3 = colonpos != std::string::npos ? qname.substr(colonpos + 1) : qname;
    local_name=temp3;
    pc = std::next(endpc);
    --depth;
}

/*
Function parses start tags in the document
*/
void xmlparser::startTagParse()
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
    const std::string temp(pc, pnameend);
    qname=temp;
    const auto colonpos = qname.find(':');
    prefix = colonpos != std::string::npos ? qname.substr(0, colonpos) : std::string("");
    local_name = colonpos != std::string::npos ? qname.substr(colonpos + 1) : qname;
    pc = pnameend;
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
    intag = true;
    statements();
}

/*
Function parses the attributes in the document
*/
void xmlparser::attParse()
{
  
    auto pnameend = std::find(pc, buffer.end(), '=');
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pnameend == buffer.end())
        std::exit(1);
    const std::string temp(pc, pnameend);
    qname = temp;
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
  
}

/*
Function parses the namespace in the document
*/
void xmlparser::namespaceParse()
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
    const std::string temp(pc, pvalueend);
    uri=temp;
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, buffer.end(), [] (char c) { return std::isspace(c); });
}

 /*
 Function parses the characters in the document
 */
void xmlparser::charcterParse()
{
    pcur = pc;
    pc = std::find_if(pc, buffer.end(), [](char c) { return c == '<' || c == '&'; });

    if (depth > 0)
    {
        if (*pcur != '&')
        {
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
        }
    }
}

/*
Function parses the empty elements in the document
*/
void xmlparser::emptyElement()
{
    std::advance(pc, 2);
    intag=false;
}

/*
Function parses the end of the start tags in the document
*/
void xmlparser::endStartTag()
{
    std::advance(pc, 1);
    intag=false;
}
