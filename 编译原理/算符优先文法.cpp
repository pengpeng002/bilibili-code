#include <bits/stdc++.h>
#include "�ʷ�����4.cpp"
using namespace std;
//�﷨�����������������������Ϊ���ֱ�Ӷ����������������ض��������ٰ���һ�����ս�����Ҳ�����������С�ض��������
//��Ȼ�ض��ﶼ�ǿɹ�Լ�����������ض�����ǵ�ǰҪ��Լ�Ĵ�
//�ڹ�Լ�Ĺ����У����ĳһ���Ӵ�������ս������ǰ��Ķ�Ҫ�ߣ����Ҳ���ս���Ⱥ����Ҳ��Ҫ�ߣ�����֤������Ӵ��������ض���
//������ȷ������ڹ�Լʱ���������������ս�������Եõ��Ľ����һ��ʱ�﷨����Ҳ�ᵼ�¿��ܻ�ʶ�𵽷Ƿ�����
//���Ϸ��ľ���һ������ʶ��� 
vector<string> inputString;//������﷨����
set<string> vn,vt;//�ս������ս����vn���ս����vt�ս�������������ս���ͷ��ս��Ϊ�ַ��� 
map<string, vector<vector<string> > >yufa;//�滻����ÿ�����ս�������ж����ѡ����ʽ
map<string, map<string, int> >F, L, table;//FIRSTVT = {a | F[P][a]=TRUE} ��LASTVTͬ��tableΪ���ȹ�ϵ�� 
stack<pair<string, string> >sta;
string start="";//�ķ��Ŀ�ʼ���� 
string S[123];
string Next()
{
	string ans="";
	char ch;
	scanf("%c",&ch);
	ans+=ch;
	if(ch=='\n') ans="#";
	return ans;
	node no=cifa();//��ȡ�ʷ���������������Ϊ�﷨������������� 
//	cout<<no.name<<" "<<no.value<<endl;
	return no.name;//��ʱ�������ɣ�ֻȡ��һ���ַ������ս�����Ժ�Ӧ�û�ĳɷ��ս��Ϊ���ʵ� 
}
void getVnVt()
{
	string s;
	while(cin>>s) inputString.push_back(s);
	sort(reserve.begin(), reserve.end());//������ֲ��ң���Ϊreserve����Ķ����ս��������ֱ����reserve���� 
	for(auto i:inputString)
	{
		int pos=i.find("->");//pos���ҵ���λ�� 
		vn.insert(i.substr(0,pos));//��0��ʼ����pos-1ֹͣ����������pos�� 
		if(start=="") start=i.substr(0,pos);//�ķ���ʼ���ż���һ������ʽ�ĵ�һ�����ս�� 
	}
	for(auto i:inputString)
	{
		int pos=i.find("->");
		string s="";//s�洢��ǰ�õ����ַ�����Ѱ���Ƿ�Ϊ�ս������ս��������Ǿ���գ�����һ�� 
		vector<string> ve;//���ս������ս���ֿ����洢����ʽ���Ҳ� 
		for(int j=pos+2; j<i.length(); j++)
		{
			s+=i[j];
			if(vn.find(s) != vn.end())
			{
				ve.push_back(s);
				s="";
				continue;
			}
			if(binary_search(reserve.begin(), reserve.end(), s))//���֣�Ӧ�ñ�find����΢��һ�㡣�ҵ��˷���1��û�ҵ�����0 
			{
				ve.push_back(s);
				vt.insert(s); 
				s="";
			}
			else if(s=="defined" || s=="number" || s=="$")
			//�����reserve��û�ҵ����п�����defined(�Զ����ʶ��),number(����),$(����) 
			{
				ve.push_back(s);
				vt.insert(s); 
				s="";
			}
			else
			{
				ve.push_back(s);
				vt.insert(s); 
				s="";
			}
		}
		yufa[i.substr(0,pos)].push_back(ve);//yufa��stringӳ���ά���� 
	}
//	for(auto i:yufa)
//	{
//		cout<<i.first<<"	";
//		for(auto j:i.second)
//		{
//			cout<<"	";
//			for(auto k:j) cout<<k<<" ";
//			cout<<endl;
//		}
//		cout<<endl;
//	}
	printf("\n���ս����");
	for(auto i:vn) cout<<i<<" ";
	printf("\n�ս����");
	for(auto i:vt) cout<<i<<" ";cout<<endl;
}
void push(map<string, map<string, int> >&F, string P, string a)//FIRSTVT = {a | F[P][a]=TRUE} F[P][a]=1, ���Ŷ�(P,a)ѹ��ջ 
{
	if(F[P][a]==0)
	{
		F[P][a]=1;
		sta.push(make_pair(P,a));
	}
}
void getFirstVt()
{
	for(auto i:vn)//��ÿ�����ս��P���ս��a����ʼ��F[P][a]Ϊ�� 
	{
		for(auto j:vt) F[i][j]=0;
	}
	for(auto i:yufa)//�������в���ʽ��������P->a...��P->Qa...�Ĳ���ʽ��F[P][a]=1, ���Ŷ�(P,a)ѹ��ջ 
	{
		vector<vector<string> >ve=i.second;
		for(auto j:ve)
		{
			for(auto k:j)
			{
				if(vt.find(k)!= vt.end())//����P->a...��P->Qa...��a����firstvt(p)�� 
				{
					push(F,i.first,k);
					break;
				}
			}
		}
	}
	while(!sta.empty())
	{
		pair<string,string> P=sta.top();//����ջ������Ϊ(Q,a)�� 
		sta.pop();
		for(auto i:yufa)//����ÿ������ʽ 
		{
			for(auto j:i.second)//jΪĳһ������ʽ 
			{
				if(j[0]==P.first)//����P->Q...�Ĳ���ʽ����F[P][a]Ϊ�٣����棬��ѹ��ջ 
				{
					push(F,i.first,P.second);
				}
			}
		 } 
	}
	cout<<"	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:F)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<j.second<<"	";
		}
		cout<<endl;
	}
}
void getLastVt()
{
	for(auto i:vn)//��ÿ�����ս��P���ս��a����ʼ��L[P][a]Ϊ�� 
	{
		for(auto j:vt) L[i][j]=0;
	}
	for(auto i:yufa)//�������в���ʽ��������P->a...��P->Qa...�Ĳ���ʽ��L[P][a]=1, ���Ŷ�(P,a)ѹ��ջ 
	{
		vector<vector<string> >ve=i.second;
		for(auto j:ve)
		{
			for(int k=j.size()-1; k>=0;k--)
			{
				if(vt.find(j[k])!= vt.end())//����P->...a��P->...aQ��a����lastvt(p)�� 
				{
					push(L,i.first,j[k]);
					break;
				}
			}
		}
	}
	while(!sta.empty())
	{
		pair<string,string> P=sta.top();//����ջ������Ϊ(Q,a)�� 
		sta.pop();
		for(auto i:yufa)//����ÿ������ʽ 
		{
			for(auto j:i.second)//jΪĳһ������ʽ 
			{
				if(j[j.size()-1]==P.first)//����P->...Q�Ĳ���ʽ����L[P][a]Ϊ�٣����棬��ѹ��ջ 
				{
					push(L,i.first,P.second);
				}
			}
		 } 
	}
	cout<<"	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:L)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<j.second<<"	";
		}
		cout<<endl;
	}
}
void getTable()
{
	for(auto i:vt)//table�洢�����֮������ȹ�ϵ�����ڣ����ڣ�С�ڣ������ܱȽϣ���0����ʾ���� 
	{
		for(auto j:vt)
		{
			table[i][j]='0';
			table["#"][j]='0';//�ر��ж�# 
		}
		table[i]["#"]='0';
	}
	table["#"]["#"]='=';
	cout<<"	";cout<<"#	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:table)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<(char)j.second<<"	";
		}
		cout<<endl;
	}
	for(auto i:yufa)
	{
		vector<vector<string> >ve=i.second;
		for(int j=0;j<ve.size();j++)
		{
			for(int k=0;(k+2)<=ve[j].size();k++)//size�ķ���������unsigned long long���������ܻ���� 
			{
				string xi=yufa[i.first][j][k], xi1=yufa[i.first][j][k+1];
				if(vt.find(xi) != vt.end() && vt.find(xi1) != vt.end())//����...ab... ,a��b���ȼ���ͬ 
				{
					table[xi][xi1]='=';
				}
				if((k+3)<=ve[j].size())
				{
					string xi2=yufa[i.first][j][k+2];
					if(vt.find(xi) != vt.end() && vt.find(xi1) == vt.end() && vt.find(xi2) != vt.end())
					//����...aQb...��a��b�����ȼ���ͬ 
					{
						table[xi][xi2]='='; 
					}
				}
				if(vt.find(xi) != vt.end() && vt.find(xi1) == vt.end())
				//����...aQ...�����κ�b����firstvt(Q)������a < b 
				{
					for(auto a:F[xi1])
					{
						if(a.second==1) table[xi][a.first]='<';
					}
				}
				if(vt.find(xi) == vt.end() && vt.find(xi1) != vt.end())
				//����...Qb...�����κ�a����lastvt(Q)������a > b 
				{
					for(auto a:L[xi])
					{
						if(a.second==1) table[a.first][xi1]='>';
					}
				}
			}
		}
	}
	//����#E#���������ߵ�#����firstvt(E)���ұߵ�#����lastvt(E) 
	for(auto i:F[start]) if(i.second==1) table["#"][i.first]='<';
	for(auto i:L[start]) if(i.second==1) table[i.first]["#"]='>';
	cout<<"	";cout<<"#	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:table)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<(char)j.second<<"	";
		}
		cout<<endl;
	}
}
void analyze()
{
	int k=0;//K��ʾջ����� 
	S[k]="#";
	string a;
	do
	{
		a=Next();
		int j;//jʼ��ָ��ջ����������Ǹ��ս��
		if(vt.find(S[k]) != vt.end() || S[k]=="#") j=k;//���ջ�����ս����jָ��ջ�� 
		else j=k-1;//����ָ��ջ������һ������Ϊ��������ķ������������������ֵķ��ս�����ʴ�ʱ�Ĵ�ջ��һ�����ս��
		cout<<a<<" "<<S[j]<<(char)table[S[j]][a]<<endl; 
		while(table[S[j]][a]=='>')//���ջ�������ȼ��������Ҫ�ߣ�ջ��Ԫ���ȹ�Լ����ʱ�Ѿ��γ��������ض�����Խ��й�Լ 
		{
//			printf("this j=%d\n",j);
			string Q=S[j];
			while(table[S[j]][Q]!='<')//��ǰ�ҵ������ض����ͷ��ֹͣ 
			{
//				cout<<Q<<" "<<S[j]<<" j="<<j<<" "<<(char)table[S[j]][Q]<<endl;
				Q=S[j];
				if(vt.find(S[j-1]) != vt.end() || S[j-1]=="#")//��֤jһ��ָ���ս�� 
				{
					j=j-1;
				}
				else j=j-2;
			}
//			printf("j=%d\n",j);
			vector<string> ve;
			string N;
			bool f=0;
			for(int i=j+1;i<=k;i++) ve.push_back(S[i]);//�������ض������ve��
			for(auto i:yufa)//���ѭ�����������ĸ�����ʽ���й�Լ 
			{
				for(auto j:i.second)
				{
					if(ve.size() == j.size())
					{
						f=0;
						for(int k=0;k<ve.size();k++)//������ȷ�������ԼʱֻҪ�ս����Ӧ��ȣ����ս���Է��ս���Ϳ����� 
						{
							if(ve[k] != j[k])
							{
								if(vt.find(ve[k]) != vt.end() || ve[k]=="#")//����ս������Ӧ��ȣ����ܹ�Լ 
								{
									f=1;
									break;
								}
							}
						}
						if(f==0)//˵����ǰ�Ĳ���ʽ���Թ�Լ 
						{
							N=i.first;
							break;
						 } 
					}
					else f=1;
					if(!f) break;
				}
				if(!f) break;
			}
			k=j+1;
			S[k]=N;
			cout<<"��Լ��";
			for(auto i:ve) cout<<i;
			cout<<"->"<<N<<endl;
		}
		if(table[S[j]][a]=='<' || table[S[j]][a]=='=')//���ջ���ս��������ĵͻ���ȣ�˵��������ȹ�Լ 
		{
			k++;
			S[k]=a;
		}
		else cout<<"error!\n"; //���Ǵ��ڣ����ڣ�С�ڣ�˵�������� 
	}
	while(a!="#");
}
int main()
{	
	init();
	freopen("prodown.txt","r",stdin);
	getVnVt();
	getFirstVt();
	getLastVt();
//	exit(0);
	getTable();
	freopen("CON","r",stdin);
	analyze();
}
