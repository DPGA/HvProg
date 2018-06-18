#ifndef HVPROG_H
#define HVPROG_H

#include <linux/types.h>
#include "vmemap.h"
#include "serial.h"


#define A_HVPROG 0xC00000

//-----------------------------------------------------------
enum __REGVME_ {
	RegBoardID    =      0x0000,
	RegBoardSN    =      RegBoardID+2,
	RegHWREV      =      RegBoardID+4,
	RegFWREV      =      RegBoardID+6,
	RegStatusCmdHv=      0x10,
	RegDacAddr    =      0x20,
	RegDacData    =      0x30,
	RegAdcBegin   =      0x40,
	RegAdcEnd     =      0x5e
};


class HvProg 
{
private :
  bool verbose;
  int file;
  VmeMap *pVmeHvProg;
  u16 BoardID,BoardSN,HwRev,FwRev;
  u16 AdrBase;
  double CoefDac[16][4];
  double CoefAdc[16][4];
  double mseAdc[16];
  double mseDac[16];
  bool   HvPos;
  string NomFichier;
  SimpleSerial *pserial;
  string m_dircoeff;

public  :
  HvProg(VmeMap *pVme,u16 Num,string dircoeff,bool Verb = true);
  ~HvProg();
  u32 GetBaseAddr();
  bool IsHvProg(); 
  void WriteReg(u16 reg,u16 offset);
  void ReadReg(u16 *reg,u16 offset);
  void InitHvProg();
  void HvOn();
  void HvOff();
  void StatHv(u16 *Stat);
  void ChoiceDac(u16 Num);
  void ValueHv(u16 Data,u16 Ch) ;
  void SendHv(u16 Ch,float Val);
  void ValueHv(u16 *TabVal);
  void ReadFile(string Nom);
  u16  CalculDac(float Hv,int Num);
  float CalculAdc(u16 Adc,int Num);
  string Replace(char C1,char C2,string s);
  void Calib(u16 Ch);
  void CalibHvIn();
  void CalibHvOut(u16 Ch);
  double ReadAdc(u16 Ch);
  void SendHv(u16 Ch,u16 Val);
  void SaveCoef();
  void SetHvPol(int pos);
  void Verif();
};

#endif
