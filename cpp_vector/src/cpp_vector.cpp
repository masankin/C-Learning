//============================================================================
// Name        : cpp_vector.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
using namespace std;
using std::vector;

int main01() {
  //c++既有类模板  也有函数模板
  
  vector<int> ivec;
  vector<int> ivec2(ivec);//把ivec的元素拷贝给ivec2
  vector<int> ivec3 = ivec;//把ivec的元素拷贝给ivec3
  
  //c++新的为vector对象的元素赋值的方法  列表初始化
  
  vector<string> articles = {"a","an","the"};
  
  vector<int> ivec01(10,-1);//10个int 类型的元素 每个元素都被初始化为-1
  
  
  //值初始化 value-initialized
  vector<int> ivec02(10);//10个元素 每个都初始化成0
  vector<string> ivec03(10);//10个元素 每个都为空
  
  //向vector对象中添加元素
  cout << "===================" << endl;
  
  //push_back 把一个值当成vector 对象的尾元素压(push)到vector对象的尾端(back)
	
  vector<int> v2;
  for (int i = 0; i != 100; ++i) {
     v2.push_back(i);
  }
  cout << v2.size() << endl;
  
  unsigned grade;
  vector<unsigned> scores(11,0);
  
  while(cin >> grade){
    if(grade <= 100){
       ++scores[grade/10];//分数除以10的结果是整数部分 即可以代表10个分数段的位置
    }
  }
  for (int i = 0; i < scores.size(); ++i) {
    cout << scores[i] << endl;
  }
  return 0;
}


int main02(int argc, char **argv) {
  
  string s("hello string");
  if(s.begin() != s.end()){
    auto it = s.begin();
    
    *it = toupper(*it);
    cout << "*it : " << *it << endl;

  }
  cout << "s : " << s << endl;
  
  
  for(auto it = s.begin(); it != s.end();++it){
      
    *it  = toupper(*it);
    cout << &it << endl;//&it 是对指针取地址
  }
  cout << "s : " << s << endl;
    
  
}


//begin end 运算符
int main(int argc, char **argv) {
  vector<int> v{1,2};
  const vector<int> cv;
  auto it1 = v.begin();//it1的类型是vector<int>::iterator
  auto it2 = cv.begin();//it2的类型是vector<int>::const_iterator
  cout << v[0] << endl;
  //如果对象只需要读操作而无须写操作的话最好使用常量类型 如const_iterator
  //为了专门得到const_iterator类型的返回值  c++11 新标准引入了两个新函数 cbegin cend
  
  //不论vector对象本身是否是常量  返回值都是const_vector
  auto it3 = v.cbegin();//it3的类型是vector<int>::const_iterator
  
  //结合解引用和成员访问操作
  
  vector<string> text {"a","b","c","d","e","f","g","h"};
  auto beg =  text.begin(), end = text.end();
  auto mid  = (end - beg) / 2 + text.begin();
  
  string sought = "a";
  
  //text 必须保证有序
  while(mid != end && *mid != sought){
    if(sought < *mid){ //如果中间值大于我们要找的值  说明我们要找的元素在前半部分
      end = mid;       //往左边找 将我们要查找的范围缩小 调整搜索范围似的忽略掉后半部分
    }else{
      beg = mid + 1;   //如果中间值小于我们要找的值 说明我们要找的元素在后半部分
                       //往右边找
    }
    mid = beg + (end - beg) / 2;
    cout << *mid << endl;
  }
}
