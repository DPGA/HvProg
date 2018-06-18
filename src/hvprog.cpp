#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

#include <string.h>
#include "serial.h"



#include "tools.h"
#include "hvprog.h"
#include <linux/types.h>
//#include <vmemap.h>


HvProg::HvProg(VmeMap *pVme,u16 Num,string dircoeff,bool Verb)
/*------------------------------------------------------------------------*/
{
   pVmeHvProg = pVme;
   AdrBase = (Num << 8);
   verbose  = Verb;
   m_dircoeff = dircoeff;
#ifdef SERIAL
   try { 
   pserial = new SimpleSerial("/dev/ttyUSB0",9600);
   pserial->writeString("VDC\n");					// Mettre le multimètre en Voltmètre continu
   if(verbose) cout<< pserial->readLine() << endl;
   pserial->writeString("RANGE 4\n");				// Calibre 300 Vdc
   if(verbose) cout << pserial->readLine() << endl;
   pserial->writeString("VAL1?\n");				// Calibre 300 Vdc
   if(verbose) cout << pserial->readLine() << endl;   
   } catch(boost::system::system_error& e)
     {
      if(verbose) cout<<"Error: "<<e.what()<<endl;
     }
#endif    
  if (verbose) cout << "Creating "<< __FUNCTION__ << endl;

} 

void HvProg::SetHvPol(int pos)
/*------------------------------------------------------------------------*/
{
   HvPos=pos;
   cout << "Haute tension ";
   if (HvPos == 1) cout << "positive" << endl;
   else cout << "negative" << endl;
}

HvProg::~HvProg()
/*------------------------------------------------------------------------*/
{
	
}

void HvProg::WriteReg(u16 reg,u16 offset)
/*------------------------------------------------------------------------*/
{
  pVmeHvProg->Write(reg,AdrBase+offset);
}

void HvProg::ReadReg(u16 *reg,u16 offset)
/*------------------------------------------------------------------------*/
{
  pVmeHvProg->Read(reg,AdrBase+offset);
}


u32 HvProg::GetBaseAddr() 
/*------------------------------------------------------------------------*/
{
  return (A_HVPROG+AdrBase);
}


bool HvProg::IsHvProg()
/*------------------------------------------------------------------------*/
{ 
  ReadReg(&BoardID,RegBoardID);
  ReadReg(&BoardSN,RegBoardSN);
  ReadReg(&HwRev,RegHWREV);
  ReadReg(&FwRev,RegFWREV);

  if (BoardID != 0xC0C0) {
    cout  << FgColor::red
	      << "HvProg not found " << CFormat("%4X",AdrBase+A_HVPROG) << "  " 
	      << CFormat("%04X",BoardID) 
	      << FgColor::white
	      << endl;
  //  return(false);
  }
  
	cout  << FgColor::green 
			<< "HvProg   Present " << FgColor::white
			<< "Base Addresse " << CFormat("%06X",GetBaseAddr()) << "     " 
			<< "Boardid=" << CFormat("%04X",BoardID) << " " 
			<< "BoardSN=" << CFormat("%X",BoardSN) << " "
			<< "HwRev=" << CFormat("%d",HwRev >> 16) 
			<< "." <<  CFormat("%02d",HwRev & 0xff) << " "
			<< "FwRev=" << CFormat("%d",FwRev >> 16) 
			<< "." <<  CFormat("%02d",FwRev & 0xff)
			<< endl;
  string chaine = m_dircoeff + "/Coef_poly_" + CFormat("%04X",GetBaseAddr()>>8) + ".txt";
  cout << "fichier = " << chaine << endl;
  ReadFile(chaine);
  return(true);
}

string HvProg::Replace(char C1,char C2,string s)
/***************************************************************************/
{
 
  string newS = s;
  for (unsigned int i=0;i< s.size();i++) {
    if (s[i] == C1) newS[i]=C2;
  }  

  return (newS);
}

void HvProg::ReadFile(string Nom)
/***************************************************************************/
{
      
  char *pEnd;
  NomFichier = Nom;
  ifstream ifs(Nom.c_str(),ifstream::in);
  string chaine,chaine1;
  
  if (ifs) {
    getline(ifs,chaine);
    for (int i=0;i<16;i++) {	    
      getline(ifs,chaine);
      chaine1 = Replace(',','.',chaine); 
      pEnd = (char *)chaine1.c_str();
      for (int k=0;k<4;k++) CoefDac[i][k] = strtof(pEnd,&pEnd);
//      mseDac[i]     = strtof(pEnd,NULL);       
//      cout << i << " coef  " << CoefDac[i][3] << "  " << CoefDac[i][2] << "  " << CoefDac[i][1] << "  " << CoefDac[i][0] << "  "<<mseDac[i] <<  endl;
    }
    getline(ifs,chaine);
    getline(ifs,chaine);
    //getline(ifs,chaine);
   
    for (int i=0;i<16;i++) {	    
      getline(ifs,chaine);
      chaine1 = Replace(',','.',chaine); 
      pEnd = (char *)chaine1.c_str();
      for (int k=0;k<4;k++) CoefAdc[i][k] = strtof(pEnd,&pEnd);
//      mseAdc[i]     = strtof(pEnd,NULL);   
//      cout << i << " coef  " << CoefAdc[i][3] << "  " << CoefAdc[i][2] << "  " << CoefAdc[i][1] << "  " << CoefAdc[i][0] << "  "<<mseAdc[i] <<  endl;    
    }	  
  }
  else cout << FgColor::red << "Error openning file " << Nom << endl;
    
  ifs.close();
}
    

