#include <iostream>
#include <ctime>
#include <string>
#include <fstream>

using namespace std;
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "input.txt"
int counter = 0;

class elem {
public:
	int key;
	string data;
};

class BTreeNode
{
	elem *elems;
	int t;
	BTreeNode **C; 
	int n;      
	bool leaf; 

public:
	static string buff;
	BTreeNode(int _t, bool _leaf); 

	void traverse();

	void singleTraverse() {
		cout << "{";
		buff += "{";
		for (int i = 0; i < n; i++) {
			if ((n - i) == 1) {
				buff += to_string(elems[i].key);
				cout << elems[i].key;
			}
			else {
				cout << elems[i].key << ", ";
				buff += to_string(elems[i].key) + ", ";
			}
		}
		cout << "} ";
		buff += "} ";
	}

	string interpolationSearch(int k);

	BTreeNode* edit(int, string);

	int findKey(int k);

	void insertNonFull(int k, string base);

	void splitChild(int i, BTreeNode *y);

	void remove(int k);

	void removeFromLeaf(int idx);

	void removeFromNonLeaf(int idx);

	int getPred(int idx);

	int getSucc(int idx);

	void fill(int idx);

	void borrowFromPrev(int idx);

	void borrowFromNext(int idx);

	void merge(int idx);

	friend class BTree;
};

string BTreeNode::buff = "";

class BTree
{
	BTreeNode *root;
	int t;
public:
	
	BTree(int _t)
	{
		root = NULL;
		t = _t;
	}

	string getBuff() { return BTreeNode::buff; }
	BTreeNode* getRoot() { return root; }

	void traverse()
	{
		if (BTreeNode::buff != "") {
			BTreeNode::buff = "";
		}
		if (root != NULL) root->traverse();
	}

	string search(int k)
	{
		return (root == NULL) ? NULL : root->interpolationSearch(k);
	}

	BTreeNode* edit(int k, string info) 
	{
		return (root == NULL) ? NULL : root->edit(k, info);
	}

	void insert(int k, string base);

	void remove(int k);

};

BTreeNode::BTreeNode(int t1, bool leaf1)
{
	t = t1;
	leaf = leaf1;

	elems = new elem[2 * t - 1];
	C = new BTreeNode *[2 * t];

	n = 0;
}

int BTreeNode::findKey(int k)
{
	int idx = 0;
	while (idx < n && elems[idx].key < k)
		++idx;
	return idx;
}

void BTreeNode::remove(int k)
{
	int idx = findKey(k);

	if (idx < n && elems[idx].key == k)
	{
		if (leaf)
			removeFromLeaf(idx);
		else
			removeFromNonLeaf(idx);
	}
	else
	{
		if (leaf)
		{
			cout << "The key " << k << " is does not exist in the tree\n";
			return;
		}

		bool flag = ((idx == n) ? true : false);

		if (C[idx]->n < t)
			fill(idx);

		if (flag && idx > n)
			C[idx - 1]->remove(k);
		else
			C[idx]->remove(k);
	}
}

void BTreeNode::removeFromLeaf(int idx)
{
	for (int i = idx + 1; i < n; ++i) {
		elems[i - 1].key = elems[i].key;
		elems[i - 1].data = elems[i].data;
	}
	n--;
}

void BTreeNode::removeFromNonLeaf(int idx)
{
	int k = elems[idx].key;

	if (C[idx]->n >= t)
	{
		int pred = getPred(idx);
		elems[idx].key = pred;
		C[idx]->remove(pred);
	}
	else if (C[idx + 1]->n >= t)
	{
		int succ = getSucc(idx);
		elems[idx].key = succ;
		C[idx + 1]->remove(succ);
	}
	else
	{
		merge(idx);
		C[idx]->remove(k);
	}
}

int BTreeNode::getPred(int idx)
{
	BTreeNode *cur = C[idx];
	while (!cur->leaf)
		cur = cur->C[cur->n];

	return cur->elems[cur->n - 1].key;
}

int BTreeNode::getSucc(int idx)
{

	BTreeNode *cur = C[idx + 1];
	while (!cur->leaf)
		cur = cur->C[0];

	return cur->elems[0].key;
}

void BTreeNode::fill(int idx)
{

	if (idx != 0 && C[idx - 1]->n >= t)
		borrowFromPrev(idx);

	else if (idx != n && C[idx + 1]->n >= t)
		borrowFromNext(idx);

	else
	{
		if (idx != n)
			merge(idx);
		else
			merge(idx - 1);
	}
}

