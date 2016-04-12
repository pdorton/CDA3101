
#include "spimcore.h"
/* ALU */ /* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero) {
	switch (ALUControl)	{
	case 000:		// Add A + B  || don't care??
		ALUresult = A + B;
		break;
	case 001:		// A - B
		ALUresult = A - B;
		break;
	case 010:		// if A < B, Z = 1; else Z = 0;
		Zero = 0;
		if (A < B)
			Zero = 1;
		break;
	case 011:		// if A < B, Z = 1; else Z = 0 (A and B are unsigned)
		Zero = 0;
		if (A < B)
			Zero = 1;
		break;
	case 100:		// Z = A AND B
		Zero = A & B;
		break;
	case 101:		// Z = A OR B
		Zero = A | B;
		break;
	case 110:		// Shift left B by 16 bits
		ALUresult = B >> 16;
		break;
	case 111:		// Z = NOT A
		Zero = !A;
		break;
	}
}


/* instruction fetch */ /* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction) {
    // PC is counter (0x4000)
    //*Mem is memory location (use 4* *MEM + PC for intended location)
    // *instruction is the HEX value being passed for action
    unsigned *op, *r1, *r2, *r3, *funct, *offset, *jsec;
    instruction_partition(instruction, &op, &r1, &r2, &r3, &funct, &offset, &jsec) 
    /* instruction_decode() */
}


/* instruction partition */ /* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec) {
}


/* instruction decode */ /* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls) {
}


/* Read Register */ /* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2) {
// two lines of code
}


/* Sign Extend */ /* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value) {
	int first_digit = offset >> 15;
    if (first_digit) // if negative then
        *extended_value = offset | 0xFFFF0000 // OR with 32 bit with leading 16 1s 
    else
        *extended_value = offset | 0x00000000 //OR with 0 which will return with leading 0s        
}


/* ALU operations */ /* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero) {
} /* Read / Write Memory */


/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem) {
}


/* Write Register */ /* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg) {
}

/* PC update */ /* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC) {
}