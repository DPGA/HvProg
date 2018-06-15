#include "vmemap.h"
#include "GenericTypeDefs.h"
#include <stdio.h>
#include <unistd.h>
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

#include "CAENVMElib.h"


void* VmeMap::ReadBlt(unsigned int *reg,u32 Cycle, u64 offset)  
/*******************************************************************************************************************************/
{
//  CAENVME_MultiRead(Handle, offset, reg,NCycle, CVAddressModifier *AMs, CVDataWidth *DWs, CVErrorCodes *ECs);
  CVAddressModifier amlocal = cvA24_U_BLT;
  std::cout << "--> in vmemap.cpp : before BLTReadCycle" << std::endl;
  std::cout << "      offset=" << std::hex << offset << std::dec << std::endl;
  std::cout << "      reg=" << reg << std::endl;
  std::cout << "      Cycle*4=" << Cycle*4 << std::endl;
  std::cout << "      am=" << am << std::endl;
  std::cout << "      amlocal=" << amlocal << std::endl;
  std::cout << "      cvD32=" << cvD32 << std::endl;
  int nb;
  //CAENVME_BLTReadCycle(Handle,offset,(char *)reg,Cycle*4,am,cvD32,&nb);
  CAENVME_BLTReadCycle(Handle,offset,(char *)reg,Cycle*4,amlocal,cvD32,&nb);
  std::cout << "--> in vmemap.cpp : after BLTReadCycle" << std::endl;
  return(reg);
}


ssize_t VmeMap::Write(u8 reg,unsigned long offset)
/*******************************************************************************************************************************/
{
  CAENVME_WriteCycle(Handle, master.vme_addr+offset,&reg,am, cvD8);
  return(1);
}

ssize_t VmeMap::Write(u16 reg,unsigned long offset)
/*******************************************************************************************************************************/
{
  CAENVME_WriteCycle(Handle, master.vme_addr+offset,&reg,am, cvD16);
  return(2);
}

ssize_t VmeMap::Write(u32 reg,unsigned long offset)
/*******************************************************************************************************************************/
{

  CAENVME_WriteCycle(Handle, master.vme_addr+offset,&reg,am, cvD32);
  return(4);
}

ssize_t VmeMap::Write(u64 reg,unsigned long offset)
/*******************************************************************************************************************************/
{
//  CAENVME_WriteCycle(int32_t Handle, uint32_t Address, void *Data,CVAddressModifier AM, CVDataWidth DW);
  return(8);
}

ssize_t VmeMap::Write(void *reg,unsigned int n,unsigned long offset) 
/*******************************************************************************************************************************/
{

//  CAENVME_WriteCycle(int32_t Handle, uint32_t Address, void *Data,CVAddressModifier AM, CVDataWidth DW);
  return(0);
}

void VmeMap::ReverseMsbLsb(u8 *reg,u32 n)
/*******************************************************************************************************************************/

{
  unsigned long long tmp;
  if (n >1) 
    for (unsigned int i=0;i<n;i+=2) {
      memcpy(&tmp,reg,2);
      tmp = SWAP_16(tmp);
      memcpy(reg,&tmp,2);
      reg +=2;
    }
}
	


ssize_t VmeMap::Read(void *reg,unsigned int n,unsigned long offset,const bool swap)  
/*******************************************************************************************************************************/
{

  ssize_t ret=pread(file,reg,n,offset);
  if (swap) ReverseMsbLsb((u8 *) reg,n);
  return(ret);  
}	

ssize_t VmeMap::Read(u8 *reg,unsigned long offset,const bool swap)  
/*******************************************************************************************************************************/
{
  CAENVME_ReadCycle(Handle, master.vme_addr+offset,reg,am, cvD8);
  return(1);
}

ssize_t VmeMap::Read(u16 *reg,unsigned long offset,const bool swap)  
/*******************************************************************************************************************************/
{
  CAENVME_ReadCycle(Handle, master.vme_addr+offset,reg,am, cvD16);
  return(2);
}

ssize_t VmeMap::Read(u32 *reg,unsigned long offset,const bool swap)  
/*******************************************************************************************************************************/
{
  CAENVME_ReadCycle(Handle, master.vme_addr+offset,reg,am, cvD32);
  return(4);
}




ssize_t VmeMap::Read(void *reg,unsigned long offset,const bool swap)  
/*******************************************************************************************************************************/
{
//  CAENVME_ReadCycle(int32_t Handle, uint32_t Address, void *Data,CVAddressModifier AM, CVDataWidth DW);
  return(0);
}


VmeMap::~VmeMap()
/*******************************************************************************************************************************/
{
//  if (master) delete master;
  NbreWindow--;close(file);

}

VmeMap::VmeMap(unsigned long VmeAddr,unsigned long Size,u32 Aspace,u32 Cycle,u32 Dwidth,const int Enable=1, const bool doInit)
/*******************************************************************************************************************************/
{
//master = new vme_master;
  master.enable = Enable;
  master.vme_addr = VmeAddr;	
  master.size     = Size;
  master.aspace = Aspace;	
  master.cycle = Cycle;
  master.dwidth = Dwidth;
  switch (master.aspace) {
    case VME_A32 : am = cvA32_U_DATA;break;
    case VME_A24 : am = cvA24_U_DATA;break;
    case VME_A16 : am = cvA16_U;break;
    default : printf("\nError Adressage impossible %d\n",master.aspace);
  }	
  am = cvA24_U_DATA;
  if(doInit) {
    
    if (CAENVME_Init(cvV1718,0,0,&Handle) != cvSuccess ) {
      printf("\n Error opening device V1718 \n");
    }
    std::cout << "-> Initializing CAENVME with parameters: " << " am " << std::hex << am << std::dec << cvV1718 << "  " << 0 << "  " << 0 << "  " << Handle << std::endl;
  }
}
VmeMap::VmeMap(u32 handle,u32 Aspace) 
/*******************************************************************************************************************************/
{
  Handle = handle;	
  switch (Aspace) {
    case VME_A32 : am = cvA32_U_DATA;break;
    case VME_A24 : am = cvA24_U_DATA;break;
    case VME_A16 : am = cvA16_U;break;
    default : printf("\nError Adressage impossible %d\n",master.aspace);
  }	
}


VmeMap::VmeMap(unsigned long long VmeAddr,unsigned long long Size,u32 Aspace,u32 Cycle,u32 Dwidth,const int Enable)
/*******************************************************************************************************************************/
{
//master = new vme_master;
  master.enable = Enable;
  master.vme_addr = VmeAddr;	
  master.size     = Size;
  master.aspace = Aspace;	
  master.cycle = Cycle;
  master.dwidth = Dwidth;
  std::cout << "debug: " << Aspace << "  " << VME_A24 << std::endl;
  switch (master.aspace) {
    case VME_A32 : am = cvA32_U_DATA;
    case VME_A24 : am = cvA24_U_DATA;
    case VME_A16 : am = cvA16_U;
    default : printf("\nError Adressage impossible %d\n",master.aspace);
  }	
  am = cvA24_U_DATA;
  std::cout << std::hex << "am = 0x" << am << std::dec <<std::endl;
  if (CAENVME_Init(cvV1718,0,0,&Handle) != cvSuccess ) {
    printf("\n Error opening device V1718 %x\n",am);
  }
}



int VmeMap::NbreWindow=0;

