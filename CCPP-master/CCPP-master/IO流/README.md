I/O��
========
##�ļ���
- fstream
  - ifstream
  - ofstream

##ifstream
istream������

���ļ�����������ʷԭ���������C����ַ�����

����Ӧ����������ʹ�����֮�����close�����ر����������ں��������󣬻��Զ����������������������������close����
������ʱ����Ϊ���ڻ������У�û�м�ʱclose���ᵼ�¶�ʧ��������������

```cpp
ifstream if("read.txt");

string str = "read.txt";
ifstream if2(str.c_str());

if.close();
if2.close();
```
Ҳ������������֮������open����ָ��Ҫ�����ļ�
```cpp
ifstream if;
if.open("read.txt");
```
���Ҫ�ı�Ҫ�����ļ��������ٴ�open������һ��Ҫ��֤close��֮ǰ���ļ���
```cpp
if.open("read.txt");
if.close();
if.open("hello.txt");
```
###����ƫ��
ʹ�ó�Ա����seekg����
```cpp
istream& seekg( streampos pos );  // ����Ϊ���ͣ���ʾ�����ļ���ʼλ�õ�ƫ��
istream& seekg( streamoff off, ios::seek_dir dir );
```
�ڶ���������ʽ�ĵڶ���������ʾ���λ�ã�
|��־|˵��|���fseek
|---|----|------|
ios::beg | �ļ���ʼ��λ��| SEEK_SET
ios::cur |��ǰλ��| SEEK_CUR
ios::end | �ļ�ĩβ��λ��| SEEK_END

##ofstream
ostream�����ࡣ

�󲿷��÷���ifstream��ͬ����Ҳ�в�ͬ��
###����ƫ��
ʹ�ó�Ա����seekp������
```cpp
ostream& seekp( streampos pos );
ostream& seekp( streamoff off, ios::seek_dir dir );
```

##fstream
ͷ�ļ�
```cpp
#include <fstream>
```
fstream�̳���ifstream��ofstream�����Խ��ж�д��
```cpp
fstream if("read.txt"); //ȱʡ�Ƕ�ģʽ
fstream of("write.txt", ios::out); //��ʽָ��дģʽ
```
|�����־|˵��
|-----|-----
ios::app |�Ӻ������
ios::ate | �򿪲��ҵ��ļ�β
ios::binary | ������ģʽ I/O (���ı�ģʽ���)
ios::in | ֻ����
ios::out | д��
ios::trunc | ���ļ���Ϊ 0 ���ȣ�����ԭ��ȫ�����ݣ�

�����ǰ�λ�����ϡ