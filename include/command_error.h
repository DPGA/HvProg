//==========================================================
//  Command_Error.h
//==========================================================
//==========================================================
//==========================================================
#ifndef COMMAND_ERROR_H
#define COMMAND_ERROR_H
//==========================================================
//==========================================================
enum _CMD{
    CMD_HELP,						// NO  : Show Usage
    CMD_NIOS_REBOOT,				// AMC : NIOS Reboot
    CMD_NIOS_GET_ID,				// AMC : Get Id,Firm & Hard Revision

    CMD_DUMP,						// ALL : Dump Data Parameters
    CMD_READ,						// ALL : Read Value from Address
    CMD_WRITE,						// ALL : Write Value to Address

    CMD_NIOS_CDCE_SPI_READ,		// AMC : Read CDCE62005 Configuration via SPI
    CMD_NIOS_CDCE_SPI_WRITE,	// AMC : Write CDCE62005 Configuration via SPI
    CMD_NIOS_CDCE_SPI_SAVE,		// AMC : Save CDCE62005 Configuration via SPI

    CMD_NIOS_MPOD_I2C_READ,		// AMC : Read MiniPod Configuration via I2C
    CMD_NIOS_MPOD_I2C_WRITE,	// AMC : Write MiniPod Configuration via I2C

    CMD_NIOS_JTAGUART,			// AMC : Set Verbose level on JTAG UART

    CMD_EXPORT = 101,				// ASM & THOR : Export External Command With Reply
    CMD_SILENT_EXPORT,			// ASM & THOR : Export External Command Wi Without Reply
    CMD_RESET,						// All : Reset Board
    CMD_RUN,							// All : DAQ on/off
    CMD_CHANGE,						// ASM & THOR : Force Changement
    CMD_CONFIG,						// All : Configuration Board
    CMD_SAVE,						// All : Save configuration File
    CMD_INTERACT,					// All : Go to Interactive Mode
    
    CMD_HV_SWITCH = 1001,		// HV  : Switch High Voltage On/Off
    CMD_HV_DUMP,					// HV  : Get High Voltage Values
    CMD_HV_ORDER,					// HV  : Set High Voltage Orders
	CMD_HV_STOP,					// HV  : Stop Server

    ZORGLUB
};
//==========================================================
//==========================================================
enum _AMC_ERROR
{
    NO_ERROR = 0,					// Success full
    UNKNOW_CMD,						// Unknown Command
    NOT_IMPLEMENTED_CMD,			// Not Implemented Command
    NBARG_ERROR,					// Wrong Number of Arguments
    INVALID_CHAN,					// Invalid Channel Error
    CDCE_SPI_ERROR,				// CDCE SPI Error
    BROADCAST_NOTALLOWED_CMD,	// Command not allowed in Broadcast Mode
    FIFO_FULL_ERROR,				// Fifo Full Error
    DCS_TMO_ERROR,					// Dcs Time Out Reached
    BAD_ACK,							// Bad Acknowledge

    MAX_AMC_ERROR
};
//==========================================================
//==========================================================
enum _MYERROR {
    NO_BAD = 0,
    BAD_STATUS = -1,
    BAD_NBARG = -2,
    BAD_INDIC = -3,
    BAD_CONNECTION = -4,
    BAD_UNKNOW = -5,
    BAD_PARM = -6,
    BAD_CHIP = -7,
    BAD_REGISTER = -8,
    BAD_ASM_NB = -9,
    BAD_ACKNOWLEDGE = -10,
    BAD_ACK_PARITY = -11,
    BAD_ACK_LEN = -12,
    BAD_ACK_CRC = -13,
    BAD_ACK_END = -14,
    BAD_ACK_START = -15,
    BAD_ACK_FENO =-16,
    BAD_ACK_BIZZ =-17,
    BAD_DCS_FNAME = -18,
    BAD_CRC = -19,
    BAD_EPCS_TYPE = -20,
    BAD_PROG_NAME = -21,
    BAD_ACK_EPCS = -22,


    BAD_BAD
};
//==========================================================
//==========================================================

#endif // COMMAND_ERROR_H
