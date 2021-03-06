// license:BSD-3-Clause
// copyright-holders:hap
/*

  Hitachi HMCS40 MCU family disassembler

*/

#include "emu.h"
#include "debugger.h"
#include "hmcs40.h"


enum e_mnemonics
{
	mLAB, mLBA, mLAY, mLASPX, mLASPY, mXAMR,
	mLXA, mLYA, mLXI, mLYI, mIY, mDY, mAYY, mSYY, mXSP,
	mLAM, mLBM, mXMA, mXMB, mLMAIY, mLMADY,
	mLMIIY, mLAI, mLBI,
	mAI, mIB, mDB, mAMC, mSMC, mAM, mDAA, mDAS, mNEGA, mCOMB, mSEC, mREC, mTC, mROTL, mROTR, mOR,
	mMNEI, mYNEI, mANEM, mBNEM, mALEI, mALEM, mBLEM,
	mSEM, mREM, mTM,
	mBR, mCAL, mLPU, mTBR, mRTN,
	mSEIE, mSEIF0, mSEIF1, mSETF, mSECF, mREIE, mREIF0, mREIF1, mRETF, mRECF, mTI0, mTI1, mTIF0, mTIF1, mTTF, mLTI, mLTA, mLAT, mRTNI,
	mSED, mRED, mTD, mSEDD, mREDD, mLAR, mLBR, mLRA, mLRB, mP,
	mNOP, mILL
};

static const char *const s_mnemonics[] =
{
	"LAB", "LBA", "LAY", "LASPX", "LASPY", "XAMR",
	"LXA", "LYA", "LXI", "LYI", "IY", "DY", "AYY", "SYY", "XSP",
	"LAM", "LBM", "XMA", "XMB", "LMAIY", "LMADY",
	"LMIIY", "LAI", "LBI",
	"AI", "IB", "DB", "AMC", "SMC", "AM", "DAA", "DAS", "NEGA", "COMB", "SEC", "REC", "TC", "ROTL", "ROTR", "OR",
	"MNEI", "YNEI", "ANEM", "BNEM", "ALEI", "ALEM", "BLEM",
	"SEM", "REM", "TM",
	"BR", "CAL", "LPU", "TBR", "RTN",
	"SEIE", "SEIF0", "SEIF1", "SETF", "SECF", "REIE", "REIF0", "REIF1", "RETF", "RECF", "TI0", "TI1", "TIF0", "TIF1", "TTF", "LTI", "LTA", "LAT", "RTNI",
	"SED", "RED", "TD", "SEDD", "REDD", "LAR", "LBR", "LRA", "LRB", "P",
	"NOP", "?"
};

// number of bits per opcode parameter, -3 means (XY) parameter
static const INT8 s_bits[] =
{
	0, 0, 0, 0, 0, 4,
	0, 0, 4, 4, 0, 0, 0, 0, -3,
	-3, -3, -3, -3, -3, -3,
	4, 4, 4,
	4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	4, 4, 0, 0, 4, 0, 0,
	2, 2, 2,
	6, 6, 5, 3, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
	0, 0, 0, 4, 4, 3, 3, 3, 3, 3,
	0, 0
};

#define _OVER DASMFLAG_STEP_OVER
#define _OUT  DASMFLAG_STEP_OUT

static const UINT32 s_flags[] =
{
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0,
	0, _OVER, 0, 0, _OUT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _OUT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0
};


