#include <bits/stdc++.h>
using namespace std;
//struct node
//{
//	int id;
//	string value;
//};
set<pair<int, string> > se;
//node ans[1005];
int tail=0;
string reserve[]= {"short","int","bool","long","float","double","char","static","for","while","do","continue","break",\
                   "return","void","switch","case","auto","goto","const"
                  };
char ch, prech='$';
string name;
bool digit()
{
	return ch>='0'&&ch<='9' ? 1 : 0;
}
bool cha()
{
	return (ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z') ? 1 : 0;
}
void in(int op,int x)
{
	if(op==0) name+=ch;
	else if(op==1) prech=ch;
//	ans[tail++]= {x,"-1"};
	se.insert(make_pair(x,"-1"));
//	cout<<name<<" "<<name<<endl;
}
void g()
{
	if(scanf("%c",&ch)==1) return;
	cout<<se.size()<<endl;
	for(auto i:se)
	{
		cout<<i.first<<" "<<i.second<<endl;
	}
	cout<<"input end!!!\n";
	exit(0);
}
void cg()
{
	name+=ch;
	g();
}
void kexue()
{
	if(ch=='+'||ch=='-') cg();
	if(digit())
	{
		while(digit())
		{
			cg();
		}
//		in(1,102);
		se.insert(make_pair(102,name));
//		cout<<name<<" 科学计数法\n";
	}
	else
	{
		cout<<ch<<" error15\n";
	}
}
void number()
{
	while(digit())//读取数字
	{
		cg();
	}
	if(ch=='.')//小数点
	{
		cg();
		if(digit())
		{
			while(digit())
			{
				cg();
			}
			if(ch=='E'||ch=='e')
			{
				cg();
				kexue();
			}
			else
			{
//				in(1,101);
//				name+=ch;
				prech=ch;
				se.insert(make_pair(101,name));
//				cout<<name<<" 小数\n";
			}
		}
		else
		{
			cout<<ch<<"error14\n";
		}
	}
	else if(ch=='E'||ch=='e')//科学计数法
	{
		cg();
		kexue();
	}
	else
	{
//		in(1,100);
//		name+=ch;
		prech=ch;
		se.insert(make_pair(100,name));
//		cout<<name<<" 整数\n";
	}
}
void words()
{
	name="";
	if(prech=='$') g();
	else ch=prech, prech='$';
	while(ch==' '||ch=='\n'||ch=='	') g();
	if(cha()||ch=='_')
	{
		while(cha()||digit()||ch=='_') cg();
		prech=ch;
		int id=0;
		for(auto s:reserve)
		{
			id++;
			if(s==name)
			{
//				ans[tail++]= {id,"-1"};
				se.insert(make_pair(id,"-1"));
//				cout<<name<<" 保留字\n";
				id=0;
				break;
			}
		}
		if(id)
		{
//			ans[tail++]= {id,name};
			se.insert(make_pair(id,name));
//			cout<<name<<" 自定义标识符\n";
		}
	}
	else if(ch=='0')//特判第一个为0
	{
		name+=ch;
		g();
		if(ch=='x'||ch=='X')//十六进制
		{
			cg();
			if(digit()||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='F'))
			{
				while(digit()||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='F'))
				{
					cg();
				}
//				in(1,103);
				prech=ch;
				se.insert(make_pair(103,name));
//				cout<<name<<" 十六进制\n";
			}
			else
			{
				cout<<ch<<" error13!\n";
			}
		}
		else if(digit())
		{
			number();
		}
	}
	else if(digit())//数字
	{
		number();
	}
	else if(ch=='+')
	{
		cg();
		if(ch=='+') in(0,31);//++
		else if(ch=='=') in(0,32);//+=
		else in(1,33);//+
	}
	else if(ch=='-')
	{
		cg();
		if(ch=='-') in(0,34);//--
		else if(ch=='=') in(0,35); //-=
		else in(1,36);//-
	}
	else if(ch=='*')
	{
		cg();
		if(ch=='*') in(0,37);//乘方，C不支持**
		else if(ch=='=') in(0,38); //*=
		else in(1,39);//*
	}
	else if(ch=='/')
	{
		cg();
		if(ch=='/')// //注释
		{
			while(ch!='\n') cg();
//			cout<<name<<" 注释\n";
		}
		else if(ch=='=') in(0,40); // /=
		else in(1,41); // /
	}
	else if(ch=='&')
	{
		cg();
		if(ch=='&') in(0,42); //&&
		else in(1,43); //&
	}
	else if(ch=='|')
	{
		cg();
		if(ch=='|') in(0,72);
		else in(1,73);
	}
	else if(ch=='^')
	{
		cg();
		if(ch=='=') in(0,44); //^=
		else
		{
			in(1,45); //^
		}
	}
	else if(ch=='%')
	{
		cg();
		if(ch=='=') in(0,46);//%=
		else in(1,47);//%
	}
	else if(ch=='#') //导入？
	{
		in(0,48);
	}
	else if(ch=='!')
	{
		cg();
		if(ch=='=') in(0,49);//!=
		else in(1,50);//!
	}
	else if(ch=='~')
	{
		in(0,51);
	}
	else if(ch=='[')
	{
		in(0,52);
	}
	else if(ch==']')
	{
		in(0,53);
	}
	else if(ch=='(') in(0,54);
	else if(ch==')') in(0,55);
	else if(ch=='{') in(0,56);
	else if(ch=='}') in(0,57);
	else if(ch=='=')
	{
		cg();
		if(ch=='=') in(0,58);
		else in(1,59);
	}
	else if(ch==',') in(0,60);
	else if(ch==';') in(0,61);
	else if(ch=='<')
	{
		cg();
		if(ch=='<') in(0,62);
		else if(ch=='=') in(0,63);
		else in(1,64);
	}
	else if(ch=='>')
	{
		cg();
		if(ch=='>') in(0,65);
		else if(ch=='=') in(0,66);
		else in(1,67);
	}
	else if(ch=='?') in(0,68);
	else if(ch==':') in(0,69);
	else if(ch=='\'')
	{
		cg();
		while(ch!='\'')cg();
		in(0,70);
//		if(ch=='\'')
//		{
//			in(0,70);
//		}
//		else
//		{
//			cout<<ch<<" error12\n";
//		}
	}
	else if(ch=='"')
	{
		cg();
		while(ch!='"') cg();
		in(0,71);
	}
	else
	{
		cout<<ch<<" error11\n";
	}
}
int main()
{
	freopen("program.txt","r",stdin);
	while(1)
	{
		words();
	}
	return 0;
}
