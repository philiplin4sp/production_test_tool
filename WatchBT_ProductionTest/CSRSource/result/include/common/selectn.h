//
// Functors for selecting parts from pairs. Handy for dealing with STL maps.
//

#ifndef SELECTN_H
#define SELECTN_H

template <class P>
struct select1st
{
    typedef typename P::first_type result_type;
    typedef const typename P::first_type const_result_type;

    result_type &operator()(P &p) const { return p.first; }
    const_result_type &operator()(const P &p) const { return p.first; }
};

template <class P>
struct select2nd
{
    typedef typename P::second_type result_type;
    typedef const typename P::second_type const_result_type;

    result_type &operator()(P &p) const { return p.second; }
    const_result_type &operator()(const P &p) const { return p.second; }
};

#endif
