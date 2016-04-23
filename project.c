#include "spimcore.h"

void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	// ALU Operations
    switch((int)ALUControl){
        case 0:
            *ALUresult = A + B;
            break;
        case 1:
            *ALUresult = A - B;
            break;
        case 2:
            if((signed) A < (signed) B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        case 3:
            if(A < B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
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
		default:
			break;
    }
    // if out result is 0, assert Zero, otherwise de-assert Zero 
    if(*ALUresult)
        *Zero = 0;
    else
        *Zero = 1;
}

int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	// if PC is not within range or word arranged, HALT 
	if (((PC % 4) != 0) || (PC >> 2) > 0x10000 || PC < 0x0)
		return 1;
	
	// Fetch instruction addressed by PC from Mem and write to instruction
	*instruction = Mem[PC >> 2];
	return 0;	
}

void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	// partition for all types
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

int instruction_decode(unsigned op,struct_controls *controls)
{
	// default valid code as true, if our OP does not comply with our cases we halt by deasserting validCode
	int validCode = 1;
	switch (op) {
	case 0:	// r-type
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
	case 8: // addi
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
	case 35: // lw
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
	case 43: // sw
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
	case 15: // lui
		controls->RegDst = 0;
		controls->ALUSrc = 1;
		controls->MemtoReg = 0;
		controls->RegWrite = 1;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 6;
		break;
	case 4: // sw 
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
	case 10: // slt
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
	case 11: // sltu
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
	case 2:	// jump
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
	default: // invalid code
		validCode = 0;
		break;
	}

	if (validCode)
		return 0;
	// HALTING
	return 1;
}

void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{   
	// read data from registers
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

void sign_extend(unsigned offset,unsigned *extended_value)
{
	// shift for first of 16 bits
    unsigned is_negative = offset >> 15;
    
	// if negative, concat leading 16 1's to our value.
    if(is_negative)
        *extended_value = offset | 0xFFFF0000;
    // else concat 16 leading 0's as our value leads with a 0
	else
        *extended_value = offset & 0x0000FFFF;
}

int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	// ALU op: 
	if (ALUOp == 2) {
		switch (funct) {
		case 32:
			// add\n
			ALUOp = 0;
			break;

		case 34:
			// sub\n
			ALUOp = 1;
			break;

		case 42:
			// slt\n
			ALUOp = 2;
			break;

		case 43:
			// sltu\n
			ALUOp = 3;
			break;

		case 36:
			// and\n
			ALUOp = 4;
			break;

		case 37:
			// or\n
			ALUOp = 5;
			break;

		case 4:
			// sll
			ALUOp = 6;
			break;

		case 39:
			// not
			ALUOp = 7;
			break;

		default:
			// HALTING
			return 1;
			break;
		}
	}
    	
	if (ALUSrc == 1) // not an ALU op
	{
		// Using Sign Extended
		data2 = extended_value;
	}
	ALU(data1, data2, ALUOp, ALUresult, Zero);
    return 0;
}

int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{ 	
	int read = (int) MemRead;
	int write = (int) MemWrite;
			
	if (read) // if read is asserted, read from mem location
		*memdata = Mem[ALUresult >> 2];
	
	else if (write) // if write is asserted, write to location
		Mem[ALUresult >> 2] = data2;
	
	return 0;
}

void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{	
    // if we are writing to a register
    if(RegWrite){		
        if(MemtoReg && RegDst) 
		    Reg[r2] = memdata;		
        
        else if(MemtoReg && RegDst)
			Reg[r3] = memdata;
			        
        else if(MemtoReg && RegDst)
            Reg[r2] = ALUresult;
		
        else if(MemtoReg && RegDst)
            Reg[r3] = ALUresult;
    }
}

void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	// always increment by 4 (word size)
	*PC += 4;

	if (Zero && Branch) // if we are branching, jump to branch loc
		*PC += extended_value << 2;

	if (Jump) // jump to new mem loc 
		*PC = (jsec << 2) | (*PC & 0xf0000000);
}