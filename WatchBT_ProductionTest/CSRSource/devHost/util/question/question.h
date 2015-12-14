////////////////////////////////////////////////////////////////////////////////
//
//  FILE:        question.h
//
//  CLASSES:     Question
//               MenuPair
//
//  PURPOSE:     Declare a class for describing systems of leading
//               questions, intended to discriminate between certain "terminal"
//               answers, and a class to present the multiple choices.
//
//               An answer is just the list of keys of the answers given.
//
//  NOTES:
//  
//  Question implements a hierachy of multiple choice questions, where the next
//  question depends on the answer to the previous one.
//  
//  Eg, in the case of transports, the structure might look like this.
//  
//  Q: Which Transport?
//  |
//  +- A: SPI
//  |   |
//  |  Q: Which lpt port?
//  |   |
//  |   +- A: lpt1
//  |   |
//  |   +- A: lpt2
//  |
//  +- A: BCSP
//      |
//     Q: Which com port?
//      |
//      +- A: com1
//      |
//      +- A: com2
//  
//  The menu of possible answers (eg, [SPI, BCSP] or [com1, com2]) to a given
//  question are given in the form of an iterator by the replies_begin() and
//  replies_end() methods. The follow-on questions corresponding to each answer
//  are given by the follow_on method.
//  
//  An Answer is simply a list of the keys of the individual answers. For
//  example, an answer specifying BCSP on com2 would be [BCSP, com2]. Methods
//  are provided to serialise and deserialise answers. 
//  
////////////////////////////////////////////////////////////////////////////////

#ifndef UTIL_QUESTION_H
#define UTIL_QUESTION_H

#ifdef WIN32
#pragma warning ( disable : 4786)
#endif
#include <string>
#include <list>
#ifndef _WIN32_WCE
#include <iosfwd>
#endif
#include "common/countedpointer.h"

// A possible answer to a question. mkey is the internal representation and
// mdisplay is the name to display to users.
class MenuPair
{
public:
    MenuPair ( const std::string& key_text , const std::string& display_text ) 
    : mkey ( key_text ) , mdisplay ( display_text ) {}
    MenuPair ( const std::string& text )
    : mkey ( text ) , mdisplay ( text ) {}
    std::string key() const { return mkey; }
    std::string display() const { return mdisplay; }
private:
    const std::string mkey;
    const std::string mdisplay;
};

class QuestionImplementation;

//  Smart pointer to the real thing.
class Question
{
public:
    typedef std::list<MenuPair>::const_iterator MenuPairIt;
    
    //  Question is a smart pointer to QuestionImplementation
    //  take responsibility for the memory,
    Question( QuestionImplementation * p = 0 );
    //  clone,
    Question( const QuestionImplementation& );
    //  copy,
    Question( const Question& );
    //  or assign.
    Question& operator= ( const Question& );

    // Returns the text of this question
    std::string ask() const;
    
    // Collection of possible answers to this question
    MenuPairIt replies_begin() const;
    MenuPairIt replies_end() const;
    
    // Given an answer to this question with a key of reply_key:
    // Returns NULL pointer (ie when cast to bool, the question is false)
    // if the key is not valid.
    // Returns a pointer to a terminal Question if no further questions are
    // necessary.
    // Returns a pointer to the next, non-terminal, question otherwise.
    const Question follow_on (const std::string& reply_key) const;

    // Indicates that this question should not be asked.
    bool is_terminal() const;
    // returns false if the pointer is NULL
    operator bool() const;
private:
    CountedPointer<QuestionImplementation> p;
};

class QuestionImplementation : public Counter
{
public:
    typedef Question::MenuPairIt MenuPairIt;
    
    QuestionImplementation( const std::string& text_to_ask );
    //  destructor deletes all entries in q_del_on_destr.
	virtual ~QuestionImplementation();

    // get a new one - use delete to clear it up.
    virtual QuestionImplementation * clone() const = 0;

    // Returns the text of this question
    virtual std::string ask() const;
    
    // Collection of possible answers to this question
    virtual MenuPairIt replies_begin() const = 0;
    virtual MenuPairIt replies_end() const = 0;
    
    // Given an answer to this question with a key of reply_key:
    // Returns NULL pointer if the key is not valid.
    // Returns a pointer to a terminal Question if no further questions are
    // necessary.
    // Returns a pointer to the next, non-terminal, question otherwise.
    virtual const Question follow_on (const std::string& reply_key) const=0;

    // Indicates that this question should not be asked.
    bool is_terminal() const;
protected:
    //  keep a list of questions whose lifetimes must exceed this
    //  QuestionImplementation.
    //  Usually these will be the follow-on Questions.
    std::list<Question> q_del_on_destr;
    // copy and assignment leave q_del_on_destr empty.
    QuestionImplementation( const QuestionImplementation& );
    void operator= ( const QuestionImplementation& );
private:
    std::string text;
};


//  The question which should never be asked!!
class TerminalQuestion : public QuestionImplementation
{
public:
    TerminalQuestion();
    ~TerminalQuestion();
private:
    QuestionImplementation * clone() const;
    MenuPairIt replies_begin() const;
    MenuPairIt replies_end() const;
    const Question follow_on ( const std::string& reply_key ) const;
};

extern const Question terminal;

//  questions where the world is laid out clearly from the start
//  Works well for bottom up construction of "questionaires".
//  Lousy for top down questions.
class PreformedQuestion : public QuestionImplementation
{
public:
    //  typedef std::list<MenuPair>::iterator MenuPairIt;
    
	PreformedQuestion ( const std::string& q_text );
	~PreformedQuestion();
	PreformedQuestion(const PreformedQuestion&);
	void operator=(const PreformedQuestion&);
    QuestionImplementation* clone() const;

    // Collection of possible answers to this question
    MenuPairIt replies_begin() const;
    MenuPairIt replies_end() const;
    
    // Given an answer to this question with a key of reply_key:
    // Returns NULL pointer if the key is not valid.
    // Returns a pointer to a terminal Question if no further questions are
    // necessary.
    // Returns a pointer to the next, non-terminal, question otherwise.
    const Question follow_on ( const std::string& reply_key ) const;

    // Construction methods for building questions
	bool add_answer ( const std::string& key,
                      const std::string& display,
                      const Question follow_on );
	bool add_answer ( const MenuPair&, const Question follow_on );
    void remove_answer ( const std::string& key );

	bool add_answer ( const std::string& key, const std::string& display )
    { return add_answer(key, display, terminal); }

	bool add_answer ( const MenuPair& menu_pair )
    { return add_answer(menu_pair, terminal); }

    // Implementation
    struct impl;
private:
    // work for copy and assignment done here
    void initialise ( const PreformedQuestion& );
    impl * p;
};


//  A question made by splicing together two questions.
//  The text of the first is asked.
//  The replies are the union of the two questions (including duplicates)
//  and the follow_on from the first question will be used if there
//  is duplication of keys.
class JoinedQuestion : public QuestionImplementation
{
public:
    JoinedQuestion ( const Question&, const Question& );

    QuestionImplementation * clone() const;
    MenuPairIt replies_begin() const;
    MenuPairIt replies_end() const;
    const Question follow_on (const std::string& reply_key) const;
private:
    Question one;
    Question two;
    std::list<MenuPair> items;
};

#endif /*UTIL_QUESTION_H*/

