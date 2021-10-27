#include <iostream>
#include "decorder.h"
#include <bitset>
#include <string>
#include <iterator>
#include <sstream>
#include <fstream>

template <class Iter, class Incr>
void safe_advance(Iter& curr, const Iter& end, Incr n)
{
  size_t remaining(std::distance(curr, end));
  if (remaining < n)
  {
    n = remaining;
  }
  std::advance(curr, n);
}

const char *bit_rep[16] = {
  [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
  [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
  [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
  [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

void print_byte(uint8_t byte)
{
  printf("%s%s", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);
  printf("\n");
}

Decorder::Decorder(std::list<unsigned char> input_bytes)
  :input_bytes(input_bytes)
{
  
}

void Decorder::showInputBytes()
{
  std::list <unsigned char>::iterator it;
  for(it = input_bytes.begin(); it != input_bytes.end(); ++it)
  {
    printf("%X", *it);
    std::cout << '\n';
  }
}


void Decorder::createOutput(int testNum, char modRM_mod, std::string R_output, std::string RM_output, unsigned char opcode, unsigned char modbyte)
{
  std::string output = "";
  int pos = (int)opcode ;

  
  std::string opCodeLiteral = std::string(standard_instructions[opcode].mnemonic);
  if(opCodeLiteral == "immGroup ")
  {
    // handling immediate group one  
    unsigned char groupbyte=  ((modbyte) >> 3) & 0b111; // Bits 5-3. 
    
    switch(groupbyte)
    {
      case 0b000 :
        output += "add ";
        break;
      case 0b001 :
        output += "or ";
        break;
      case 0b010 :
        output += "adc ";
        break;
      case 0b011 :
        output += "sbb ";
        break;
      case 0b100 :
        output += "and ";
        break;
      case 0b101 :
        output += "sub ";
        break;
      case 0b110 :
        output += "xor ";
        break;
      case 0b111 :
         output += "cmp ";
    }
    oneInstruction.push_back( output );
  }

  else if(opCodeLiteral == "immGroup2 ")
  {
    // handling immediate group one  
    unsigned char groupbyte=  ((modbyte) >> 3) & 0b111; // Bits 5-3. 
    
    switch(groupbyte)
    {
      case 0b000 :
        output += "rol ";
        break;
      case 0b001 :
        output += "ror ";
        break;
      case 0b010 :
        output += "rcl ";
        break;
      case 0b011 :
        output += "rcr ";
        break;
      case 0b100 :
        output += "sal ";
        break;
      case 0b101 :
        output += "shl ";
        break;
      case 0b111 :
         output += "sar ";
    }
    oneInstruction.push_back( output );
  }

  else
  {
    output += std::string(standard_instructions[opcode].mnemonic);
    oneInstruction.push_back( standard_instructions[opcode].mnemonic );
  }

  
  unsigned long x;
  for (int i = 0; i < standard_instructions[opcode].argument_count; i++) 
  {
		if (i > 0) {
			output += ",";
		}
    

    switch (standard_instructions[opcode].arguments[i]) 
    {
      case RM:
        if(modRM_mod != 0b11)
        {
          switch(standard_instructions[opcode].size)
          {
						case BYTE:
							output += "BYTE PTR ";
							break;
						case WORD:
							output += "WORD PTR ";
							break;
						case DWORD:
							output += "DWORD PTR ";
							break;
						case QWORD:
							output += "QWORD PTR ";
							break;
						case FWORD:
							output += "FWORD PTR ";
							break;
						case XMMWORD:
							output += "XMMWORD PTR ";
							break;
					}
        }
        output += RM_output;
        oneInstruction.push_back(RM_output);
        break;
      case R:
        output += R_output;
        oneInstruction.push_back( R_output );
        break;
      case IMM8:
        {
        std::stringstream ss;
        ss << std::hex << (int)*it;
        output += "0x" + ss.str();
        oneInstruction.push_back( "0x" + ss.str());
        safe_advance(it, input_bytes.end(), 1);
        break;
        }
      case IMM16:
        std::cout << "IMM16" << std::endl;
        output += "0x" + std::string(*it,2);
        oneInstruction.push_back( "0x" + std::string(*it,2) );
        safe_advance(it, input_bytes.end(), 2);
        break;
      case IMM32:
        oneInstruction.push_back( "0x" + std::string(*it,4) );
        safe_advance(it, input_bytes.end(), 4);
        break;
      case REL8:
        {
        std::stringstream ss;
        ss << std::hex << (int)(*it+2);
        output += "0x" + ss.str();
        x =  (int)(*it+2);
        safe_advance(it, input_bytes.end(), 1);
        break;
        }
      case REL32:
        std::cout << "REL32" << std::endl;
        output += "0x"+ std::string(*it,1);
        oneInstruction.push_back( "0x"+ std::string(*it, 4) );
        safe_advance(it, input_bytes.end(), 4);
        break;
      case PTR1632:
        std::cout << "PTR1632" << std::endl;
        output += "0x" + std::string(*it,1);
        oneInstruction.push_back("0x" + std::string(*it,6));
        safe_advance(it, input_bytes.end(), 6);
        break;
      case AL:
        output += "al";
        oneInstruction.push_back("al" );
        break;
      case EAX:
        output += "eax";
        oneInstruction.push_back("eax" );
        break;
      case ES:
        output += "es";
        oneInstruction.push_back("es" );
        break;
      case CS:
        output += "cs";
        oneInstruction.push_back("cs" );
        break;
      case DS:
        output += "ds";
        oneInstruction.push_back("ds" );
        break;
      case ONE:
        output += "1";
        oneInstruction.push_back("1" );
        break;
      case CL:
        output += "cl";
        oneInstruction.push_back("cl" );
        break;
      case XMM0:
        output += "xmm0";
        oneInstruction.push_back("xmm0" );
        break;
      case BND0:
        output += "bnd0";
        break;
      case BAD:
        safe_advance(it, input_bytes.end(), 1);
        break;
      case MM0:
        output += "mm0";
        break;
    }
  }
  std::cout << output << std::endl;
  

  if(opCodeLiteral == "jo ")
  {
    std::cout << " Jump detected " << x << std::endl;
    it = input_bytes.begin();
    std::advance(it, 0);
    
  }
  else{
    std::string before = e.dump();
    printf("--------------------\n");
    e.decider(oneInstruction);
    std::string after =e.dump();
    printf("++++++++++++++++++++\n\n");

    std::cout << "Test number " << testNum << std::endl;

    std::string filename = "output" + std::to_string(testNum) + ".txt";
    std::ofstream MyFile(filename);

    // Write to the file
    MyFile << "Instruction is "<<output <<"\n\n\n" <<before <<"\n\n\n" << after;

    // Close the file
    MyFile.close();

  }
}

void Decorder::decode(int testNum)
{
  
  //std::list <unsigned char>::iterator it;

  it = input_bytes.begin();
  //it = e->getIterator();
  
  while(it != input_bytes.end())
  {
    oneInstruction.clear();

    unsigned char opcode = *it;
    safe_advance(it, input_bytes.end(), 1);
    
    int opcode_index = (int)opcode ;

    if(opcode == 0x0F)
    {
      std::cout << "2, 3byte opcodes are not handled in the program";
      exit(-1);
    }

    if (!standard_instructions[opcode_index].hasModRM) {
      
		  createOutput(testNum, 0xFF , "", "", opcode, opcode);
      continue;
	  }

    unsigned char modbyte = *it;
    safe_advance(it, input_bytes.end(), 1);

    std::string R_output = "";
    std::string RM_output = "";

    char modRM_mod = ((modbyte) >> 6) & 0b11; // Bits 7-6.
	  char modRM_reg = ((modbyte) >> 3) & 0b111; // Bits 5-3.
	  char modRM_rm = (modbyte) & 0b111; // Bits 2-0.
    
    
    switch(standard_instructions[opcode_index].size)
    {
      case WORD:
			  R_output += register_mnemonics16[modRM_reg];
        
			  break;
	  	case BYTE:
			  R_output += register_mnemonics8[modRM_reg];
        
			  break;
		  default:
			  R_output += register_mnemonics32[modRM_reg];
    }
    
    
    
    if(modRM_mod == 0b11)
    { // Register only addressing
      switch(standard_instructions[opcode_index].size)
      { 
        case BYTE:
          RM_output = register_mnemonics8[modRM_rm];
          innerInstruction.push_back( register_mnemonics8[modRM_rm] );
          break;
        case WORD:
          RM_output = register_mnemonics16[modRM_rm];
          innerInstruction.push_back( register_mnemonics16[modRM_rm] );
          break;
        default:
          RM_output = register_mnemonics32[modRM_rm];
          innerInstruction.push_back( register_mnemonics32[modRM_rm] );
      }
    }
    else if(modRM_mod == 0b00 && modRM_rm == 0b101)
    { // Displacement only addressing

      RM_output = "[0x" +  std::string(*it,1)  +  "]"; // -- v is not required
      innerInstruction.push_back( std::string(*it,1) );
      safe_advance(it, input_bytes.end(), 4);
    }
    else
    {
      if(modRM_rm == 0b100)
      { // Contains SIB byte
        char SIB_scale = ((*it) >> 6) & 0b11; // 7-6bits
        char SIB_index = ((*it) >> 3) & 0b111; // 5-3bits
        char SIB_base  = (*it) & 0b111; // 2-0 bits
        safe_advance(it, input_bytes.end(), 1);
        

        if (SIB_base == 0b101 && modRM_mod == 0b00) 
        {
          RM_output = "[0x" +  std::string(*it,1) + "]"; 
          
          safe_advance(it, input_bytes.end(), 4);
        }
        else
        {
          RM_output = sib_base_mnemonics[SIB_base];
          
        }

        if(SIB_index != 0b100)
        {
          RM_output = RM_output + "+";
          RM_output = RM_output + register_mnemonics32[SIB_index]+ sib_scale_mnemonics[SIB_scale];
          
        }

      }
      else
      {
        RM_output =  "[" + register_mnemonics32[modRM_rm] ;
        
      }

      
      if(modRM_mod == 0b01)
      { // setting up addressing mode bytes - one byte
        if(*it > 0x7f)
        {
          RM_output = RM_output + "-0x" + std::to_string(*it) + "]"; 
          safe_advance(it, input_bytes.end(), 1);
        }
        else
        {
          RM_output = RM_output + "+0x" + std::to_string(*it) + "]"; 
          safe_advance(it, input_bytes.end(), 1);
        }
      }
      else if (modRM_mod == 0b10)
      { // setting up addressing mode bytes - four byte
        if(*it > 0x7f)
        {
          RM_output = RM_output + " -0x" + std::string(*it,1) + "]"; 
          
        }
        else
        {
          RM_output = RM_output + " +0x" + std::string(*it,1) + "]"; 
          
        }
        safe_advance(it, input_bytes.end(), 4);
      }
      else
      {
        RM_output = RM_output + "]";
        
      }
  }

  createOutput(testNum,modRM_mod, R_output, RM_output,opcode, modbyte);
  

  }

}

