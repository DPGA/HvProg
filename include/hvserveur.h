/*
 * hvserveur.h
 * 
 * Copyright 2016 Robert Chadelas <robert.chadelas@clermont.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

//=========================================================================
#ifndef HVSERVEUR_H
#define HVSERVEUR_H
//=========================================================================
//=========================================================================
#include <linux/types.h>
#include "GenericTypeDefs.h"
#include "hvprog.h"
//=========================================================================
//=========================================================================
#define NB_HVMOD       (4)
#define NB_HVCHAN_MOD  (16)
#define NB_HVCHAN      (NB_HVMOD * NB_HVCHAN_MOD)
#define NB_COEF        (4)
//=========================================================================
//=========================================================================
#define TCP_BUF8_LEN   (1500)
#define TCP_BUF16_LEN  (TCP_BUF8_LEN >> 1)
#define TCP_BUF32_LEN  (TCP_BUF8_LEN >> 2)
#define TCP_NB_ARG_16  (TCP_BUF16_LEN - 6)
#define TCP_NB_ARG_32  (TCP_BUF32_LEN - 3)
//=========================================================================
typedef union _TCP_MSG
{
	u8  buf8[TCP_BUF8_LEN];
	u16 buf16[TCP_BUF16_LEN];
	u32 buf32[TCP_BUF32_LEN];
	struct {
		u16 indic;                     // Frame Index
		u16 stat;                      // Command Status
		u16 cmd;                       // Command Number
		u16 nbarg;                     // Number of Arguments
		u32 mask;                      // Destination Mask
		union {
			u16 arg16[TCP_NB_ARG_16];   // List of arguments (unsigned short)
			u32 arg32[TCP_NB_ARG_32];   // List of arguments (unsigned long)
		};
	};
}TCP_Msg;
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
typedef union _HVSTAT
{
	u16 global;
	struct {
		u16 actived:1;
		u16 positive:1;
		u16 on:1;
	};
}HVSTAT;
//=========================================================================
//=========================================================================
typedef struct HVCHAN HVCHAN;
struct HVCHAN
{
	short val;
	short order;
}__attribute__((__packed__));
//=========================================================================
//=========================================================================
typedef struct HVMOD HVMOD;
struct HVMOD
{
	u16    addr;
	HVSTAT hvstat;
	HVCHAN hvchan[NB_HVCHAN_MOD];
}__attribute__((__packed__));
//=========================================================================
//=========================================================================
typedef struct HVDATA HVDATA;
struct HVDATA
{
	HVMOD hvmod[NB_HVMOD];  
}__attribute__((__packed__));

//=========================================================================
//=========================================================================
class CHvServeur
{
	public:
		CHvServeur(int port, VmeMap *vme,std::string fileconfig,string dircoeff, int verbose);
		virtual ~CHvServeur();
		int  GetPort  (void);
		void SetPort  (int port);
		void Run      (void);
		void SetVme   (VmeMap *Vme); 

	private:
		HVDATA hvdt;
		int    m_port;
		int    m_verbose;
		HvProg *pHvProg[NB_HVMOD];
		VmeMap *pVme;
		int    m_servSock;
		int    m_socket;
		int    m_stop;
		std::string m_fileconfig;
		std::string m_dircoeff;
	
		int    CreateConnection (void);
		void   WaitConnection  (void);
	    void   CloseConnection(void);
		bool   WaitRequest (TCP_Msg *msg);
		bool   Answer(TCP_Msg *msg);

		
		int  XmlParser    (string f_name);
		void Update       (void);
		void TCPAction    (TCP_Msg *msg);
		void TCPClearArgs (TCP_Msg *msg);
		void TCPErrorCmd  (TCP_Msg *msg, u16 error);
		void TCPGetParms  (TCP_Msg *msg);
		void TCPSWitchHV  (TCP_Msg *msg);
		void TCPOrder     (TCP_Msg *msg);
		void TCPStop      (TCP_Msg *msg);
};
//=========================================================================
//=========================================================================
inline void CHvServeur::SetPort(int port)     { m_port = port; }
inline int  CHvServeur::GetPort(void)         { return(m_port); }
//=========================================================================
//=========================================================================
//=========================================================================

#endif /* HVSERVEUR_H */ 
