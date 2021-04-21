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
			for(int i=0; i<maxn; i++)
				a[l][i]==-1 ? a[l][i]=r : 0; //˫Ŀ������������-1���͸�ֵΪr������ʲô������
		else if(s=="space")//cinĬ�����޷����ո�ģ�ֻ����space����ո�
			a[l][' ']=r;
		else a[l][s[0]]=r;//����Ӧ�þ���һ��״̬ʶ��һ���ַ��ģ�Ϊ���������ļ���һ�㣬�������ַ���
	}
	a[0]['\n']=0;//��ʦ���Ǹ�DFAû�п����������������ֻ����һ�� 
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
