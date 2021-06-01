#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <utility>
#include <stdexcept>


template <typename T, typename V>
class AVL_Tree {
public:
    using traversal_type = void (*)(void*&, void*&, void*&);
public:
    AVL_Tree() :
            _root(nullptr),
            _size(0)
    {}

    AVL_Tree(const AVL_Tree<T,V> &Tree);
    AVL_Tree& operator=(const AVL_Tree<T,V> &Tree);

    AVL_Tree(AVL_Tree<T,V>&& Tree) noexcept;
    AVL_Tree& operator=(AVL_Tree<T,V>&& Tree) noexcept;

    template<typename ...Args,
            typename = typename std::enable_if<
                    (std::is_same<
                            typename std::remove_reference<typename std::tuple_element<0,Args>::type>::type,
                            typename std::remove_reference<T>::type>::value && ...)>::type>
    AVL_Tree(Args&& ...args);

    ~AVL_Tree();


    template<typename TT, typename VV>
    void insert(TT&& key, VV&& val);

    template<typename TT>
    void erase(TT&& key) {_assert_empty(); _root = _remove(_root, std::forward<TT>(key)); --_size;}

    void clear();

    template<typename TT>
    V& get(TT&& key) {return _get(_root, std::forward<TT>(key))->val;}

    template<typename TT>
    const V& get(TT&& key) const {return _get(_root, std::forward<TT>(key))->val;}

    template<typename TT>
    bool find(TT&& key) const {return _find(_root, std::forward<TT>(key)) != nullptr;}

    std::pair<const T&,V&> find_min() const {_assert_empty(); Node* min_node = _find_min(_root); return {min_node->key, min_node->val};}

    std::pair<const T&,V&> find_max() const {_assert_empty(); Node* max_node = _find_max(_root); return {max_node->key, max_node->val};}

    template<typename TT>
    V& operator[] (TT&& key);

    template<typename TT>
    const V& operator[] (TT&& key) const {return get(std::forward<TT>(key));}

    template <typename Func>
    void traversal(traversal_type t, Func func) {_traversal(_root, nullptr, t, func);}

    template <typename Func>
    void const_traversal(traversal_type t, Func func) const {_const_traversal(_root, nullptr, t, func);}


    template<typename TT>
    AVL_Tree<T,V> subtree(TT&& key) const;

    size_t size() const noexcept {return _size;}
    int height() const noexcept {return _root->height;}

private:
    struct Node {
    public:
        template<typename TT, typename VV>
        Node(TT&& k, VV&& v):
                key(std::forward<TT>(k)),
                val(std::forward<VV>(v)),
                height(1),
                left(nullptr),
                right(nullptr)
        {}
        explicit Node(const Node *node);
        ~Node();

        T key;
        V val;
        unsigned int height;
        Node *left, *right;
    };


    int _height(Node *p) const;

    void _fixheight(Node *p);

    int _factor(Node *p) const;

    Node *_rotate_right(Node *p);

    Node *_rotate_left(Node *q);

    Node *_balance(Node *p);

    template<typename TT>
    Node *_find(Node *p, TT&& key) const;

    Node *_find_min(Node *p) const;

    Node *_find_max(Node *p) const;

    Node *_remove_min(Node *p);

    template<typename TT>
    Node *_remove(Node *p, TT&& k);

    template<typename TT, typename VV>
    Node *_insert(Node *p, TT&& k, VV&& val);

    template<typename TT>
    Node *_get(Node *p, TT&& k) const;

    template <typename Func>
    void _traversal(Node *p, Node *parent, traversal_type t, Func func);

    template <typename Func>
    void _const_traversal(Node *p, Node *parent, traversal_type t, Func func) const;

    size_t _calc_size(Node *p) const;

    void _assert_empty() const;
public:
    static void RtLR(void *&n1, void *&n2, void *&n3){std::swap(n1, n2);}
    static void RtRL(void *&n1, void *&n2, void *&n3){std::swap(n2, n3); std::swap(n1, n3);}
    static void LRRt(void *&n1, void *&n2, void *&n3){std::swap(n2, n3);}
    static void LRtR(void *&n1, void *&n2, void *&n3){}
    static void RLRt(void *&n1, void *&n2, void *&n3){std::swap(n1, n3); std::swap(n2, n3);}
    static void RRtL(void *&n1, void *&n2, void *&n3){std::swap(n1, n3);}

private:
    template<typename FV, typename ...Args>
    void _list_initializer(FV&& p, Args&& ...args);
    void _list_initializer(){}

    Node *_root;
    size_t _size;
};


template <typename T, typename V>
AVL_Tree<T, V>::AVL_Tree(const AVL_Tree<T,V> &Tree):
        _root(Tree._root ? new Node(Tree._root) : nullptr),
        _size(Tree._size)
{}

template <typename T, typename V>
AVL_Tree<T,V>& AVL_Tree<T,V>::operator=(const AVL_Tree<T, V> &Tree)
{
    delete _root;
    _root = new Node(Tree._root);
    return *this;
}

