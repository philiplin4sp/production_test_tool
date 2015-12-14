#include "scoengine.h"
#include "scologger.h"
#include "scologger_impl.h"
#include <cstdio>

ScoLogger::ScoLogger()
    : pImpl(new Implementation()) 
{
    
}

ScoLogger::ScoLogger(const ScoLogger &from)
    : pImpl(from.pImpl)
{
    pImpl->inc();
}

ScoLogger &ScoLogger::operator=(const ScoLogger &from)
{
    pImpl->inc();
    pImpl = from.pImpl;
    return *this;
}

ScoLogger::~ScoLogger()
{
    Implementation *impl = pImpl;
    pImpl = 0;
    if (!impl->dec())
    {
        delete pImpl;
    }
}

void ScoLogger::restart()
{
    pImpl->restart();
}

void ScoLogger::disconnect()
{
    pImpl->disconnect();
}

bool ScoLogger::enable(const std::string &fileName)
{
    return pImpl->enable(fileName);
}

void ScoLogger::disable()
{
    pImpl->disable();
}

void ScoLogger::setCh(uint16 ch, ScoEngine *scoEngine)
{
    pImpl->setCh(ch, scoEngine);
}

uint16 ScoLogger::getCh() const
{
    return pImpl->getCh();
}

bool ScoLogger::hasCh() const
{
    return pImpl->hasCh();
}

ScoLogger::Implementation::Implementation()
{

}

ScoLogger::Implementation::~Implementation()
{
    disconnect();
    disable();
}

void ScoLogger::Implementation::inc()
{
    CriticalSection::Lock lock(data.mCs);
    ++data.count;
}

size_t ScoLogger::Implementation::dec()
{
    CriticalSection::Lock lock(data.mCs);
    return --data.count;

}

void ScoLogger::Implementation::restart()
{
    CriticalSection::Lock lock(data.mCs);
    data.state->restart(data);
}

void ScoLogger::Implementation::disconnect()
{
    CriticalSection::Lock lock(data.mCs);
    data.state->disconnect(data);
}

bool ScoLogger::Implementation::enable(const std::string &fileName)
{
    CriticalSection::Lock lock(data.mCs);
    return data.state->enable(data, fileName);
}

void ScoLogger::Implementation::disable()
{
    CriticalSection::Lock lock(data.mCs);
    data.state->disable(data);
}

void ScoLogger::Implementation::setCh(uint16 ch, ScoEngine *scoEngine)
{
    CriticalSection::Lock lock(data.mCs);
    data.state->setCh(data, ch, scoEngine);
}

uint16 ScoLogger::Implementation::getCh()
{
    CriticalSection::Lock lock(data.mCs);
    return data.ch;
}

bool ScoLogger::Implementation::hasCh()
{
    CriticalSection::Lock lock(data.mCs);
    return data.state->hasCh();
}

ScoLogger::Implementation::Callback::Callback(CriticalSection &cs, 
                                              FILE *logFile)
    : mCs(cs),
      logFile(logFile)
{

}

bool ScoLogger::Implementation::Callback::receive(HCISCOPDU pdu)
{
    CriticalSection::Lock lock(mCs);

    if (!logFile)
    {
        // Callback has been disabled
        return false;
    }

    return fwrite(pdu.get_dataPtr(), 
                  sizeof(uint8), 
                  pdu.get_length(), 
                  logFile) == pdu.get_length();
}

void ScoLogger::Implementation::Callback::disable()
{
    logFile = 0;    
}

ScoLogger::Implementation::Data::Data() 
    : state(Idle::getInstance()),
      logger(0),
      ch(0),
      scoEngine(0),
      logFile(0),
      count(1)
{

}

ScoLogger::Implementation::Idle idle;

ScoLogger::Implementation::Idle *
    ScoLogger::Implementation::Idle::getInstance()
{
    return &idle;
}

