#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include "avl_tree.hpp"


template <typename V, typename T=size_t>
class PriorityQueue {
public:
    PriorityQueue() = default;
    PriorityQueue(const PriorityQueue<V,T> &queue);
    PriorityQueue(PriorityQueue &&queue) noexcept;

    template <typename TT, typename VV>
    void push(TT&& priority, VV&& val);

    V& top() const {return _tree.find_max().second;}
    V pop();
    void clear() {_tree.clear();}

    size_t size() const noexcept {return _tree.size();}
    bool empty() const noexcept {return _tree.size() == 0;}
private:
    AVL_Tree<T,V> _tree;
};

template <typename V, typename T>
PriorityQueue<V,T>::PriorityQueue(const PriorityQueue<V,T> &queue):
        _tree(queue._tree)
{}

template <typename V, typename T>
PriorityQueue<V,T>::PriorityQueue(PriorityQueue<V,T> &&queue) noexcept:
        _tree(std::move(queue._tree))
{}

template <typename V, typename T>
template <typename TT, typename VV>
void PriorityQueue<V,T>::push(TT &&priority, VV &&val)
{
    _tree[std::forward<TT>(priority)] = std::forward<VV>(val);
}

template <typename V, typename T>
V PriorityQueue<V,T>::pop()
{
    std::pair<const T&, V&> mx = _tree.find_max();
    V ret(std::move(mx.second));
    _tree.erase(mx.first);

    return ret;
}

#endif