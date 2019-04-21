#include <iostream>
#include <ctime>
#include <string>

using namespace std;

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

	BTreeNode(int _t, bool _leaf); 

	void traverse();

	void singleTraverse() {
		cout << "{";
		for (int i = 0; i < n; i++) {
			if ((n - i) == 1) {
				cout << elems[i].key;
			}
			else {
				cout << elems[i].key << ", ";
			}
		}
		cout << "} ";
		
	}

	string interpolationSearch(int k);

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

	BTreeNode* getRoot() { return root; }

	void traverse()
	{
		if (root != NULL) root->traverse();
	}

	string search(int k)
	{
		return (root == NULL) ? NULL : root->interpolationSearch(k);
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
		if (k < elems[begin].key){
			counter++;
			return C[begin]->interpolationSearch(k);
		}
		if (k > elems[end].key) {
			counter++;
			return C[end + 1]->interpolationSearch(k);
		}
	}
	while (true)
	{	
		if (k > elems[end].key) return "Key wasn't found.";
		if (k < elems[begin].key) return "Key wasn't found.";
		double temp = (k - elems[begin].key);
		double temp_1 = (elems[end].key - elems[begin].key);
		double temp_2 = temp / temp_1;
		mid = begin + temp_2 * (end - begin);
		if (elems[mid].key == k) {
			counter++;
			return elems[mid].data;
		}
		if (mid < end && k > elems[mid].key && k < elems[mid + 1].key && !leaf) {
				counter++;
				return C[mid + 1]->interpolationSearch(k);
		}
		if (mid > begin && k > elems[mid - 1].key && k < elems[mid].key && !leaf) {
				counter++;
				return C[mid]->interpolationSearch(k);
		}
		if (elems[begin].key == k) {
			counter++;
			return elems[begin].data;
		}
		if (elems[end].key == k) {
			counter++;
			return elems[end].data;
		}
		if (k > elems[mid].key) {
			counter++;
			begin = mid + 1;
			continue;
		}
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

int main() {
	int t;
	cout << "Input parametr t: "; cin >> t;
	BTree tree(t);
	int size, *Array;
	string  *Data, num;
	cout << "Input size of array: "; cin >> size;
	Array = new int[size];
	Data = new string[size];
	fillingArray(Array, size);
	fillingData(Data, size);
	showData(Data, Array, size);
	for (int i = 0; i < size; i++) {
		tree.insert(Array[i], Data[i]);
	}
	tree.traverse();
	cout << endl;
	while (true) {
		cout << "Input key which you wanna find (if you wanna stop type quit): ";
		cin >> num;
		if (num == "quit") { break; }
		cout << tree.search(stoi(num)) << endl;
		cout << "Comparisons: " << counter << endl;
		counter = 0;
	}
	cout << endl;
	while (true) {
		cout << "Input key which you wanna delete (if you wanna stop type quit): ";
		cin >> num;
		if (num == "quit") { break; }
		tree.remove(stoi(num));
	}
	tree.traverse();

	delete[] Array;
	system("pause");
	return 0;
}
