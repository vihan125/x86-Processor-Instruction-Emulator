#pragma once
#include <list>
#include <iterator>
#include <string>
#include <map>
#include<iostream>
#include <vector>
using namespace std;

class Emulator{

//Registers
private:
  std::list <unsigned char> rom;
  
  std::map<std::string,uint32_t> register_mnemonics32 = {{"eax",4},{"ecx",2},{"edx",3},{"ebx",4},{"esp",5},{"ebp",6},{"esi",7},{"edi",8}};

  std::map<uint32_t,uint32_t> Memory = {};

  // std::map<std::string,uint32_t> register_flags = {{"cf",0},{"pf",0},{"af",0},{"zf",0},{"sf",0},{"tf",0},{"if",0},{"df",0},{"of",1}};
  
  
  // std::map<std::string,uint8_t> register_mnemonics8 = {{"al",0x4},{"cl",0x3},{"dl",0x1},{"bl",0xa4},{"ah",0xc},{"ch",0x8},{"dh",0xa0},{"bh",0x2}};

public:
  std::map<std::string,uint32_t> register_flags = {{"cf",0},{"of",0}};
  // Utilizing functions
  int check_len(std::string reg);
  std::string get_reg(std::string reg);
  uint32_t get_value(int dlen,std::string data);
  void decider(std::vector<std::string>);
  std::string dump();

  // Functions related to instructions
  void ADD (std::string data1, std::string data2);
  void ADC (std::string data1, std::string data2);
  void SUB (std::string data1, std::string data2);
  void OR (std::string data1, std::string data2);
  void XOR (std::string data1, std::string data2);
  void AND (std::string data1, std::string data2);
  
  void LEA (std::string reg1, std::string data);
  void MOV (std::string reg1, std::string reg2);
  // void CMP (std::string reg1, std::string reg2);
  void ROL (std::string reg1, std::string reg2);
  void SAR (std::string reg1, std::string reg2);
  void ROR (std::string reg1, std::string reg2);
  void SHL (std::string reg1, std::string reg2);
  void RCR (std::string reg1, std::string reg2);
  void RCL (std::string reg1, std::string reg2);
  
  
   
};