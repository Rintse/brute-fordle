#include "evalnode.h"

#include <memory>
#include <numeric>

EvalNode::EvalNode() { multiplicity = 1; }


size_t EvalNode::size() const {
    if(children.empty()) return multiplicity;
   
    return std::accumulate(
        children.begin(), children.end(), 0,
        [](size_t acc, auto &c) { return acc += c.second->size(); }
    );
}


void EvalNode::print(const int depth) const {
    for(auto &[g,c] : children) {
        // Indent to the depth level
        std::cout << std::string(4*depth, ' ');
        std::cout << *g;
        std::cout << std::endl;

        // Recurse with increaded depth
        c->print(depth+1);
    }
}


void EvalNode::insert(
    std::vector<std::unique_ptr<LetterEval>>& q, 
    const size_t idx
) {
    if(idx == q.size()) return;

    const auto &[child,inserted] = children.emplace(
        std::move(q.at(idx)), std::make_unique<EvalNode>()
    );
    const auto &[_, node] = *child;

    // This is a duplicate leaf inc multiplicity
    if(!inserted && node->children.empty()) { 
        node->multiplicity++; 
    }

    node->insert(q, idx+1);
}

