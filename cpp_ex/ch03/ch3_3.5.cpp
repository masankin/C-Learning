///
/// Write a program to create a vector with ten int elements.
/// Using an iterator, assign each element a value that is twice its current
/// value.
/// Test your program by printing the vector.
///

#include <vector>
#include <iostream>
#include <iterator>
#include <string>

using std::vector;
using std::iterator;
using std::cout;
using std::endl;
using std::string;

int main() {
  vector<int> text(20);

  const unsigned sz = 3;
  int ia1[sz] = {0,1,2};
 
  int len = sizeof(ia1) / sizeof(ia1[0]);
  cout << sizeof(ia1) << endl;
  cout << sizeof(ia1[0]) << endl;
  cout << len << endl;
  return 0;
}
 