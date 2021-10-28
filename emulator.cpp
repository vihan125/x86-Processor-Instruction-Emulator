#include "emulator.h"
#include<iostream>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;



// Methods to utilize by every function
int Emulator::check_len(std::string reg){
  std::string last = reg.substr(1,1);
  if(last == "l" || last == "h"){
    return 8;
  }
  
  if (last == "x"){
    return 16;
  }
  else{
    return 0;
  }
}

std::string Emulator::get_reg(std::string reg){
    std::string id = reg.substr(0,1);
    std::string regName = "e"+id+"x";  
    return regName;
}

uint32_t Emulator::get_value(int dlen, std::string d){

      uint32_t value;
      uint16_t value_16;
      uint8_t value_8;
      if(dlen == 32)
        value = std::stoul(d,nullptr,16);
      else if (dlen == 16){
        value_16 = std::stoul(d,nullptr,16);
        value = value_16;
      }else{
        value_8 = std::stoul(d,nullptr,16);
        value = value_8;
      }
      return value;
}

// Decider method which guids the instruction to the relevant method
void Emulator::decider(std::vector<std::string> decoded_data){

  std::string data = decoded_data[0];
  int sp_index = data.find(" ");
  std::string inst = data.substr(0,sp_index);

// Passing operands to the relevant functions
  if(inst == "lea"){
    LEA(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "mov" || inst == "movl"){
    MOV(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "add"){
    ADD(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "or"){
    OR(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "sub"){
    SUB(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "xor"){
    XOR(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "and"){
    AND(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "adc"){
    ADC(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "rol"){
    ROL(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "sbb"){
    SUB(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "sar"){
    SAR(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "shl" || inst == "sal"){
    SHL(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "ror"){
    ROR(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "rcr"){
    RCR(decoded_data[1],decoded_data[2]);
  }
  else if(inst == "rcl"){
    RCL(decoded_data[1],decoded_data[2]);
  }
  else{
    std::cout<<"Not handled"<<std::endl;
  }

}

// Dump method to dump the values of all the registers
std::string Emulator::dump(){

  std::stringstream result;

  result<<"eax : "<<register_mnemonics32["eax"]<<"\n";
  result<<"ecx : "<<register_mnemonics32["ecx"]<<"\n";
  result<<"edx : "<<register_mnemonics32["edx"]<<"\n";
  result<<"ebx : "<<register_mnemonics32["ebx"]<<"\n";
  result<<"esp : "<<register_mnemonics32["esp"]<<"\n";
  result<<"ebp : "<<register_mnemonics32["ebp"]<<"\n";
  result<<"esi : "<<register_mnemonics32["esi"]<<"\n";
  result<<"edi : "<<register_mnemonics32["edi"]<<"\n";

  result<<"\nMEMORY :\n";

  if(Memory.size()==0){

    result<<"All addresses hold zero value\n";
  }
  else{
    result<<"Address => Value\n";

    map<uint32_t, uint32_t>::iterator itr;
    for (itr = Memory.begin(); itr != Memory.end(); ++itr) {
        result<<std::to_string(itr->first)<<"=>"<<std::to_string(itr->second)<<'\n';
    }
  }

  result<<"\nFlags :\n";

  if(register_flags.size()==0){
    result<<"All addresses hold zero value\n";
  }
  else{
    map<std::string, uint32_t>::iterator itr;
    for (itr = register_flags.begin(); itr != register_flags.end(); ++itr) {
        result<<itr->first<<"=>"<<std::to_string(itr->second)<<'\n';
    }
  }

  std::cout<<result.str()<<std::endl;
  return result.str();

}


//=========================Method which handles ADD instruction===================================
void Emulator::ADD(std::string data1, std::string data2){

  int data_length = 32;
  std::cout<<"here"<<data2<<std::endl;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  // ADD RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //ADD RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address]+value;
      }
      // ADD RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address]+value;
      }

      if(Memory[Mem_address]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //ADD RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address]+value;
      }
      // ADD RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address]+value;
      }

      if(Memory[Mem_address]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
    }
  }

  // ADD R/RM
  else if (data2.find("[") != -1){

    data2 = data2.substr(1,data2.length()-2); // remove []
    // Base + offset addressing
    if(data2.find("+") != -1){

      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;

      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 
      std::cout<< "Printed here "<<Mem_address<<std::endl;

      
      register_mnemonics32[data1] = register_mnemonics32[data1] + Memory[Mem_address];

      if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      register_mnemonics32[data1] = register_mnemonics32[data1] + Memory[Mem_address]; 

      if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
    }
  }
  //ADD R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    register_mnemonics32[data1] = register_mnemonics32[data1] + register_mnemonics32[data2]; 

    if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
  }
  // ADD R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
      
    uint32_t value = get_value(data_length,data2);
    register_mnemonics32[data1] = register_mnemonics32[data1] + value ; 

    if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  

}

//=========================Method which handles ADC instruction===================================
void Emulator::ADC(std::string data1, std::string data2){

  int data_length = 32;

  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  // ADC RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //ADC RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address]+value;
      }
      // ADC RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] + value + register_flags["cf"];
      }

      if(Memory[Mem_address]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //ADC RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address]+value;
      }
      // ADC RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] + value + register_flags["cf"];
      }

      if(Memory[Mem_address]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
    }
  }

  // ADC R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      
      register_mnemonics32[data1] = register_mnemonics32[data1] + Memory[Mem_address]+register_flags["cf"];

      if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      register_mnemonics32[data1] = register_mnemonics32[data1] + Memory[Mem_address]+register_flags["cf"]; 

      if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
    }
  }
  //ADC R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    register_mnemonics32[data1] = register_mnemonics32[data1] + register_mnemonics32[data2]+ register_flags["cf"]; 

    if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
  }
  // ADC R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
      
    uint32_t value = get_value(data_length,data2);
    register_mnemonics32[data1] = register_mnemonics32[data1] + value +register_flags["cf"] ; 

    if(register_mnemonics32[data1]==0x7fffffff){
        register_flags["of"] = 0x1;
        register_flags["cf"] = 0x1;
      }
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  

}


//=========================Method which handles LEA instruction===================================
void Emulator::LEA(std::string reg1, std::string data){

  int data_length = 32;

  //Check the length of the operands 
  if(isalpha(reg1[0]) && reg1.length() == 2){
    data_length = check_len(reg1);
    reg1 = get_reg(reg1);
  }

  //LEA R/RM
  if (data.find("[")!= -1){
    
    data = data.substr(1,data.length()-2); // remove []

    // Base + offset addressing
    if(data.find("+") != -1){
      int index = data.find("+");
      std::string regdata = data.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      
      register_mnemonics32[reg1] = Memory[Mem_address]; 
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data,nullptr,16);
      register_mnemonics32[reg1] = Memory[Mem_address]; 
    }
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
}


//=========================Method which handles MOV instruction===================================
void Emulator::MOV(std::string reg1, std::string reg2){

  int data_length = 32;
  //Check the length of the operands 
  if(isalpha(reg1[0]) && reg1.length() == 2){
    data_length = check_len(reg1);
    reg1 = get_reg(reg1);
  }

  if(isalpha(reg2[0]) && reg2.length() == 2){
    data_length = check_len(reg2);
    reg2 = get_reg(reg2);
  }

  //MOV RM/X
  if(reg1.find("[") != -1){

    reg1 = reg1.substr(1,reg1.length()-2); // remove []

    // Base + offset addressing
    if(reg1.find("+") != -1){
      
      int index = reg1.find("+");
      std::string regdata = reg1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = reg1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //MOV RM/R
      if(isalpha(reg2[0])){
        uint32_t value = register_mnemonics32[reg2] ; 
        Memory[Mem_address] = value;
      }
      //MOV RM/IMM
      else{
        uint32_t value = get_value(data_length,reg2);
        Memory[Mem_address] = value;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(reg1,nullptr,16);
      //MOV RM/R
      if(isalpha(reg2[0])){
        uint32_t value = register_mnemonics32[reg2] ; 
        Memory[Mem_address] = value;
      }
      //MOV RM/IMM
      else{
        uint32_t value = get_value(data_length,reg2);
        Memory[Mem_address] = value;
      }
    }
  }

  //MOV R/RM
  else if (reg2.find("[") != -1){
    
    reg2 = reg2.substr(1,reg2.length()-2); // remove []

    // Base + offset addressing
    if(reg2.find("+") != -1){
      int index = reg2.find("+");
      std::string regdata = reg2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = reg2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      
      register_mnemonics32[reg1] =Memory[Mem_address]; 
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(reg2,nullptr,16);
      register_mnemonics32[reg1] =  Memory[Mem_address]; 
    }
  }
  //MOV R/R
  else if(isalpha(reg1[0]) && isalpha(reg2[0])){
    register_mnemonics32[reg1] = register_mnemonics32[reg2]; 
  }
  //MOV R/Imm
  else if (isalpha(reg1[0]) && !(isalpha(reg2[0]))){
      
    uint32_t value = get_value(data_length,reg2);
    register_mnemonics32[reg1] = value ; 
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  
  
}

//=========================Method which handles OR instruction===================================
void Emulator::OR(std::string data1, std::string data2){

   int data_length = 32;

  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  // OR RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //OR RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address] | value;
      }
      //OR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] | value;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //OR RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address] | value;
      }
      //OR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] | value;
      }
    }
  }

  //OR R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      
      register_mnemonics32[data1] = register_mnemonics32[data1] | Memory[Mem_address]; 
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      register_mnemonics32[data1] = register_mnemonics32[data1] | Memory[Mem_address]; 
    }
  }
  //OR R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    register_mnemonics32[data1] = register_mnemonics32[data1] | register_mnemonics32[data2]; 
  }
  //OR R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
      
    uint32_t value = get_value(data_length,data2);
    register_mnemonics32[data1] = register_mnemonics32[data1] | value ; 
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  

}


