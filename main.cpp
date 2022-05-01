#include <iostream>
#include <string>

using namespace std;


int POWER(int value, int pow) {
	if (pow == 0) return 1;
	for (int i = 1; i < pow; i++) {
		value = value * value;
	}
	return value;
}

class BTreeNode {
	int* V;
	BTreeNode** C;
	bool is_leaf;
	int count;
	int order;
public:
	BTreeNode(int order, bool is_leaf);
	void traverse();
	void insert(int value);
	void split(int index, BTreeNode* x);
	bool search(int value);
	int search_h(int value);
	BTreeNode* LoadedNodeCreation(string s, int &i);
	void Save(string &s);
	void free_nodes();
	
	friend class BTree;
	friend class Cache;
};

class BTree {
	BTreeNode* root;
	int order;
public:
	BTree(int order);
	void add(int value);
	void print();
	bool search(int value);
	void Load(string s);
	void SaveTree();
	BTreeNode* GetRoot();
	void free_memory();
};

class Cache {
	int* cache;
	int size;
	int ACC_cache;
	int ACC_no_cache;
	string s;
	BTreeNode* B;
	int curr;
public:
	Cache(int size, string s, BTreeNode* B);
	void Measure();
	void push(int value);
};


BTreeNode::BTreeNode(int order, bool is_leaf) {
	this->count = 0;
	this->V = new int[2*order - 1];
	this->C = new BTreeNode* [2*order];
	this->is_leaf = is_leaf;
	this->order = order;
}

void BTreeNode::traverse() {
	for (int i = 0; i < count; i++) {
		if(!this->is_leaf) this->C[i]->traverse();
		cout << this->V[i] << " ";
	}
	if (!this->is_leaf) this->C[this->count]->traverse();
}

void BTreeNode::insert(int value) {
	int index = 0;
	while (this->V[index] < value && index < this->count) index++;

	if (this->is_leaf) {
		for (int i = this->count; i > index; i--) {
			this->V[i] = this->V[i - 1];
		}
		this->V[index] = value;
		this->count++;
	}
	else {
		if (this->C[index]->count == this->order*2 - 1) {
			this->split(index, this->C[index]);

			if (value > this->V[index]) index++;
		}
		this->C[index]->insert(value);
	}
}

void BTreeNode::split(int index, BTreeNode* x) {
	BTreeNode* y = new BTreeNode(this->order, x->is_leaf);

	for (int i = 0; i < this->order - 1; i++) {
		y->V[i] = x->V[this->order + i];
		x->V[this->order + i] = NULL;
	}

	x->count = y->count = this->order - 1;

	if (x->is_leaf == false) {
		for (int i = 0; i < this->order; i++) {
			y->C[i] = x->C[this->order + i];
			x->C[this->order + i] = NULL;
		}
	}

	for (int i = this->count; i > index; i--) {
		this->C[i + 1] = this->C[i];
	}
	this->C[index + 1] = y;

	for (int i = this->count - 1; i > index - 1; i--) {
		this->V[i + 1] = this->V[i];
	}
	this->V[index] = x->V[this->order - 1];
	x->V[this->order - 1] = NULL;
	this->count++;
}

bool BTreeNode::search(int value) {
	int i=0;
	while (this->V[i] < value && i < this->count) i++;
	if (this->V[i] == value) return true;
	else if (this->is_leaf) return false;
	else return this->C[i]->search(value);
}

int BTreeNode::search_h(int value) {
	int i = 0;
	while (this->V[i] < value && i < this->count) i++;
	if (this->V[i] == value || this->is_leaf) return 1;
	else return 1 + this->C[i]->search_h(value);
}

BTreeNode* BTreeNode::LoadedNodeCreation(string s, int &i) {
	while (i < s.size()) {
		if (s[i] >= 48 && s[i] <= 57) {
			string a;
			a.push_back(s[i]);
			
			while (s[i + 1] >= 48 && s[i + 1] <= 57) {
				a.push_back(s[i + 1]);
				i++;
			}

			int value = 0;
			int l = 0;
			for (int k = a.size() - 1; k >= 0; k--) {
				value += (a[k] - 48) * POWER(10, l);
				l++;
			}
			
			this->V[this->count] = value;
			this->count++;
			i++;
		}
		else if (s[i] == '(') {
			BTreeNode* x = new BTreeNode(this->order, true);
			this->is_leaf = false;
			i++;
			this->C[this->count] = x->LoadedNodeCreation(s, i);
		}
		else if (s[i] == ')') {
			i++;
			return this;
		}
		else i++;
	}
	return this;
}

