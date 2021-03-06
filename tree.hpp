#include <iostream>
#include <fstream>
#include <cmath>

struct Node
{
	int key;
	size_t height = 1;
	size_t size   = 1;
	Node* left    = nullptr;
	Node* right   = nullptr;
	Node* prev    = nullptr;

	Node(int key = 0): key{key} {}

	Node(const Node& other): 
		key{other.key}, height{other.height}, 
		size{other.size}, prev{other.prev}
	{
		Node *curr = this;
		Node const* cp_node = &other;

		while(true)
		{
			if (cp_node->left && !curr->left)
			{
				curr->left = new Node;
				curr->left->prev = curr;
				cp_node = cp_node->left;
				curr 	= curr->left;
			}
			else if (cp_node->right && !curr->right)
			{
				curr->right = new Node;
				curr->right->prev = curr;
				cp_node = cp_node->right;
				curr 	= curr->right;
			}
			else if (cp_node->prev)
			{
				cp_node = cp_node->prev;
				curr	= curr->prev;
			} 
			else break;

			curr->key    = cp_node->key;
			curr->height = cp_node->height;
			curr->size   = cp_node->size;
		}
	}

	Node(Node&& other):
		key{other.key}, height{other.height}, size{other.size},
		left{other.left}, right{other.right}, prev{other.prev}
	{
		if (left)   left->prev = this;
		if (right) right->prev = this;

		other.left  = nullptr;
		other.right = nullptr;
		other.prev  = nullptr;
	}

	Node& operator= (const Node& other)
	{
		if (this != &other)
			*this = Node{other};

		return *this;
	}

	Node& operator= (Node&& other)
	{
		if (this != &other)
		{
			delete left;
			delete right;

			key    = other.key, 	 
			height = other.height, size = other.size;
			left   = other.left,   other.left  = nullptr;
			right  = other.right,  other.right = nullptr;
			prev   = other.prev,   other.prev  = nullptr;

			if (left)   left->prev = this;
			if (right) right->prev = this;
		}
		return *this;
	}

	~Node()
	{
		if (!left && !right)
			return;

		Node *curr = this, *tmp{};

		while (true)
		{
			if (curr->left)
				curr = curr->left;
			else if (curr->right)
				curr = curr->right;
			else if (curr->prev)
			{
				tmp  = curr;
				curr = curr->prev;

				if (curr->left == tmp)
					curr->left = nullptr;
				else if (curr->right == tmp)
					curr->right = nullptr;

				delete tmp;
			}
			else break;
		}
	}

	void calc_data()
	{
		calc_height();
		calc_size();
	}

	void calc_height()
	{
		int h_left  =  left ?  left->height : 0;
		int h_right = right ? right->height : 0;
		height = std::max(h_left, h_right) + 1;
	}

	void calc_size()
	{
		int s_left  =  left ?  left->size : 0;
		int s_right = right ? right->size : 0;
		size = s_left + s_right + 1;
	}

	int height_diff()
	{
		int h_left  =  left ?  left->height : 0;
		int h_right = right ? right->height : 0;
		return h_right - h_left;
	}

	Node* insert(int Key)
	{
		if (Key < key)
		{
			if (left)
				left = left->insert(Key);
			else
				left = new Node{Key};

			left->prev = this;
		}
		else
		{
			if (right)
				right = right->insert(Key);	
			else
				right = new Node{Key};

			right->prev = this;
		}

		return balance();
	}

	Node* balance()
	{
		calc_data();

		if (height_diff() == 2)
		{
			if (right->height_diff() < 0)
				right = right->r_rotation();

			return l_rotation();
		} 
		else if (height_diff() == -2)
		{
			if (left->height_diff() > 0)
				left = left->l_rotation();

			return r_rotation();
		}

		return this;
	}

	Node* l_rotation()
	{
		Node* tmp = right;
		right = right->left;
		tmp->left = this;

		if (right)
			right->prev = this;

		tmp->prev = prev;
		prev = tmp;

		calc_data();
		tmp->calc_data();

		return tmp;
	}

	Node* r_rotation()
	{
		Node* tmp = left;
		left = left->right;
		tmp->right = this;

		if (left) 
			left->prev = this;
		
		tmp->prev = prev;
		prev = tmp;

		calc_data();
		tmp->calc_data();
	
		return tmp;
	}

	void graph(std::string f_name) const
	{
		std::string filename(f_name);
		filename += ".dot";

		std::ofstream fgraph(filename);

		fgraph << "digraph Tree {\n";
		
		make_graph(fgraph);
		
		fgraph << "}";
	}

	void make_graph(std::ofstream& fgraph) const
	{
		fgraph  << "\n\tp" 		<< this << " [label = \"" 
				<< 	 "key: "    << key
				<< "  height: " << height
				<< "  size: "   << size << "\"]\n";

		if (left && right)
		{
			fgraph << "\tp" << this << " -> { "
				   << "p"   << left << " p" << right << " }\n";

			 left->make_graph(fgraph);
			right->make_graph(fgraph);
		}
		else if (left)
		{
			fgraph << "\tp"  << this << " -> { "
				   << "p"    << left << " p" 
				   << &right << " [label = \"\"] }\n";

			left->make_graph(fgraph);
		}
		else if (right)
		{
			fgraph << "\tp" << this  << " -> { "
				   << "p"   << &left << " [label = \"\"] "
				   << "p"   << right << " }\n";

			right->make_graph(fgraph);
		}
	}
};

class Tree
{
	Node* top;
	bool  init;
public:
	Tree(): top{new Node}, init{false} {}

	Tree(const Tree& other): top{new Node}, init{other.init} { *top = *other.top; }

	Tree(Tree&& other): top{other.top}, init{other.init} { other.top = nullptr; }

	Tree& operator= (const Tree& other)
	{
		if (this != &other)
			*this = Tree{other};

		return *this;
	}

	Tree& operator= (Tree&& other)
	{
		if (this != &other)
		{
			delete top;

			top  = other.top;
			init = other.init;
			other.top = nullptr;
		}
		return *this;
	}

	size_t size() const { return init ? top->size : 0; }

	void insert(int Key)
	{
		if (init)
			top = top->insert(Key);
		else
		{
			top->key = Key;
			init = true;
		}
	}

	void graph(std::string f_name = std::string{"graph"}) const { if (init) top->graph(f_name); }

	int k_min(int k) const
	{
		//Tree isn't initialized
		if (!init) return 0;

		Node* curr = top;
		size_t l_size{};

		while (true)
		{
			l_size = curr->left ? curr->left->size : 0;

			if (k == l_size + 1)
				return curr->key;

			if (k > l_size)
			{
				k   -= l_size + 1;
				curr = curr->right;
			}
			else curr = curr->left;
		}
	}

	size_t cnt_less(int k) const
	{
		//Tree isn't initialized
		if (!init) return 0;

		Node*  curr = top;
		size_t cnt_min{}, l_size{};

		while (true)
		{
			if (curr)
				l_size = curr->left ? curr->left->size : 0;

			if (!curr || k == curr->key)
				return cnt_min + l_size;

			if (k > curr->key)
			{
				cnt_min += l_size + 1;
				curr = curr->right;
			}
			else curr = curr->left;

			l_size = 0;
		}
	}

	~Tree() { delete top; }

	friend bool is_equal(const Tree& tr1, const Tree& tr2);
};