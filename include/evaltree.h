// The WordTree class wraps the WordNode class' 
// recursive functions, as well as keeping track 
// of some extra information in its member variables

#ifndef wordtree_h
#define wordtree_h

#include "bar.h"
#include "evalnode.h"

#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <list>

using scoremap = std::map<double, std::list<std::string>>;
using smap = std::map<std::string, double>;


class EvalTree {
    public:
        EvalTree(
            std::list<std::string>* wl,     // list of remaining words
            std::list<std::string>* d,      // complete dictionary
            std::string g                   // letters we got so far
        );
        
        EvalTree(EvalTree&& e);
        
        // Returns the amount of leaf nodes in the tree
        size_t size() const;

        // Prints entire tree
        // Increases indentaion of nodes as depth increases
        void print() const;
       
        // Inserts a wordguess into the tree
        // Increases the final leaf's multiplicity
        // field if it already exists
        void insert(std::vector<std::unique_ptr<const LetterEval>>& q);

        // Sorts guesses on how many words are left and shows the best
        void best_guesses() const;

        // Shows the current guess, and how many remaining words 
        // there are. If there are less than SHOW_LEFT_C (in .cc), 
        // also prints the words and how many words are left if guessed
        void show_remaining() const;

        std::unique_ptr<smap> w_scores;
        // All the words still in the running
        std::list<std::string>* words_left;
        // Easy lookup for leftover word scores
        std::map<std::string, double> leftover_scores;
    private:
        // Generates an evaluation of a guess against a word
        std::vector<std::unique_ptr<const LetterEval>>
        evaluate(const std::string& guess, std::string word);

        // Generates all possible evaluations
        void generate_evaluations();

        // For all these evalutions, calculates how
        // many words are left after applying. Calls get_e
        void generate_elims();
        
        // Walks though all paths in the tree, calculating
        // how many words are left in the dictionary if the 
        // guesses that said path represents would be made
        void get_e(
            std::string s, 
            const EvalNode* cur, 
            const std::list<std::string>& d
        );
        
        // Holds the letters we already know are right
        const std::string guessed;
        // The entire dictionary
        const std::list<std::string>* dict;
        const int wlen;
        
        // Root of the tree
        std::unique_ptr<EvalNode> root;

        // For each word, we keep a list of how many words
        // are left after filtering each evalution
        std::map<std::string, std::list<size_t>> elims;
        // The average of elims, in an inverted map structure
        std::unique_ptr<scoremap> scores;
        
        // Loading bar for get_e 
        // (avoids having to pass it in the recursion)
        std::unique_ptr<LoadingBar> lbar;
};

#endif
