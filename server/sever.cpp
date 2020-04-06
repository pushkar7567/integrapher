#include <iostream>
#include <string>
#include "exprtk.hpp"
#include <fstream>


using namespace std;

string expression_string;
double lowerlimit, upperlimit;

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
   parser.compile(expression_string,expression);

   T area1 = exprtk::integrate(expression, x, T(lowerlimit), T(upperlimit));
   cout << area1 << endl;
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
   parser.compile(expression_string,expression);

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

void graph_builder() {
  ifstream myfile;
  myfile.open("graph_points.txt", ios::out);
  double x, y;
  double max = -1000000; min = 10000000;

  string line;
  while(myfile) {
    getline(myfile, line);
    if (line != "endfile"){
      int pos = line.find(",");
      string x_str = line.substr(0, pos);
      string y_str = line.substr(pos+1, line.size()-pos);
      x = stod(x_str); y = stod(y_str);
      cout << x << " " << y << endl;
      if (y>max) {
        max = y;
      }
      else if (y<min) {
        min = y;l
      }
    }  
  }
  myfile.close();
}


int main() {
  cout << "enter expression:"; 
  cin >> expression_string;
  cin >> lowerlimit >> upperlimit;
  //lin_function<double>();
  graph_evaluator<double>();
  graph_builder();

  return 0;
}
