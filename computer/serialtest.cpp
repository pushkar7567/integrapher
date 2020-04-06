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

template <typename T>
void graph_evaluator()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>     expression_t;
   typedef exprtk::parser<T>             parser_t;
   T x;

   symbol_table_t symbol_table;
   symbol_table.add_variable("x",x);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(equation_str,expression);

   ofstream myfile;
   myfile.open("graph_points.txt", ios::trunc);

   if (myfile.is_open())
   { 
      for (x = T(lowerlimit); x <= T(upperlimit); x += T(0.001))
      {
        T y = expression.value();
        myfile << x << "," << y << endl;
      }  
      myfile << "endfile";
      myfile.close(); 
   }
   else cout << "Unable to open file";
}

/*
string graph_builder() {
  ifstream myfile;
  myfile.open("graph_points.txt", ios::out);
  //double x, y;

  string line;
  //while(myfile) {
    getline(myfile, line);
    myfile.close();
    return line;

    //assert(Serial.writeline(line+"\n"));
    //if (line != "endfile"){
      //counter++;
      //int pos = line.find(",");
      //string x_str = line.substr(0, pos);
      //string y_str = line.substr(pos+1, line.size()-pos);
      //x = stod(x_str); y = stod(y_str);
      //cout << x_str << " " << y_str << endl;
    //}  
  //}
}
*/

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
  int flag =0;

  while (flag == 0){
    int pos;
    line = Serial.readline();
    cout << "Received: " << line;
    //int flag =0;

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
      graph_evaluator<double>();
      flag = 1;
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

  ifstream myfile;
  myfile.open("graph_points.txt", ios::out);      
  while(myfile) {
    getline(myfile, line);
    assert(Serial.writeline("P "+line+"\n"));
  }
  myfile.close();

	return 0;
}
