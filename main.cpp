#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include "avl_tree.hpp"
#include "priority_queue.hpp"
#include "tests.hpp"

using namespace std;


int main() {


    AVL_Tree<int, string> Tree, Tree2;

    for(int i = 0; i < 15; ++i)
        Tree[i] = std::to_string(i);

    Tree2 = Tree.subtree(7);

//	Tree2 = map<int,string>(Tree2, [](string &s){
//		return "-" + s;
//	});

    Tree2.traversal(Tree2.LRtR, [](const int &a, string &b){
        std::cout << b << ' ';
    });
    cout << '\n';
    Tree.traversal(Tree.RRtL, [](const int &a, string &b){
        std::cout << b << ' ';
    });
    cout << '\n';

    PriorityQueue<string> queue;
    queue.push(10, "iueto");
    queue.push(15, "fndj2");
    queue.push(4321, "reqeq");

    while (!queue.empty()){
        std::cout << queue.pop() << ' ';
    }


    TestFunction<void> functions[] = {
            {"avl_tree_basics", test_avl_tree_basics},
            {"avl_tree_remove", test_avl_tree_remove},
            {"avl_tree_sort", test_avl_tree_sort},
            {"avl_tree_map", test_avl_tree_map},
            {"avl_tree_where", test_avl_tree_where},
            {"avl_tree_reduce", test_avl_tree_reduce},

            {"priority_queue", test_priority_queue}
    };

    run_tests(functions, sizeof(functions) / sizeof (TestFunction<void>));

    return 0;

}