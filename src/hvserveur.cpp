/*
 * hvserveur.cpp
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
#include <iostream>
using namespace std;
#include <tinyxml2.h>
using namespace tinyxml2;
#include <linux/types.h>
//#include <sys.types.h>
//#include <netinet.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//#include "GenericTypeDefs.h"

#include "hvserveur.h"
#include "tools.h"
#include "command_error.h"

//=========================================================================
//=========================================================================
CHvServeur::CHvServeur(int port, VmeMap *vme , int verbose)
{
	cout << "Create CHvServeur Class" << endl;
	m_port = port;
	m_verbose = verbose;
	pVme   = vme;
	memset((void*)&hvdt, 0, sizeof(HVDATA));
	cout << "Sizeof(HVDATA) " << sizeof(HVDATA) << " Verbose " << m_verbose << endl;
	for(int i=0; i<NB_HVMOD; i++) {
		hvdt.hvmod[i].addr = 0;
		hvdt.hvmod[i].hvstat.actived = false;
		pHvProg[i] = nullptr;
	}
	m_servSock = 0;
	m_socket = 0;
	m_stop = 0;
}	// Constructor

//=========================================================================
CHvServeur::~CHvServeur()
{
	if(m_servSock>0) shutdown(m_servSock, 2);
	cout << "Delete CHvServeur Class" << endl;
}	// Destructor

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
void CHvServeur::Run(void)
{
	int HvPol = 0;
	TCP_Msg msg;
	
	cout << "Start Serveur on port " << m_port << endl;
	int ret = XmlParser ("Coeff/config.xml");
	cout << "XmlParser ret : " << ret << endl;
	for(int i=0; i<NB_HVMOD; i++) {
		pHvProg[i] = new HvProg( pVme ,hvdt.hvmod[i].addr, m_verbose);
		if(hvdt.hvmod[i].hvstat.actived) {
			pHvProg[i]->IsHvProg();
			pHvProg[i]->SetHvPol(HvPol);
			hvdt.hvmod[i].hvstat.positive = HvPol;
		}
	}
	Update();
	if(CreateConnection() != 0) return;
	for(;;) {
		WaitConnection();
		for(;;) {
			if(!WaitRequest(&msg)) break;
			TCPAction(&msg);
			Answer(&msg);
		}
		CloseConnection();
		if(m_stop) break;
	}
} // Run

//=========================================================================
//=========================================================================
int CHvServeur::XmlParser (string f_name)
{
	int mod= 0;
	string tempstr;
	XMLDocument xmlDoc;
	XMLError eResult = xmlDoc.LoadFile(f_name.c_str());
	if(eResult != XML_SUCCESS) {
		cerr << FgColor::red << "Error during Loading File " << f_name << ": ";
		xmlDoc.PrintError();
		cout << FgColor::white;
		return (eResult);
	}
	XMLNode *pRoot = xmlDoc.FirstChild();
	if(pRoot == nullptr ) {
		cerr << FgColor::red << "XML_ERROR_FILE_READ_ERROR" <<  FgColor::white<< endl;
		return (XML_ERROR_FILE_READ_ERROR);
	}
	
	for( ; pRoot; pRoot = pRoot->NextSibling()) {
		cout << " pRoot value : " << pRoot->Value() << endl;
		if(strcmp(pRoot->Value() ,"CNFG") == 0) {
			XMLElement *pElement = pRoot->FirstChildElement("NAME");
			if(pElement == nullptr) {
				cerr << FgColor::red << "XML_ERROR_PARSING_ELEMENT" << FgColor::white << endl;
				return(XML_ERROR_PARSING_ELEMENT);
			}
			for( ; pElement; pElement = pElement->NextSiblingElement()) {
				cout << "TITTLE : "  << pElement->Attribute("TITLE") 
				     << " ACTIVED : " << pElement->Attribute("ACTIVED")
				     << " ADDRESS : " << pElement->Attribute("ADDRESS")
				     << " COMMENT : " << pElement ->Attribute("COMMENT");
				if     (strcmp(pElement->Attribute("TITLE"), "HV_MODULE_0") == 0 ) mod = 0;
				else if(strcmp(pElement->Attribute("TITLE"), "HV_MODULE_1") == 0 ) mod = 1;
				else if(strcmp(pElement->Attribute("TITLE"), "HV_MODULE_2") == 0 ) mod = 2;
				else if(strcmp(pElement->Attribute("TITLE"), "HV_MODULE_3") == 0 ) mod = 3;
				else {
					mod = 0;
					cerr << FgColor::red << "Invalid Module Number" << FgColor::white << endl;
					return(XML_ERROR_PARSING_ELEMENT);
				}
				cout << "....Module :" << mod << endl;
				hvdt.hvmod[mod].hvstat.actived = pElement->BoolAttribute("ACTIVED");
				tempstr = pElement->Attribute("ADDRESS");
				hvdt.hvmod[mod].addr = stoul(tempstr, nullptr, 16);
			}
		}
	}  
	//for (int i=0; i<NB_HVMOD; i++)
		//cout << "Module :" << i << " Actived :" << hvdt.hvmod[i].stat.actived << " Address :0x" << hex << hvdt.hvmod[i].addr << dec << endl;
	return (XML_SUCCESS);
}	// XmlParser

//=========================================================================
//=========================================================================
void CHvServeur::Update(void)
{
	u16 tab[NB_HVCHAN_MOD];
	u16 stat;

	cout << "Update Start" << endl;
	for(int mod=0; mod<NB_HVMOD; mod++) {
		cout << "Udpate Module " << mod << endl;
//		cout << __FUNCTION__ << __LINE__<< " " << mod << endl;
		if(hvdt.hvmod[mod].hvstat.actived) {
			pHvProg[mod]->ValueHv(&tab[0]);
			for(int chan=0; chan<NB_HVCHAN_MOD; chan ++) {
				float ftmp= pHvProg[mod]->CalculAdc(tab[chan], chan);
				hvdt.hvmod[mod].hvchan[chan].val= (short) (ftmp *10.0);
				if(m_verbose) cout << "Mod:" << mod << " Chan:" << chan << " Hv:" << ftmp << " -> " << hvdt.hvmod[mod].hvchan[chan].val << endl;
			}
			pHvProg[mod]->StatHv(&stat);
			hvdt.hvmod[mod].hvstat.on = (stat >> 15) & 0x1;
			if(m_verbose) cout << "Mod:" << mod << CFormat(" Status: 0x%4x",stat) << " -> " << hvdt.hvmod[mod].hvstat.on << endl; 
		}
	}
}	//Update

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
void CHvServeur::TCPClearArgs (TCP_Msg *msg)
{
	memset((void*)&(msg->arg32[0]), 0, sizeof(u32) * TCP_NB_ARG_32);
}
//=========================================================================
//=========================================================================
void CHvServeur::TCPErrorCmd  (TCP_Msg *msg, u16 error)
{
	TCPClearArgs(msg);
	msg->stat  = error;
	msg->nbarg = 0;
}	// TCPErrorCmd

//=========================================================================
//=========================================================================
void CHvServeur::TCPAction(TCP_Msg *msg)
{
	static u32 cnt = 0;
	cnt++;
	cout << cnt << "..TCP Command N° " << msg->cmd << " with " << msg->nbarg << " args" << endl; 
	switch(msg->cmd) {
		case CMD_HV_SWITCH :					// HV  : Switch High Voltage On/Off
			TCPSWitchHV (msg);
			break;
		case CMD_HV_DUMP   :					// HV  : Get High Voltage Values
			TCPGetParms (msg);
			break;
		case CMD_HV_ORDER  :					// HV  : Set High Voltage Orders
			TCPOrder (msg);
			break;
		case CMD_HV_STOP   :					// HV : Stop Server
			TCPStop(msg);
			break;
		default :
			TCPErrorCmd(msg, UNKNOW_CMD);
	}
}	// TCPAction 

//=========================================================================
//=========================================================================
void CHvServeur::TCPGetParms  (TCP_Msg *msg)
{
	if(msg->nbarg == 0) {
		cout << "sizeof(HVDATA) " << sizeof(HVDATA) << endl;
		Update();
		TCPClearArgs(msg);
		msg->stat = NO_ERROR;
		msg->nbarg = sizeof(HVDATA) >> 1;
		memcpy((void*)&msg->arg16[0], (void*) &hvdt, sizeof(HVDATA));
	} else
		TCPErrorCmd(msg, NBARG_ERROR);
}	// TCPGetParms

//=========================================================================
//=========================================================================
void CHvServeur::TCPSWitchHV  (TCP_Msg *msg)
{
	if(msg->nbarg == 1) {
		for(int mod=0; mod<NB_HVMOD; mod++) {
			if((1<< mod) & msg->mask) {
				if(msg->arg16[0] == 1) {
					pHvProg[mod]->HvOn();
				} else {
					pHvProg[mod]->HvOff();
				}
			}
		}
		TCPErrorCmd(msg, NO_ERROR);
	} else
		TCPErrorCmd(msg, NBARG_ERROR);
}	// TCPSWitchHV

//=========================================================================
//=========================================================================
void CHvServeur::TCPOrder     (TCP_Msg *msg)
{
	if(msg->nbarg == NB_HVCHAN_MOD) {
		for(int mod=0; mod<NB_HVMOD; mod++) {
			if((1 << mod) & msg->mask) {
				for(int chan=0; chan<NB_HVCHAN_MOD; chan++) {
					hvdt.hvmod[mod].hvchan[chan].order = msg->arg16[chan]; 
					float Val = (float) msg->arg16[chan] /10.0;
					cout << "Send Hv -> Mod:" << mod << " Chan:" << chan << " Val:" << Val << endl; 
					pHvProg[mod]->SendHv(chan, Val);
				} 
			}
		}
		TCPErrorCmd(msg, NO_ERROR);
	} else
		TCPErrorCmd(msg, NBARG_ERROR);
}	// TCPOrder

//=========================================================================
//=========================================================================
void CHvServeur::TCPStop(TCP_Msg *msg)
{
	if(msg->nbarg == 0 ) {
		m_stop = 1;
		TCPErrorCmd(msg, NO_ERROR);
	} else
		TCPErrorCmd(msg, NBARG_ERROR);
}	//TCPStop

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
int CHvServeur::CreateConnection(void)
{
	// Create Socket of the server
	m_servSock = socket(AF_INET, SOCK_STREAM, 0);
	if(m_servSock < 0) {
		cerr << FgColor::red << "Could not Create Socket" << FgColor::white << endl;
		return 1;
	}
	cout << FgColor::green << "Socket Created" << FgColor::white << endl;
	// Allow Reuse od Address for Bind
	int val = 1;
	setsockopt(m_servSock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	// Bind Server Socket to given Port
	struct sockaddr_in inAddr;
	memset(&inAddr, 0, sizeof(inAddr));
	inAddr.sin_family = AF_INET;
	inAddr.sin_addr.s_addr = htons(INADDR_ANY);
	inAddr.sin_port = htons(m_port);
	if(bind(m_servSock, (struct sockaddr*)&inAddr, sizeof(inAddr))) {
		cerr << FgColor::red << "Bind Failed on port " << m_port << FgColor::white << endl;
		return(2);
	}
	cout << FgColor::green << "Bind Done" << FgColor::white << endl;
	// Listen for  request
	if(listen(m_servSock, 10)) {
		cerr << FgColor::red << "Listen Failed" << FgColor::white << endl;
		return(3);
	}
	return (0);
}	// Create

//=========================================================================
//=========================================================================
void CHvServeur::WaitConnection(void)
{
	cout << "Waiting for Connection... " << flush;
	m_socket = accept(m_servSock, 0, 0);
	cout << "Accepted " << endl;
} // WaitConnection

//=========================================================================
//=========================================================================
void CHvServeur::CloseConnection(void)
{
	cout << "Closing Connection... " << flush;
	shutdown(m_socket,2);
	cout << "Done" << endl;
}	// CloseConnection

//=========================================================================
//=========================================================================
bool CHvServeur::WaitRequest (TCP_Msg *msg)
{
	cout << " Waiting for Request... " << flush;
	memset((void*)msg, 0, TCP_BUF8_LEN);
	int nRecv = recv(m_socket, &(msg->buf8[0]), TCP_BUF8_LEN, 0);
	if(nRecv > 0) {
//		cout << endl << "NRecV " << nRecv << " ...";
//		for(int i=0; i<(nRecv/2); i++)
//			cout << hex << msg->buf[i] << dec;
//		cout << endl;
		return true;
	} else {
		return false;
	}
}	// WaitRequest

//=========================================================================
//=========================================================================
bool CHvServeur::Answer(TCP_Msg *msg)
{
	cout << "Sending Answer " << msg->stat << " ... " << flush;
	ssize_t nSent;
	// Send data Until all data have been sent
	nSent = send (m_socket, msg, TCP_BUF8_LEN, 0);
	if(nSent == 0) {
		cerr << endl << "Connection closed by Client" << endl;
		return false;
	} else if (nSent< 0){
		cerr << endl << "Error while Sending Answer : " << errno << strerror(errno) << endl;
		return false;
	}
	cout << "Done" << endl;
	return true;
}	// Answer

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================

