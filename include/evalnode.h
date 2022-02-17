#ifndef wordnode_h
#define wordnode_h

#include "lguess.h"

#include <memory>
#include <vector>
#include <map>
#include <functional>

// We want the children map to be sorted by LetterEval's 
// < operator, so we have to supply a lambda to the map 
// declaration that sorts using the dereferenced elements
const auto wnode_cmp = [](
    const std::unique_ptr<const LetterEval> &l1, 
    const std::unique_ptr<const LetterEval> &l2
) {
    return *l1 < *l2; 
};


class EvalNode {
    public:
        EvalNode();

        size_t size() const;
        void print(const int depth) const;
        void insert(
            std::vector<std::unique_ptr<const LetterEval>>& q, 
            const size_t idx
        );
        
        std::map<
            const std::unique_ptr<const LetterEval>,
            const std::unique_ptr<EvalNode>,
            decltype(wnode_cmp)
        > children;
        
        uint32_t multiplicity;
};

#endif
