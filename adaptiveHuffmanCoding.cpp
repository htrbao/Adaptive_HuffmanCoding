#include <bits/stdc++.h>

using namespace std;

string str, ans = "";

struct node {
	long c;
	long amount;
	long order;
	bool isNYT; //not yet transfer
	node* left, * right, *parent;

	node(long val, long am, long ord, node* l, node* r, node* p)
	{
		c = val;
		amount = am;
		order = ord;
		left = l;
		right = r;
		parent = p;
	}
};

node* HuffTree = new node(-1, 0, 512, nullptr, nullptr, nullptr), *NYTnode = HuffTree;

node* getSymbolNode(long symbol, node* crr)
{
	if(crr == nullptr || crr->c == symbol)
		return crr;

	node* leftResult = getSymbolNode(symbol, crr->left);

	return (leftResult == nullptr ? getSymbolNode(symbol, crr->right) : leftResult);
}

void preOrder(node* n)
{
	if(!n) return;

	preOrder(n->left);
	cout << char(n->c + 'a') <<'-' <<n->order <<'-' <<n->amount <<' ';
	preOrder(n->right);
}

string getPathSymbolNode(node* crr, node* result, string curPath)
{
	if(crr == result) 
		return curPath;

	if(!crr)
		return "";

	string left = getPathSymbolNode(crr->left, result, curPath + "0");
	return (left == "" ? getPathSymbolNode(crr->right, result, curPath + "1") : left);
}

void findBlockLeader(node* crr, node*& crrMax)
{
	if(!crr || crrMax == HuffTree)
		return;
	if(crr->order < crrMax->order) return;
	if(crr->amount == crrMax->amount && crr != crrMax->parent && crr->order > crrMax->order)
		crrMax = crr;

	findBlockLeader(crr->left, crrMax);
	findBlockLeader(crr->right, crrMax);
}

void swapNode(node* node1, node* node2)
{
	if(!node1->parent || !node2->parent)
		return;
	if(node1->parent == node2 || node2->parent == node1)
		return;

	node *&firstRef = node1->parent->left == node1 ? node1->parent->left : node1->parent->right;
	node *&secondRef = node2->parent->left == node2 ? node2->parent->left : node2->parent->right;

	swap(firstRef, secondRef);
	swap(firstRef->parent, secondRef->parent);
	swap(firstRef->order, secondRef->order);
}

void slideAndIncrement(node* crr)
{
	if(!crr) return;

	node* blockLeader = crr;
	findBlockLeader(HuffTree, blockLeader);
	if(blockLeader != crr)
	{
		swapNode(blockLeader, crr);
	}

	crr->amount += 1;
	slideAndIncrement(crr->parent);
}

void updateTree(long symbol)
{
	node* symbolNode = getSymbolNode(symbol, HuffTree);

	if(!symbolNode)
	{
		NYTnode->isNYT = false;
		string result = getPathSymbolNode(HuffTree, symbolNode, "");
		long minOrder = NYTnode->order;

		NYTnode->left = new node(-1, 0, minOrder - 2, nullptr, nullptr, NYTnode);
		NYTnode->left->isNYT = true;

		NYTnode->right = new node(symbol, 0, minOrder - 1, nullptr, nullptr, NYTnode);
		NYTnode->isNYT = false;

		symbolNode = NYTnode->right;
		NYTnode = NYTnode->left;
	}

	slideAndIncrement(symbolNode);
}

string encode(long symbol)
{
	node* symbolNode = getSymbolNode(symbol, HuffTree);
	if (symbolNode != nullptr)
	{
		string result = getPathSymbolNode(HuffTree, symbolNode, "");
		updateTree(symbol);
		return result;
	}

	long num = 5 - (symbol + 1 <= 20 ? 0 : 1);
	long symbol1 = symbol - (symbol + 1 <= 20 ? 0 : 10);

	stringstream ss;
	ss << getPathSymbolNode(HuffTree, NYTnode, "");
	if(num == 5) ss << bitset<5>(symbol1);
	else ss << bitset<4>(symbol1);

	updateTree(symbol);
	return ss.str();
}

long binToInt(string bin)
{
	long a = 0;
	for(long i = 0; i < bin.size(); i++)
	{
		//cout << i;
		//cout << bin;
		a += pow(2, bin.size() - i - 1) * (bin[i] == '0' ? 0 : 1);
	}
	return a;
}

string decode(string s)
{
	string res = "";
	for(long i = 0; i < s.size(); i++)
	{
		long result = -1;
		node* crr = HuffTree;
		while(result == -1)
		{
			if(crr->c != -1)
			{
				i--;
				result = crr->c;
				updateTree(result);
			}
			else if(crr->isNYT)
			{
				string tmp = "";
				long k = 3;
				tmp += s[i];
				while(k--)
				{
					tmp += s[++i];
				}

				result = binToInt(tmp);

				if(result < 10) {
					tmp += s[++i];
					result = binToInt(tmp);
				}
				else result += 10;

				updateTree(result);

			}
			else if(s[i] == '0')
			{
				i++;
				crr = crr->left;
			}
			else 
			{
				i++;
				crr = crr->right;		
			}
		}
		res += char(result + long('a'));
	}
	return res;
}

void deleteTree(node*& cur)
{
	if(!cur) return;

	deleteTree(cur->left);
	deleteTree(cur->right);

	delete cur;
	cur = nullptr;
}

int main()
{
	getline(cin, str);
	for(long i = 0; i < str.size(); i++)
	{
		ans += encode(str[i] - 'a');
		//cout << encode(str[i] - 'a');
	}
	cout << ans << endl;
	deleteTree(HuffTree);
	//for(long i = 0; i < ans.size(); i++)
	//	cout << i % 10;
	HuffTree = new node(-1, 0, 512, nullptr, nullptr, nullptr);
	HuffTree->isNYT = true;
	NYTnode = HuffTree;

	cout << decode(ans);

	deleteTree(HuffTree);
	return 0;
}