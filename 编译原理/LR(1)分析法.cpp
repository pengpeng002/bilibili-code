#include <bits/stdc++.h>
#include "�ʷ�����6.cpp"
using namespace std;
vector<string> inputString;//������﷨����
set<string> vn,vt,se;//�ս������ս����vn���ս����vt�ս�������������ս���ͷ��ս��Ϊ�ַ���
//se����ʱ�洢�����ַ�����first����
map<string, vector<vector<string> > >yufa;//�滻����ÿ�����ս�������ж����ѡ����ʽ
string start="";//�ķ��Ŀ�ʼ����
vector<vector<string> >input;//��¼����ʽ��������һ����ţ����������е�λ�� ����ͨ�������ò���ʽ
map<vector<string>, int> rInput; //ͨ������ʽ�õ���ţ���input�෴
typedef pair<vector<string>, int> Obj;//����ʽ�� ����λ�� ������Ϊһ����Ŀ
typedef pair<Obj, string> OObj;//LR(1)��Ŀ 
map<pair<int, string>, int>GO;//pair<��Ŀ��ţ��ս��/���ս��>����Ŀ���  I״̬ʶ��һ��X֮�󵽴�J״̬
vector<set<OObj> >DFA;//��Ŀ���淶��
stack<pair<int, string> > sta;//����ջ
map<int, map<string, pair<string, int> > > action;//action��action[��Ŀ�����][�ս��]=pair<�ƽ�/��Լ�� ��Ŀ��/����ʽ���>
map<int, map<string, int> >GOTO; //GOTO��goto[��Ŀ�����][���ս��]=��Ŀ�����
map<string, set<string> >first, follow;
string Next()//���ôʷ�������ȡ��һ�����뵥�� 
{
	node no=cifa();
	cout<<no.name<<" "<<no.value<<endl;
	return no.name;//��Ϊ��return������Ĳ���ִ��
	char ch;
	if(scanf("%c",&ch)!=1) return "#";//����ֵ������1˵��������
	string ans="";
	ans+=ch;
	if(ch=='\n') ans="#";
	return ans;
}
void getVnVt()//�����ս������ս�� 
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
		vector<string> ne;//��¼һ������ʽ����������
		ne.push_back(i.substr(0,pos)); //������ʽ���󲿷��ڵ�һ��λ��
		for(int j=pos+2; j<i.length(); j++)
		{
			s+=i[j];
			if(vn.find(s) != vn.end())
			{
				ve.push_back(s);
				ne.push_back(s);
				s="";
				continue;
			}
			if(binary_search(reserve.begin(), reserve.end(), s))//���֣�Ӧ�ñ�find����΢��һ�㡣�ҵ��˷���1��û�ҵ�����0
			{
				ve.push_back(s);
				ne.push_back(s);
				vt.insert(s);
				s="";
			}
			else if(s=="defined" || s=="number" || s=="$")
				//�����reserve��û�ҵ����п�����defined(�Զ����ʶ��),number(����),$(����)
			{
				ve.push_back(s);
				ne.push_back(s);
				vt.insert(s);
				s="";
			}
			else if(s=="a" || s=="b")
			{
				ve.push_back(s);
				ne.push_back(s);
				vt.insert(s);
				s="";
			}
		}
		yufa[i.substr(0,pos)].push_back(ve);//yufa��stringӳ���ά����
		input.push_back(ne);
	}
	vt.insert("#");//��������#�����ս������
	for(int i=0; i<input.size(); i++) //�������м�¼�Ĳ���ʽ�����������Ŷ�Ӧ���������rInput��
	{
		rInput[input[i]]=i;
	}
	printf("\n���ս����");
	for(auto i:vn) cout<<i<<" ";
	printf("\n�ս����");
	for(auto i:vt) cout<<i<<" ";
	cout<<endl;
	for(int i=0; i<input.size(); i++)
	{
		cout<<"number "<<i<<endl;
		cout<<input[i][0]<<"->";
		for(int j=1; j<input[i].size(); j++)
		{
			cout<<input[i][j]<<" ";
		}
		cout<<endl;
	}
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
				vector<string> s=j;
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
							if(k!="$")//�������Ԫ��
							{
								if(first[i].insert(k).second) change=1;//����ɹ���first���Ϸ����˱仯
							}
							else flag=1;//�п�Ԫ�أ���ǣ�������һ��
						}
					}
					if(flag==0) break;//������Ԫ�أ���������һ��
				}
				if(flag==1 && first[i].insert("$").second) change=1;
				//ÿ�����ս����first���϶��п�Ԫ�أ����Խ���Ԫ�ط��롣����ɹ��ͱ�Ƿ����˱仯
			}
		}
		for(auto i:vt) first[i].insert(i);//�ս����first������������
	}
	for(auto i:first)
	{
		string ch=i.first;
		cout<<"char="<<ch<<": ";
		for(auto j:first[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
}
void getFirst(vector<string> s)//�������⴮�ķ��ս��
{
	se.clear();//��ʱʹ�ã�ÿ�����
	bool flag=1;//��ǿ�Ԫ�أ���ʼΪ1���Բ��ÿ���s����Ϊ0�����
	for(int i=0; i<s.size(); i++) //����
	{
		flag=0;//��ʼĬ��û��
		for(auto j:first[s[i]])//��ÿ���ַ���first���ϼ��뵽s��first�����У������ַ���
		{
			if(j!="$") se.insert(j);
			else flag=1;
		}
		if(flag==0) break;//���û�п��ַ�����������һ��
	}
	if(flag==1) se.insert("$");//ÿ���ַ���first���϶��������ַ�����Ȼs�������ɲ��Ƶ�������
}
void getFollow()//������ս����follow����
{
	bool change=1;
	follow[start].insert("#");//�ķ���ʼ���ŵ�follow�����з���һ��#��
	while(change)
	{
		change=0;
		for(auto i:vn)//�������ս��
		{
			for(auto j:yufa[i])//����ÿ������ʽ
			{
				for(int pos=0; pos<j.size(); pos++) //��������ʽ
				{
					if(vn.find(j[pos]) != vn.end())//���ս��
					{
						vector<string> s;
						s.assign(j.begin()+pos+1, j.end());//����A->aBb��ȡ���Ӵ�b
//						string s=j.substr(pos+1);
						getFirst(s);//����b��first���ϣ�������se��
						if(pos+1 != j.size())//����A->aBb����b��first���������зǿ�Ԫ�ؼ��뵽B��follow������
						{
							for(auto k:se)//����se�����ǿ�Ԫ�ؼ���
							{
								if(k!="$")
								{
									if(follow[j[pos]].insert(k).second) change=1;//����ɹ�������иı�
								}
							}
						}
						if(se.find("$") != se.end())
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
		string ch=i.first;
		cout<<"char="<<ch<<": ";
		for(auto j:follow[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
}

set<OObj> closure(OObj x)//������Ŀ�հ� 
{
	set<OObj> ans;//��Ҫ���صıհ�
	ans.insert(x);
	bool f=1;//���Ｏ���Ƿ��б仯
	while(f)//ֱ�����ϲ�������Ϊֹ
	{
		f=0;//�������������f=1
		for(auto x1:ans)
		{
			auto x=x1.first;//Ϊ�˼��ٴ���Ķ� 
			if(x.second == x.first.size())//������Ѿ��������һ��Ϊֹ����Ȼ�������������������հ�
			{
				f=0;
				break;
			}
			string s=x.first[x.second];//�õ������ķ���
			if(vn.find(s) != vn.end())//��������ʽ���ս��������A->X ��BY,���������� B->X�Ĳ���ʽ��Ӧ����������հ�
			{
				for(auto i:yufa[s])
				{
					vector<string> temp=i;
					temp.insert(temp.begin(), s);
					//��Ϊyufa������Ĳ���ʽ�ǲ������󲿷��ŵģ�������Ҫ���󲿷��Ų��뵽�ͷ
					vector<string> xx;
					//����Ŀ[A->X��BY,a]���ڱհ�ans��B->R��һ������ʽ����first(Ra)�е�ÿ���ս��b����[B->R,b]����ans�У����� 
					xx.assign(x.first.begin()+x.second+1, x.first.end());
					xx.push_back(x1.second);
					getFirst(xx);
					for(auto b:se)//getfirst����õ��Ľ����ʱ�����se�� 
					if(ans.insert(make_pair(make_pair(temp,1),b)).second)
					{
						f=1;//�������ɹ���f=1������������
					}
				}
			}
		}
	}
	return ans;
}
void go(int pos, string xx)//������Ŀת������GO 
{
	set<OObj> ans;
	for(auto x1:DFA[pos])
	{
		auto x=x1.first;
		if(x.first.size() == x.second) continue;//����Ѿ��������һ��λ���ˣ�˵����һ����Լ��Ŀ������������
		if(x.first[x.second] == xx)//ֻ���ڵ�ǰ״̬��������һ��������xx�����ܺ��Ƶ���м���հ�
		{
			set<OObj> temp=closure(make_pair(make_pair(x.first, x.second+1),x1.second));//����������һλ�ıհ�
			ans.insert(temp.begin(), temp.end());//set�����Զ�ȥ��
		}
	}
	if(ans.empty()) return ;//����õ����ǿյģ���ôֱ�ӷ���
	int loc=find(DFA.begin(), DFA.end(), ans) - DFA.begin();//find�����������е�λ�ã���ȥ��ʼλ�õõ����
	if(loc != DFA.size())
	{
		GO[make_pair(pos, xx)]=loc; 
	}
	else
	{
		DFA.push_back(ans);
		GO[make_pair(pos, xx)] = DFA.size()-1;
	}
}
void itemSets()//������Ŀ���淶�����˵�γ�DFA 
{
	set<OObj> statu=closure(make_pair(make_pair(input[0],1),"#"));//��ʼ��Ŀ�ıհ�����Ϊһ����Ŀ��
	DFA.push_back(statu);//����DFA�У��õ�һ�����Ϊ0����Ϊλ��Ϊ0��λ��
	for(int i=0; i<DFA.size(); i++)
		//����ÿһ����Ŀ��֪��DFA��������Ϊֹ����Ϊpush_back���Զ�������������һ��Ϊֹ�����Կ���ֱ�ӱ���
	{
		for(auto t:vt) go(i,t);//����ÿһ����Ŀ����ÿһ���ķ����ţ����Ƿ��п��ܵõ�һ���µ���Ŀ����\
		������ԣ����µ���Ŀ�����뵽��Ŀ���淶����
		for(auto n:vn) go(i,n);
	}
	for(int i=0; i<DFA.size(); i++)
	{
		cout<<i<<endl;
		for(auto j1:DFA[i])
		{
			auto j=j1.first;
			vector<string> temp=j.first;
			cout<<temp[0]<<"->";
			for(int k=1; k<temp.size(); k++)
			{
				if(k==j.second) cout<<" ��";
				cout<<temp[k]<<" ";
			}
			if(j.second == temp.size()) cout<<" ��";
			cout<<", "<<j1.second<<"\n";
		}
		cout<<endl;
	}
	for(auto i:GO)
	{
		cout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
	}
}
void getActionAndGoto()//����action���goto�� 
{
	for(int i=0; i<DFA.size(); i++) //��action���goto���ʼ��
	{
		for(auto j:vt) action[i][j]=make_pair("error",0);
		for(auto j:vn) GOTO[i][j]=-1;
	}
	for(auto i:GO)//map<pair<int, string>, int>
	{
		if(vt.find(i.first.second) != vt.end())//���� A->Xa��Y,������Ŀ��k��GO[k][a]=j,��action[k][a]=<s,j>
		{
			if(action[i.first.first][i.first.second].first != "error")//���֮ǰ��ֵ��Ϊerror��˵�����ظ���
			{
				cout<<"have an error\n";
			}
			action[i.first.first][i.first.second]=make_pair("s",i.second);
		}
		else//GO[k][A]=j,GOTO[k][A]=j
		{
			if(GOTO[i.first.first][i.first.second] != -1)
			{
				cout<<"have an error\n";
			}
			GOTO[i.first.first][i.first.second]=i.second;
		}
	}
	for(int i=0; i<DFA.size(); i++)
	{
		set<OObj> x=DFA[i];
		for(auto j1:x)
		{
			auto j=j1.first;
			if(j.first.size() == j.second)//����������һλ��˵����һ����Լ��Ŀ
			{
				if(j.first[0] == start)//������ķ���ʼ���ţ�˵���������
				{
					if(action[i]["#"].first != "error")
					{
						cout<<"have an error\n";
					}
					action[i]["#"]=make_pair("acc",0);
					continue;
				}
				string a=j1.second;//LR(1)������ֻ�е��������Ĵ��� 
//				for(auto a:follow[j.first[0]])//SLR(1)ʱ����Ҫ����һ���ַ�����follow���ϵ���r 
//				for(auto a:vt)//LR(0)������������Լ��Ŀʱ������Ҫ����һ���ַ���ֱ�ӽ��й�Լ�����������е��ս��������r
//				{
					if(action[i][a].first != "error")
					{
						cout<<"have an error\n";
					}
					action[i][a]=make_pair("r",rInput[j.first]);//��Լ��Ŀ���õĲ���ʽ���ͨ��rinput��
//				}
			}
		}
	}
	cout<<"	";
	for(auto i:vt) cout<<i<<"	";
	cout<<endl;
	for(int i=0; i<DFA.size(); i++)
	{
		cout<<i<<"	";
		for(auto j:vt)
		{
			cout<<action[i][j].first<<action[i][j].second<<"	";
		}
		cout<<endl;
	}
	cout<<"	";
	for(auto i:vn) cout<<i<<"	";
	cout<<endl;
	for(int i=0; i<DFA.size(); i++)
	{
		cout<<i<<"	";
		for(auto j:vn)
		{
			cout<<GOTO[i][j]<<"	";
		}
		cout<<endl;
	}
}
string guiyue(int pos)//���й�Լ 
{
	string s=input[pos][0];//s�����󲿷���
	cout<<"���ò���ʽ"<<pos<<"���й�Լ\n";
	cout<<s<<"->";
	for(int i=1; i<input[pos].size(); i++)cout<<input[pos][i]<<" ";
	cout<<endl;
	for(int i=0; i<input[pos].size()-1; i++) //-1����Ϊ��һ���ǻ�ȡ��ʽ�Ӷ���һ���󲿷��ţ�����Ҫ-1��������Ƴ�
	{
		sta.pop();
	}
	return s;
}
void analyze()//�﷨���� 
{
	sta.push(make_pair(0,"#"));//����ջ�ȷ�һ��״̬0������Ϊ#
	string a=Next();
	while(1)
	{
		pair<int, string> x=sta.top();//��ȡջ��Ԫ��
		pair<string, int> op=action[x.first][a];//ʼ���õ�ǰջ����״̬�����ٵ��ս��ȥ��action����ȡ�������Ĳ���
		if(op.first=="s")//�����s����ʾ���ƽ���Ŀ
		{
			sta.push(make_pair(op.second, a));//action�ĵڶ���Ԫ�ؾ�����һ��״̬
			cout<<"��ǰ����Ϊ"<<a<<"���ƽ�����\n";
			a=Next();//��ȡ��һ������
		}
		else if(op.first=="r")//�����r��˵���ǹ�Լ��Ŀ
		{
			cout<<"��ǰ������"<<a<<endl;
			string N=guiyue(op.second);//��ȡ��Լ����ʽ���󲿣����뵽ջ��
			int st=GOTO[sta.top().first][N];//��ȡ��һ��״̬
			sta.push(make_pair(st,N));//��Լ����û���õ���ǰ���ٵ��ս�������Բ��û�ȡNext
		}
		else if(op.first=="acc") break;//�����acc������״̬��˵����������
		else
		{
			if(action[x.first]["$"].first != "error")//������ÿ�����չ��ѡ���ÿ�����չ
			{
				cout<<"�ƽ�һ������\n";
				sta.push(make_pair(action[x.first]["$"].second,"$"));
//				a=Next();
			}
			else cout<<"error!\n";//����״̬������error
		}
	}
}
int main()
{
	freopen("DFA.txt","r",stdin);
	init();
	cin.clear();//cin�����⣬����������֮�������clear�ͻᵼ�º����cin����������� 
	freopen("prodown.txt","r",stdin);
	getVnVt();
	getFirst();
	getFollow();
	itemSets();
	getActionAndGoto();
	freopen("temp.txt","r",stdin);
	analyze();
}
