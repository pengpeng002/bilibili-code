//DFA����һ�������NFA
//Ϊ���ó���������ܹ�����DFA.txt�������򲻸�������̬Ψһ�ԣ������˹���
//���Ե�NFA����Ӧ������ʽ�� (a|b)*(aa|bb)(a|b)*
#include<bits/stdc++.h>
using namespace std;
struct node
{
	string name;//��������
	string value;//��������ֵ
};
bool duwanle=0;
FILE *f=fopen("temp.txt","r");//ֻ�����ļ�temp.txt
vector<string> reserve= {"short","int","bool","long","float","double","char","static","for","while","do","continue","break",\
                         "return","void","switch","case","auto","goto","const","if","else","+","++","+=","-","-=","--","*",\
                         "*=","/","/=","%","%=","<","<<","<=",">",">>",">=","&","|","^","&&","||","^=","!=","!","~","#","=",\
                         "(",")",",",";","?",":","[","]","{","}","==","<<=",">>=","&=","|="
                        };
char ch,prech='$';
string name;
const int maxn=130;//128��ASCII�ַ�
vector<int> statu, acc;//����״̬��ţ�����״̬���
map<int, int[maxn]> a,aTemp;//��ά���飬����ѡ����a[xx][130]
map<pair<int, char>, int>GO1;//pair<��Ŀ��ţ������ַ�>����Ŀ���  I״̬ʶ��һ��X֮�󵽴�J״̬
vector<set<int> >DFA1;//��Ŀ���淶��
int Node=2147483647;//int���ֵ���ٳ��������½��ڵ�ı�� 
vector<vector<int> >state;//״̬���ֵļ���
set<char> character;//�洢�����ַ���������� 
void mini()
{
	vector<int> tt;//��ʱ�洢���еķ���̬ 
	for(int i=0;i<DFA1.size();i++)
	{
		if(find(acc.begin(), acc.end(), i) == acc.end())
		{
			tt.push_back(i);
		}
	}
	state.push_back(tt);//������״̬����Ϊ��̬�����̬���ϣ����������Ի����Ӽ� 
	state.push_back(acc); 
	tt.clear();
	for(int i=0;i<state.size();)//������л��ֵ�ÿ�������Ƿ���Խ�һ�����ֳ��Ӽ� 
	{
		vector<int> xx=state[i];
		if(xx.size() == 1)//���ֻ��һ��Ԫ�أ��Ͳ��ؿ��ǽ�һ������ 
		{
			i++;
			continue;
		}
		bool f=0;
		for(auto c:character)//�������е��ַ� 
		{
			map<int, set<int> >ttt;
			for(auto x:xx)//�Լ����ڵ�ÿ��Ԫ����c����Ƿ���ܻ��ֵ���ͬ�Ӽ� 
			{
				int y=a[x][c];//xʶ��c֮��y״̬ 
				bool f=0;
				for(int i=0;i<state.size();i++)//�������� 
				{
					if(find(state[i].begin(), state[i].end(), y) != state[i].end())//����ڸü����ҵ��� 
					{
						f=1;
						ttt[i].insert(x);//˵��y�����л��ֵ��Ӽ�i�� 
						break;
					}
				}
				if(!f) ttt[-1].insert(x);//�����û�ҵ���˵��Ӧ���ǿ��� 
			}
			if(ttt.size() > 1)//˵�����ٻ��ֳ������������ϵ����Ӽ� 
			{
				for(auto i:ttt)
				{
					tt.assign(i.second.begin(), i.second.end());//��ÿ���Ӽ���ȡ�������뵽state�� 
					state.push_back(tt);
					tt.clear();
				}
				state.erase(state.begin()+i);//�����Ӽ�ɾ��
				f=1;
				break; 
			}
		}
		if(f) i=0;//������ֳ����µ��Ӽ����ʹ�ͷ��=��ʼ���� 
		else i++;//�����������һ�����Ի��� 
	}
	for(int i=0;i<state.size();i++)//�����г�ʼ״̬�ļ��Ϸŵ��ͷ��λ�ã���֤���±�ŵ�ʱ�򣬳�ʼ״̬�ı��Ϊ0 
	{
		for(auto j:state[i])
		{
			if(j==0)
			{
				swap(state[i], state[0]);
			}
		}
	}
	map<int, int> mapp;
	for(int i=0;i<state.size();i++)//���±�� 
	{
		for(auto j:state[i])
		{
			mapp[j]=i;
		}
	}
	mapp[-1]=-1;
	aTemp.clear();
	for(int i=0;i<state.size();i++)//ȫ����ʼ��Ϊ-1��������״̬ 
	{
		for(auto j:character)
		{
			aTemp[i][j]=-1;
		}
	}
	for(int i=0;i<DFA1.size();i++)//����״̬ת�ƾ��� 
	{
		for(auto j:character)
		{
			aTemp[mapp[i]][j]=mapp[a[i][j]];
		}
	}
	a.clear();
	a=aTemp;
	int ans=find(state.begin(), state.end(), acc) - state.begin();//�ҵ��µ���̬��λ�ã����Ա�ֻ֤��һ��
	acc.clear();
	acc.push_back(ans); 
	printf("��̬�����%d\n",ans);
}
set<int> closure(int x)//������Ŀ�հ�
{
	set<int> ans;//��Ҫ���صıհ�������
	ans.insert(x);//��ǰ״̬���հ��õ��Ľ����������������
	bool f=1;//��Ǽ����Ƿ����仯
	while(f)//����б仯�ͼ����㣬֪�����ٷ����仯Ϊֹ
	{
		f=0;//����б仯�ͼ�Ϊ1
		for(auto x:ans)//�����������ÿһ��Ԫ��
		{
			if(a[x]['$'] != -1)//״̬x����������������ջ����ﵽ��״̬������closure(x)
			{
				if(ans.insert(a[x]['$']).second) f=1;//insert�ķ���ֵ��pair��first�ǲ����Ԫ�ص�λ�ã�second���Ƿ����ɹ�
			}
		}
	}
	return ans;
}
void go(int pos, char xx)//������Ŀת������GO ,�����õ���GO1���ݾ���ȷ�������DFA
{
	set<int> ans;
	for(auto x:DFA1[pos])//�����հ��е�ÿһ��Ԫ��x
	{
		if(a[x][xx] != -1)//���x�����xx�����״̬����error
		{
			set<int> temp=closure(a[x][xx]);//�����հ������뵽ans��
			ans.insert(temp.begin(), temp.end());//set�����Զ�ȥ��
		}
	}
	if(ans.empty()) return ;//����õ����ǿյģ���ôֱ�ӷ���
	int loc=find(DFA1.begin(), DFA1.end(), ans) - DFA1.begin();//find�����������е�λ�ã���ȥ��ʼλ�õõ����
	if(loc != DFA1.size())
	{
		GO1[make_pair(pos, xx)]=loc;
	}
	else
	{
		DFA1.push_back(ans);
		GO1[make_pair(pos, xx)] = DFA1.size()-1;
	}
}
void item()
{
	set<int> se=closure(0);//�ȼ����̬�ıհ�
	for(auto i:se)cout<<i<<" ";cout<<endl;
	DFA1.push_back(se);//���뵽��Ŀ���淶����
	for(int i=0; i<DFA1.size(); i++) //��ÿһ��closure(i)
	{
		for(auto x:character)
//		for(int x=0; x<maxn-2; x++) //���Լ���Ia,Ib,Ic������
		{
			if(x!='$') go(i,x);//$��ʾ���֣����Ա�����ʶ���DFA�ǲ�����$�ַ��ģ������I$�൱�ڶԱհ�����һ�αհ�
		}
	}
//	for(int i=0;i<DFA1.size();i++)
//	{
//		cout<<i<<endl;
//		for(auto j:DFA1[i])
//		{
//			cout<<j<<" ";
//		}
//		cout<<"\n";
//	}
//	for(auto i:GO1)
//	{
//		cout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
//	}
}
void init()
{
	int x;//״̬���
	set<int> se;//�洢���ж����״̬��set���Զ�ȥ�أ����Ա�������Ԫ���ظ��Ĵ���
	//���˱Ƚ�����״̬��������һ�У�û����״̬�ĸ�����
	while(scanf("%d%c",&x,&ch)==2)//��ѭ��������ȡ���н���״̬
	{
		se.insert(x);
		if(ch=='\n') break;
	}
	acc.assign(se.begin(), se.end());//��se�е�����Ԫ��ȫ���ŵ�acc�У�se�����
	while(scanf("%d%c",&x,&ch)==2)//��ѭ��������ȡ���н���״̬
	{
		se.insert(x);
		if(ch=='\n') break;
	}
	statu.assign(se.begin(), se.end());//��se�е�����Ԫ�ض��ŵ�statu��
	//��ʱstatu�е�������DFA������״̬��ţ������������
	for(auto i:statu)
		for(int j=0; j<maxn; j++)
			a[i][j]=-1; //��a����ȫ����ֵΪ-1����ʾ����״̬
	int l, r;
	string s;//�� l ״̬�� r ״̬��ʶ��һ�� s �õ���
	while(cin>>l>>s>>r)
	{
		if(r>=96 && r<=100) r=-1;//DFA��ֱ�ӳ������Ǳ���ԭ��ʵ���DFA������96��100��״̬��Ϊerror״̬
		if(s=="dig")//��������֣�����д10�У�����dig������1��10
			for(int i=0; i<10; i++)
				a[l]['0'+i]=r, character.insert('0'+i); //�� l ״̬ʶ����������ת�� r ״̬
		else if(s=="cha")//�����cha������cha����26����ĸ
			for(int i=0; i<26; i++)
				a[l]['a'+i]=r, a[l]['A'+i]=r, character.insert('a'+i), character.insert('A'+i);
		else if(s=="other")//������ʾ����û�н�������ַ�
			for(int i=0; i<maxn-2; i++)
			{
				if(i=='$')continue;
				character.insert(i);
				a[l][i]==-1 ? a[l][i]=r : 0; //˫Ŀ������������-1���͸�ֵΪr������ʲô������
			}

		else if(s=="space")//cinĬ�����޷����ո�ģ�ֻ����space����ո�
			a[l][' ']=r, character.insert(' ');
		else//��������ʱ�����ǻ���������ʽ����� 
		{
			int len=s.length(), pre=l;
			if(len>1)//������ȴ���1��˵�����ϲ��ǵ����ַ� 
			{
				for(int i=0;i<len-1;i++)//�Ǿ�һ�����𣬶Բ������ÿ���ַ����½�һ���ڵ� 
				{
					character.insert(s[i]);
					a[pre][s[i]]=Node;//�½�һ���ڵ� 
					pre=Node--; //�ڵ�ı��-1��Ϊ�˱�������б�ų�ͻ����Žڵ�Ӵ�С�ݼ����������ȸ�ֵ����Node--�� 
				}
			}
			character.insert(s[len-1]);
			a[pre][s[len-1]]=r;//����Ӧ�þ���һ��״̬ʶ��һ���ַ��ģ�Ϊ���������ļ���һ�㣬�������ַ���
		}
	}
	a[0]['\n']=0;//��ʦ���Ǹ�DFAû�п����������������ֻ����һ��
	item();
	a.clear();
	for(int i=0;i<DFA1.size();i++)//��a���Ȼ��ȫ����ʼ��Ϊ���󣬱���֮ǰ������Ӱ������ʹ�� 
		for(int j=0;j<maxn;j++)
			a[i][j]=-1;
	for(auto i:GO1)//GO�õ�����ȷ�������DFA��ȫ������a�У�����Ĵ���Ϳ��Բ����� 
	{
		a[i.first.first][i.first.second]=i.second;
	}
	vector<int> temp;//temp��ʱ����õ����µ���̬���� 
	for(int i=0;i<DFA1.size();i++)//����Ŀ�����ÿ��Ԫ�ؽ��б�����������ԭ������̬�����״̬��Ϊ�µ���̬ 
	{
		set<int> s=DFA1[i];
		for(auto x:s)
		{
			if(find(acc.begin(), acc.end(), x) - acc.begin() != acc.size())//����ԭ������̬ 
			{
				temp.push_back(i);
				break;//������һ����Ŀ���Ĳ��ң�����Ŀ���Ѿ�����Ҫ���� 
			}
		}
	}
	acc=temp;
	mini();
	if(character.find('$') != character.end())//����п��֣���ɾ����DFA�ﲻ���ܴ��ڿ��ֻ� 
	{
		character.erase('$');
	}
//	cout<<"\t";for(auto i:character) cout<<i<<"\t";cout<<endl;
//	for(auto i:a)
//	{
//		printf("״̬%2d��",i.first);
//		for(auto j:character)
//		{
//			printf("%d\t",i.second[j]);
//		}
//		printf("\n");
//	}
//	exit(0);
}
void g()//��һ���ַ�
{
	if(!duwanle)//���û�ж����ļ�β�����¶�
		if(fscanf(f,"%c",&ch)!=1)
			duwanle=1;//����ֵ������1˵�����ļ�β��EOF����-1��
//	if(!duwanle) cout<<"ch="<<ch<<endl;
}
bool words()//ʶ�𵥴�
{
	name="";
	if(prech=='$') g();//���ǰ����һ��δ������ַ�������Ҫ��ȡ��һ���ַ�
	else ch=prech, prech='$';
	int sta=0;
	while(a[sta][ch] != -1)//error״̬�˳�
	{
		if(duwanle) return 1;//�����˾Ͳ��ü�����������
		for(auto i:acc)//����������̬���
			if(a[sta][ch]==i)//��̬���˳�
				return 1;
		if(ch>=0&&ch<128) sta=a[sta][ch];//������һ��״̬
		else name+=ch;//��Ӣ���ַ���Ȼ�������޷��ɹ����������ַ��������롣����
		if(sta) name+=ch;//��0״̬����������һ����������ǰ�ļ��뵽name�У�����Ͳ�����
		g();
	}
	return 0;
}
bool find(string name)//����һ���ַ����Ƿ��ڱ����ֶ��ڣ����ж��Ƿ�ΪԤ�����ʶ������֪�����
{
	for(auto i:reserve)
	{
		if(name==i)
		{
			return 1;
		}
	}
	return 0;
}
node cifa()
{
	if(words())//��һ�����ʳ���
	{
		if(duwanle) return {"#","-"};//�����ˣ�����һ��#���������﷨���������Ԥ�����������ջ�ڳ�ʼ��#��Ӧ
		if(find(name+ch)==0)//���name+ch���Ǳ����ֶ�
		{
			if(find(name))//�ж�name�ǲ��Ǳ����ֶ�
			{
				prech=ch;
				return {name,"-"};
			}
			//���Ҳ���ǣ���ʲô������������ִ���ж�name���ֱ�
		}
		else//������if����else���֣���˵�����ص���һ��Ԥ�����ʶ��������������
		{
			return {name+ch,"-"};
		}
		if(name[0]>='0'&&name[0]<='9')//˵��������
		{
			prech=ch;
			return {"number",name};
		}
		else if((name[0]>='a'&&name[0]<='z')||(name[0]>='A'&&name[0]<='Z')||name[0]=='_')//�û��Զ����ʶ��
		{
			prech=ch;
			return {"defined",name};
		}
		//��ȻӦ�û��в������������ʱ�����ˡ��ʷ�������������������﷨�������򡣡������벻����ȫ��ƥ����ķ�
		//�Ժ�����Ȥ���ټӰ�
	}
	else
	{
		prech=ch;
		return {"error","-"};//����״̬
	}
}
int main()
{
	freopen("DFA.txt","r",stdin);//����������
	init();
	while(!duwanle)//���ص� 
	{
		node no=cifa();
		cout<<no.name<<" "<<no.value<<endl;
	}
	return 0;
}
