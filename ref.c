#include "spimcore.h"

void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
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
			printf("something went wrong");
			break;
    }
    
    
    if(*ALUresult == 0)
        *Zero = 1;
    else
        *Zero = 0;
}

int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	// printf("fetch pc: %d, memloc: %x\n, value: %x\n", (int)PC, PC>>2, Mem[PC>>2] );
	if (((PC % 4) != 0) || (PC >> 2) > 0x10000 || PC < 0x0)
		return 1;
	
	
	// Fetch instruction addressed by PC from Mem and write to instruction
		*instruction = Mem[PC >> 2];
		return 0;
	
}

void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

	printf("instruction partition-------------------------\n");
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
	printf("instruction decode: ");

	int validCode = 1;
	switch (op) {
	case 0:
	printf("r-type\n");
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
	printf("addi\n");
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
	printf("lw\n");
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
		printf("jump\n");
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
		printf("invalid code\n");
		validCode = 0;
		break;
	}
	printf("\nregdest: \t%x\nregwrite : \t%x\nalusrc : \t%x\nmemread : \t%x\nmemwrite : \t%x	\nmemtoreg : \t%x\njump : \t\t%x\nbranch : \t%x\naluop : \t%d\n",
		controls->RegDst,
		controls->RegWrite,
		controls->ALUSrc,
		controls->MemRead,
		controls->MemWrite,
		controls->MemtoReg,
		controls->Jump, 
		controls->Branch,	
		controls->ALUOp);
	if (validCode)
		return 0;
	printf("HALTING\n");
	return 1;
}

void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{   
    *data1 = Reg[r1];
    *data2 = Reg[r2];
	printf("data1:\t%d\ndata2:\t%x\n", *data1, *data2);
}

void sign_extend(unsigned offset,unsigned *extended_value)
{
    unsigned negative = offset >> 15;
    
    if(negative == 1)
        *extended_value = offset | 0xFFFF0000;
    
    else
        *extended_value = offset & 0x0000FFFF;
}

int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	
	if (ALUOp == 7) {
		printf("is an ALU op\n");
		switch (funct) {
		case 32:
			printf("add\n");
			ALUOp = 0;
			break;

		case 34:
			printf("sub\n");
			ALUOp = 1;
			break;

		case 42:
			printf("slt\n");
			ALUOp = 2;
			break;

		case 43:
			printf("sltu\n");
			ALUOp = 3;
			break;

		case 36:
			printf("and\n");
			ALUOp = 4;
			break;

		case 37:
			printf("or\n");
			ALUOp = 5;
			break;

		case 4:
			printf("sll");
			ALUOp = 6;
			break;

		case 39:
			printf("not");
			ALUOp = 7;
			break;

		default:
			printf("HALTING\n");
			return 1;
			break;
		}
	}
	else printf("is not an ALU op\n");
    
	printf("ALU Operations-------------------\naluop:\t%x\ndata1:\t%x\ndata2:\t%x", ALUOp, data1, data2);
	
	if (ALUSrc == 1)
	{
		printf("using sign extended\n");
		data2 = extended_value;
	}
	ALU(data1, data2, ALUOp, ALUresult, Zero);
    return 0;
}

int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{ 
	
	printf("info dump:\naluresult:\t%x \ndata2:\t\t%x \nmemwrite:\t%x \nmemread:\t%x \nmemdata:\t%x \nmem:\t\t%x \n",
		ALUresult, data2, MemWrite, MemRead, *memdata, *Mem);      
	unsigned newLoc;
	
	int read = (int) MemRead;
	int write = (int) MemWrite;
	
	if(read || write) {
		
		if (read)
		{// if 1 then asserted if 0 the de-asserted
			printf("reading to memory\n");
			newLoc = ALUresult >> 2 ; /* to change ALUresult to a word indicated at its location */
			*memdata = Mem[newLoc];
		}
		else if (write)
		{// 1 for asserted and 0 for de-asserted
			printf("writing to memory\n");
			newLoc = data2;
		}
	}
	printf("not reading or writing to memory\n");
	return 0;
}

void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    
    if(RegWrite==1){
		printf("writing to a register\n");
        
		
        if(MemtoReg == 1 && RegDst == 0) {
        	printf("1mem->reg | %x -> %x\n", memdata, r2);
		    Reg[r2] = memdata;
		}
        
        else if(MemtoReg == 1 && RegDst == 1){
        	printf("2mem->reg | %x -> %x\n", memdata, r3);
			Reg[r3] = memdata;
			}
        
        
        else if(MemtoReg == 0 && RegDst == 0){
        	printf("1alu->reg | %x -> %x\n", memdata, ALUresult);
            Reg[r2] = ALUresult;
		}
        
        else if(MemtoReg == 0 && RegDst == 1){
        	printf("2alu->reg | %x -> %x\n", memdata, ALUresult);
            Reg[r3] = ALUresult;
		}
    }
}

void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    printf("%x", *PC);
	printf("updating PC!\n");
	*PC += 4;
    printf("%x", *PC);


	if (Zero && Branch)
	{
		printf("branching\n");
		*PC += extended_value << 2;
	}


	else if (Jump)
		printf("jumping\n");
	*PC = (jsec << 2) | (*PC & 0xf0000000);
}