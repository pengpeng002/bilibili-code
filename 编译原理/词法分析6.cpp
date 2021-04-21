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
map<int, int[maxn]> a;//��ά���飬����ѡ����a[xx][130]
map<pair<int, char>, int>GO1;//pair<��Ŀ��ţ������ַ�>����Ŀ���  I״̬ʶ��һ��X֮�󵽴�J״̬
vector<set<int> >DFA1;//��Ŀ���淶��
int Node=2147483647;//int���ֵ���ٳ��������½��ڵ�ı�� 
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
		for(int x=0; x<maxn-2; x++) //���Լ���Ia,Ib,Ic������
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
				a[l]['0'+i]=r; //�� l ״̬ʶ����������ת�� r ״̬
		else if(s=="cha")//�����cha������cha����26����ĸ
			for(int i=0; i<26; i++)
				a[l]['a'+i]=r, a[l]['A'+i]=r;
		else if(s=="other")//������ʾ����û�н�������ַ�
			for(int i=0; i<maxn-2; i++)
			{
				if(i=='$')continue;
				a[l][i]==-1 ? a[l][i]=r : 0; //˫Ŀ������������-1���͸�ֵΪr������ʲô������
			}

		else if(s=="space")//cinĬ�����޷����ո�ģ�ֻ����space����ո�
			a[l][' ']=r;
		else//��������ʱ�����ǻ���������ʽ����� 
		{
			int len=s.length(), pre=l;
			if(len>1)//������ȴ���1��˵�����ϲ��ǵ����ַ� 
			{
				for(int i=0;i<len-1;i++)//�Ǿ�һ�����𣬶Բ������ÿ���ַ����½�һ���ڵ� 
				{
					a[pre][s[i]]=Node;//�½�һ���ڵ� 
					pre=Node--; //�ڵ�ı��-1��Ϊ�˱�������б�ų�ͻ����Žڵ�Ӵ�С�ݼ����������ȸ�ֵ����Node--�� 
				}
			}
			a[pre][s[len-1]]=r;//����Ӧ�þ���һ��״̬ʶ��һ���ַ��ģ�Ϊ���������ļ���һ�㣬�������ַ���
		}
	}
	a[0]['\n']=0;//��ʦ���Ǹ�DFAû�п����������������ֻ����һ��
	item();
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
int mainn()
{
	freopen("DFA.txt","r",stdin);//����������
	init();
	while(!duwanle)
	{
		node no=cifa();
		cout<<no.name<<" "<<no.value<<endl;
	}
	return 0;
}
