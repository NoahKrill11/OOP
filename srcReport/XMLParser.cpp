#include <iostream>
#include <iterator>
#include <string>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <cstring>
#include "XMLParser.hpp"
  
/*
 Function for checking if the conditions are met to fill the buffer
 @param endbuffer; iterator to the end position of the buffer
 @param pc; iterator stored to current spot
 @param intag; boolen that determines if we parsing something inside of the tag
 */
bool bufferCheck(std::vector<char>::iterator endBuffer, std::vector<char>::iterator &pc, bool &intag)
{
return (pc == endBuffer ||
(*pc == '<' && std::find(pc, endBuffer, '>') == endBuffer) ||
(!intag && *pc == '&' && std::distance(pc, endBuffer) <= strlen("&amp;")) ||
        (!intag && *pc == '<' && std::distance(pc, endBuffer) > 1 && *std::next(pc) == '!'));
}
/*
Function for checking if the conditions are met to parse the declaration
@param pc; iterator stored to current spot
*/
bool declarationCheck(std::vector<char>::iterator &pc)
{
    return (*pc == '<' && *std::next(pc) == '?');
   
}
/*
Function for checking if the conditions are met to parse the comments
@param pc; iterator stored to current spot
*/
bool commentCheck(std::vector<char>::iterator &pc)
{
    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '-' && *std::next(pc, 3) == '-');
}
/*
Function for checking if the conditions are met to parse the cdatas
@param pc; iterator stored to current spot
*/
bool cDataCheck(std::vector<char>::iterator &pc)
{
    return (*pc == '<' && *std::next(pc) == '!' && *std::next(pc, 2) == '[');
}
/*
Function for checking if the conditions are met to parse the namespaces
@param endbuffer; iterator to the end position of the buffer
@param pc; iterator stored to current spot
@param intag; boolen that determines if we parsing something inside of the tag
*/
bool namespaceCheck(bool &intag, std::vector<char>::iterator endBuffer, std::vector<char>::iterator &pc)
{
   return (intag && *pc != '>' && *pc != '/' && std::distance(pc, endBuffer) > strlen("xmlns") && std::string(pc, std::next(pc, strlen("xmlns"))) == "xmlns"
      && (*std::next(pc, strlen("xmlns")) == ':' || *std::next(pc, strlen("xmlns")) == '='));
}
/*
Function for checking if the conditions are met to parse the start tags
@param pc; iterator stored to current spot
*/
bool startTagCheck(std::vector<char>::iterator &pc)
{
    return (*pc == '<' && *std::next(pc) != '/' && *std::next(pc) != '?');
}
/*
Function for checking if the conditions are met to parse the end tags
@param pc; iterator stored to current spot
*/
bool endTagCheck(std::vector<char>::iterator &pc)
{
    return (*pc == '<' && *std::next(pc) == '/');
    
}
/*
Function for checking if the conditions are met to parse the empty elements
@param pc; iterator stored to current spot
@param intag; boolen that determines if we parsing something inside of the tag
*/
bool emptyElementCheck(std::vector<char>::iterator &pc, bool &intag)
{
    return (intag && *pc == '/' && *std::next(pc) == '>');
   
}
/*
Function for checking if the conditions are met to parse the attributes
@param pc; iterator stored to current spot
@param intag; boolen that determines if we parsing something inside of the tag
*/
bool attCheck(bool &intag, std::vector<char>::iterator &pc)
{
    return (intag && *pc != '>' && *pc != '/');
}
/*
Function for checking if the conditions are met to parse the end of the start tag
@param pc iterator stored to current spot
@param intag boolen that determines if we parsing something inside of the tag
*/
bool endStartTagCheck(std::vector<char>::iterator &pc, bool &intag)
{
    return (intag && *pc == '>');
}
/*
 Function fills the buffer
 @param vector; thats declared as buffer holds all the values
 @param pc; iterator stored to current spot
 @param numbytes; ssize_t that stores the number of bytes for checking
 @param BUFFER_SIZE; const int that states how big the buffer needs to be
 @param total; long stores number of characters are in the document
 */
