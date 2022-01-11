/* The goal of this task is to implement a B+ search tree, with
 * insertion and removal of keys. Assume that both keys and values
 * can be copied and that keys can be compared using ‹<› and ‹==›.
 *
 * The ‹max_fanout› specifies the ‘branching factor’ ⟦b⟧ of the
 * tree: the maximum number of children a node can have. Each node
 * then stores at most ⟦b - 1⟧ keys. As is usual with B trees, the
 * minimum number of children for an internal node, with the
 * exception of the root, is ⟦⌈b/2⌉⟧ (the upper integral part of
 * ⟦b/2⟧). */

#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <iterator>

using path_t = std::vector< int >;

template< typename key_t, typename value_t, int max_fanout >
struct bplus;

template< typename key_t, typename value_t, int max_fanout >
struct node {
    using path_type = std::vector<node<key_t, value_t, max_fanout>*>;
    std::map<key_t, std::unique_ptr< node<key_t, value_t, max_fanout> > > children;
    std::unique_ptr< node<key_t, value_t, max_fanout> > last_child;
    std::map<key_t, value_t> data;
    node *next = nullptr;
    node *prev = nullptr;
    bool isLeaf;

    void check_and_split(bplus<key_t, value_t, max_fanout> &tree, path_type &path) {
	path.pop_back();
	if(children.size() < max_fanout)
	    return;
	//splits children into two
	std::map<key_t, std::unique_ptr< node<key_t, value_t, max_fanout> > > left_children;
	for(int i = 0; i < max_fanout/2; ++i) {
	    auto it = children.begin();
	    auto nh = children.extract(it);
	    left_children.insert(left_children.begin(), std::move(nh));
	}
	auto left = std::make_unique< node<key_t, value_t, max_fanout>>();
	left->isLeaf = false;
	left->children = std::move(left_children);
	auto key = children.begin()->first;
	auto nh = children.extract(children.begin());
	left->last_child = std::move(nh.mapped());
	// if this is root
	if(path.empty()) {
	    auto new_root = std::make_unique< node<key_t, value_t, max_fanout>>();
	    new_root->isLeaf = false;
	    tree.root_helper = std::move(new_root);
	    tree.root_helper->last_child = std::move(tree.root);
	    tree.root = std::move(tree.root_helper);
	    path.push_back(tree.root.get());
	}
	path.back()->children.insert({key, std::move(left)});
	path.back()->check_and_split(tree, path);
    }
    void split_data(bplus<key_t, value_t, max_fanout> &tree, path_type &path) {
	path.pop_back();
	//splits data into two
	std::map<key_t, value_t> left_data;
	for(int i = 0; i < (max_fanout)/2; ++i) {
	    auto it = data.begin();
	    auto nh = data.extract(it);
	    left_data.insert(left_data.end(), std::move(nh));
	}
	//assert(left_data.size() <= data.size());

	// puts a pointer into parent
	auto left = std::make_unique< node<key_t, value_t, max_fanout>>();
	left->isLeaf = true;
	left->data = std::move(left_data);
	//set next and prev pointers
	if (prev != nullptr)
	    prev->next = left.get();
	left->next = this;
	left->prev = prev;
	prev = left.get();

	// if this is root
	if(path.empty()) {
	    auto new_root = std::make_unique< node<key_t, value_t, max_fanout>>();
	    new_root->isLeaf = false;
	    tree.root_helper = std::move(new_root);
	    tree.root_helper->last_child = std::move(tree.root);
	    tree.root = std::move(tree.root_helper);
	    path.push_back(tree.root.get());
	}
	path.back()->children.insert({data.begin()->first, std::move(left)});
	path.back()->check_and_split(tree, path);
    }

    bool insert_data(bplus<key_t, value_t, max_fanout> &tree, const key_t &key,
	    const value_t &val, path_type &path) {
	auto it = data.find(key);
	if (it != data.end()) {
	    return false;
	}
	data.insert({key, val});
	if(data.size() == max_fanout) {
	    split_data(tree, path);
	}
	return true;
    }

    void delete_data(const key_t &key) {
	data.erase(key);
	//if (data.size() >= (max_fanout-1)/2) 
	 //   return;
    }

    void search_node(key_t key, path_type &path) {
	if (isLeaf)
	    return;

	auto it = path.back()->children.upper_bound(key);
	if(it == path.back()->children.end())
	    path.push_back(path.back()->last_child.get());
	else
	    path.push_back(it->second.get());
	path.back()->search_node(key, path);
    }

    void path_recursive(const key_t &key, path_t &ret_path) {
	if (isLeaf)
	    return;

	auto it = children.upper_bound(key);
	if(it == children.end()) {
	    ret_path.push_back(children.size());
	    last_child->path_recursive(key, ret_path);
	}
	else {
	    ret_path.push_back(std::distance(children.begin(), it));
	    it->second->path_recursive(key, ret_path);
	}
    }

};

