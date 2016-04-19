
#include "spimcore.h"
/* ALU 
1.	Implement the operations on input parameters A and B according to ALUControl.
2.	Output the result (Z) to ALUresult.
3.	Assign Zero to 1 if the result is zero; otherwise, assign 0.
4.	The following table shows the operations of the ALU.
*/
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero) 
{
	switch (ALUControl) {
	case 0x0:		// Add A + B  || don't care??
		*ALUresult = A + B;
		break;
	case 0x1:		// A - B
		*ALUresult = A - B;
		break;
	case 0x2:		// if A < B, Z = 1; else Z = 0;
		*ALUresult = 0;
		if (A < B)
			*ALUresult = 1;
		break;
	case 0x3:		// if A < B, Z = 1; else Z = 0 (A and B are unsigned)
		*ALUresult = 0;
		if (A < B)
			*ALUresult = 1;
		break;
	case 0x4:		// Z = A AND B
		*ALUresult = A & B;
		break;
	case 0x5:		// Z = A OR B
		*ALUresult = A | B;
		break;
	case 0x6:		// Shift left B by 16 bits
		*ALUresult = B >> 16;
		break;
	case 0x7:		// Z = NOT A
		*ALUresult = ~A;

		break;
	}
	if(ALUresult * 0x0)
		*Zero = 1;
	*Zero = 0;
}

/*
If one of the following situations is encountered, the global flag Halt is set to 1, and hence the simulation halts.
 	An illegal instruction is encountered.
 	Jumping to an address that is not word-aligned (being multiple of 4).
 	The address of lw or sw is not word-aligned.
  	Accessing data or jump to address that is beyond the memory.
Note: The instructions beyond the list of instructions in Figure 1 are illegal.
*/
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
	// No halt condition has been met here. This needs to be addressed
	*instruction = *Mem + 4 * PC;
	return 0;
}

/*
1.	Fetch the instruction addressed by PC from Mem and write it to instruction.
2.	Return 1 if a halt condition occurs; otherwise, return 0.
*/
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	/* Required Instructions
	Hex			Command				Type
	0x000000 // add					R
	0x000000 // sub					R
	0x001000 // addi				I
	0x000000 // and					R
	0x000000 // or					R
	0x100011 // lw load word		I
	0x101011 // sw store word		I
	0x001111 // lui					I
	0x000100 // branch on eq		I
	0x000000 // slt					R
	0x001010 // slti				I
	0x000000 // sltu				R
	0x001011 // sltiu				I
	0x000010 // jump				J
	*/

	*op = instruction >> 26;

	/* R-Type */
	if (*op == 0x0) {
		*r1 = ((instruction >> 21) & 0x1F);
		*r2 = ((instruction >> 16) & 0x1F);
		*r3 = ((instruction >> 11) & 0x1F);
		*offset = ((instruction >> 6) & 0x1F);
		*funct = instruction & 0x3F;
	}

	/* J-Type */
	else if (*op == 0x10) { // if jump 
		*jsec = (instruction & 0x3FFFFFF);
	}

	/* I-Type */
	else if (*op == 0x001000 || // addi
		*op == 0x100011 || // lw
		*op == 0x101011 || // sw
		*op == 0x001111 || // lui
		*op == 0x000100 || // beq
		*op == 0x001010 || // slti
		*op == 0x001011) { // sltui 
		*r1 = ((instruction >> 21) & 0x1F);
		*r2 = ((instruction >> 16) & 0x1F);
		*jsec = instruction & 0xFFFF;
	}

	/* Unsupported Op Code */
	else
		return 1; // hey that's bad HALT!
	return 0; // everything is okay
}


/*
1.	Decode the instruction using the opcode (op).
2.	Assign the values of the control signals to the variables in the structure controls (See spimcore.h file).
The meanings of the values of the control signals:
For MemRead, MemWrite or RegWrite, the value 1 means that enabled, 0 means that disabled, 2 means �don�t care�.
For RegDst, Jump, Branch, MemtoReg or ALUSrc, the value 0 or 1 indicates the selected path of the multiplexer; 2 means �don�t care�.
The following table shows the meaning of the values of ALUOp.
3.	Return 1 if a halt condition occurs; otherwise, return 0.
*/
int instruction_decode(unsigned op, struct_controls *controls) 
{

	/*
	{	
		Below these controls need to be set to 1 or 0 according to the instruction
		that is being read in from the op code except for RegDest which is an address:
		
		char RegDst; 
		char Jump; 
		char Branch; 
		char MemRead; 
		char MemtoReg; 
		char ALUOp; 
		char MemWrite; 
		char ALUSrc; 
		char RegWrite; 
	}struct_controls;
	*/
	
	switch( op )
	{
		
		
		case 0x0:
		{// addition or don't care
			
		}
		case 0x1:
		{// subtraction
			
		}
		case 0x2:
		{// set on less than
			
		}
		case 0x3:
		{//set less than unsigned
			
		}
		case 0x4:
		{//and operator 
 			
		}
		case 0x5:
		{// or operator		
			
		}
		case 0x6:
		{ // shift left extended by 16 bits
			
		}
		case 0x7:
		{ // rtypes 
			
		}	
	}// end of switch 
	
	
	
		
	/*take in the op code and see if the instruction is an 
	R , I, or J type and set op codes 
	1 : enabled
	0 : Disabled 
	2 : Don't Care */
	
	return 0;
}


/*
1.	Read the registers addressed by r1 and r2 from Reg, and write the read values to data1 and data2 respectively.
*/
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2) 
{
	*data1 = r1; // assign r1 to data 1 
	*data2 = r2; // assign r2 to data 2
}


/*
1.	Assign the sign-extended value of offset to extended_value.
*/
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
	if( MemRead && MemWrite)
	{// if both are assered then this is a don't care and return 0
		return 0;
	}
	else if(MemRead)
	{// if 1 then asserted if 0 the de-asserted
		unsigned newLoc = ALUresult<<; /* to change ALUresult 
			to a word indicated at its location */
		*memdata = Mem[newLoc];
	}
	else if(MemWrite)
	{// 1 for asserted and 0 for de-asserted
		newLoc = data2;
	}
	return 0;
}


/* Write Register */ /* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg) 
{
}

/* PC update */ /* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC) 
{
	if(Jump)
		*PC = jsec*4;
	if(Branch )
}