void BTreeNode::borrowFromPrev(int idx)
{

	BTreeNode *child = C[idx];
	BTreeNode *sibling = C[idx - 1];

	for (int i = child->n - 1; i >= 0; --i) {
		child->elems[i + 1].data = child->elems[i].key;
		child->elems[i + 1].key = child->elems[i].key;
	}

	if (!child->leaf)
	{
		for (int i = child->n; i >= 0; --i)
			child->C[i + 1] = child->C[i];
	}

	child->elems[0].key = elems[idx - 1].key;
	child->elems[0].data = elems[idx - 1].data;

	if (!child->leaf)
		child->C[0] = sibling->C[sibling->n];

	elems[idx - 1].key = sibling->elems[sibling->n - 1].key;
	elems[idx - 1].data = sibling->elems[sibling->n - 1].data;

	child->n += 1;
	sibling->n -= 1;
}

void BTreeNode::borrowFromNext(int idx)
{

	BTreeNode *child = C[idx];
	BTreeNode *sibling = C[idx + 1];

	child->elems[(child->n)].key = elems[idx].key;
	child->elems[(child->n)].data = elems[idx].data;

	if (!(child->leaf))
		child->C[(child->n) + 1] = sibling->C[0];

	elems[idx].key = sibling->elems[0].key;
	elems[idx].data = sibling->elems[0].data;

	for (int i = 1; i < sibling->n; ++i) {
		sibling->elems[i - 1].data = sibling->elems[i].data;
		sibling->elems[i - 1].key = sibling->elems[i].key;
	}

	if (!sibling->leaf)
	{
		for (int i = 1; i <= sibling->n; ++i)
			sibling->C[i - 1] = sibling->C[i];
	}

	child->n += 1;
	sibling->n -= 1;
}

void BTreeNode::merge(int idx)
{
	BTreeNode *child = C[idx];
	BTreeNode *sibling = C[idx + 1];

	child->elems[t - 1].key = elems[idx].key;
	child->elems[t - 1].data = elems[idx].data;

	for (int i = 0; i < sibling->n; ++i){
		child->elems[i + t].data = sibling->elems[i].data;
		child->elems[i + t].key = sibling->elems[i].key;
	}
	if (!child->leaf)
	{
		for (int i = 0; i <= sibling->n; ++i)
			child->C[i + t] = sibling->C[i];
	}

	for (int i = idx + 1; i < n; ++i) {
		elems[i - 1].data = elems[i].data;
		elems[i - 1].key = elems[i].key;
	}

	for (int i = idx + 2; i <= n; ++i)
		C[i - 1] = C[i];

	child->n += sibling->n + 1;
	n--;

	delete(sibling);
}

void BTree::insert(int k, string base)
{
	if (root == NULL)
	{
		root = new BTreeNode(t, true);
		root->elems[0].key = k;
		root->elems[0].data = base;
		root->n = 1; 
	}
	else 
	{ 
		if (root->n == 2 * t - 1)
		{ 
			BTreeNode *s = new BTreeNode(t, false);
 
			s->C[0] = root;
 
			s->splitChild(0, root);
 
			int i = 0;
			if (s->elems[0].key < k)
				i++;
			s->C[i]->insertNonFull(k, base);
 
			root = s;
		}
		else   
			root->insertNonFull(k, base);
	}
}

void BTreeNode::insertNonFull(int k, string base)
{
	int i = n - 1;
	if (leaf == true)
	{
		while (i >= 0 && elems[i].key > k)
		{
			elems[i + 1].key = elems[i].key;
			elems[i + 1].data = elems[i].data;
			i--;
		}

		elems[i + 1].key = k;
		elems[i + 1].data = base;
		n = n + 1;
	}
	else 
	{
		while (i >= 0 && elems[i].key > k)
			i--;

		if (C[i + 1]->n == 2 * t - 1)
		{
			splitChild(i + 1, C[i + 1]);

			if (elems[i + 1].key < k)
				i++;
		}
		C[i + 1]->insertNonFull(k, base);
	}
}