template< typename key_t, typename value_t, int max_fanout >
struct bplus
{
    value_t dummy{};
    using path_type = std::vector<node<key_t, value_t, max_fanout>*>;
    std::unique_ptr< node<key_t, value_t, max_fanout> > root;
    std::unique_ptr< node<key_t, value_t, max_fanout> > root_helper;

    /* Insert an element, maintaining the invariants of the B+ tree.
     * Must run in expected logarithmic time in the general case and
     * worst-case logarithmic if ‹erase› has not been called on the
     * tree. Return ‹true› if the tree was changed. */

    bplus() {
	root = std::make_unique< node<key_t, value_t, max_fanout> >();
	root->isLeaf = true;
    }
    bool insert( const key_t &key, const value_t &val ) {
	path_type path;
	if (root->isLeaf && root->data.empty()) {
	    root->insert_data(*this, key, val, path);
	    return true;
	}
	path.push_back(root.get());
	root->search_node(key, path);
	//assert(path.back());
	return path.back()->insert_data(*this, key, val, path);
    }

    /*
    void print() {
	std::cout << "printing\n";
	path_type path;
	path.push_back(root.get());
	std::cout << "root keys\n";
	for (auto &x : root->children)
	    std::cout << x.first << " ";
	std::cout << std::endl;
	root->search_node(-1, path);
	std::cout << path.size() << " is the depth\n";
	auto node = path.back();
	do {
	    for(auto &x : node->data)
		std::cout << x.second << " ";
	    std::cout << "| ";
	    node = node->next;
	}
	while(node != nullptr);
    }
    */

    /* Erase an element from the tree. Lazy removal is permissible,
     * i.e. without rebalancing the tree. Must run in expected
     * logarithmic time. Return ‹true› if the tree was changed. */

    bool erase( const key_t &key ) {
	// empty tree
	// root is leaf
	if (root->isLeaf) {
	    if( root->data.empty() )
		return false;
	    auto it = root->data.find(key);
	    if (it == root->data.end())
		return false;
	    root->data.erase(key);
	    return true;
	}

	path_type path;
	path.push_back(root.get());
	root->search_node(key, path);

	// key not found
	auto it = path.back()->data.find(key);
	if (it == path.back()->data.end())
	    return false;

	path.back()->delete_data(key);

	return true;
    }


    /* Look up elements. The ‹at› method should throw
     * ‹std::out_of_range› if the key is not present in the tree.
     * The indexing operator should insert a default-constructed
     * value if the key is absent, and return a reference to this
     * value. */

    bool contains( const key_t &key ) const {
	path_type path;
	path.push_back(root.get());
	root->search_node(key, path);
	auto it = path.back()->data.find(key);
	return (it != path.back()->data.end());
    }

    value_t &at( const key_t &key ) {
	return _at(*this, key);
    }
    const value_t &at( const key_t &key ) const {
	return _at(*this, key);
    }
    template <typename Self>
    static auto& _at(Self& self, key_t key) {
	path_type path;
	path.push_back(self.root.get());
	self.root->search_node(key, path);
	auto it = path.back()->data.find(key);
	if(it == path.back()->data.end())
	    throw std::out_of_range("not found ");
	return it->second;
    }

    value_t &operator[]( const key_t &key ) {
	value_t val{};
	try {
	    return at(key);
	} catch (std::out_of_range& ex) {
	    insert(key, std::move(val));
	}
	return at(key);
    }

    /* Look up an element and return the path that leads to it in
     * the tree, i.e. the index of the child node selected during
     * lookup at each level. Return an empty path if the key is not
     * present. The fetch operation then takes a path returned by
     * ‹path› and fetches the corresponding value from the tree.
     * Please note that the paths must reflect the layout of a
     * correct B+ tree. */

    path_t path( const key_t &key ) const {
	path_type p;
	p.push_back(root.get());
	path_t ret_path;
	root->search_node(key, p);
	auto it = p.back()->data.find(key);
	if(it == p.back()->data.end())
	    return ret_path;
	root->path_recursive(key, ret_path);
	ret_path.push_back(std::distance(p.back()->data.begin(), it));
	return ret_path;
    }

    const value_t &fetch( const path_t &path ) const {
	if(path.empty())
	    return dummy;
	auto node = root.get();
	for ( int i : path ) {
	    if(node->isLeaf) {
		auto it = node->data.begin();
		std::advance(it, i);
		return it->second;
	    }
	    auto it = node->children.begin();
	    std::advance(it, i);
	    if(it == node->children.end())
		node = node->last_child.get();
	    node = it->second.get();
	}
	return dummy;
    }
};