//=========================Method which handles SUB instruction===================================
void Emulator::SUB(std::string data1, std::string data2){

    int data_length = 32;

  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  // SUB RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //SUB RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address]-value;
      }
      //SUB RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address]-value;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //SUB RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address]-value;
      }
      // SUB RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address]-value;
      }
    }
  }

  //SUB R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      
      register_mnemonics32[data1] = register_mnemonics32[data1] - Memory[Mem_address]; 
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      register_mnemonics32[data1] = register_mnemonics32[data1] - Memory[Mem_address]; 
    }
  }
  //SUB R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    register_mnemonics32[data1] = register_mnemonics32[data1] - register_mnemonics32[data2]; 
  }
  //SUB R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
      
    uint32_t value = get_value(data_length,data2);
    register_mnemonics32[data1] = register_mnemonics32[data1] - value ; 
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  

}


//=========================Method which handles AND instruction===================================
void Emulator::AND(std::string data1, std::string data2){
  int data_length = 32;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  // AND RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //AND RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address] & value;
      }
      // AND RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] & value;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //AND RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address] & value;
      }
      // AND RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] & value;
      }
    }
  }

  // AND R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      
      register_mnemonics32[data1] = register_mnemonics32[data1] & Memory[Mem_address]; 
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      register_mnemonics32[data1] = register_mnemonics32[data1] & Memory[Mem_address]; 
    }
  }
  //AND R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    register_mnemonics32[data1] = register_mnemonics32[data1] & register_mnemonics32[data2]; 
  }
  // AND R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
    uint32_t value = get_value(data_length,data2);
    register_mnemonics32[data1] = register_mnemonics32[data1] & value ; 
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  

}

