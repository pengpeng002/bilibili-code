#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <cstring>

#define MAX 100
using namespace std;
int flag = 1;

// ����ʽ�����ݽṹ
struct node
{
	char left; //��һ�����ս����ĸ
	string right; //�Ҳ�һ���ַ���
};

static int T;
static node Pro[MAX]; // ����ʽ

//set�������ǣ�����Ԫ�ض������Ԫ�صļ�ֵ�Զ�����
//vector ���������ͣ�����������������͵�����
static set<char> firstSet[MAX]; // First��
static set<char> followSet[MAX]; // Follow��
static vector<char> terminal; // �ս��
static vector<char> terminalN; // ȥ�յ��ս��
static vector<char> nonterminal; // ���ս��
static vector<char> analyticalS; // ����ջ
static vector<char> RemainE; // ʣ�����봮
static int forecastAnaly[100][100]; // Ԥ���

//�ж��Ƿ�Ϊ���ս��
bool IsNonterminal(char c)
{
	if (c >= 'A' && c <= 'Z')
		return true;
	return false;
}

// ��÷��ս�������е�Ԫ���±�
int GetindexN(char c)
{
	for (int i = 0; i < nonterminal.size(); i++)
	{
		if (c == nonterminal[i])
			return i;
	}
	return -1;
}

// ����ս�������е�Ԫ���±�
int Getindex(char c)
{
	for (int i = 0; i < terminalN.size(); i++)
	{
		if (c == terminalN[i])
			return i;
	}
	return -1;
}

// ����ս��X�ĵ�First��
void GFirstSet(char X)
{
	int countEmpty = 0; // ��������ж��Ҳ����ս����First���У��ǲ��Ƕ�����$
	int hasEmpty = 0;//First(X)�����Ƿ���$
	int Xindex = GetindexN(X);//���ս���ڷ��ս�����е�λ��
	for (int i = 0; i < T; i++)
	{
		// ƥ�����ʽ��
		if (Pro[i].left == X)
		{
			// �Ҳ���һ��������Ϊ�ս����ֱ�Ӽ���first��
			if (!IsNonterminal(Pro[i].right[0]))
			{
				firstSet[Xindex].insert(Pro[i].right[0]);
			}
			else
			{
				// ����ʽΪX->Y1..Yj..Yk��ʽ
				for (int j = 0; j < Pro[i].right.length(); j++)
				{
					char Yj = Pro[i].right[j];//YjΪ����ʽ�Ҳ��ķ��ţ�����Ϊ�ս��Ҳ����Ϊ���ս����

					// ��Yj���ս��(���ܲ���$),�����First(X)������������ѭ��
					if (!IsNonterminal(Yj))
					{
						firstSet[Xindex].insert(Yj);
						break;
					}

					// ��Yj�Ƿ��ս�����ݹ����First(Yj)
					GFirstSet(Yj);
					int YjIndex = GetindexN(Yj);//�ҵ�Yj�ڷ��ս�����е��±�

					//��Yj��First���еķǿյ��ս������First(X)��
					set<char>::iterator it;//����������
					for (it = firstSet[YjIndex].begin(); it != firstSet[YjIndex].end(); it++)
					{
						if (*it == 'e') // �����鿴First(Yj)���Ƿ���'$'(�ܲ�����)
							hasEmpty = 1;
						else
							firstSet[Xindex].insert(*it);//��FIRST(Yj)�еķ�$�ͼ���FIRST(X)
					}

					// Yj���ܲ�����, ��������
					if (hasEmpty == 0)
						break;
					//  Yj�ܲ�����,�������е������㣬���ۼӵ�countEmpty��
					else
					{
						countEmpty += hasEmpty;
						hasEmpty = 0;
					}
				}
				//�����Ҳ�first(Y)����$,��$����First(X)��
				if (countEmpty == Pro[i].right.length())
					firstSet[GetindexN(X)].insert('e');
			}
		}
	}
}

