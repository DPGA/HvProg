#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;
using namespace boost;
using namespace boost::posix_time;
#include "GenericTypeDefs.h"
#include <vector>
#include <string>
#include <map>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <getopt.h>
#include "timeoutserial.h"

#include "vme.h"
#include "vme_user.h"

std::map <std::string,class VmeMap *> pVme;

#include "vmemap.h"
#include "tools.h"
#include "hvprog.h"
#include "hvserveur.h"

//typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define HV   1
#define NONE 0
#define CAL  2
#define READHV 3
#define VERIF 4
#define SERVEUR 10
using namespace std;

//HvProg *pHvProg;

int main(int argc, char **argv)
{
 cout  << BgColor::black << FgColor::yellow << "HvProg   Version 1.02" << FgColor::white << endl;
 int HtOn = 0;
 int HtOff = 0; 
 unsigned int Ch = 0;
 float Value=0.0;
 unsigned int Mode=NONE;
 int HvPos=0;
 int verbose = 0;
 u16 BoardSn=0;
 u16 TabVal[16];
 std::map <std::string,class VmeMap *> pVme; 
 int port=0;

 
/*----------------------------------------------------------------*/
/* Ouverture des fenetres VME					 				  */ 
/*----------------------------------------------------------------*/ 



  pVme["A24D32"] = new VmeMap((unsigned long long)0xC00000,(unsigned long long)0x1000000,VME_A24,VME_USER | VME_DATA,VME_D16,1);    
	  
//  VmeMap *pVmeHvProg = new VmeMap(0xC00000,0x1000000,VME_A24,0x39,VME_D16,1); //A24 D16
//  if (!pVmeHvProg->GetMap()) {printf("erreur ouverture\n");return(-1);}  // test si l'ouverture est correcte

  
 int optionindex;
 int option_char;
 static struct option long_options[] = {
      { "hton"			,no_argument			,&HtOn	, 1 },
      { "htoff"		,no_argument			,&HtOff	, 1 },
      { "ch"			,required_argument	,NULL		,'C'},
      { "hv"			,required_argument	,NULL		,'H'},
      { "display"		,no_argument			,NULL		,'D'},
      { "pos"			,no_argument			,&HvPos	, 1 },
      { "cal"			,required_argument	,NULL		,'L'},
      { "serial"		,required_argument	,NULL		,'S'},
      { "verif"		,no_argument			,NULL		,'V'},
      { "serveur"		,required_argument	,NULL		,'R'},
      { "verbose"		,no_argument			,&verbose, 1 },
      { NULL			,no_argument			,NULL		, 0 }
    };
    
    
  // Invokes member function `int operator ()(void);'
  while ((option_char = getopt_long (argc, argv, "t:i:n:b:e:s:m:a:c:l:L:P:A:M:v:N:T:S:y:x",long_options,&optionindex)) != EOF)
    switch (option_char) {  
	 case 'C': Mode = HV;Ch = atoi(optarg); cout << "Channel "  << FgColor::yellow << CFormat("%04X",Ch)    << FgColor::white << endl;break;
	 case 'H': Mode = HV;Value=atof(optarg);cout << "Value Hv "  << FgColor::yellow << Value  << FgColor::white << endl;break;
	 case 'D': Mode = READHV;break;
	 case 'S': BoardSn= atoi(optarg);cout << "Num Board "  << FgColor::yellow << CFormat("%02X",BoardSn)    << FgColor::white << endl;break;
	 case 'L': Mode = CAL;Ch = atoi(optarg);break;
	 case 'V': Mode = VERIF;break;
	 case 'R': Mode = SERVEUR; port= atoi(optarg); break;
	 case '?': break; 
    }

	if(Mode != SERVEUR) {
		HvProg *pHvProg;
		pHvProg = new HvProg( pVme["A24D32"],BoardSn); 
		pHvProg->IsHvProg();   

		if ((HtOn == 1) && (HtOff == 0)) {
			cout << FgColor::green << "Hv On" << FgColor::white << endl;
			pHvProg->HvOn();
		}
		if ((HtOn == 0) && (HtOff == 1)) {
			cout << FgColor::green << "Hv Off" << FgColor::white << endl;
			pHvProg->HvOff();
		}
		if ((HtOn == 1) && (HtOff == 1)) {
			cout << FgColor::red <<  "erreur imposible de mettre Hv on et off en meme temps" << FgColor::white << endl;	
			exit (1);
		}
		pHvProg->SetHvPol(HvPos);
		printf("ht %d %d\n",HtOn,HtOff);
 
		switch (Mode) {
			case NONE : break; 
			case VERIF : pHvProg->Verif();break;
			case READHV : pHvProg->ValueHv(&TabVal[0]);break;
			case CAL  : printf("Mode Calibration\n");pHvProg->Calib(Ch);break;
			case HV   : printf("mode hv \n");pHvProg->SendHv(Ch,Value);break;
		}

		u16 Stat;
		pHvProg->StatHv(&Stat);
		cout << FgColor::red << "Status " << CFormat("%4x",Stat) << FgColor::white<< endl;

		if (pHvProg) delete(pHvProg);
 //	if (pVmeHvProg) delete(pVmeHvProg);  
	} else {
		cout << __FUNCTION__ << __LINE__ << " port " << port << endl;
		CHvServeur *phvserv;
		phvserv = new CHvServeur(port, pVme["A24D32"], verbose);
		phvserv->Run();
		if(phvserv) delete (phvserv);
	}
  
	return 0;
}
//=========================================================================