bool ScoLogger::Implementation::Idle::enable(Data &data, 
                                             const std::string &fileName)
{
    FILE *logFile = fopen(fileName.c_str(), "a+b");

    if (!logFile)
    {
        // Couldn't open file. Do not change state return failure.
        return false;
    }

    data.logFile = logFile;
    data.state = HasFile::getInstance();
    return true;
}

void ScoLogger::Implementation::Idle::setCh(Data &data, 
                                            uint16 ch, 
                                            ScoEngine *scoEngine)
{
    data.ch = ch;
    data.scoEngine = scoEngine;
    data.state = HasCh::getInstance();
}

ScoLogger::Implementation::Idle *ScoLogger::Implementation::Idle::instance = new Idle();

ScoLogger::Implementation::HasFile *
    ScoLogger::Implementation::HasFile::getInstance()
{
    return instance;
}

bool ScoLogger::Implementation::HasFile::enable(Data &data, 
                                                const std::string &fileName)
{
    // Already enabled.
    return false;
}

void ScoLogger::Implementation::HasFile::disable(Data &data)
{
    FILE *logFile = data.logFile;
    data.logFile = 0;
    fclose(logFile);
    data.state = Idle::getInstance();
}

void ScoLogger::Implementation::HasFile::setCh(Data &data, 
                                               uint16 ch, 
                                               ScoEngine *scoEngine)
{
    Callback *callback = new ScoLogger::Implementation::Callback(data.mCs, data.logFile);
    CountedPointer<Callback> tmp(callback);
    scoEngine->setCallback(ch, callback);
    data.scoEngine = scoEngine;
    data.ch = ch;
    data.logger = tmp;
    data.state = HasFileAndCh::getInstance();
}

ScoLogger::Implementation::HasFile *ScoLogger::Implementation::HasFile::instance = new HasFile();

ScoLogger::Implementation::HasCh *ScoLogger::Implementation::HasCh::getInstance()
{
    return instance;
}

void ScoLogger::Implementation::HasCh::restart(Data &data)
{
    data.scoEngine = 0;
    data.ch = 0;
    data.state = Idle::getInstance();
}

void ScoLogger::Implementation::HasCh::disconnect(Data &data)
{
    restart(data);
}

bool ScoLogger::Implementation::HasCh::enable(Data &data, const std::string &fileName)
{
    FILE *logFile = fopen(fileName.c_str(), "a+b");

    if (!logFile)
    {
        // Couldn't open file. Do not change state return failure.
        return false;
    }
    
    Callback *callback = new Callback(data.mCs, logFile);
    CountedPointer<Callback> tmp(callback);

    if (!data.scoEngine->setCallback(data.ch, callback))
    {
        fclose(logFile);
        return false;
    }
    
    data.logger = tmp;
    data.logFile = logFile;
    data.state = HasFileAndCh::getInstance();
    return true;
}

ScoLogger::Implementation::HasCh *
    ScoLogger::Implementation::HasCh::instance = new HasCh();

ScoLogger::Implementation::HasFileAndCh *
    ScoLogger::Implementation::HasFileAndCh::getInstance()
{
    return instance;
}

void ScoLogger::Implementation::HasFileAndCh::restart(Data &data)
{
    data.scoEngine->unsetCallback(data.ch);
    data.logger = 0;
    data.state = HasFile::getInstance();
}

void ScoLogger::Implementation::HasFileAndCh::disconnect(Data &data)
{
    restart(data);
}

bool ScoLogger::Implementation::HasFileAndCh::enable(
                                                Data &data, 
                                                const std::string &fileName)
{
    return false;
}

void ScoLogger::Implementation::HasFileAndCh::disable(Data &data)
{
    data.logger->disable();
    data.logger = 0;
    data.scoEngine->unsetCallback(data.ch);
    FILE *logFile = data.logFile;
    data.logFile = 0;
    fclose(logFile);
    data.state = HasCh::getInstance();
}

ScoLogger::Implementation::HasFileAndCh *
    ScoLogger::Implementation::HasFileAndCh::instance = new HasFileAndCh();
