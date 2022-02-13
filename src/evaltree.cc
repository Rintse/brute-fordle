#include "evaltree.h"

#include <memory>
#include <numeric>
#include <assert.h>
#include <iomanip>

// When there are fewer words left than this 
// number we start listing them out
#define SHOW_LEFT_C 8
// Column width for suggestion table
#define COL_W 15
// Number of suggestions in the table
#define SUGGESTION_C 10


EvalTree::EvalTree(
    std::list<std::string>* wl,
    std::list<std::string>* d, 
    std::string g
) :   
    guessed(g), dict(d), words_left(wl),
    root(std::make_unique<EvalNode>()),
    scores(std::make_unique<scoremap>())
{
    generate_evaluations();
    generate_elims();
}


std::vector<std::unique_ptr<LetterEval>> 
evaluate(std::string guess, std::string word) {
    std::vector<std::unique_ptr<LetterEval>> result;
    std::vector<bool> done(word.length(), false);
    std::map<char,int> places;

    // Correct letters
    for(int i = 0; i < guess.length(); i++) {
        if(guess[i] == word[i]) {
            result.push_back(
                std::make_unique<EvalRight>(i, guess[i])
            );

            word[i] = '.';
            done[i] = true;
        }
    }

    // Somewhere else in the word
    for(int i = 0; i < guess.length(); i++) {
        // Already marked as correct
        if(word[i] == '.') continue;

        size_t location;
        if((location = word.find(guess[i])) != std::string::npos) {
            result.push_back(
                std::make_unique<EvalPlace>(i, guess[i])
            );

            places[guess[i]]++;
            word[location] = ',';
            done[i] = true;
        }
    
    }
    
    // Not in word (except if already marked as a EvalPlace)
    for(int i = 0; i < guess.length(); i++) {
        if(!done[i]) {
            result.push_back(std::make_unique<EvalWrong>(
                i, guess[i], places[guess[i]]
            ));
        }
    }

    return result;
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
    std::vector<std::unique_ptr<LetterEval>>& q
) { 
    root->insert(q, 0); 
}


void EvalTree::generate_elims() {
    assert(dict->size() > 0);
    size_t wlen = dict->front().length();

    // Calculate the leftover wordscounts for each evaluations
    std::cout << "\nCalculating elimininations\n";
    lbar = std::make_unique<LoadingBar>(size());

    auto tmp_dict = *words_left;
    get_e(std::string(wlen, ' '), root.get(), tmp_dict, guessed);

    // Calculate the averages per word
    std::cout << "\nCalculating averages\n";
    lbar = std::make_unique<LoadingBar>(elims.size());

    leftover_scores.clear();
    for(auto &[w,l] : elims) {
        double avg = std::accumulate(l.begin(), l.end(), 0.0) / l.size();

        if( // Easy lookup for leftover words
            words_left->size() <= SHOW_LEFT_C &&
            std::find(words_left->begin(), words_left->end(), w) != 
            words_left->end()
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
    std::list<std::string>& d,
    std::string g
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
        std::string tg(g);

        guess->filter(&td, tg);

        // Build up the word and recurse
        s[guess->idx] = guess->letter;
        get_e(s, next.get(), td, tg);
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
    
    if(words_left->size() < SHOW_LEFT_C) {
        for(auto &word : *words_left) {
            std::cout 
                << std::setw(COL_W) << std::left << word
                << std::setw(COL_W) << std::left << leftover_scores.at(word)
                << std::endl;
        }
    }
    std::cout << "\n";
}

