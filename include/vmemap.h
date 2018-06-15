#ifndef VMEMAP_H
#define VMEMAP_H

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <linux/types.h>
#include "GenericTypeDefs.h"
#include <sys/ioctl.h>
#include <vme.h>
#include <vme_user.h>


const unsigned int Major = 1;
const unsigned int Minor = 1;
const unsigned int SWRelease = ((Major<<16) | Minor);


#include <CAENVMElib.h>
#include <CAENVMEtypes.h>


#define SWAP_16(x) (((x & 0xff) << 8) | ((x &0xff00) >> 8))
#define SWAP_32(x) (((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | ((x & 0xff000000) >> 24))

class VmeMap 
{
private :
  int file;
  char *laddr;
  int Window;	       /* Compteur de fenetre maitre  */
  int32_t Handle;
  CVAddressModifier am;


  struct vme_master master;  
  struct vme_master *pMaster;


public  :
unsigned int LibRelease();

  VmeMap(unsigned long vme_addr,unsigned long size,u32 aspace,u32 cycle,u32 dwidth,const int enable, const bool doInit=true);
  VmeMap(unsigned long long VmeAddr,unsigned long long Size,u32 Aspace,u32 Cycle,u32 Dwidth,const int Enable=1);
  VmeMap(u32 handle,u32 Aspace);
 // vme_master *GetMap(); 
  u32 GetHandle() {return(Handle);}


  ~VmeMap();

  ssize_t Read(u8 *reg,unsigned long offset,const bool swap=false);
  ssize_t Read(u16 *reg,unsigned long offset,const bool swap=false);
  ssize_t Read(u32 *reg,unsigned long offset,const bool swap=false); 
  ssize_t Read(void *reg,unsigned long offset,const bool swap=false);
  ssize_t Read(void *reg,unsigned int n, unsigned long offset,const bool swap=false);
  void* ReadBlt(unsigned int *reg,u32 Cycle, u64 offset);
  ssize_t Write(u8 reg,unsigned long offset);
  ssize_t Write(u16 reg,unsigned long offset);
  ssize_t Write(u32 reg,unsigned long offset);
  ssize_t Write(u64 reg,unsigned long offset);
  ssize_t Write(void *reg,unsigned int n,unsigned long offset);
  
  void ReverseMsbLsb(u8 *reg,u32 n);
  static int NbreWindow;
};
#endif