//=========================Method which handles XOR instruction===================================
void Emulator::XOR(std::string data1, std::string data2){

    int data_length = 32;

  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  //XOR RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //XOR RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address] ^ value;
      }
      //XOR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] ^ value;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //XOR RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        Memory[Mem_address] = Memory[Mem_address] ^ value;
      }
      //XOR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        Memory[Mem_address] = Memory[Mem_address] ^ value;
      }
    }
  }

  //XOR R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      
      register_mnemonics32[data1] = register_mnemonics32[data1] ^ Memory[Mem_address]; 
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      register_mnemonics32[data1] = register_mnemonics32[data1] ^ Memory[Mem_address]; 
    }
  }
  //XOR R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    register_mnemonics32[data1] = register_mnemonics32[data1] ^ register_mnemonics32[data2]; 
  }
  //XOR R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
      
    uint32_t value = get_value(data_length,data2);
    register_mnemonics32[data1] = register_mnemonics32[data1] ^ value ; 
  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }

}

//=========================Method which handles ROL instruction===================================
void Emulator::ROL(std::string data1, std::string data2){

 int data_length = 32;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  //ROL RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //ROL RM/R
      if(isalpha(data2[0])){

        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;
        register_flags["cf"] = shift & 1;
        Memory[Mem_address] = result || shift;
      }
      //ROL RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;
        register_flags["cf"] = shift & 1;
        Memory[Mem_address] = result || shift;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //ROL RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;
        register_flags["cf"] = shift & 1;
        Memory[Mem_address] = result || shift;
      }
      //ROL RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;
        register_flags["cf"] = shift & 1;
        Memory[Mem_address] = result || shift;

      }
    }
  }

  // ROL R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;
      register_flags["cf"] = shift & 1;
      register_mnemonics32[data1] = result || shift;
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;
      register_flags["cf"] = shift & 1;
      register_mnemonics32[data1] = result || shift;
    }
  }
  //ROL R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    uint32_t value = register_mnemonics32[data2]  ;
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num >> (32-value);
    uint32_t result = num << value;
    register_flags["cf"] = shift & 1;
    register_mnemonics32[data1] = result || shift; 
  }
  //ROL R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
    uint32_t value = get_value(data_length,data2);
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num >> (32-value);
    uint32_t result = num << value;
    register_flags["cf"] = shift & 1;
    register_mnemonics32[data1] = result || shift; 

  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  
}