u16 HvProg::CalculDac(float Hv,int Num)
/***************************************************************************/
{
  cout << "Coef = " << CoefDac[Num][3] << " " << CoefDac[Num][2] << " " << CoefDac[Num][1] << " " << CoefDac[Num][0] << endl;
  int val = (Hv*Hv*Hv*CoefDac[Num][3] + Hv*Hv*CoefDac[Num][2] + Hv*CoefDac[Num][1] + CoefDac[Num][0]);
  cout << "Dac " << val << endl;
  if (val > 4095) val = 4095;
  if (val <0) val =0;
  return ((u16) val);
}

float HvProg::CalculAdc(u16 Adc,int Num)
/***************************************************************************/
{
  return((float) (Adc*Adc*Adc*CoefAdc[Num][3] + Adc*Adc*CoefAdc[Num][2] + Adc*CoefAdc[Num][1] + CoefAdc[Num][0]));
}

void HvProg::HvOn()
/***************************************************************************/
{
  WriteReg(0x8000,RegStatusCmdHv);
}

void HvProg::HvOff()
/***************************************************************************/
{
  WriteReg(0x00,RegStatusCmdHv);
}


void HvProg::StatHv(u16 *Stat)
/***************************************************************************/
{
  ReadReg(Stat,RegStatusCmdHv);
}


void HvProg::ChoiceDac(u16 Num)
/***************************************************************************/
{
  WriteReg(Num,RegDacAddr);
}


void HvProg::ValueHv(u16 Data,u16 Ch)
/***************************************************************************/
{
  u16 test;
//  cout << "Ecriture dans le dac " << CFormat("%04X",(Ch << 12) | (Data & 0xfff)) << endl;
  WriteReg((Ch << 12) | (Data & 0xfff),RegDacData);
  ReadReg(&test,RegDacData);
  ReadReg(&test,RegDacData);
 // cout << FgColor::red << "dddddddddddddddddd   " << CFormat("%4x",test) << FgColor::white << endl;
}

void HvProg::SendHv(u16 Ch,u16 Val)
/************************************************************************************************/
{
  if(verbose) cout << "Send hv" << endl;;
  if (Ch > 7 ) ChoiceDac(1);  
  else ChoiceDac(0);
  if(verbose) cout << "valeur dac = 0x" << CFormat("%4x",Val) << endl;  
  ValueHv(Val,Ch & 0x7);
}


void HvProg::SendHv(u16 Ch,float Val)
/************************************************************************************************/
{
  if (Ch > 7 ) ChoiceDac(1);  
  else ChoiceDac(0);
  u16 ValDac = CalculDac(Val,Ch);
//  cout << "valeur dac = " << CFormat("%4x",ValDac) << endl;  
  ValueHv(ValDac,Ch & 0x7);
}

double HvProg::ReadAdc(u16 Ch)
/************************************************************************************************/
{
u16 adc;
int sum=0;

  for (int i=0;i<50;i++) {
    ReadReg(&adc,RegAdcBegin+Ch*2);
//    cout << i << "  adc = 0x" << CFormat("%4x",adc) << endl; 
    sum += (adc&0xfff);
  }  
  return ((double) sum / 50);
}

void HvProg::ValueHv(u16 *TabVal)
/************************************************************************************************/
{
  for (int i=0;i<16;i++) {
     TabVal[i] = ReadAdc(i);
//     ReadReg(&TabVal[i],RegAdcBegin+i*2);
     if(verbose) cout << "Read Adc ch " << i << " = " << FgColor::green << ((TabVal[i] & 0x0f000) >>12) <<  " RawData = "<<(TabVal[i] & 0x0fff) << " Hv = " << CalculAdc((TabVal[i] & 0x0fff),i) <<FgColor::white << endl; 
     usleep(1);
  }    
}


