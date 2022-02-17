#ifndef lguess_h
#define lguess_h

#include <string>
#include <list>
#include <iostream>

#define LGUESS_RIGHT 0
#define LGUESS_PLACE 1
#define LGUESS_WRONG 2

// Purely virutal base class for a letter class
class LetterEval { 
    public:
        LetterEval(int i, char l) : idx(i), letter(l) {}
        virtual ~LetterEval() {}

        virtual void filter(
            std::list<std::string>* const dict
        ) const = 0;

        virtual std::ostream& print(std::ostream& s) const = 0;
        bool operator <(const LetterEval &g) const;
        
        // This is so sad, why can't I
        // perform polymorphic comparisons?
        char type; 
        const int idx;
        const char letter; 
};

// Make letterguesses streamable
std::ostream& operator << (std::ostream& s, const LetterEval& l);


// This letter was guessed right
class EvalRight : public LetterEval {
    public:
        EvalRight(char l, int i) : LetterEval(l,i) { 
            type = LGUESS_RIGHT; 
        }

        ~EvalRight(){}
        
        void filter(
            std::list<std::string>* const dict
        ) const; 

        std::ostream& print(std::ostream& s) const;

};

// This letter was guessed in the wrong position
class EvalPlace : public LetterEval {
    public:
        EvalPlace(char l, int i) : LetterEval(l,i) { 
            type = LGUESS_PLACE; 
        }

        ~EvalPlace(){}
        
        void filter(
            std::list<std::string>* const dict
        ) const; 

        std::ostream& print(std::ostream& s) const;
};

// This letter was guessed wrong (not in word)
class EvalWrong : public LetterEval {
    public:
        EvalWrong(char l, int i) : LetterEval(l,i), allow(0) { 
            type = LGUESS_WRONG; 
        }
        EvalWrong(char l, int i, int a) : LetterEval(l,i), allow(a) { 
            type = LGUESS_WRONG;
        }

        ~EvalWrong(){}
        
        void filter(
            std::list<std::string>* const dict
        ) const; 

        std::ostream& print(std::ostream& s) const;

        // The amount of "letter"s to allow
        const int allow;
};

#endif
