#ifndef SCOLOGGER_IMPL_H
#define SCOLOGGER_IMPL_H

#include "common/countedpointer.h"
#include "thread/critical_section.h"
#include "scologger.h"
#include "scoengine.h"
#include <cstdio>
#include <string>

class ScoLogger::Implementation
{
public:
    Implementation();
    ~Implementation();
    void inc();
    size_t dec();

    void restart();
    void disconnect();
    bool enable(const std::string &fileName);
    void disable();
    void setCh(uint16 ch, ScoEngine *scoEngine);
    uint16 getCh();
    bool hasCh();

    class Callback : public ScoEngine::Callback
    {
    public:
        Callback(CriticalSection &cs, FILE *logFile);
        virtual bool receive(HCISCOPDU pdu);
        void disable();
    private:
        // Reference so as to use ScoLogger::Implemenatations cs.
        CriticalSection &mCs;
        // Using c style io because evc 4 doesn't support streams.
        FILE *logFile;
    };

    class State;

    struct Data
    {
        Data();
        State *state;
        CountedPointer<Callback> logger;
        uint16 ch;
        ScoEngine *scoEngine;
        FILE *logFile;
        size_t count;
        CriticalSection mCs;
    };

    class State
    {
    public:
        virtual void restart(Data &data) {};
        virtual void disconnect(Data &data) {};
        virtual bool enable(Data &data, const std::string &fileName) = 0;
        virtual void disable(Data &data) {};
        virtual void setCh(Data &data, uint16 ch, ScoEngine *scoEngine) {};
        virtual bool hasCh() = 0;
    };

    class Idle : public State
    {
    private:
        static Idle *instance;    
    public:
        static Idle *getInstance();
        virtual bool enable(Data &data, const std::string &fileName);
        virtual void setCh(Data &data, uint16 ch, ScoEngine *scoEngine);
        virtual bool hasCh() {return false;}
    };
    
    class HasFile : public State
    {
    public:
        static HasFile *getInstance();
        virtual bool enable(Data &data, const std::string &fileName);
        virtual void disable(Data &data);
        virtual void setCh(Data &data, uint16 ch, ScoEngine *scoEngine);
        virtual bool hasCh() {return false;}
    private:
        static HasFile *instance;
    };

    class HasCh : public State
    {
    public:
        static HasCh *getInstance();
        virtual void restart(Data &data);
        virtual void disconnect(Data &data);
        virtual bool enable(Data &data, const std::string &fileName);
        virtual bool hasCh() {return true;}
    private:
        static HasCh *instance;
    };

    class HasFileAndCh : public State
    {
    public:
        static HasFileAndCh *getInstance();
        virtual void restart(Data &data);
        virtual void disconnect(Data &data);
        virtual bool enable(Data &data, const std::string &fileName);
        virtual void disable(Data &data);
        virtual bool hasCh() {return true;}
    private:
        static HasFileAndCh *instance;
    };

private:
    Data data;
};

#endif // SCOLOGGER_IMPL_H
