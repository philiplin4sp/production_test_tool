////////////////////////////////////////////////////////////////////////////////
//
//  FILE:        question.cpp
//
//  CLASSES:     Question
//               QuestionImplemenation
//               TerminalQuestion
//               PreformedQuestion
//
//  PURPOSE:     Define a class for describing systems of leading
//               questions, intended to discriminate between certain "terminal"
//               answers, and a class to present the multiple choices.
//
////////////////////////////////////////////////////////////////////////////////

#include "question.h"
#include <algorithm>
#include <map>

//  Smart pointer class

Question::Question( QuestionImplementation * x )
: p (x) {}
Question::Question( const QuestionImplementation& x )
: p (x.clone()) {}
Question::Question( const Question& x )
: p (x.p) {}
Question& Question::operator= ( const Question& x )
{ p = x.p; return *this; }
std::string Question::ask() const
{ return p->ask(); }
Question::MenuPairIt Question::replies_begin() const
{ return p->replies_begin(); }
Question::MenuPairIt Question::replies_end() const
{ return p->replies_end(); }
const Question Question::follow_on (const std::string& reply_key) const
{ return p->follow_on(reply_key); }
bool Question::is_terminal() const
{ return p->is_terminal(); }
Question::operator bool() const
{ return p; }


//  Base class.
QuestionImplementation::QuestionImplementation( const std::string& text_to_ask )
: text ( text_to_ask ) {}
QuestionImplementation::QuestionImplementation( const QuestionImplementation& from )
: text (from.text)
{}
void QuestionImplementation::operator= ( const QuestionImplementation& from )
{ text = from.text; }
QuestionImplementation::~QuestionImplementation()
{}
std::string QuestionImplementation::ask() const
{ return text; }
bool QuestionImplementation::is_terminal() const
{ return ask() == terminal.ask(); }

//  Terminal class
TerminalQuestion::TerminalQuestion()
: QuestionImplementation ("*TERMINAL*") {}
TerminalQuestion::~TerminalQuestion()
{}
QuestionImplementation * TerminalQuestion::clone() const
{return new TerminalQuestion;}
Question::MenuPairIt TerminalQuestion::replies_begin() const
{std::list<MenuPair> x;return x.begin();}
Question::MenuPairIt TerminalQuestion::replies_end() const
{std::list<MenuPair> x;return x.end();}
const Question TerminalQuestion::follow_on ( const std::string& reply_key ) const
{return 0;}

// instantiate terminal class;
const Question terminal (new TerminalQuestion);

// preformed questions.
struct PreformedQuestion::impl
{
    typedef std::map<std::string,Question> map;
    std::list<MenuPair> possible_answers;
    map next;
};

PreformedQuestion::PreformedQuestion ( const std::string& q_text )
: QuestionImplementation ( q_text ) , p ( new impl ) {}
PreformedQuestion::~PreformedQuestion ()
{ delete p; }
PreformedQuestion::PreformedQuestion(const PreformedQuestion& from )
: QuestionImplementation (from) , p ( new impl )
{ initialise(from); }
void PreformedQuestion::operator=(const PreformedQuestion& from)
{ delete p; p = new impl; initialise(from); }
QuestionImplementation * PreformedQuestion::clone() const
{return new PreformedQuestion(*this);}

//  get array of possible answers
Question::MenuPairIt PreformedQuestion::replies_begin() const
{
    return p->possible_answers.begin();
}
Question::MenuPairIt PreformedQuestion::replies_end() const
{
    return p->possible_answers.end();
}

//  get the next question to ask, depending on the chosen answer.
//  returns zero pointer if the reply is not valid, or
//  the "terminal" Question if the answer is a terminal node.
const Question PreformedQuestion::follow_on ( const std::string& reply_key ) const
{
    //  use find to avoid adding keys to the map.
    impl::map::const_iterator i = p->next.find(reply_key);
    if( i != p->next.end() )
        return (i->second);
    return 0;
}

bool PreformedQuestion::add_answer ( const MenuPair& x,
                                     const Question toCopy )
{
    //  if the key is unique...
    if ( p->next.find(x.key()) == p->next.end() )
    {
        p->possible_answers.push_back ( x );
        p->next.insert(impl::map::value_type(x.key(),toCopy));
        return true;
    }
    return false;
}

bool PreformedQuestion::add_answer ( const std::string& key,
                                     const std::string& display,
                                     const Question toCopy )
{
    return add_answer( MenuPair ( key, display ) , toCopy );
}

class KeyMatch
{
public:
    KeyMatch ( const std::string& s )
    : str ( s ) {}
    bool operator() ( const MenuPair& i ) const
    { return str == i.key(); }
private:
    const std::string str;
};

void PreformedQuestion::remove_answer ( const std::string& key )
{
    impl::map::iterator i = p->next.find(key);
    if ( i != p->next.end() )
        p->next.erase(i);
    std::list<MenuPair>::iterator j = std::find_if ( p->possible_answers.begin(),
                                  p->possible_answers.end(),
                                  KeyMatch ( key ) );
    if ( j != p->possible_answers.end() )
        p->possible_answers.erase(j);
}

void PreformedQuestion::initialise( const PreformedQuestion& from )
{
    for ( MenuPairIt i = from.replies_begin();
          i != from.replies_end() ; ++i )
    {
        const Question f = from.follow_on(i->key());
        q_del_on_destr.push_back(f);
        add_answer(*i,f);
    }
}

//  A question made by splicing together two questions.
JoinedQuestion::JoinedQuestion ( const Question& a, const Question& b )
: QuestionImplementation (a.ask()), one(a), two(b)
{
    items.insert(items.end(),one.replies_begin(),one.replies_end());
    items.insert(items.end(),two.replies_begin(),two.replies_end());
}

QuestionImplementation * JoinedQuestion::clone() const
{ return new JoinedQuestion (one,two); }
Question::MenuPairIt JoinedQuestion::replies_begin() const
{ return items.begin(); }
Question::MenuPairIt JoinedQuestion::replies_end() const
{ return items.end(); }
const Question JoinedQuestion::follow_on (const std::string& reply_key) const
{
    Question rv = one.follow_on(reply_key);
    if (!rv)
        rv = two.follow_on(reply_key);
    return rv;
}

