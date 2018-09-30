#include <iostream>

using namespace std; 
 
struct Teacher {
    int age; 
}; 

void setTea10(Teacher *p){
    p->age = 10;
}

 
Teacher& setTea20(Teacher &c){
    c.age = 20; 
      return c;
}

   
 
int main() { 
    Teacher t; 
    t.age = 1;
    setTea10(&t);       

    cout << t.age << endl; 
    cout << sizeof(t) << endl;
    return 0;   
}   