template <typename T, typename V>
AVL_Tree<T, V>::AVL_Tree(AVL_Tree<T,V>&& Tree) noexcept:
        _root(Tree._root),
        _size(Tree._size)
{
    Tree._root = nullptr;
    Tree._size = 0;
}

template <typename T, typename V>
AVL_Tree<T,V>& AVL_Tree<T,V>::operator=(AVL_Tree<T, V> &&Tree) noexcept
{
    delete _root;
    _root = Tree._root;
    _size = Tree._size;
    Tree._root = nullptr;
    Tree._size = 0;
    return *this;
}


template <typename T, typename V>
template <typename ...Args, typename>
AVL_Tree<T, V>::AVL_Tree(Args&& ...args):
        AVL_Tree()
{
    _list_initializer(std::forward<Args>(args)...);
}

template <typename T, typename V>
AVL_Tree<T,V>::~AVL_Tree()
{
    delete _root;
}


template <typename T, typename V>
template<typename TT, typename VV>
void AVL_Tree<T,V>::insert(TT&& key, VV&& val)
{
    if(_root == nullptr){
        _root = new Node(std::forward<TT>(key), std::forward<VV>(val));
    }
    else{
        _root = _insert(_root, std::forward<TT>(key), std::forward<VV>(val));
    }

    ++_size;
}

template <typename T, typename V>
void AVL_Tree<T,V>::clear()
{
    delete _root;
    _root = nullptr;
}

template <typename T, typename V>
template<typename TT>
V& AVL_Tree<T,V>::operator[](TT&& key)
{
    if(!find(key)){
        insert(std::forward<TT>(key), V());
    }

    return get(std::forward<TT>(key));
}

template <typename T, typename V>
template<typename TT>
AVL_Tree<T,V> AVL_Tree<T,V>::subtree(TT&& key) const
{
    const Node *p = _find(_root, std::forward<TT>(key));
    AVL_Tree<T,V> ret;
    ret._root = new Node(p);
    ret._size = ret._calc_size(ret._root);

    return ret;
}

template <typename T, typename V>
template <typename Func>
void AVL_Tree<T,V>::_traversal(Node *p, Node *parent, traversal_type t, Func func)
{
    if(p == nullptr)
        return;

    if(p == parent) {
        func(p->key, p->val);
    }
    else {
        void *n1 = p->left, *n2 = p, *n3 = p->right;
        t(n1, n2, n3);

        _traversal((Node*)n1, p, t, func);
        _traversal((Node*)n2, p, t, func);
        _traversal((Node*)n3, p, t, func);
    }
}

template <typename T, typename V>
template <typename Func>
void AVL_Tree<T,V>::_const_traversal(Node *p, Node *parent, traversal_type t, Func func) const
{
    if(p == nullptr)
        return;

    if(p == parent) {
        func(p->key, p->val);
    }
    else {
        void *n1 = p->left, *n2 = p, *n3 = p->right;
        t(n1, n2, n3);

        _const_traversal((Node*)n1, p, t, func);
        _const_traversal((Node*)n2, p, t, func);
        _const_traversal((Node*)n3, p, t, func);
    }
}

template <typename T, typename V>
template <typename FV, typename ...Args>
void AVL_Tree<T, V>::_list_initializer(FV&& p, Args&& ...args)
{
    insert(std::forward<typename FV::first_type>(p.first),
           std::forward<typename FV::second_type>(p.second));

    _list_initializer(std::forward<Args>(args)...);
}


template <typename T, typename V>
AVL_Tree<T,V>::Node::Node(const AVL_Tree<T, V>::Node *node):
        key(node->key),
        val(node->val),
        height(node->height)
{
    if(node->left != nullptr)
        left = new Node(node->left);
    else
        left = nullptr;

    if(node->right != nullptr)
        right = new Node(node->right);
    else
        right = nullptr;
}

template <typename T, typename V>
AVL_Tree<T,V>::Node::~Node()
{
    delete left;
    delete right;
}

template <typename T, typename V>
int AVL_Tree<T,V>::_height(Node *p) const{
    return p ? p->height : 0;
}

template <typename T, typename V>
void AVL_Tree<T,V>::_fixheight(Node *p){
    p->height = std::max(_height(p->left), _height(p->right)) + 1;
}

template <typename T, typename V>
int AVL_Tree<T,V>::_factor(Node *p) const {
    return _height(p->right) - _height(p->left);
}

template <typename T, typename V>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_rotate_right(Node *p)
{
    Node *q = p->left;
    p->left = q->right;
    q->right = p;

    _fixheight(p);
    _fixheight(q);

    return q;
}

template <typename T, typename V>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_rotate_left(Node *q)
{
    Node *p = q->right;
    q->right = p->left;
    p->left = q;

    _fixheight(q);
    _fixheight(p);

    return p;
}


