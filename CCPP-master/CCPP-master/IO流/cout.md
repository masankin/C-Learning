cout������ݷ�
=====
##���ݷ�
cout����û���廨���ŵ������ʽ���ı����������Ϊ��ͨ��ʹ�ò��ݷ������ݺ������ķ�ʽ��ɵġ�  

���ݺ���������ͷ�ļ�`<iomanip>`�У�manip��manipulator��������������д��
##���ݷ���ʹ��
�����ݺ���ֱ�Ӷ���<<���ź��漴�ɡ�  
���磺`cout<<setioflag(ios::left);`������������롣
###���ò��ݷ�
|���ݷ�|����|���ݷ�|����
|-----|----|-----|-----
|dec|ʮ����|setiosflags(ios::fixed)|�����ʽ��ʾ
|hex|ʮ������|setiosflags(ios::left)|�����
|oct|�˽���|setiosflags(ios::right)|�Ҷ���
|setfill(`c`)|��������ַ�c|setiosflags(ios::skipws)|����ǰ���ո�
|setprecision(`n`)|����С������Ϊnλ|setiosflags(ios::uppercase)|ʮ����������д���
|setw(`n`)|�����Ϊn���ַ�|setiosflags(ios::lowercase)|ʮ��������Сд���

ע��
- ʮ��������ʾ��ȱʡ����ʾСд��ĸ�� 
- ios::fixed�ǽ���������ʾΪ����+С����+С�����ֵĸ�ʽ��С��λ��Ϊ6��ȱʡΪ5λ��
- setprecision(2)ʱ��12.123��ʾΪ12.12����setprecision(0)ʱΪĬ��С�����ȣ����ض�
- ���ݷ����Ի��ʹ�ã��ö��<<���
- һ��cout�п�����;�ı������ʽ�����������ʱ�����ʽ����ǰ��Ĳ��ݷ�Ϊ׼
- ���ݷ���ȫ�ֵģ���ʹ��д�˶��cout����ͬ��cout֮���������ʽҲ�ǹ�ͬ�ġ�

һ�����ӣ�
```cpp
double pi = 3.14125926;
cout<<pi<<endl;
cout<<setprecision(0)<<pi<<endl
    <<setprecision(2)<<pi<<endl
	<<setprecision(4)<<pi<<endl;
cout<<pi<<endl;
```
�����
```
3.14159
3
3.14
3.1416
3.1416
```