void BTreeNode::Save(string &s) {
	s += "( ";
	for (int i = 0; i < this->count; i++) {
		if (!this->is_leaf) this->C[i]->Save(s);

		if(V[i] >= 0 && V[i] <= 9) s += this->V[i] + 48;
		else {
			int x = V[i];
			string tmp1,tmp2;
			while (x != 0) {
				tmp1 += (x % 10) + 48;
				x = x / 10;
			}
			for (int i = 0; i < tmp1.size(); i++) {
				tmp2 += tmp1[tmp1.size() - 1 - i];
			}
			s += tmp2;
		}

		s += " ";
	}
	if (!this->is_leaf) this->C[this->count]->Save(s);
	s += ") ";
}

void BTreeNode::free_nodes() {
	if (!this->is_leaf) {
		for (int i = 0; i <= count; i++) {
			this->C[i]->free_nodes();
		}
	}
	delete this->V;
	delete this->C;
	delete this;
}


BTree::BTree(int order) {
	this->order = order;
	this->root = new BTreeNode(order,true);
}

void BTree::add(int value) {
	if (this->root->count == this->order*2 - 1) {
		BTreeNode* r = new BTreeNode(this->order, false);

		r->C[0] = root;
		r->split(0, root);

		int index = 0;
		if (r->V[0] < value) index++;

		r->C[index]->insert(value);
		this->root = r;
	}
	else {
		this->root->insert(value);
	}
}

void BTree::print() {
	if (this->root->count != 0) this->root->traverse();
	else cout << "NULL";

	cout << endl;
}

bool BTree::search(int value) {
	if (this->root->count != 0) return this->root->search(value);
	else return false;
}

void BTree::Load(string s) {
	int i = 1;
	this->root->LoadedNodeCreation(s,i);
}

void BTree::SaveTree() {
	string s = {};
	this->root->Save(s);
	cout << this->order << endl;
	cout << s << endl;
}

BTreeNode* BTree::GetRoot() {
	return this->root;
}

void BTree::free_memory() {
	this->root->free_nodes();
	delete this;
}


Cache::Cache(int size, string s, BTreeNode* B) {
	this->cache = new int[size];
	for (int i = 0; i < size; i++) {
		cache[i] = NULL;
	}
	this->ACC_cache = 0;
	this->ACC_no_cache = 0;
	this->s = s;
	this->size = size;
	this->B = B;
	this->curr = 0;
}

void Cache::push(int value) {
	int i;
	if (this->curr < this->size) {
		i = curr;
		this->curr++;
	}
	else i = curr - 1;
	
	while (i > 0) {
		this->cache[i] = this->cache[i - 1];
		i--;
	}

	this->cache[0] = value;
}

void Cache::Measure() {
	for (int i = 0; i < this->s.size(); i++) {
		if (s[i] >= 48 && s[i] <= 57) {
			string a;
			a.push_back(s[i]);

			while (s[i + 1] >= 48 && s[i + 1] <= 57) {
				a.push_back(s[i + 1]);
				i++;
			}

			int value = 0;
			int l = 0;

			for (int j = a.size() - 1; j >= 0; j--) {
				value += (a[j] - 48) * POWER(10, l);
				l++;
			}
			
			int in_cache = 0;
			for (int j = 0; j < this->size; j++) {
				if (this->cache[j] == value) {
					in_cache++;
					break;
				}
			}

			if (in_cache == 0) {
				this->push(value);
				ACC_cache += this->B->search_h(value);
			}

			ACC_no_cache += this->B->search_h(value);
		}
	}
	cout << "NO CACHE: " << this->ACC_no_cache << " CACHE: " << this->ACC_cache << endl;

	delete this->cache;
	delete this;
}

int main() {
	char arg;
	int quit = 0;
	int value;
	BTree* Btree=NULL;
	Cache* CACHE = NULL;
	string s;

	while (!quit) {
		cin >> arg;
		switch (arg) {
		case 'I':
			cin >> value;
			Btree = new BTree(value);
			break;
		case 'A':
			cin >> value;
			Btree->add(value);
			break;
		case '?':
			cin >> value;
			if (Btree->search(value)) cout << value << " +" << endl;
			else cout << value << " -" << endl;
			break;
		case 'P':
			Btree->print();
			break;
		case 'L':
			cin >> value;
			cin.ignore();
			getline(cin,s);
			Btree = new BTree(value);
			Btree->Load(s);
			break;
		case 'S':
			Btree->SaveTree();
			break;
		case 'C':
			cin >> value;
			getline(cin, s);
			CACHE = new Cache(value, s, Btree->GetRoot());
			CACHE->Measure();
			break;
		case 'X':
			quit = 1;
			break;
		}
	}

	Btree->free_memory();

	return 0;
}