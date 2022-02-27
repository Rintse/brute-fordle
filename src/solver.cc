#include "solver.h"
#include "evaltree.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>

#define COLUMN_W 15
// Number of suggestions in the table
#define SUGG_C 10

// Word guess: collection of letter guesses
WordleSolver::WordleSolver(std::string filename, size_t l, int n) : wlen(l), n_instances(n) {
    // Read in a dictionary file
    read_dict(filename, wlen);
    // Set the guess to nothing
    for(int i = 0; i < n_instances; i++) {
        guessed.push_back(std::string(wlen, '.'));
    }
}


// Reads a sorted dictionary file to use for suggestion
void WordleSolver::read_dict(std::string filename, size_t wlen) {
    dict = std::make_unique<std::list<std::string>>();
    std::ifstream dict_file(filename);

    if(dict_file.is_open()){
        std::copy_if(
            std::istream_iterator<std::string>(dict_file),
            std::istream_iterator<std::string>(), // OEF
            std::back_inserter(*dict),
            // Only copies words of length wlen
            [wlen](const std::string &s) { 
                return s.length() == wlen 
                && std::all_of(
                    s.begin(), s.end(), 
                    [](const char c){ return std::islower(c); }
                ); 
            }
        );
    }

    else { 
        throw std::runtime_error("Error reading file."); 
    }
    
    // We start with all words still in the running
    for(int i = 0; i < n_instances; i++) {
        words_lefts.push_back(
            std::make_unique<std::list<std::string>>(*dict)
        );
    }
}



// Check if done and print something if so
bool WordleSolver::done() const {
    int n_done = 0;

    for(int i = 0; i < n_instances; i++) {
        switch (words_lefts[i]->size()) {
            case 0: 
                std::cout << "Word not in dictionary.\n";
                return true;

            case 1:
                std::cout 
                    << "The word for instance " << i 
                    << " is " << words_lefts[i]->front() << "\n";
                n_done++;
                break;

            default: break;
        }
    }

    if(n_done == n_instances) return true;
    else return false;
}


// TODO: see if we can do something about the code 
// duplicaiton with evaluate(...) in evaltree.cc
void WordleSolver::update(
    const std::string& guess, 
    const std::vector<std::string> marks
) {
    for(int i = 0; i < n_instances; i++) {
        std::vector<std::unique_ptr<const LetterEval>> result;
        std::map<char,int> places;
        for(auto &c : guessed[i]) { places[c]++; }

        if(guess.length() != marks[i].length()) {
            throw std::runtime_error("Word and marks dont have equal length");
        }

        // TODO: can this be done in a prettier way than 3 consecutive loops?
        for(size_t j = 0; j < guess.length(); j++) {
            if(marks[i][j] == 'v') {
                result.push_back(std::make_unique<EvalRight>(j, guess[j]));
                places[guess[j]]++;
                guessed[i][j] = guess[j];
            }
        }
        
        for(size_t j = 0; j < guess.length(); j++) {
            if(marks[i][j] == 'c') {
                result.push_back(std::make_unique<EvalPlace>(j, guess[j]));
                places[guess[j]]++;
            }
        }
        
        for(size_t j = 0; j < guess.length(); j++) {
            if(marks[i][j] == 'x') {
                result.push_back(std::make_unique<EvalWrong>(
                    j, guess[j], places[guess[j]]
                ));
            }
        }

        if(result.size() != wlen)
            throw std::runtime_error("Invalid mark. Use [xcv]");

        for(auto &letter : result) {
            letter->filter(words_lefts[i].get());
        }
    }
}


void WordleSolver::calculate_best_guess() const {
    std::vector<EvalTree> trees;
    for(int i = 0; i < n_instances; i++) {
        trees.emplace_back(words_lefts[i].get(), dict.get(), guessed[i]);
    }


    std::unique_ptr<std::map<std::string, double>> scores;
    scores = std::make_unique<std::map<std::string, double>>();

    std::unique_ptr<std::map<double, std::list<std::string>>> s_map;
    s_map = std::make_unique<std::map<double, std::list<std::string>>>();

    std::vector<std::map<std::string, double>::iterator> its;
    for(auto &tree : trees) {
        its.push_back(tree.w_scores->begin());
    }

    for(auto &w : *dict) {
        for(auto &it : its) {
            if(it->first == w) {
                (*scores)[w] += it->second;
            }

            it++;
        }
        (*scores)[w] /= n_instances;
    }

    for(auto &[w,s] : *scores) {
        (*s_map)[s].push_back(w);
    }


    std::cout 
        << "\n\nBEST GUESSES:\n"
        << std::setw(COLUMN_W) << std::left << "Guessed word" 
        << std::setw(COLUMN_W) << std::left << "Avg words after\n";
    
    int done = 0;
    for(auto &[score, words] : *s_map) {
        for(auto &word : words) {
            std::cout 
                << std::setw(COLUMN_W) << std::left << word
                << std::setw(COLUMN_W) << std::left << score
                << std::endl;

            if(++done >= SUGG_C) return;
        }
    }
}