void BTreeNode::splitChild(int i, BTreeNode *y)
{
	BTreeNode *z = new BTreeNode(y->t, y->leaf);
	z->n = t - 1;

	for (int j = 0; j < t - 1; j++) {
		z->elems[j].key = y->elems[j + t].key;
		z->elems[j].data = y->elems[j + t].data;
	}
	if (y->leaf == false)
	{
		for (int j = 0; j < t; j++)
			z->C[j] = y->C[j + t];
	}

	y->n = t - 1;

	for (int j = n; j >= i + 1; j--)
		C[j + 1] = C[j];

	C[i + 1] = z;

	for (int j = n - 1; j >= i; j--) {
		elems[j + 1].key = elems[j].key;
		elems[j + 1].data = elems[j].data;
	}
	elems[i].key = y->elems[t - 1].key;
	elems[i].data = y->elems[t - 1].data;

	n = n + 1;
}

void BTreeNode::traverse()
{
	singleTraverse();
	int i;
	for (i = 0; i < n; i++) {
		if(!leaf)
			C[i]->traverse();
	}

	if (!leaf)
		C[i]->traverse();
}

string BTreeNode::interpolationSearch(int k)
{
	int begin = 0;
	int end = n - 1;
	int mid;
	if (!leaf) {
		counter++;
		if (k < elems[begin].key){
			return C[begin]->interpolationSearch(k);
		}
		counter++;
		if (k > elems[end].key) {
			return C[end + 1]->interpolationSearch(k);
		}
	}
	while (true)
	{	
		counter++;
		if (k > elems[end].key) return "Key wasn't found.";		
		counter++;
		if (k < elems[begin].key) return "Key wasn't found.";
		double temp = (k - elems[begin].key);
		double temp_1 = (elems[end].key - elems[begin].key);
		double temp_2 = temp / temp_1;
		mid = begin + temp_2 * (end - begin);
		counter++;
		if (elems[mid].key == k) {
			return elems[mid].data;
		}
		counter++;
		if (mid < end && k > elems[mid].key && k < elems[mid + 1].key && !leaf) {
				counter++;
				return C[mid + 1]->interpolationSearch(k);
		}
		counter++;
		if (mid > begin && k > elems[mid - 1].key && k < elems[mid].key && !leaf) {
				counter++;
				return C[mid]->interpolationSearch(k);
		}
		counter++;
		if (elems[begin].key == k) {
			counter++;
			return elems[begin].data;
		}
		counter++;
		if (elems[end].key == k) {
			counter++;
			return elems[end].data;
		}
		counter++;
		if (k > elems[mid].key) {
			counter++;
			begin = mid + 1;
			continue;
		}
		counter++;
		if (k < elems[mid].key) {
			counter++;
			end = mid - 1;
			continue;
		}
		return "Key wasn't found.";
	}
}

void BTree::remove(int k)
{
	if (!root)
	{
		cout << "The tree is empty\n";
		return;
	}

	root->remove(k);

	if (root->n == 0)
	{
		BTreeNode *tmp = root;
		if (root->leaf)
			root = NULL;
		else
			root = root->C[0];

		delete tmp;
	}
	return;
}

BTreeNode* BTreeNode::edit(int k, string info) {
	int i = 0;
	while (i < n && k > elems[i].key)
		i++;

	if (elems[i].key == k) {
		elems[i].data = info;
		return this;
	}
	if (leaf == true)
		return NULL;
 
	return C[i]->edit(k, info);
}

void fillingArray(int *Array, int size) {
	srand(time(NULL));
	for (int i = 0; i < size; i++) {
		bool flag = true;
		int newRandom = rand() % (size * 2) + 1;
		for (int j = 0; j < i; j++) {
			if (newRandom == Array[j]) {
				flag = false;
			}
		}
		if (flag) {
			Array[i] = newRandom;
		}
		else { i--; }
	}
}

void fillingData(string *Array, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 5; j++) {
			char temp = rand() % 24 + 65;
			Array[i] += temp;
		}
	}
}

void showData(string *Data, int *Array, int size) {
	for (int i = 0; i < size; i++) {
		cout << Array[i] << "-" << Data[i] << " ";
	}
	cout << endl;
}

istream& openfileINPUT(ifstream& fin, string name) {
	fin.open(name);
	if (!fin.is_open()) {
		cout << "Error!" << endl;
		system("pause");
		exit(1);
	}
	cout << "File was opened." << endl;
	return fin;
}

int readFileParametr(string buff) {
	string temp;
	while(true) {
		if (!isdigit(buff[0])) break;
		temp += buff[0];
		buff.erase(0, 1);
	}
	return stoi(temp);
}

