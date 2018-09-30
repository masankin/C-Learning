#include "myhash.h"

int main() {
  Elemtype r[12] = {{17, 1}, {60, 2}, {29, 3}, {38, 4}, {1, 5},  {2, 6},
                    {3, 7},  {4, 8},  {5, 9},  {6, 10}, {7, 11}, {8, 12}};
  HashTable H;
  int i, p, j;
  KeyType k;
  H.Init_HashTable();
  for (i = 0; i < 11; i++)  //插入前11个记录
  {
    j = H.Insert_Hash(r[i]);
    if (j == -1)
      cout << "表中已有关键字为" << r[i].key << "  " << r[i].ord << "的记录"
           << endl;
  }

  cout << "按哈希地址顺序遍历哈希表" << endl;
  H.Traverse_HashTable();
  cout << endl;

  cout << "输入要查找的记录的关键字：";
  cin >> k;
  j = H.Search_Hash(k, p);
  if (j == 1)
    H.Get_Data(p);
  else
    cout << "无此记录" << endl;

  j = H.Insert_Hash(r[11]);  //插入最后一个元素
  if (j == 0) {
    cout << "插入失败" << endl;
    cout << "需要重建哈希表才可以插入" << endl;
    cout << "____重建哈希表____" << endl;
    H.Insert_Hash(r[i]);  //重建后重新插入
  }

  cout << "遍历重建后的哈希表" << endl;
  H.Traverse_HashTable();
  cout << endl;

  cout << "输入要查找的记录的关键字：";
  cin >> k;
  j = H.Search_Hash(k, p);
  if (j == 1)
    H.Get_Data(p);
  else
    cout << "该记录不存在" << endl;

  cout << "____销毁哈希表____" << endl;
  H.Destroy_HashTable();

  return 0;  
}  