void fillTheBuffer(std::vector<char> &buffer, std::vector<char>::iterator &pc, ssize_t &numbytes, const int BUFFER_SIZE, long &total)
{
    auto d = std::distance(pc, buffer.end());
    std::copy(pc, buffer.end(), buffer.begin());
    while (((numbytes = read(0, buffer.data() + d, BUFFER_SIZE - d)) == -1) && (errno == EINTR))
    { }
    //numytes is negitive meaning that file is not properly filled out or made
    if (numbytes == -1)
        exit (1);
    numbytes += d;
   //Resizes the buffer if the buffer is bigger then the numbytes
    if (numbytes < BUFFER_SIZE)
        buffer.resize(numbytes);
    pc = buffer.begin();
    total += numbytes;
}
/*
Function parses the declarations in the document
@param endbuffer; iterator to the end position of the buffer
@param pc; iterator stored to current spot
*/
void declartionParse(std::vector<char>::iterator endBuffer, std::vector<char>::iterator &pc)
{
    std::vector<char>::iterator endpc = std::find(pc, endBuffer, '>');//finds the end tag
    pc = std::next(endpc);
    pc = std::find_if_not(pc, endBuffer, [] (char c) { return std::isspace(c); });
   /*
   If the iterator reaches the end of the of the buffer without
   finding the relevant value something went wrong and exit with code 1
   */
    if (endpc == endBuffer)
        std::exit(1);
}
  /*
  Function parses the comments in the document
  @param endbuffer; iterator to the end position of the buffer
  @param pc; iterator stored to current spot
  */
void commentParse(std::vector<char>::iterator endBuffer, std::vector<char>::iterator &pc)
{
    const std::string endcomment = "-->";
    std::vector<char>::iterator endpc = std::search(pc, endBuffer, endcomment.begin(), endcomment.end());
    pc = std::next(endpc, strlen("-->"));
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == endBuffer)
        std::exit(1);
    pc = std::find_if_not(pc, endBuffer, [] (char c) { return std::isspace(c); });
}
/*
Function parses the cdata in the document
@param endbuffer; iterator to the end position of the buffer
@param pc; iterator stored to current spot
@param textsize int that stores source characters
@param loc; int that stores the lines of code
*/
void cDataParse(std::vector<char>::iterator endBuffer, std::vector<char>::iterator &pc, int &textsize, int &loc)
{
    const std::string endcdata = "]]>";
    const std::string endcomment = "-->";
    std::vector<char>::iterator endpc = std::search(pc, endBuffer, endcdata.begin(), endcdata.end());
    std::advance(pc, strlen("<![CDATA["));
    std::string characters(pc, endpc);
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == endBuffer)
        std::exit(1);
    textsize += characters.size();
    loc += std::count(characters.begin(), characters.end(), '\n');
    pc = std::next(endpc, strlen("]]>"));
}
/*
Function parses the end tags in the document
@param endbuffer; iterator to the end position of the buffer
@param pc; iterator stored to current spot
@param depth; int number of tags within the code
@param qname; string is a qulified name for an attribute, or tag
@param prefix; string that catches all of the prefixes in the document
@param local_name; string that stores the type of local name used
*/
void endTagParse(std::vector<char>::iterator &pc,std::vector<char>::iterator endBuffer, int &depth, std::string &qname, std::string &prefix ,std::string &local_name)
{
    std::vector<char>::iterator endpc = std::find(pc, endBuffer, '>');
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (endpc == endBuffer)
        std::exit(1);
    std::advance(pc, 2);
    std::vector<char>::iterator pnameend = std::find_if(pc, endBuffer, [] (char c) { return std::isspace(c) || c == '>' || c == '/'; });
    /*
    If the iterator reaches the end of the of the buffer without
    finding the relevant value something went wrong and exit with code 1
    */
    if (pnameend == endBuffer)
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
@param endbuffer; iterator to the end position of the buffer
@param pc; iterator stored to current spot
@param depth; int number of tags within the code
@param local_name; string that stores the type of local name used
@param prefix; string that catches all of the prefixes in the document
@param qname; string is a qulified name for an attribute, or tag
*/
void startTagParse(std::vector<char>::iterator endBuffer,std::vector<char>::iterator &pc, int &depth, std::string &local_name, std::string &prefix, std::string &qname)
{
    std::vector<char>::iterator endpc = std::find(pc, endBuffer, '>');
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
    */
    if (endpc == endBuffer)
        exit (1);
    std::advance(pc, 1);
    if (*std::prev(endpc) != '/')
        ++depth;
    const std::vector<char>::iterator pnameend = std::find_if(pc, endBuffer, [] (char c) { return std::isspace(c) || c == '>' || c == '/'; });
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
    */
    if (pnameend == endBuffer)
       exit(1);
    const std::string temp(pc, pnameend);
    qname=temp;
    const auto colonpos = qname.find(':');
    prefix = colonpos != std::string::npos ? qname.substr(0, colonpos) : std::string("");
    local_name = colonpos != std::string::npos ? qname.substr(colonpos + 1) : qname;
    pc = pnameend;
    pc = std::find_if_not(pc, endBuffer, [] (char c) { return std::isspace(c); });
}
/*
Function parses the attributes in the document
@param endbuffer; iterator to the end position of the buffer
@param pc; iterator stored to current spot
@param local_name; string that stores the type of local name used
@param prefix; string that catches all of the prefixes in the document
@param qname; string is a qulified name for an attribute, or tag
*/
void attParse(std::vector<char>::iterator endBuffer,std::vector<char>::iterator &pc, std::string &local_name,std::string &prefix,std::string &value,std::string &qname)
{
    auto pnameend = std::find(pc, endBuffer, '=');
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pnameend == endBuffer)
        std::exit(1);
    const std::string temp(pc, pnameend);
    qname = temp;
    const auto colonpos = qname.find(':');
    prefix = colonpos != std::string::npos ? qname.substr(0, colonpos) : std::string("");
    local_name = colonpos != std::string::npos ? qname.substr(colonpos + 1) : qname;
    pc = std::next(pnameend);
    pc = std::find_if_not(pc, endBuffer, [] (char c) { return std::isspace(c); });
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pc == endBuffer)
        std::exit(1);
    char delim = *pc;
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 2
    */
    if (delim != '"' && delim != '\'')
        std::exit(2);
    std::advance(pc, 1);
    auto pvalueend = std::find(pc, endBuffer, delim);
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pvalueend == endBuffer)
        std::exit(1);
    
    const std::string temp2(pc, pvalueend);
    value=temp2;
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, endBuffer, [] (char c) { return std::isspace(c); });
  
}
/*
Function parses the namespace in the document
@param pc; iterator stored to current spot
@param endbuffer; iterator to the end position of the buffer
@param ufi; string catches Uniform Resource Identifiers
@param prefix; string that catches all of the prefixes in the document
*/
void namespaceParse(std::vector<char>::iterator &pc, std::vector<char>::iterator endBuffer, std::string &uri, std::string &prefix)
{
    std::advance(pc, strlen("xmlns"));
    auto pnameend = std::find(pc, endBuffer, '=');
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pnameend == endBuffer)
        std::exit(1);
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pc == endBuffer)
        std::exit(1);
    pc = pnameend;
    if (*pc == ':')
    {
        std::advance(pc, 1);
        prefix.assign(pc, pnameend);
    }
    pc = std::next(pnameend);
    pc = std::find_if_not(pc, endBuffer, [] (char c) { return std::isspace(c); });
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pc == endBuffer)
        std::exit(1);
    char delim = *pc;
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 2
      */
    if (delim != '"' && delim != '\'')
        std::exit(2);
    std::advance(pc, 1);
    auto pvalueend = std::find(pc, endBuffer, delim);
    /*
      If the iterator reaches the end of the of the buffer without
      finding the relevant value something went wrong and exit with code 1
      */
    if (pvalueend == endBuffer)
        std::exit(1);
    const std::string temp(pc, pvalueend);
    uri=temp;
    pc = std::next(pvalueend);
    pc = std::find_if_not(pc, endBuffer, [] (char c) { return std::isspace(c); });
}
 /*
 Function parses the characters in the document
 @param pc; iterator stored to current spot
 @param endbuffer; iterator to the end position of the buffer
 @param depth; int number of tags within the code
 @param loc; int nuber of lines in the documenr
 @param textsize; int number of source characters
 */