void parseInfo(BTree &tree, string buff) {
	string key, data;
	int count = 0;
	for (int i = 0; i < buff.length(); i++) {
		if (buff[i] == 'i') {
			for (int j = i + 2; ; j++) {
				count++;
				if (buff[j] == ',') break;
				key += buff[j];
			}
			for (int j = i + count + 2; ; j++) {
				if (buff[j] == ')') {
					count = 0;
					break;
				}
				data += buff[j];
			}
			tree.insert(stoi(key), data);
			key = "";
			data = "";
		}
		if (buff[i] == 'r') {
			for (int j = i + 2; ; j++) {
				if (buff[j] == ')') break;
				key += buff[j];
			}
			tree.remove(stoi(key));
			key = "";
		}
		if (buff[i] == 'e') {
			for (int j = i + 2; ; j++) {
				count++;
				if (buff[j] == ',') break;
				key += buff[j];
			}
			for (int j = i + count + 2; ; j++) {
				if (buff[j] == ')') {
					count = 0;
					break;
				}
				data += buff[j];
			}
			tree.edit(stoi(key), data);
			key = "";
			data = "";
		}
	}
}

ostream& FillDeleteSearch(ofstream &fout, BTree &tree, int num_3) {
	int number_1;
	string num;
	if (num_3 != 1) {
		cout << "If you wanna fill our tree from random numbers press 1, if manually press another num: ";
		cin >> number_1;

		if (number_1 == 1) {
			int size;
			cout << "Input size of array: "; cin >> size;
			int* Array = new int[size];
			string* Data = new string[size];
			fillingArray(Array, size);
			fillingData(Data, size);
			showData(Data, Array, size);
			for (int i = 0; i < size; i++) {
				tree.insert(Array[i], Data[i]);
				fout << "i(" << Array[i] << "," << Data[i] << ")";
			}
			tree.traverse();
			cout << endl;
			delete[] Array;
			delete[] Data;
		}
		else {
			string temp_1 = "";
			while (true) {
				string key;
				string data;
				cout << "Input your number and data (if u wanna exit type quit): "; cin >> key >> data;
				if (data == "quit") break;
				fout << "i(" << key << "," << data << ")";
				tree.insert(stoi(key), data);
				tree.traverse();
				cout << endl;
			}
		}
		cout << endl;
	}
	while (true) {
		string temp_, information;
		cout << "If you wanna add element type key (if you wanna stop type 'quit'): "; cin >> temp_;
		if (temp_ == "quit") break;
		cout << "Type information for this key: "; cin >> information;
		fout << "i(" << temp_ << "," << information << ")";
		tree.insert(stoi(temp_), information);
	}
	while (true) {
		cout << "Input key which you wanna delete (if you wanna stop type quit): ";
		cin >> num;
		if (num == "quit") { break; }
		fout << "r(" << num << ")";
		tree.remove(stoi(num));
	}
	tree.traverse();
	cout << endl;
	while (true) {
		string info = "";
		cout << "Input key which you wanna edit (if you wanna stop type quit): "; cin >> num;
		if (num == "quit") { break; }
		cout << "Input new info for this key: "; cin >> info;
		fout << "e(" << num << "," << info << ")";
		tree.edit(stoi(num), info);
	}
	while (true) {
		cout << "Input key which you wanna find (if you wanna stop type quit): ";
		cin >> num;
		if (num == "quit") { break; }
		cout << tree.search(stoi(num)) << endl;
		cout << "Comparisons: " << counter << endl;
		counter = 0;
	}
	tree.traverse();
	cout << endl;
	return fout;
}

int main() {
	ofstream fout;
	int number_2;
	cout << "If you wanna restore the last B-tree type 1, if no - any number: "; cin >> number_2;
	if (number_2 == 1) {
		ifstream fin;
		string temp_2;
		fout.open(OUTPUT_FILE, ofstream::app);
		openfileINPUT(fin, INPUT_FILE);
		getline(fin, temp_2);
		BTree tree(readFileParametr(temp_2));
		parseInfo(tree, temp_2);
		tree.traverse();
		cout << endl;
		FillDeleteSearch(fout, tree, number_2);
		fout.close();
		fin.close();
	}
	else {
		fout.open(OUTPUT_FILE);
		int t;
		cout << "Input parametr t: "; cin >> t; fout << t << " ";
		BTree tree(t);
		FillDeleteSearch(fout, tree, number_2);
		fout.close();
	}

	system("pause");
	return 0;
}
