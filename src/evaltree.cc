#include "evaltree.h"

#include <memory>
#include <numeric>
#include <assert.h>
#include <iomanip>
#include <unordered_map>

// When there are fewer words left than this 
// number we start listing them out
#define SHOW_LEFT_C 8
// Column width for suggestion table
#define COL_W 15
// Number of suggestions in the table
#define SUGGESTION_C 10
// Number of evaluations to work out before inserting
#define BATCH_C

EvalTree::EvalTree(
    std::list<std::string>* wl,
    std::list<std::string>* d, 
    std::string g
) :   
    guessed(g), dict(d), words_left(wl),
    wlen(d->front().length()),
    root(std::make_unique<EvalNode>()),
    scores(std::make_unique<scoremap>())
{
    generate_evaluations();
    generate_elims();
}


std::vector<std::unique_ptr<const LetterEval>> 
EvalTree::evaluate(const std::string& guess, std::string word) {
    std::vector<std::unique_ptr<const LetterEval>> res;
    res.reserve(wlen);

    size_t loc;

    std::map<char,int> allow;
    for(auto &c : guessed) { allow[c]++; }

    std::vector<int> todo;
    todo.reserve(wlen);

    for(size_t i = 0; i < wlen; i++) {
        // Correct letter
        if(guess[i] == word[i]) {
            res.emplace_back(std::make_unique<EvalRight>(i, guess[i]));
            allow[guess[i]]++;
            word[i] = '.';
        }

        // Wrong position
        else if((loc = word.find(guess[i])) != std::string::npos) {
            res.emplace_back(std::make_unique<EvalPlace>(i, guess[i]));
            allow[guess[i]]++;
            word[loc] = '.';
        }

        // Not in word, fill in later
        else { todo.push_back(i); }
    }

    for(auto &i : todo) {
        res.emplace_back(
            std::make_unique<EvalWrong>(i, guess[i], allow[guess[i]])
        );
    }

    return res;
}


// Calculates all possible evaluations and stores
// them in a compressed (and sorted) tree container
void EvalTree::generate_evaluations() {
    std::cout << "\nCalculating all possible evaluations\n";
    lbar = std::make_unique<LoadingBar>(dict->size()*words_left->size());

    for(auto &guess : *dict) {
        for(auto &word : *words_left) {
            auto evaluation = evaluate(guess, word);
            insert(evaluation);
        }

        lbar->inc(words_left->size());
    }
}


// Recusrive wrapper
size_t EvalTree::size() const { 
    return root->size(); 
}


// Recusrive wrapper
void EvalTree::print() const { 
    std::cout << "\n"; 
    root->print(0); 
}


// Recusrive wrapper
void EvalTree::insert(
    std::vector<std::unique_ptr<const LetterEval>>& q
) { 
    root->insert(q, 0); 
}


// Writing this out mid-function is too ugly and hinders readability imo
bool list_contains(
    const std::list<std::string>* l, 
    const std::string& s
) {
    return std::find(l->begin(), l->end(), s) != l->end();
}


void EvalTree::generate_elims() {
    assert(dict->size() > 0);
    size_t wlen = dict->front().length();

    // Calculate the leftover wordscounts for each evaluations
    std::cout << "\nCalculating elimininations\n";
    lbar = std::make_unique<LoadingBar>(size());

    auto tmp_dict = *words_left;
    get_e(std::string(wlen, ' '), root.get(), tmp_dict);

    // Calculate the averages per word
    std::cout << "\nCalculating averages\n";
    lbar = std::make_unique<LoadingBar>(elims.size());

    leftover_scores.clear();
    for(auto &[w,l] : elims) {
        const double avg = 
            std::accumulate(l.begin(), l.end(), 0.0)
            / l.size();

        if( // Easy lookup for leftover words
            words_left->size() <= SHOW_LEFT_C &&
            list_contains(words_left, w)
        ) { 
            leftover_scores[w] = avg; 
        }

        // Uses a map (score->word) such that scores are sorted upon insertion
        (*scores)[avg].push_back(w);
        lbar->inc();
    }
}


void EvalTree::get_e(
    std::string s, 
    const EvalNode* cur,
    const std::list<std::string>& d
) {
    // End of recursion, store result
    if(cur->children.empty()) {
        // This is where the compression happens:
        // We have tracked how often this path occurs in the
        // multiplicity field of the leaf. We can now simply
        // add the result of this path that number of times.
        for(uint32_t i = 0; i < cur->multiplicity; i++) {
            elims[s].push_back(d.size());
        }

        lbar->inc(cur->multiplicity);
        return;
    }

    // Recurse over all children
    for(auto &[guess, next] : cur->children) {
        // We need a copies to filter down
        std::list<std::string> td(d);

        guess->filter(&td);

        // Build up the word and recurse
        s[guess->idx] = guess->letter;
        get_e(s, next.get(), td);
    }
}


// Suggest the most useful guesses
void EvalTree::best_guesses() const {
    std::cout 
        << "\n\nBEST GUESSES:\n"
        << std::setw(COL_W) << std::left << "Guessed word" 
        << std::setw(COL_W) << std::left << "Avg words after\n";
    
    int done = 0;
    for(auto &[score, words] : *scores) {
        for(auto &word : words) {
            std::cout 
                << std::setw(COL_W) << std::left << word
                << std::setw(COL_W) << std::left << score
                << std::endl;

            if(++done >= SUGGESTION_C) return;
        }
    }
}


// Show the current status of the game
void EvalTree::show_remaining() const {
    std::cout 
        << "\nGuessed so far: [" << guessed << "]\n\n"
        << "POSSIBLE WORDS: " << words_left->size() << "\n";

    if(words_left->size() <= SHOW_LEFT_C) {
        std::cout
            << std::setw(COL_W) << std::left << "Guessed word" 
            << std::setw(COL_W) << std::left << "Avg words after\n";
    }
    
    if(words_left->size() <= SHOW_LEFT_C) {
        for(auto &word : *words_left) {
            std::cout 
                << std::setw(COL_W) << std::left << word
                << std::setw(COL_W) << std::left << leftover_scores.at(word)
                << std::endl;
        }
    }
    std::cout << "\n";
}