// ����ս��X�ĵ�Follow��
void GFollowSet(char X)
{
	int Xindex = GetindexN(X);//���ս���ڷ��ս�����е�λ��
	for (int i = 0; i < T; i++)
	{
		int index = -1;//indexΪ���ս��X�ڲ���ʽPro[i]���Ҳ���λ�ã���ʼ��Ϊ-1

		int len = Pro[i].right.length();//����ʽPro[i]���Ҳ��ĳ���

		// Ѱ��X�ڲ���ʽ�е�λ��index
		for (int j = 0; j < len; j++)
		{
			if (Pro[i].right[j] == X)
			{
				index = j;
				break;
			}
		}
		// �ҵ�X�ڲ���ʽ�е�λ��index
		// ����A->aBb, ��FIRST(b)�г��˿�$֮������з���FOLLOW(B)��
		// ����B��ӦX, b��Ӧnext0
		if (index != -1 && index < len - 1)
		{
			char next0 = Pro[i].right[index + 1];//X�Ҳ��ĵ�һ������

			// ��b���ս�� ��FIRST(b)=b��ֱ�Ӳ���B��Follow����
			if (!IsNonterminal(next0))
			{
				followSet[Xindex].insert(next0);
			}
			// ��b�Ƿ��ս��
			else
			{
				int hasEmpty = 0;//First(b)�����Ƿ���$
				set<char>::iterator it;
				int next0Index = GetindexN(next0);

				// ����FIRST(b)�г���$֮������е����з���
				for (it = firstSet[next0Index].begin(); it != firstSet[next0Index].end(); it++)
				{
					if (*it == 'e')
						hasEmpty = 1;
					else
						followSet[Xindex].insert(*it);
				}

				// ����A->aBb��FIRST(b)->$
				// FOLLOW(A)����FOLLOW(B)��
				if (hasEmpty && Pro[i].left != X)
				{
					GFollowSet(Pro[i].left);//�ݹ����A��Follow��

					//��A��Follow���е�Ԫ��ȫ���ӵ�B��Follow����
					char A = Pro[i].left;
					int AIndex = GetindexN(A);

					set<char>::iterator it;
					for (it = followSet[AIndex].begin(); it != followSet[AIndex].end(); it++)
						followSet[Xindex].insert(*it);
				}
			}
		}
		// ����A->aB ,FOLLOW(A)����FOLLOW(B)��,�������ϲ�����
		else if (index != -1 && index == len - 1 && X != Pro[i].left)
		{
			GFollowSet(Pro[i].left);//�ݹ����A��Follow��

			//��A��Follow���е�Ԫ��ȫ���ӵ�B��Follow����
			char A = Pro[i].left;
			int AIndex = GetindexN(A);

			set<char>::iterator it;
			for (it = followSet[AIndex].begin(); it != followSet[AIndex].end(); it++)
				followSet[Xindex].insert(*it);
		}
	}
}

