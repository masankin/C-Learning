///
/// Write a program to create a vector with ten int elements.
/// Using an iterator, assign each element a value that is twice its current
/// value.
/// Test your program by printing the vector.
///
#include <iostream>
#include <vector>
#include <iterator>
using std::vector;
using std::cout;
using std::endl;
using std::cin;



/**

cin.get()是保留回车在输入流队列中的.而cin是丢弃回车的.
ch=cin.get();是第一次输入,因为是刚开始,所以没有回车保留在输入流的队列中.这时你输入一个字符,然后回车.get()收到回车或空格或制表符就停止输入了.此时,输入流队列中留下一个回车.
num=cin.get();这是第二次输入.因为第一次用了get,所以输入流队列中有个回车.则get()接受这个回车.又因为get()收到回车或空格或制表符就停止输入.所以这一次的输入就是一个回车.
可以num=cin.get();之前把回车给弄了.你应该想到了.就用cin.get();把第一次输入留下的回车给弄走.则num=cin.get()就可以正常了.

 
 * */
int main() {
  
  //用迭代器改写用下标运算符 划分分数段
  
  int i;
  vector<int> ivec;
  
  char c;
  // cin.get() 等效 c = cin.get();
  while((cin >> i).get(c)){ //输入时会自动根据输出的类型自动判断是否停止输入  不是根据输入回车结束的  可使用getline
    cout << "=" <<  c << endl;
   
    ivec.push_back(i);
    if(c=='\n')
          break;
  }
                                                                                                 
  for (auto it = ivec.begin();  it != ivec.end(); ++it) {
    cout << *it << endl;
  }
}


/**
 
 
 cin.get()
用法1： cin.get(字符变量名)可以用来接收字符
#include <iostream>
using namespace std;
main ()
{
char ch;
ch=cin.get();               //或者cin.get(ch);
cout<<ch<<endl;
}
输入：jljkljkl
输出：j
用法2：cin.get(字符数组名,接收字符数目)用来接收一行字符串,可以接收空格
#include <iostream>
using namespace std;
main ()
{
char a[20];
cin.get(a,20);
cout<<a<<endl;
}
输入：jkl jkl jkl
输出：jkl jkl jkl
输入：abcdeabcdeabcdeabcdeabcde （输入25个字符）
输出：abcdeabcdeabcdeabcd              （接收19个字符+1个'\0'）
用法3：cin.get(无参数)没有参数主要是用于舍弃输入流中的不需要的字符,或者舍弃回车,
弥补cin.get(字符数组名,接收字符数目)的不足.

* */