//=========================Method which handles SAR instruction===================================
void Emulator::SAR(std::string data1, std::string data2){

 int data_length = 32;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  //SAR RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //SAR RM/R
      if(isalpha(data2[0])){

        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        if(num >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;
      }
      //SAR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;
          
        if(num >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //SAR RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;
          
        if(num >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;
      }
      //SAR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;
          
        if(num >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;

      }
    }
  }

  //SAR R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num << (32-value);
      uint32_t result = num >> value;

      if((shift & 0x80000000) >= 0x80000000)
        register_flags["cf"] = 0x1;
      else
        register_flags["cf"] = 0x1;
        
      if(num >= 0x80000000)
        register_mnemonics32[data1] = result | 0x80000000;
      else 
        register_mnemonics32[data1] = result;
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num << (32-value);
      uint32_t result = num >> value;

      if((shift & 0x80000000) >= 0x80000000)
        register_flags["cf"] = 0x1;
      else
        register_flags["cf"] = 0x1;
        
      if(num >= 0x80000000)
        register_mnemonics32[data1] = result | 0x80000000;
      else 
        register_mnemonics32[data1] = result;
    }
  }
  //SAR R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    uint32_t value = register_mnemonics32[data2]  ;
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num << (32-value);
    uint32_t result = num >> value;

    if((shift & 0x80000000) >= 0x80000000)
      register_flags["cf"] = 0x1;
    else
      register_flags["cf"] = 0x1;
      
    if(num >= 0x80000000)
      register_mnemonics32[data1] = result | 0x80000000;
    else 
      register_mnemonics32[data1] = result;
  }
  //SAR R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
    uint32_t value = get_value(data_length,data2);
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num << (32-value);
    uint32_t result = num >> value;

    if((shift & 0x80000000) >= 0x80000000)
      register_flags["cf"] = 0x1;
    else
      register_flags["cf"] = 0x1;
      
    if(num >= 0x80000000)
      register_mnemonics32[data1] = result | 0x80000000;
    else 
      register_mnemonics32[data1] = result;

  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  
}