template <typename T, typename V>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_balance(Node *p)
{
    _fixheight(p);

    if(_factor(p) == 2)
    {
        if(_factor(p->right) < 0)
            p->right = _rotate_right(p->right);

        p = _rotate_left(p);
    }
    else if(_factor(p) == -2)
    {
        if(_factor(p->left) > 0)
            p->left = _rotate_left(p->left);

        p = _rotate_right(p);
    }

    return p;
}

template <typename T, typename V>
template<typename TT>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_find(Node *p, TT&& key) const
{
    if(p == nullptr)
        return nullptr;
    if(key == p->key)
        return p;
    if(key < p->key)
        return _find(p->left, std::forward<TT>(key));
    if(key > p->key)
        return _find(p->right, std::forward<TT>(key));
}

template<typename T, typename V>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_find_min(Node *p) const
{
    return p->left ? _find_min(p->left) : p;
}

template<typename T, typename V>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_find_max(Node *p) const
{
    return p->right ? _find_max(p->right) : p;
}

template<typename T, typename V>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_remove_min(Node *p)
{
    if(p->left == nullptr)
        return p->right;
    p->left = _remove_min(p->left);
    return _balance(p);
}

template<typename T, typename V>
template<typename TT>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_remove(Node *p, TT&& k)
{
    if(p == nullptr)
        throw std::out_of_range("AVL_Tree out of range!");
    if(k < p->key)
        p->left = _remove(p->left, std::forward<TT>(k));
    else if(k > p->key)
        p->right = _remove(p->right, std::forward<TT>(k));
    else
    {
        Node *q = p->left;
        Node *r = p->right;

        p->left = nullptr;
        p->right = nullptr;
        delete p;

        if(r == nullptr)
            return q;

        Node *m = _find_min(r);
        m->right = _remove_min(r);
        m->left = q;
        return _balance(m);
    }
    return _balance(p);
}


template <typename T, typename V>
template <typename TT, typename VV>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_insert(Node *p, TT&& k, VV&& val)
{
    if(p == nullptr)
        return new Node(std::forward<TT>(k), std::forward<VV>(val));
    if(p->key == k)
        throw std::runtime_error("AVL_Tree trying to insert by existing key");
    if(k < p->key)
        p->left = _insert(p->left, std::forward<TT>(k), std::forward<VV>(val));
    else
        p->right = _insert(p->right, std::forward<TT>(k), std::forward<VV>(val));

    _fixheight(p);
    return _balance(p);
}

template <typename T, typename V>
template <typename TT>
typename AVL_Tree<T,V>::Node *AVL_Tree<T,V>::_get(Node *p, TT&& k) const
{
    if(p == nullptr)
        throw std::out_of_range("AVL_Tree out of range!");
    if(k == p->key)
        return p;
    if(k < p->key)
        return _get(p->left, k);
    else
        return _get(p->right, k);
}

template <typename T, typename V>
size_t AVL_Tree<T,V>::_calc_size(Node *p) const
{
    if(p == nullptr)
        return 0;
    else
        return _calc_size(p->left) + _calc_size(p->right) + 1;
}

template <typename T, typename V>
void AVL_Tree<T,V>::_assert_empty() const
{
    if(_root == nullptr)
        throw std::logic_error("AVL_Tree assert empty");
}


template <typename T, typename V, typename Func>
AVL_Tree<T,V> map(const AVL_Tree<T,V> &tree, Func f)
{
    AVL_Tree<T,V> ret = tree;
    ret.traversal(ret.LRtR, [f](const T &key, V &val){
        val = f(val);
    });

    return ret;
}

template <typename T, typename V, typename Func>
AVL_Tree<T,V> map(AVL_Tree<T,V> &&tree, Func f)
{
    AVL_Tree<T,V> ret = tree;
    ret.traversal(ret.LRtR, [f](const T &key, V &val){
        val = f(val);
    });

    return ret;
}

template <typename T, typename V, typename Func>
AVL_Tree<T,V> where(const AVL_Tree<T,V> &tree, Func f)
{
    AVL_Tree<T,V> ret;
    tree.const_traversal(tree.LRtR, [&f, &ret](const T &key, const V &val){
        if(f(val)){
            ret.insert(key, val);
        }
    });
    return ret;
}

template <typename T, typename V, typename Func>
AVL_Tree<T,V> where(AVL_Tree<T,V> &&tree, Func f)
{
    AVL_Tree<T,V> ret;
    tree.const_traversal(tree.LRtR, [&f, &ret](const T &key, const V &val){
        if(f(val)){
            // Are we actually not able to move key too?
            ret.insert(key, std::move(val));
        }
    });
    return ret;
}

template <typename T, typename V, typename VV, typename Func>
V reduce(const AVL_Tree<T,V> &tree, VV&& init, Func f, typename AVL_Tree<T,V>::traversal_type t_type = AVL_Tree<T,V>::LRtR)
{
    V ret = init;

    tree.const_traversal(t_type, [&f, &ret](const T &key, const V &val){
        ret = f(val, ret);
    });

    return ret;
}

#endif