void HvProg::SaveCoef()
/************************************************************************************************/
{
  ofstream fichier(NomFichier.c_str());
  fichier << "Coef Dac" << endl;
  for (int i=0;i<16;i++) {
    for (int j=0;j<4;j++) 
      fichier << CoefDac[i][j] << "\t";
    fichier << endl;
  }    
  fichier << endl << "Coef Adc" << endl;
  for (int i=0;i<16;i++) {
    for (int j=0;j<4;j++) 
      fichier << CoefAdc[i][j] << "\t";  
    fichier << endl;   
  }
  fichier.close();
}



void HvProg::CalibHvIn()
/************************************************************************************************/
{
double HvMes[12];
double Adc[12];
//double Coef[5];
const double TabPos[10] = {3800.0,3600.0,3200.0,2800.0,2400.0,2000.0,1600.0,1200.0,800.0,400.0};
const double TabNeg[10] = {400.0,800.0,1200.0,1600.0,2000.0,2400.0,2800.0,3200.0,3600.0,3800.0};

double *val;

  if (HvPos) val = (double *) &TabPos[0];
  else val = (double *) &TabNeg[0];
  cout << "Hvpos = " << HvPos << endl;
  pserial->readLine();
  for (int i=0;i<10;i++) { 
    SendHv(0,(u16) val[i]);
    sleep(4);  			// attendre que la tension soit stable
    Adc[i] = ReadAdc(0);
    cout << "valeur Dac = " << val[i] << " | Valeur Adc = " << Adc[i] << " | valeur du multimetre : ";
    sleep(2);
    pserial->writeString("VAL1?\n");					// demande la valeur au multimètre
    string Val = pserial->readLine();
    HvMes[i] = atof(Val.c_str())*10.0;					
	cout<< HvMes[i] << "  " << pserial->readLine()<<endl;
    //cin >> HvMes[i];
    //cout << endl;
  }
 /* fit(&HvMes[0],val,10,&Coef[0]);
  for (int j=0;j<4;j++) CoefDac[0][j] = Coef[j];
  fit(&Adc[0],&HvMes[0],10,&Coef[0]); 
  for (int j=0;j<4;j++) CoefAdc[0][j] = Coef[j];
  SaveCoef();
  if (HvPos) SendHv(0,(u16) 4095);
  else  SendHv(0,(u16) 0);
  * */
}


void HvProg::CalibHvOut(u16 Ch)
/************************************************************************************************/
{
  const double Tab[10] = {400.0,800.0,1200.0,1600.0,2000.0,2400.0,2800.0,3200.0,3600.0,3800.0};
  double Adc[12];
  double HvMes[12];
 // double Coef[5];
  double *val = (double *) &Tab[0];
  int hvin;
  pserial->readLine();
  for (int i=0;i<10;i++) { 
    if (HvPos) {
      hvin = 4095 - val[i];
      if (hvin < 0) hvin = 0;
    }
    else {
      hvin = val[i]+400;
      if (hvin > 4095) hvin=4095;
    }

    SendHv(0,(u16) hvin);
    sleep(1);
    SendHv(Ch,(u16) val[i]);
    sleep(4);  			// attendre que la tension soit stable
    Adc[i] = ReadAdc(Ch);
    cout << "valeur Dac = " << val[i] << " Valeur Adc = " << Adc[i] << " entrez la valeur du multimetre : " << endl;
    pserial->writeString("VAL1?\n");					// demande la valeur au multimètre
    string Val = pserial->readLine();
    HvMes[i] = atof(Val.c_str())*10.0;					
	 cout<< HvMes[i] << "  " << pserial->readLine()<<endl;
    //cin >> HvMes[i];
    //cout << endl;
  }
/*  fit(&HvMes[0],val,10,&Coef[0]);
  for (int j=0;j<4;j++) CoefDac[Ch][j] = Coef[j];
  fit(&Adc[0],&HvMes[0],10,&Coef[0]); 
  for (int j=0;j<4;j++) CoefAdc[Ch][j] = Coef[j];
  SaveCoef();
  if (HvPos) SendHv(0,(u16) 4095);
  else  SendHv(0,(u16) 0);
*/
}



void HvProg::Calib(u16 Ch)
/************************************************************************************************/
{
  HvOn();
  if (Ch == 0)    
    CalibHvIn();
  else 
    CalibHvOut(Ch);
  HvOff();  
}


void HvProg::Verif()
/************************************************************************************************/
{
u16 Tab[16];
float hvin;
   HvOn();
   for (int i=200;i<1800;i=i+250) {
     if (i<800) hvin = 1000.0;
     else if (i <1500) hvin = 1600.0;
          else hvin=2000.0;
     SendHv(0, hvin);
     sleep(1);
     for (int j=1;j<16;j++)
       SendHv(j,(float) i);
     sleep(1);
     ValueHv(&Tab[0]);
     cout << "************************************************" << endl;
  }
  SendHv(0,(u16) 0);
  HvOff();
}