// ��ʼ�����룬������������ս����First��Follow��
void FFInitialization()
{
	string s = "E->TA";//��Ϊ��ʱ�ַ������洢����ʽ
	int Ipro = 0;//��¼����ʽ����Ŀ
	//string filename = "resource.txt";//�����ļ���

	//ifstream in_file;
	//in_file.open(filename);//���ļ�
	//getline(in_file, s);  //���ļ��л�ȡ��'\n'Ϊ��β���ַ�����Ҳ���Ƕ���ÿһ������ʽ������s
	int line = 5;
	// �ֱ���ÿһ������ʽ
	while (line--)
	{
		//cout << s<<endl;
		string temp = ""; // �洢ȥ���ո�Ĳ���ʽ
		// ȥ������ʽ�е�' '
		for (int i = 0; i < s.length(); i++)
		{
			if (s[i] != ' ')
				temp += s[i];
		}

		Pro[Ipro].left = temp[0]; // ����ʽ����

		// ����ʽ���Ҳ�
		//����|��������
		//ȥ��X->,�ӵ��ĸ��ַ���ʼ
		for (int i = 3; i < temp.length(); i++)
		{
			//���������|������ʼ����һ���²���ʽ���˲���ʽ������ɲ���ʽ��ͬ
			if (temp[i] == '|')
			{
				Ipro++;
				Pro[Ipro].left = temp[0];
			}
			else
			{
				Pro[Ipro].right += temp[i];
			}
		}
		Ipro++;//�ǵõ���һ������

		// �洢�����ս���ͷ��ս��
		for (int i = 0; i < temp.length(); i++)
		{
			// ������������->��|
			if (i == 1 || i == 2 || temp[i] == '|')
				continue;
			//����һ�����ս��
			if (IsNonterminal(temp[i]))
			{
				int flag = 0;
				//�����ս��ջ�����޴洢�˷��ս������û�У�������ջ
				for (int j = 0; j < nonterminal.size(); j++)
				{
					if (nonterminal[j] == temp[i])
					{
						flag = 1;
						break;
					}
				}
				if (!flag)
					nonterminal.push_back(temp[i]);
			}
			//����һ���ս����ͬ��
			else
			{
				int flag = 0;
				for (int j = 0; j < terminal.size(); j++)
				{
					if (terminal[j] == temp[i])
					{
						flag = 1;
						break;
					}
				}
				if (!flag)
					terminal.push_back(temp[i]);
			}
		}
		//������һ��
		if (line == 4) s = "A->+TA|e";
		else if (line == 3) s = "T->FB";
		else if (line == 2) s = "B->*FB|e";
		else if (line == 1) s = "F->(E)|a";
		else break;
		//getline(in_file, s);
	}
	//in_file.close();             //������ �ر��ļ�

	terminal.push_back('#'); //����ֹ����ջ

	T = Ipro;//������ʽ������Ŀ�����������ɵģ���ֵ��T

	// �洢û��$���ŵ��ս��
	for (int i = 0; i < terminal.size(); i++)
	{
		if (terminal[i] != 'e')
			terminalN.push_back(terminal[i]);
	}
	// ���first��
	for (int i = 0; i < nonterminal.size(); i++)
	{
		GFirstSet(nonterminal[i]);
	}
	// ���follow��
	for (int i = 0; i < nonterminal.size(); i++)
	{
		if (i == 0) // ��ʼ����, �ȼ����������
			followSet[0].insert('#');
		GFollowSet(nonterminal[i]);
	}
}
//�õ�����Ԥ������
void GforecastAnaly()
{
	//cout << "���ʽ�ķ�Ԥ�������" << endl;

	// ����ÿ������ʽ(���index):A->��
	for (int index = 0; index < T; index++)
	{

		int row = GetindexN(Pro[index].left);//���ݲ���ʽ�����ս��ȷ���������к�
		int emptyCount = 0;

		// ��First(��)�е�ÿ���ս����a,��index����(A, a)��
		for (int i = 0; i < Pro[index].right.size(); i++)
		{
			char tmp = Pro[index].right[i];//��ʱ�������������ʽindex�ĵ�i����
			// ��tmp���ս��
			if (!IsNonterminal(tmp))
			{
				//��tmp��Ϊ�գ��������ʽ��ŷ��������Ĵ��ж�Ӧ�ķ��ս�������У�����index����(A,tmp)��
				if (tmp != 'e')
					forecastAnaly[row][Getindex(tmp)] = index;
				//��tmpΪ�գ������¼����
				if (tmp == 'e')
				{
					emptyCount++;
				}
				break;
			}
			// ��tmp�Ƿ��ս��
			else
			{
				int tmpIndex = GetindexN(tmp); //��ʱ����������˷��ս���ı��

				// ��First(tmp)�е�ÿ���ս����a,��index����(A, a)��
				set<char>::iterator it;
				for (it = firstSet[tmpIndex].begin(); it != firstSet[tmpIndex].end(); it++)
				{
					forecastAnaly[row][Getindex(*it)] = index;
				}
				// �������First(tmp)��,�����¼�����������������е���һ������
				if (firstSet[tmpIndex].count('e') != 0)
				{
					emptyCount++;
				}
				else
				{
					break;
				}
			}
		}

		//�������First(��)��,��Follow(A)�е�ÿ���ս���������b,��index����(A,b)��
		if (emptyCount == Pro[index].right.size())
		{
			set<char>::iterator it;
			for (it = followSet[row].begin(); it != followSet[row].end(); it++)
			{
				forecastAnaly[row][Getindex(*it)] = index;
			}
		}
	}
	//���������

	// ��ͷ���ս������
	/*for (int i = 0; i < terminalN.size(); i++)
	{
		//setw(x);//��ʹ�������
		cout << setw(15) << terminalN[i];
	}
	cout << endl;*/

	//������ս������Ԥ�⶯����
	/*for (int i = 0; i < nonterminal.size(); i++)
	{
		cout << nonterminal[i];
		for (int j = 0; j < terminalN.size(); j++)
		{
			if (forecastAnaly[i][j] == -1)
				cout << setw(15)<<" ";
			else
				cout << setw(15)<<"->" << Pro[forecastAnaly[i][j]].right;
		}
		cout << endl;
	}
	cout << endl;*/
}
//�����봮���з���
void AnalyticalProcess()
{
	int m = 1;//��¼������Ŀ

	string s;
	//cout << "��������������Ŵ���" << endl;
	cin >> s;
	if(s=="E")
	{
		flag=0;
		return ;
	}
	//cout << "����ȫ���̣�" << endl;
	//�����ͷ
	//cout << "����"<<setw(30) << "����ջ" << setw(30) << "ʣ�����봮" << setw(30) << "�Ƶ����ò���ʽ��ƥ��" << endl;
	//ʣ�����봮ջ������ӷ��Ŵ�������ӽ�������Ϊ���������
	for (int i = 0; i <s.size() ; i++)
		RemainE.push_back(s[i]);
	RemainE.push_back('#');
	//����ջ������ӽ�����������ӿ�ʼ����
	analyticalS.push_back('#');
	analyticalS.push_back(nonterminal[0]); // ���뿪ʼ����
	while (analyticalS.size() > 0)
	{
		//cout << m;
		m++;
		//���������ջ�ĵ�ǰ״��
		string outs = "";
		for (int i = 0; i < analyticalS.size(); i++)
			outs += analyticalS[i];
		//cout << setw(30) << outs;
		//�����ʣ�����ջ�ĵ�ǰ״��
		outs = "";
		for (int i = 0; i < RemainE.size(); i++)
			outs += RemainE[i];
		//cout << setw(30) << outs;
		// ������Ƶ����̣����Է���ջ��ʣ�����ջ���д���
		char char1 = analyticalS.back();
		char char2 = RemainE.front();
		if (char1 == char2 && char1 == '#')
		{
			//cout << setw(30) << "����" << endl;
			return;
		}
		//������ջ��ʣ�����ջ��ʼ���ս����ȣ���ֱ��ջ
		if (char1 == char2)
		{
			analyticalS.pop_back();
			RemainE.erase(RemainE.begin());
			//cout << "Syntax analysis is right" << endl;
			//cout << setw(30) <<"\""<<char1<<"\"ƥ��"<<endl;
		}
		// Ԥ��������Ƶ���ɽ����Ƶ�
		else if (forecastAnaly[GetindexN(char1)][Getindex(char2)] != -1)
		{
			int tg = forecastAnaly[GetindexN(char1)][Getindex(char2)];
			analyticalS.pop_back();
			//������ʽ��Ϊ�գ���������Ƶ�����������ջ
			if (Pro[tg].right != "e")
			{
				for (int i = Pro[tg].right.length() - 1; i >= 0; i--)
					analyticalS.push_back(Pro[tg].right[i]);
			}
			//cout << setw(30) << Pro[tg].left << "->" << Pro[tg].right << endl;
		}
		// ���������Ӧ��Ϊ�գ�����ִ���
		else
		{
			flag = 0;
			//cout << "Error on syntax analysis" << endl;
			return;
		}
	}
}
int main()
{
	memset(forecastAnaly, -1, sizeof(forecastAnaly));//��ʼ������Ԥ�������ʼ��ÿ��ֵΪ-1
	FFInitialization();
	//DFirstAFollow();
	GforecastAnaly();
	while(1)
	{
		AnalyticalProcess();
		if(flag==0) cout << "Error on syntax analysis" << endl;
		else cout << "Syntax analysis is right" << endl;
		flag=1;
		RemainE.clear();
		analyticalS.clear();
	}
	system("pause");
	return 0;
}
