#ifndef TOOLS_H
#define TOOLS_H
/*
 *********************************************************************
*	Projet		:	Atlas
*	Identification	:	Color.h
*	Contents		:	Header file
 *	Author		:	Remy MARCHIONINI
 *	Created on		:	18-dec-2006
 *	Last Update 	:
 *********************************************************************
 */

#include <string>
#include <cstdio>

template <typename T>
std::string CFormat(const char *format,const T value)
{
  char buffer[128];

  std::sprintf(buffer,format,value);

  return std::string(buffer);
}


class CtColor
{
  public:
    static const char * none;
    static const char * bold;
    static const char * underscore;
    static const char * blink;
    static const char * reverse;
    static const char * cancel;
};

class FgColor
{
  public:
    static const char * green;
    static const char * white;
    static const char * normal;
    static const char * cyan;
    static const char * yellow;
    static const char * red;
    static const char * black;
    static const char * blue;
    static const char * magenta;
    static const char * test1;
    static const char * test2;
};

class BgColor
{
  public:
    static const char * green;
    static const char * white;
    static const char * normal;
    static const char * cyan;
    static const char * yellow;
    static const char * red;
    static const char * black;
    static const char * blue;
    static const char * magenta;
    static const char * test1;
    static const char * test2;
};


void Date(const char *prefix,const char *suffix,char *s);
void fit(double *x,double*y,int nb,double *CoefPoly);
#endif
