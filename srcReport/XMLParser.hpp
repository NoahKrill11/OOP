/*
    XMLParser.hpp
    
    Defining the the free functions for use in other cpp files
*/

#include<iostream>
#ifndef XMLPARSER
#define XMLPARSER
//Function for checking if the conditions are met to fill the buffer
bool bufferCheck(std::vector<char>::iterator, std::vector<char>::iterator& ,bool&);

//Function for checking if the conditions are met parse the declarations
bool declarationCheck(std::vector<char>::iterator&);

//Function for checking if the conditions are met parse the comments
bool commentCheck(std::vector<char>::iterator&);

//Function for checking if the conditions are met parse the cdata
bool cDataCheck(std::vector<char>::iterator&);

//Function for checking if the conditions are met parse the end tags
bool endTagCheck(std::vector<char>::iterator&);

//Function for checking if the conditions are met parse the namespace
bool namespaceCheck(bool&, std::vector<char>::iterator, std::vector<char>::iterator&);

//Function for checking if the conditions are met parse the start tag
bool startTagCheck(std::vector<char>::iterator&);

//Function for checking if the conditions are met parse the end of the start tag
bool endStartTagCheck(std::vector<char>::iterator&, bool&);

//Function for checking if the conditions are met parse the empty element
bool emptyElementCheck(std::vector<char>::iterator&, bool&);

//Function for checking if the conditions are met parse the attributes
bool attCheck(bool&, std::vector<char>::iterator&);

//Function for filling the buffer
void fillTheBuffer(std::vector<char>&, std::vector<char>::iterator&, ssize_t&, const int, long&);

//Function for parsing the declarations in the document
void declartionParse(std::vector<char>::iterator, std::vector<char>::iterator&);

//Function for parsing the comments in the document
void commentParse(std::vector<char>::iterator, std::vector<char>::iterator&);

//Function for parsing the cdata in the document
void cDataParse(std::vector<char>::iterator, std::vector<char>::iterator&, std::string&);

//Function for parsing the end tags in the document
void endTagParse(std::vector<char>::iterator&, std::vector<char>::iterator, int&, std::string&, std::string&, std::string&);

//Function for parsing the start tags in the document
void startTagParse (std::vector<char>::iterator, std::vector<char>::iterator&, int&, std::string&, std::string&, std::string&);

//Function for parsing the atributes in the document
void attParse(std::vector<char>::iterator, std::vector<char>::iterator&,std::string &,std::string &,std::string &,std::string &);

//Function for parsing the characters in the document
void charcterParse(std::vector<char>::iterator&, std::vector<char>::iterator, int&, std::string&, int&);

//Function for parsing the namespace in the document
void namespaceParse(std::vector<char>::iterator&, std::vector<char>::iterator, std::string&, std::string&);

//Function for parsing the empty elements in the document
void emptyElement(std::vector<char>::iterator&, bool&);

//Function for parsing the end of start tags in the document
void endStartTag(std::vector<char>::iterator&, bool&);

#endif//XMLPARSER

