#ifndef SCOLOGGER_H
#define SCOLOGGER_H

#include "scoengine.h"
#include <string>
#include "common/types.h"
#include "unicode/ichar.h"

class ScoLogger
{
public:
    class Implementation;

    ScoLogger();
    ScoLogger(const ScoLogger &from);
    ScoLogger &operator=(const ScoLogger &from);
    ~ScoLogger();
    
    void restart();
    void disconnect();
    bool enable(const std::string &fileName);
    void disable();
    void setCh(uint16 ch, ScoEngine *scoEngine);
    uint16 getCh() const;
    bool hasCh() const;
private:
    Implementation *pImpl;
};

#endif