void charcterParse(std::vector<char>::iterator &pc, std::vector<char>::iterator endBuffer, int &depth, int &loc, int &textsize)
{
    std::vector<char>::iterator pcur = pc;
    pc = std::find_if(pc, endBuffer, [] (char c) { return c == '<' || c == '&'; });
    if (depth > 0)
    {
        std::string characters;
        if (*pcur != '&')
        {
            std::string characters(pcur, pc);
            loc += std::count(characters.begin(), characters.end(), '\n');
        }
        else
        {
            if (*std::next(pc) == 'l' && *std::next(pc, 2) == 't' && *std::next(pc, 3) == ';')
            {
                characters += '<';
                std::advance(pc, strlen("&lt;"));
            }
            else if (*std::next(pc) == 'g' && *std::next(pc, 2) == 't' && *std::next(pc, 3) == ';') {
                characters += '>';
                std::advance(pc, strlen("&gt;"));
            }
            else if (*std::next(pc) == 'a' && *std::next(pc, 2) == 'm' && *std::next(pc, 3) == 'p' &&
                     *std::next(pc, 4) == ';') {
                characters += '&';
                std::advance(pc, strlen("&amp;"));
            }
            else
            {
                characters += '&';
                std::advance(pc, 1);
            }
        }
        textsize += characters.size();
    }
}
/*
Function parses the empty elements in the document
@param pc; iterator stored to current spot
@param intag; boolen that determines if we parsing something inside of the tag
*/
void emptyElement(std::vector<char>::iterator &pc, bool &intag)
{
    std::advance(pc, 2);
    intag=false;
}
/*
Function parses the end of the start tags in the document
@param pc; iterator stored to current spot
@param intag; boolen that determines if we parsing something inside of the tag
*/
void endStartTag(std::vector<char>::iterator &pc, bool &intag)
{
    std::advance(pc, 1);
    intag=false;
}
