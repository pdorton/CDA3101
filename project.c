#include "spimcore.h"

void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
	switch ((signed)ALUControl) {
	case 0:
		*ALUresult = A + B;
		break;
	case 1:
		*ALUresult = A - B;
		break;
	case 2:
		if ((signed)A < B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
		break;
	case 3:
		*ALUresult = 0;
		if (A < B)
			*ALUresult = 1;
		break;
	case 4:
		*ALUresult = A & B;
		break;
	case 5:
		*ALUresult = A | B;
		break;
	case 6:
		*ALUresult = B << 16;
		break;
	case 7:
		*ALUresult = ~A;
		break;
	}
	if (ALUresult == 0)
		*Zero = 1;
	else
		*Zero = 0;
}

int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
	printf("instruction fetch");
	if ((signed)PC >= 0 && PC % 4 == 0 && (signed)PC <= 65535) {
		*instruction = Mem[4 * PC];
		return 0;
	}
	return 1;
}

void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	printf("instruction part");

	const unsigned FIVE_BITS = 0x1F;
	const unsigned SIX_BITS = 0x3F;
	const unsigned SIXTEEN_BITS = 0xFFFF;
	const unsigned TWENTYSIX_BITS = 0x3FFFFFF;
	*op = ((instruction >> 26) & SIX_BITS);
	*r1 = ((instruction >> 21) & FIVE_BITS);
	*r2 = ((instruction >> 16) & FIVE_BITS);
	*r3 = ((instruction >> 11) & FIVE_BITS);
	*funct = instruction & SIX_BITS;
	*offset = instruction & SIXTEEN_BITS;
	*jsec = instruction & TWENTYSIX_BITS;
}


int instruction_decode(unsigned op, struct_controls *controls)
{
	printf("instruction decode");

	int validCode = 1;
	switch (op) {
	case 0:
		controls->RegDst = 1;
		controls->ALUSrc = 0;
		controls->MemtoReg = 0;
		controls->RegWrite = 1;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 2;
		break;
	case 8:
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		break;
	case 35:
		controls->RegDst = 0;
		controls->ALUSrc = 1;
		controls->MemtoReg = 1;
		controls->RegWrite = 1;
		controls->MemRead = 1;
		controls->MemWrite = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 0;
		break;
	case 43:
		controls->RegDst = 2;
		controls->ALUSrc = 1;
		controls->MemtoReg = 2;
		controls->RegWrite = 0;
		controls->MemRead = 0;
		controls->MemWrite = 1;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 0;
		break;
	case 15:
		controls->RegDst = 1;
		controls->ALUSrc = 1;
		controls->MemtoReg = 0;
		controls->RegWrite = 1;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 2;
		break;
	case 4:
		controls->RegDst = 2;
		controls->ALUSrc = 0;
		controls->MemtoReg = 2;
		controls->RegWrite = 0;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->Jump = 0;
		controls->Branch = 1;
		controls->ALUOp = 1;
		break;
	case 10:
		controls->RegDst = 1;
		controls->RegWrite = 1;
		controls->ALUSrc = 0;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->MemtoReg = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 2;
		break;
	case 11:
		controls->RegDst = 1;
		controls->RegWrite = 1;
		controls->ALUSrc = 0;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->MemtoReg = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 3;
		break;
	case 2:
		controls->RegDst = 0;
		controls->RegWrite = 0;
		controls->ALUSrc = 0;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->MemtoReg = 0;
		controls->Jump = 1;
		controls->Branch = 0;
		controls->ALUOp = 0;
		break;
	default:
		validCode = 0;
		break;
	}
	if (validCode)
		return 0;
	return 1;
}

void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}

void sign_extend(unsigned offset, unsigned *extended_value)
{
	int first_digit = offset >> 15;
	if (first_digit)
		*extended_value = offset | 0xFFFF0000;
	else
		*extended_value = offset | 0x00000000;
}

int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
	unsigned char ALUControl = ALUOp;
	if (ALUSrc == 1)
		data2 = extended_value;

	if (ALUControl == 7)
		switch (funct) {
		case 32:
			ALUControl = 0;
			break;
		case 34:
			ALUControl = 1;
			break;
		case 42:
			ALUControl = 2;
			break;
		case 43:
			ALUControl = 3;
			break;
		case 36:
			ALUControl = 4;
			break;
		case 37:
			ALUControl = 5;
			break;
		case 4:
			ALUControl = 6;
			break;
		case 39:
			ALUControl = 7;
			break;
		default:
			return 1;
		}

	ALU(data1, data2, ALUControl, ALUresult, Zero);
	return 0;
}

int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
	//if we are reading,
	//Read data from ALUresult * 4 index in Memory.
	if (MemRead == 1) {
		if ((ALUresult % 4) == 0) {
			*memdata = Mem[ALUresult >> 2];
		}
		else
			return 1;
	}

	//if we are writing,
	//Write data to ALUresult * 4 index in Memory
	//Word aligned
	if (MemWrite == 1) {
		if ((ALUresult % 4) == 0) {
			Mem[ALUresult >> 2] = data2;
		}
		else
			return 1;
	}

	return 0;
}

void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{

	if (RegWrite == 1)
	{
		if (MemtoReg == 1)
		{
			Reg[r2] = memdata;
		}
		else if (MemtoReg == 0)
		{
			if (RegDst == 1)
			{
				Reg[r3] = ALUresult;
			}
			else
			{
				Reg[r2] = ALUresult;
			}
		}
	}
}

void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{

	*PC += 4;


	if (Zero && Branch)
		*PC += extended_value << 2;


	else if (Jump)
		*PC = (jsec << 2) | (*PC & 0xf0000000);
}