//=========================Method which handles ROR instruction===================================
void Emulator::ROR(std::string data1, std::string data2){

 int data_length = 32;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  //ROL RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //ROR RM/R
      if(isalpha(data2[0])){

        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        Memory[Mem_address] = result || shift;
      }
      //ROR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;
        
        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        Memory[Mem_address] = result || shift;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //ROR RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;
        
        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        Memory[Mem_address] = result || shift;
      }
      //ROR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;
        
        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        Memory[Mem_address] = result || shift;

      }
    }
  }

  // ROR R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num << (32-value);
      uint32_t result = num >> value;
      
      if((shift & 0x80000000) >= 0x80000000)
        register_flags["cf"] = 0x1;
      else
        register_flags["cf"] = 0x1;

      register_mnemonics32[data1] = result || shift;
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num << (32-value);
      uint32_t result = num >> value;
      
      if((shift & 0x80000000) >= 0x80000000)
        register_flags["cf"] = 0x1;
      else
        register_flags["cf"] = 0x1;

      register_mnemonics32[data1] = result || shift;
    }
  }
  //ROR R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    uint32_t value = register_mnemonics32[data2]  ;
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num << (32-value);
    uint32_t result = num >> value;
    
    if((shift & 0x80000000) >= 0x80000000)
      register_flags["cf"] = 0x1;
    else
      register_flags["cf"] = 0x1;

    register_mnemonics32[data1] = result || shift;
  }
  //ROR R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
    uint32_t value = get_value(data_length,data2);
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num << (32-value);
    uint32_t result = num >> value;
    
    if((shift & 0x80000000) >= 0x80000000)
      register_flags["cf"] = 0x1;
    else
      register_flags["cf"] = 0x1;

    register_mnemonics32[data1] = result || shift;

  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  
}


//=========================Method which handles SHL instruction===================================
void Emulator::SHL(std::string data1, std::string data2){

 int data_length = 32;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  //SHL RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //SHL RM/R
      if(isalpha(data2[0])){

        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;

        Memory[Mem_address] = result;
      }
      //SHL RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;

        Memory[Mem_address] = result;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //SHL RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;

        Memory[Mem_address] = result;
      }
      //SAR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;

        Memory[Mem_address] = result;

      }
    }
  }

  //SHL R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;

      register_flags["cf"] = shift & 0x1;

      register_mnemonics32[data1] = result;
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;

      register_flags["cf"] = shift & 0x1;

      register_mnemonics32[data1] = result;
    }
  }
  //SHL R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    uint32_t value = register_mnemonics32[data2]  ;
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num >> (32-value);
    uint32_t result = num << value;

    register_flags["cf"] = shift & 0x1;

    register_mnemonics32[data1] = result;
  }
  //SHL R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
    uint32_t value = get_value(data_length,data2);
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num >> (32-value);
    uint32_t result = num << value;

    register_flags["cf"] = shift & 0x1;

    register_mnemonics32[data1] = result;

  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  
}

//=========================Method which handles RCR instruction===================================
void Emulator::RCR(std::string data1, std::string data2){

 int data_length = 32;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  //RCR RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //RCR RM/R
      if(isalpha(data2[0])){

        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        if(shift >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;
      }
      //RCR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        if(shift >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //RAR RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        if(shift >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;
      }
      //RCR RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num << (32-value);
        uint32_t result = num >> value;

        if((shift & 0x80000000) >= 0x80000000)
          register_flags["cf"] = 0x1;
        else
          register_flags["cf"] = 0x1;

        if(shift >= 0x80000000)
          Memory[Mem_address] = result | 0x80000000;
        else 
          Memory[Mem_address] = result;

      }
    }
  }

  //RCR R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num << (32-value);
      uint32_t result = num >> value;

      if((shift & 0x80000000) >= 0x80000000)
        register_flags["cf"] = 0x1;
      else
        register_flags["cf"] = 0x1;

      if(shift >= 0x80000000)
        register_mnemonics32[data1] = result | 0x80000000;
      else 
        register_mnemonics32[data1] = result;
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num << (32-value);
      uint32_t result = num >> value;

      if((shift & 0x80000000) >= 0x80000000)
        register_flags["cf"] = 0x1;
      else
        register_flags["cf"] = 0x1;

      if(shift >= 0x80000000)
        register_mnemonics32[data1] = result | 0x80000000;
      else 
        register_mnemonics32[data1] = result;
    }
  }
  //RCR R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    uint32_t value = register_mnemonics32[data2]  ;
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num << (32-value);
    uint32_t result = num >> value;

    if((shift & 0x80000000) >= 0x80000000)
      register_flags["cf"] = 0x1;
    else
      register_flags["cf"] = 0x1;

    if(shift >= 0x80000000)
      register_mnemonics32[data1] = result | 0x80000000;
    else 
      register_mnemonics32[data1] = result;
  }
  //RCR R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
    uint32_t value = get_value(data_length,data2);
    uint32_t num = register_mnemonics32[data1];

    uint32_t shift = num << (32-value);
    uint32_t result = num >> value;

    if((shift & 0x80000000) >= 0x80000000)
      register_flags["cf"] = 0x1;
    else
      register_flags["cf"] = 0x1;

    if(shift >= 0x80000000)
      register_mnemonics32[data1] = result | 0x80000000;
    else 
      register_mnemonics32[data1] = result;

  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  
}


