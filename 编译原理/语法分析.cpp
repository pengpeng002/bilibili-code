#include <bits/stdc++.h>
#include "�ʷ�����4.cpp"
using namespace std;
vector<string>inputString;//������﷨����
set<char> vn,vt;//�ս������ս����vn���ս����vt�ս��
map<char, vector<string> >yufa;//�滻����ÿ�����ս�������ж����ѡ����ʽ
map<char, set<char> >first, follow;//first������follow����
set<char> se;//��ʱ�洢�����ַ�����first����
map<char, map<char, string> > table;//�ɿ�����ά���飬ֵ��string
stack<char> sta;//����ջ / ����ջ
char Next()
{
	node no=cifa();//��ȡ�ʷ���������������Ϊ�﷨������������� 
//	cout<<no.name<<" "<<no.value<<endl;
	return no.name[0];//��ʱ�������ɣ�ֻȡ��һ���ַ������ս�����Ժ�Ӧ�û�ĳɷ��ս��Ϊ���ʵ� 
}
void getVnVt()//��ȡ�ս������ս��������ÿ���﷨���򶼱��浽yufa��
{
	string s;
	while(cin>>s) inputString.push_back(s);//���������﷨������ΪҪ�������Σ��ȴ洢
	for(auto i:inputString)
	{
		vn.insert(i[0]);//ÿ������ĵ�һ���ַ�һ���Ƿ��ս������ʱ�ٶ��ս���ͷ��ս����Ϊ�����ַ�
		yufa[i[0]].push_back(i.substr(3)); //���ս�������ʽ��Ӧ
	}
	for(auto i:inputString)
	{
		for(int j=3; j<i.length(); j++) //����ÿһ������ʽ��ÿ���ַ�
		{
			if(vn.find(i[j]) == vn.end()) vt.insert(i[j]);//����ڷ��ս���Ҳ����Ǿ�һ�����ս��
		}
	}
	printf("\n���ս����");
	for(auto i:vn) cout<<i<<" ";
	printf("\n�ս����");
	for(auto i:vt) cout<<i<<" ";
}
void getFirst()//����ÿ�����ս����first����
{
	bool change=1;
	while(change)//���first�����з����仯�ͼ�����֪��first���ϲ��ٱ仯��ֹͣ
	{
		change=0;
		for(auto i:vn)//�������ս��
		{
			for(auto j:yufa[i])//�������ս����ÿ������ʽ
			{
				string s=j;
				bool flag=0;//����Ƿ��п�Ԫ��
				for(auto ch:s)//�Բ���ʽ���б���
				{
					if(vn.find(ch) == vn.end())//������ս������ֱ�Ӽ��뵽first������
					{
						if(first[i].insert(ch).second) change=1;
						//���insert�ķ���ֵΪ�棬˵������ɹ�����Ǳ���first�����з����仯
						flag=0;//û�п�Ԫ��
					}
					else
					{
						flag=0;
						for(auto k:first[ch])//�Ը÷��ս����first���Ͻ��б���
						{
							if(k!='$')//�������Ԫ��
							{
								if(first[i].insert(k).second) change=1;//����ɹ���first���Ϸ����˱仯
							}
							else flag=1;//�п�Ԫ�أ���ǣ�������һ��
						}
					}
					if(flag==0) break;//������Ԫ�أ���������һ��
				}
				if(flag==1 && first[i].insert('$').second) change=1;
				//ÿ�����ս����first���϶��п�Ԫ�أ����Խ���Ԫ�ط��롣����ɹ��ͱ�Ƿ����˱仯
			}
		}
		for(auto i:vt) first[i].insert(i);//�ս����first������������
	}
	for(auto i:first)
	{
		char ch=i.first;
		cout<<"char="<<ch<<": ";
		for(auto j:first[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
}
void getFirst(string s)//�������⴮�ķ��ս��
{
	se.clear();//��ʱʹ�ã�ÿ�����
	bool flag=1;//��ǿ�Ԫ�أ���ʼΪ1���Բ��ÿ���s����Ϊ0�����
	for(int i=0; i<s.length(); i++) //����
	{
		flag=0;//��ʼĬ��û��
		for(auto j:first[s[i]])//��ÿ���ַ���first���ϼ��뵽s��first�����У������ַ���
		{
			if(j!='$') se.insert(j);
			else flag=1;
		}
		if(flag==0) break;//���û�п��ַ�����������һ��
	}
	if(flag==1) se.insert('$');//ÿ���ַ���first���϶��������ַ�����Ȼs�������ɲ��Ƶ�������
}
void getFollow()//������ս����follow����
{
	bool change=1;
	follow[inputString[0][0]].insert('#');//�ķ���ʼ���ŵ�follow�����з���һ��#��
	while(change)
	{
		change=0;
		for(auto i:vn)//�������ս��
		{
			for(auto j:yufa[i])//����ÿ������ʽ
			{
				for(int pos=0; pos<j.length(); pos++) //��������ʽ
				{
					if(vn.find(j[pos]) != vn.end())//���ս��
					{
						string s=j.substr(pos+1);//����A->aBb��ȡ���Ӵ�b
						getFirst(s);//����b��first���ϣ�������se��
						if(pos+1 != j.length())//����A->aBb����b��first���������зǿ�Ԫ�ؼ��뵽B��follow������
						{
							for(auto k:se)//����se�����ǿ�Ԫ�ؼ���
							{
								if(k!='$')
								{
									if(follow[j[pos]].insert(k).second) change=1;//����ɹ�������иı�
								}
							}
						}
						if(se.find('$') != se.end())
							//����A->aB����A��follow���ϼ��뵽B��follow�����С���pos�Ѿ������һλ��s�ĳ���Ϊ0��seֻ��������
							//������A->aB��A->aBb��b�Ǻ��Ƴ��������������������
						{
							for(auto k:follow[i])
							{
								if(follow[j[pos]].insert(k).second) change=1;
							}
						}
					}
				}
			}
		}
	}
	for(auto i:follow)
	{
		char ch=i.first;
		cout<<"char="<<ch<<": ";W
		for(auto j:follow[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
	exit(0); 
}
void getTable()
{
	for(auto i:vn)
	{
		for(auto j:vt)
		{
			table[i][j]="error";//������ M[A,a] ��ʼ��Ϊerror
		}
	}
	for(auto A:vn)//�������з��ս������
	{
		for(auto j:yufa[A])//��ÿ�� A->c ִ��������������
		{
			getFirst(j);//��ȡfirst(c)
			for(auto a:se)//��ÿ���ս��a����first(c)
			{
				if(table[A][a] != "error") cout<<"this is a error!\n"; 
				table[A][a]=j;//�� A->c ���뵽 M[A,a];
				if(se.find('$') != se.end())//���������� first(c)
				{
					for(auto b:follow[A])//����κ� b ���� follow(A)
					{
						if(table[A][b] != "error") cout<<"this is a error!\n"; 
						table[A][b]=j;//�� A->c ���뵽 M[A,b];
					}
					
				}
			}
		}
	}
	exit(0);
}
void analyze()
{
	sta.push('#');//�ȰѾ�ĩ��ѹ��ջ��
	sta.push(inputString[0][0]);//���ķ���ʼ����ѹ��ջ
	char in;
	in=Next();//��ȡ��һ������ 
	while(in!='#' || sta.top()!='#')//��X=a='#'�������ɹ���ֹͣ����
	{
		if(sta.top() == in)//����ܹ�ƥ�䣬��ջ��������Ȼ�����һ���ַ�
		{
			cout<<"delete "<<in<<endl;
			sta.pop();//ջ��Ԫ�ص���
			in=Next();
			if(in=='\n') in='#';//��ĩ��
			continue;
		}
		string s=table[sta.top()][in];//��ȡԤ��������ŵ���һ������
		if(s!="error")//������Ǵ���
		{
			cout<<sta.top()<<"->"<<s<<endl;//���ִ�еĶ���
			sta.pop();//ջ��Ԫ�ص���
			for(int i=s.length()-1; i>=0; i--)//������ʽ�Ҳ����Ŵ�����ѹ��ջ��
			{
				if(s[i]!='$') sta.push(s[i]);
			}
		}
		else cout<<"error!\n";//����ǳ����־�ͱ���
	}
	cout<<"�����ɹ�!\n";
}
int main()
{
	init();//��ʼ���ʷ����������״̬ת�ƾ��� 
	freopen("prodown.txt","r",stdin);
	getVnVt();
	getFirst();
	getFollow();
	getTable();
//	freopen("CON","r",stdin);//�ض��򵽱�׼���������Ӽ��̻�ȡ����
	analyze();
}
