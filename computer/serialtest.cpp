#include <iostream>
#include <cassert>
#include <string>
#include "serialport.h"
#include "exprtk.hpp"

using namespace std;

string equation_str, lowerL_str, upperL_str;
double lowerlimit, upperlimit;
double calculated_area;

template <typename T>
void lin_function()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>     expression_t;
   typedef exprtk::parser<T>             parser_t;

   T x = T(0);
   symbol_table_t symbol_table;
   symbol_table.add_variable("x",x);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(equation_str,expression);

   T area1 = exprtk::integrate(expression, x, T(lowerlimit), T(upperlimit));
   //cout << "area = "<< area1 << endl;
   calculated_area = area1;
}




int main() {
  SerialPort Serial("/dev/ttyACM0");

  string curPhase = "PHASE00\n";
  string nextPhase = "PHASE01\n";
  string line;

  cout << "Server started, you might need to restart your Arduino" << endl;
  cout << "Server is in phase " << curPhase;
  cout << "Waiting for PHASE01 message from Arduino..." << endl;

  // read and ignore lines until we get the message PHASE01
  do {
    line = Serial.readline();
  } while (line != nextPhase);

  // switch to next phase
  curPhase = nextPhase;
  nextPhase = "PHASE02\n";
  cout << "Server is in phase " << curPhase;

  // read the introductory lines until get the message PHASE01
  do {
    line = Serial.readline();
    cout << "Received: " << line; // note '\n' is in the string already
  } while (line != nextPhase);
  cout << "Sending message <Ack\\n>" << endl;
  assert(Serial.writeline("Ack\n"));

  // switch to next phase
  curPhase = nextPhase;

  cout << "Server is in phase " << curPhase;
  cout << "Waiting for client to reply to previous message..." << endl;

  line = Serial.readline(); // This reads lcd image shit


  while (true){
    int pos;
    //bool equation_rev = false, lower_rev = false, upper_rev = false;
    line = Serial.readline();
    cout << "Received: " << line;


    if (line.find("Equation:") != std::string::npos) {
      equation_str = line;
      pos = equation_str.find(":");
      equation_str = equation_str.substr(pos+2, equation_str.size()-pos-1);
      equation_str.erase(remove(equation_str.begin(), equation_str.end(), '\n'), equation_str.end());
    }

    if (line.find("LowerL:") != std::string::npos) {
      lowerL_str = line;
      lowerL_str.erase(remove(lowerL_str.begin(), lowerL_str.end(), '\n'), lowerL_str.end());
      pos = lowerL_str.find(":");
      lowerL_str = lowerL_str.substr(pos+2, lowerL_str.size()-pos-1);
      lowerlimit = stod(lowerL_str);
    }

    if (line.find("UpperL:") != std::string::npos) {
      upperL_str = line;
      upperL_str.erase(remove(upperL_str.begin(), upperL_str.end(), '\n'), upperL_str.end());
      pos = upperL_str.find(":");
      upperL_str = upperL_str.substr(pos+2, upperL_str.size()-pos-1);
      upperlimit = stod(upperL_str);
      lin_function<double>();
      cout << "Area: " << calculated_area << endl;
      string area_str = to_string(calculated_area); 
      assert(Serial.writeline("Area: "+area_str+"\n"));
    }

  };

	return 0;
}