//=========================Method which handles RCL instruction===================================
void Emulator::RCL(std::string data1, std::string data2){

 int data_length = 32;
  //Check the length of the operands 
  if(isalpha(data1[0]) && data1.length() == 2){
    data_length = check_len(data1);
    data1 = get_reg(data1);
  }

  if(isalpha(data2[0]) && data2.length() == 2){
    data_length = check_len(data2);
    data1 = get_reg(data2);
  }

  //RCL RM/X
  if(data1.find("[") != -1){

    data1 = data1.substr(1,data1.length()-2); // remove []

    // Base + offset addressing
    if(data1.find("+") != -1){
      
      int index = data1.find("+");
      std::string regdata = data1.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data1.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset;

      //RCL RM/R
      if(isalpha(data2[0])){

        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;

 
        Memory[Mem_address] = result |(shift & 0x1);

      }
      //RCL RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;

 
        Memory[Mem_address] = result |(shift & 0x1);
      }
    }
    // direct memory access
    else{

      uint32_t Mem_address = std::stoul(data1,nullptr,16);
      //RCL RM/R
      if(isalpha(data2[0])){
        uint32_t value = register_mnemonics32[data2] ; 
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;
        Memory[Mem_address] = result |(shift & 0x1);
      }
      //RCL RM/IMM
      else{
        uint32_t value = get_value(data_length,data2);
        uint32_t num = Memory[Mem_address];

        uint32_t shift = num >> (32-value);
        uint32_t result = num << value;

        register_flags["cf"] = shift & 0x1;
        Memory[Mem_address] = result |(shift & 0x1);

      }
    }
  }

  //RCL R/RM
  else if (data2.find("[") != -1){
    
    data2 = data2.substr(1,data2.length()-2); // remove []

    // Base + offset addressing
    if(data2.find("+") != -1){
      int index = data2.find("+");
      std::string regdata = data2.substr(0,index);
      std::string reg;
      int multiplier;
      if(regdata.find("*") != -1){
        int index_mul = regdata.find("*"); 
        reg = regdata.substr(0,index_mul);
        std::string multi = regdata.substr(index_mul+1);
        multiplier = std::stoul(multi,nullptr,10);
      }
      else{
        reg = regdata;
        multiplier = 1;
      }

      std::string offstr = data2.substr(index+1);
      uint32_t offset = std::stoul(offstr,nullptr,16);
    
      uint32_t base = register_mnemonics32[reg];
      uint32_t Mem_address = (base*multiplier) + offset; 

      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;

      register_flags["cf"] = shift & 0x1;
      register_mnemonics32[data1] = result |(shift & 0x1);
      
    }
    // direct memory access
    else{
      uint32_t Mem_address = std::stoul(data2,nullptr,16);
      uint32_t value = Memory[Mem_address] ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;

      register_flags["cf"] = shift & 0x1;
      register_mnemonics32[data1] = result |(shift & 0x1);
    }
  }
  //RCL R/R
  else if(isalpha(data1[0]) && isalpha(data2[0])){
    uint32_t value = register_mnemonics32[data2]  ;
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;

      register_flags["cf"] = shift & 0x1;
      register_mnemonics32[data1] = result |(shift & 0x1);
  }
  //RCL R/Imm
  else if (isalpha(data1[0]) && !(isalpha(data2[0]))){
    uint32_t value = get_value(data_length,data2);
      uint32_t num = register_mnemonics32[data1];

      uint32_t shift = num >> (32-value);
      uint32_t result = num << value;

      register_flags["cf"] = shift & 0x1;
      register_mnemonics32[data1] = result |(shift & 0x1);

  }
  else{
    std::cout<<"Not Handled"<<std::endl;
  }
  
}


