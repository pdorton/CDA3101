
#include "spimcore.h"
/* ALU */ /* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero) 
{
	switch (ALUControl)	{
	case 0x0:		// Add A + B  || don't care??
		*ALUresult = A + B;
		break;
	case 0x1:		// A - B
		*ALUresult = A - B;
		break;
	case 0x2:		// if A < B, Z = 1; else Z = 0;
		*Zero = 0;
		if (A < B)
			*Zero = 1;
		break;
	case 0x3:		// if A < B, Z = 1; else Z = 0 (A and B are unsigned)
		*Zero = 0;
		if (A < B)
			*Zero = 1;
		break;
	case 0x4:		// Z = A AND B
		*Zero = A & B;
		break;
	case 0x5:		// Z = A OR B
		*Zero = A | B;
		break;
	case 0x6:		// Shift left B by 16 bits
		*ALUresult = B >> 16;
		break;
	case 0x7:		// Z = NOT A
		*Zero = !A;
		break;
	}
}


/* instruction fetch */ /* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction) 
{
	/* Need to make sure that *instruction is of type HEX (0x); if not then convert from decimal to hex */

    // PC is counter (0x4000)
    //*Mem is memory location (use 4* *MEM + PC for intended location)
    // *instruction is the HEX value being passed for action
	unsigned *op = NULL, 
		*r1 = NULL,
		*r2 = NULL,
		*r3 = NULL,
		*funct = NULL,
		*offset = NULL,
		*jsec = NULL;
	instruction_partition(*instruction, op, r1, r2, r3, funct, offset, jsec);
    /* instruction_decode() */
}


/* instruction partition */ /* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	*op = instruction >> 26;
	/* R-Type */// op|6   rs|5   rt|5   rd|5   shamt|5   funct|6
	if (*op == 0x0) { // this is a r-type
		r1 = ((instruction >> 21) & 0x1F);
		*r2 = ((instruction >> 16) & 0x1F);
		*r3 = ((instruction >> 11) & 0x1F); 
		*offset = ((instruction >> 6) & 0x1F);
		*funct = instruction & 0x3F;
	}


	/* J-Type */// op|6   address|26 
	else if(*op == 0x000010 || *op == 0x000011) { // if jump ot jump link
		// address = (instruction & 0x3FFFFFF);
	}
	else if 
	/* I-Type */
	// op|6   rs|5   rt|5   conts/addy|16
	
	/*
	2x 0x000000 // R-Type 

	0x000010 // jump				J
	0x000011 // jump link			J
	0x000100 // branch if eq		I
	0x000101 // branch not eq		I
	0x001000 // addi				R
	0x001001 // addiu				R
	0x001010 // slti				I
	0x001011 // sltiu				I
	0x001100 // andi				I		
	0x001101 // ori					I
	0x001111 // lui					I	
	0x100000 // lb load byte		I
	0x100001 // lh load half		I
	0x100011 // lw load word		I
	0x100100 // lbu					I
	0x100101 // lhu					I
	0x101000 // sb store byte		I
	0x101001 // sh store half		I
	0x101011 // sw store word		I
	0x110001 // lwcl				I
	0x111001 // swcl				I
	*/


}


/* instruction decode */ /* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls) 
{
	
	/*take in the op code and see if the instruction is a R , I, or J type
	and set op codes */
	return 0;
}


/* Read Register */ /* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2) 
{
// two lines of code
}


/* Sign Extend */ /* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value) 
{
	int first_digit = offset >> 15;
	if (first_digit)							// if negative then
		*extended_value = offset | 0xFFFF0000;	// OR with 32 bit with leading 16 1s 
	else										// otherwise positive
		*extended_value = offset | 0x00000000;	// OR with 0 which will return with leading 0s        
}


/* ALU operations */ /* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero) 
{
	return 0;
}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem) 
{
	return 0;
}


/* Write Register */ /* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg) 
{
}

/* PC update */ /* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC) 
{

}