#include <iostream>
#include <string.h>
#include "decorder.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


int main() {

  std::vector <std::vector <std::string> > data;
  ifstream infile( "testcases.txt" );

  int i = 1;

  while (infile)
  {
    string s;
    if (!getline( infile, s )) break;

    istringstream ss( s );
    vector <string> record;

    std::list <unsigned char> input_bytes;

    std::cout << " Test Case : " << i << std::endl;
    std::cout << "==================================== " << std::endl;
    while (ss)
    {
      string s;
      if (!getline( ss, s, ',' )) break;
      //std::cout << s << std::endl;
      input_bytes.push_back(std::stoul(s,nullptr,16));
      //record.push_back( s );
    }

    //data.push_back( record );

    Decorder decorder(input_bytes);
  
    decorder.decode(i);

    i++;
  
  }
  if (!infile.eof())
  {
    cerr << "end!\n";
  }
  
  return 0;
}
 