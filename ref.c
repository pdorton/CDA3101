#include "spimcore.h"

void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch((int)ALUControl){
        //Z = A+B
        case 0:
            *ALUresult = A + B;
            break;
        //Z = A-B
        case 1:
            *ALUresult = A - B;
            break;
        //If A<B, Z = 1, otherwise Z = 0
        case 2:
            if((signed) A < (signed) B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        //If A<B, Z = 1, otherwise Z = 0 UNSIGNED
        case 3:
            if(A < B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        //Z = A AND B
        case 4:
            *ALUresult = A & B;
            break;
        //Z = A OR B
        case 5:
            *ALUresult = A | B;
            break;
        //Shift left B by 16 bits
        case 6:
            *ALUresult = B << 16;   
            break;
        //Z = NOT A
        case 7:
            *ALUresult = ~A;
            break;
		default:
			break;
    }
    
    //Now check to see if the result is zero, and set the zero bit
    if(*ALUresult == 0)
        *Zero = 1;
    else
        *Zero = 0;
}

int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	//Multiply by 4 to get the proper address in memory
	unsigned index = PC >> 2;

    // if PC isn't byte addressed (divisible by 4)
	if(PC % 4 != 0)
	    return 1;
    // set intruction equal to memory at proper address
	*instruction = Mem[index];
	return 0;
}

void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // shift bits until we have only what we want for each instruction field
    // mask with 0s and 1s to isolate only the digits for that field
	*op = (instruction >> 26) & 0b00000000000000000000000000111111; // instruction[31-26]
	*r1 = (instruction >> 21) & 0b11111; // instruction[25-21]
	*r2 = (instruction >> 16) & 0b11111; // instruction[20-16]
	*r3 = (instruction >> 11) & 0b11111; // instruction[15-11]
	*funct = instruction & 0b00000000000000000000000000111111; // instruction[5-0]
	*offset = instruction & 0b00000000000000001111111111111111; // instruction[15-0]
	*jsec = instruction & 0b00000011111111111111111111111111; // instruction[25-0]
}

int instruction_decode(unsigned op,struct_controls *controls)
{
    // the values we set depend on which opcode is recieved
    switch(op){
        //R Type Instructions
        case 0:
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 7;
            break;
        //End R Type Instructions
        
        //I Type Instructions
        //addi
        case 8:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
        
        //slt
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
            
        //sltu
        case 11:
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 0 ;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 3;
            break;
            
        //beq
        case 4:
            controls->RegDst = 2;
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 2;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->ALUOp = 1;
            break;
            
        //lw
        case 35:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 1;
            controls->MemWrite = 0;
            controls->MemtoReg = 1;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
            
        //lui
        case 15:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 6;
            break;
            
        //sw
        case 43: ++++++++++++++
            controls->RegDst = 2; // 2 for don't care
            controls->RegWrite = 0;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->MemtoReg = 2;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            break;
        //End I Type Instructions
        
        //J Type Instructions
        //Jump
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
        //End J Type Instructions
        
        default:
            return 1;
    }
    return 0;
}

void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // fill data1 and data2 with the values from register array (registers 1 and 2)
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

void sign_extend(unsigned offset,unsigned *extended_value)
{
    unsigned negative = offset >> 15;
    //if the number is negative (MSB = 1), extend with 1s
    if(negative == 1)
        *extended_value = offset | 0xFFFF0000;
    //otherwise extend with 0's
    else
        *extended_value = offset & 0x0000FFFF;
}

int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    // if the ALUSrc is 1, set data2 to extended_value
	if (ALUSrc == 1) {
		data2 = extended_value;
	}

	// 7 is R-type instruction
    if (ALUOp == 7) {
    	// set the correct ALUOp for each R-type instruction
    	switch(funct) {
	    	// add
	    	case 32:
	    		ALUOp = 0;
	    		break;
	    	// subtract
	    	case 34:
	    		ALUOp = 1;
	    		break;
	    	// set less than signed
	    	case 42:
	    		ALUOp = 2;
	    		break;
	    	// set less than unsigned
	    	case 43:
	    		ALUOp = 3;
	    		break;
	    	// and
	    	case 36:
	    		ALUOp = 4;
	    		break;
	    	// or
	    	case 37:
	    		ALUOp = 5;
	    		break;
	    	// shift left right variable
	    	case 4:
	    		ALUOp = 6;
	    		break;
	    	// nor
	    	case 39:
	    		ALUOp = 7;
	    		break;
	    	// default for halt or don't care
	    	default:
	    		return 1;
	    }
	    // call ALU function
	    ALU(data1, data2, ALUOp, ALUresult, Zero);
    } else {
    	// call ALU for non-functions
    	ALU(data1, data2, ALUOp, ALUresult, Zero);
    }

    return 0;
}

int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{       
    //if we are reading,
    //Read data from ALUresult * 4 index in Memory.
    if(MemRead == 1){
        if((ALUresult % 4) == 0){
            *memdata = Mem[ALUresult >> 2];         
        }
        else
            return 1;     
    }
            
    //if we are writing,
    //Write data to ALUresult * 4 index in Memory
    //Word aligned
    if(MemWrite == 1){
        if((ALUresult % 4) == 0){
            Mem[ALUresult >> 2] = data2;
        }
        else
            return 1;
    }

    return 0;
}

void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    // if we're writing
    if(RegWrite==1){
        //Memory to register
        if(MemtoReg == 1 && RegDst == 0)
            Reg[r2] = memdata;
        //memory to register but to r3
        else if(MemtoReg == 1 && RegDst == 1)
            Reg[r3] = memdata;
        
        //store ALUResult in r2
        else if(MemtoReg == 0 && RegDst == 0)
            Reg[r2] = ALUresult;
        //store ALUResult in r3
        else if(MemtoReg == 0 && RegDst == 1)
            Reg[r3] = ALUresult;
    }
}

void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //always increment by 4 no matter what.
    *PC += 4;
    
    //if branching, and we got a zero, add the extended_value, bitshifted (multiplied by 4)
    if(Zero == 1 && Branch == 1)
        *PC += extended_value << 2;
        
    //if jumping, jump to the Jump register, and multiply by 4 (word align)
    if(Jump == 1)
        *PC = (jsec << 2) | (*PC & 0xf0000000);
}