static const UINT8 hmcs40_mnemonic[0x400] =
{
/*  0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F	   */
	/* 0x000 */
	mNOP,   mXSP,   mXSP,   mXSP,   mSEM,   mSEM,   mSEM,   mSEM,   mLAM,   mLAM,   mLAM,   mLAM,   mILL,   mILL,   mILL,   mILL,
	mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY, mLMIIY,
	mLBM,   mLBM,   mLBM,   mLBM,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mAMC,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mLTA,   mILL,   mILL,   mILL,
	/* 0x040 */
	mLXA,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mREC,   mILL,   mILL,   mILL,   mSEC,
	mLYA,   mILL,   mILL,   mILL,   mIY,    mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mLBA,   mILL,   mILL,   mILL,   mIB,    mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,   mLAI,
	/* 0x080 */
	mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,    mAI,
	mSED,   mILL,   mILL,   mILL,   mTD,    mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mSEIF1, mSECF,  mSEIF0, mILL,   mSEIE,  mSETF,  mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	/* 0x0c0 */
	mLAR,   mLAR,   mLAR,   mLAR,   mLAR,   mLAR,   mLAR,   mLAR,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,  mSEDD,
	mLBR,   mLBR,   mLBR,   mLBR,   mLBR,   mLBR,   mLBR,   mLBR,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,  mXAMR,

/*  0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F	   */
	/* 0x100 */
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mLMAIY, mLMAIY, mILL,   mILL,   mLMADY, mLMADY, mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mOR,    mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	/* 0x140 */
	mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,   mLXI,
	mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,   mLYI,
	mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,   mLBI,
	mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,   mLTI,
	/* 0x180 */
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mTIF1,  mTI1,   mTIF0,  mTI0,   mILL,   mTTF,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	/* 0x1c0 */
	mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,
	mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,
	mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,
	mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,    mBR,

/*  0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F	   */
	/* 0x200 */
	mTM,    mTM,    mTM,    mTM,    mREM,   mREM,   mREM,   mREM,   mXMA,   mXMA,   mXMA,   mXMA,   mILL,   mILL,   mILL,   mILL,
	mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,  mMNEI,
	mXMB,   mXMB,   mXMB,   mXMB,   mROTR,  mROTL,  mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mSMC,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mLAT,   mILL,   mILL,   mILL,
	/* 0x240 */
	mLASPX, mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mTC,
	mLASPY, mILL,   mILL,   mILL,   mDY,    mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mLAB,   mILL,   mILL,   mILL,   mDB,    mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,  mALEI,
	/* 0x280 */
	mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,  mYNEI,
	mRED,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mREIF1, mRECF,  mREIF0, mILL,   mREIE,  mRETF,  mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	/* 0x2c0 */
	mLRA,   mLRA,   mLRA,   mLRA,   mLRA,   mLRA,   mLRA,   mLRA,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,  mREDD,
	mLRB,   mLRB,   mLRB,   mLRB,   mLRB,   mLRB,   mLRB,   mLRB,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,

/*  0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F	   */
	/* 0x300 */
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mCOMB,  mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	/* 0x340 */
	mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,
	mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,   mLPU,
	mTBR,   mTBR,   mTBR,   mTBR,   mTBR,   mTBR,   mTBR,   mTBR,   mP,     mP,     mP,     mP,     mP,     mP,     mP,     mP,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	/* 0x380 */
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mRTNI,  mILL,   mILL,   mRTN,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,   mILL,
	/* 0x3c0 */
	mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,
	mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,
	mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,
	mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL,   mCAL
};



CPU_DISASSEMBLE(hmcs40)
{
	int pos = 0;
	UINT16 op = (oprom[pos] | oprom[pos + 1] << 8) & 0x3ff;
	pos++;
	char *dst = buffer;
	UINT8 instr = hmcs40_mnemonic[op];
	INT8 bits = s_bits[instr];
	
	// special case for (XY) opcode
	if (bits == -3)
	{
		dst += sprintf(dst, "%s", s_mnemonics[instr]);

		if (op & 1)
			dst += sprintf(dst, "X");
		if (op & 2)
			dst += sprintf(dst, "Y");
	}
	else
		dst += sprintf(dst, "%-6s ", s_mnemonics[instr]);
	
	// opcode parameter
	if (bits > 0)
	{
		UINT8 param = op & ((1 << bits) - 1);

		if (bits > 5)
			dst += sprintf(dst, "$%02X", param);
		else
			dst += sprintf(dst, "%d", param);
	}

	return pos | s_flags[instr] | DASMFLAG_SUPPORTED;
}
