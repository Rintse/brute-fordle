#include "lguess.h"
#include <iostream>

#define VERBOSE false
        
bool LetterEval::operator <(const LetterEval &g) const { 
    if(type == g.type) {
        if(idx == g.idx) {
            return letter < g.letter;
        }
        return idx < g.idx;
    }
    return type < g.type;
}

std::ostream& operator << (std::ostream& s, const LetterEval& l) {
    l.print(s);
    return s;
}


void EvalRight::filter(
    std::list<std::string>* dict,
    std::string &got
) const 
{
    if(VERBOSE) {
        std::cerr 
            << "removing words that do not contain " 
            << letter 
            << " at position "
            << (int) idx 
            << std::endl ;
    }

    std::erase_if(*dict, 
        [this](const std::string &s){ return s[idx] != letter; }
    );
    
    got[idx] = letter;
} 
        
std::ostream& EvalRight::print(std::ostream& s) const {
    s << "Right, " << (int) idx << " , " << letter;
    return s;
}


void EvalPlace::filter(
    std::list<std::string>* dict,
    std::string &got
) const 
{
    if(VERBOSE) {
        std::cerr 
            << "removing words that dont contain " 
            << letter
            << " and words that contain it at position"
            << (int) idx
            << std::endl ;
    }

    std::erase_if(*dict, 
        [this](const std::string &s){ 
            return s.find(letter) == std::string::npos
            || s[idx] == letter; 
        }
    );
} 

std::ostream& EvalPlace::print(std::ostream& s) const {
    s << "Place, " << (int) idx << " , " << letter;
    return s;
}


void EvalWrong::filter(
    std::list<std::string>* dict,
    std::string &got
) const 
{
    if(VERBOSE) {
        std::cerr 
            << "removing words that contain " 
            << letter 
            << " in an unguessed position"
            << std::endl;
    }

    std::erase_if(*dict,
        [this, got](const std::string &s){ 
            int found = 0;

            for(int i = 0; i < s.length(); i++) {
                if(s[i] == letter && got[i] == '.') {
                    found++;
                }
            }

            if (found > allow) return true;
            else return false;
        }
    );
}

std::ostream& EvalWrong::print(std::ostream& s) const {
    s << "Wrong, " << (int) idx << " , " << letter;
    if(allow != 0) { s << " (allow " << allow << ")"; }
    return s;
        }
