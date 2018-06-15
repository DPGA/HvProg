
/*dentification	:	Color.cxx
*	Contents		:	Header file
 *	Author		:	Remy MARCHIONINI
 *	Created on		:	18-dec-2006
 *	Last Update 	:
 *********************************************************************
 */

#include "tools.h"
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
//#include <TF1.h>
//#include <TMath.h>
//#include <TGraph.h>
using namespace std;


struct  termios argin, argout;
 

/*
const char * Color::green = "\033[42;30m";
const char * Color::white = "\033[0;40;1;37m";
const char * Color::normal = "\033[1;47;0;30m";
const char * Color::cyan = "\033[46;30m";
const char * Color::yellow = "\033[43;30m";
const char * Color::red = "\033[41;1;37m";
const char * Color::black = "\033[40m";
const char * Color::blue = "\033[44m";
const char * Color::test = "\033[40;33;04m";
const char * Color::underscore = "\033[04m";
*/

const char * CtColor::none = "\033[00m";
const char * CtColor::bold = "\033[01m";
const char * CtColor::underscore = "\033[04m";
const char * CtColor::blink = "\033[05m";
const char * CtColor::reverse = "\033[07m";
const char * CtColor::cancel = "\033[08m";

const char * FgColor::black = "\033[30m";
const char * FgColor::red = "\033[31m";
const char * FgColor::green = "\033[32m";
const char * FgColor::yellow = "\033[33m";
const char * FgColor::blue = "\033[34m";
const char * FgColor::magenta = "\033[35m";
const char * FgColor::cyan = "\033[36m";
const char * FgColor::white = "\033[37m";
const char * FgColor::test1 = "\033[38m";
const char * FgColor::test2 = "\033[39m";

const char * BgColor::black = "\033[40m";
const char * BgColor::red = "\033[41m";
const char * BgColor::green = "\033[42m";
const char * BgColor::yellow = "\033[43m";
const char * BgColor::blue = "\033[44m";
const char * BgColor::magenta = "\033[45m";
const char * BgColor::cyan = "\033[46m";
const char * BgColor::white = "\033[47m";
const char * BgColor::test1 = "\033[48m";
const char * BgColor::test2 = "\033[49m";

/*
void fit(double *x,double *y,int nb,double *CoefPoly)
{
   //Example of fitting with a linear function, using TLinearFitter
   //This example is for a TGraphErrors, but it can also be used 
   //when fitting a histogram, a TGraph2D or a TMultiGraph
   //Author: Anna Kreshuk
   Double_t params[5];

   TGraph *gr= new TGraph(nb,x,y);

   gr->Fit("pol3");
   //Access the fit resuts
   TF1 *f3 = gr->GetFunction("pol3");
   f3->GetParameters(&params[0]);
   f3->SetLineWidth(1);
   for (int k=0;k <4 ;k++)
   cout << "p" <<k << " " << params[k] << endl;
   for (int i=0;i<4;i++) CoefPoly[i] = params[i];
   
 
} 
*/

void Date(const char *prefix,const char *suffix,char *s)
{

time_t rawtime;
struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  sprintf (s, "%s-%4d%02d%02d-%02d%02d%02d%s",prefix,timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,suffix);

}



unsigned char kbhit()
{
unsigned char   ch = 0;

        tcgetattr(0,&argin);
        argout = argin;                                                        
        argout.c_lflag &= ~(ICANON);
        argout.c_iflag &= ~(ICRNL);
        argout.c_oflag &= ~(OPOST);
        argout.c_cc[VMIN] = 1;
        argout.c_cc[VTIME] = 0;
        tcsetattr(0,TCSADRAIN,&argout);
        read(0, &ch, 1);
        tcsetattr(0,TCSADRAIN,&argin);
        return(ch);
} 

struct timeval utime()
{
    struct timeval start;    

    gettimeofday(&start, NULL);

    return (start);
}

double DiffTime(struct timeval start)
{
    struct timeval end;

    double mtime;
    long seconds, useconds;    

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0);

//    printf("%ld %ld Elapsed time: %f milliseconds %ld  %ld \n",start.tv_sec,start.tv_usec, mtime,seconds,useconds);

    return (mtime);
}
