#ifndef STAGECONTROLLER_H
#define STAGECONTROLLER_H

#include <iostream>
#include <string>
#include <strstream>
#include <ahm.h>  // include AHM header
#include <ahwbasic.h>
#include <ahwmic.h>
#include <ahwmicpropid.h>
#include <ahwprop2.h>
#include <reuse/proptools.h>
#include <windows.h>


using namespace ahm;

class MTLock {
public:
    MTLock() {
        ::InitializeCriticalSection(&m_cs);
    }
    void lock() {
        ::EnterCriticalSection(&m_cs);
    }
    void unlock() {
        ::LeaveCriticalSection(&m_cs);
    }

private:
    CRITICAL_SECTION m_cs;
};

class MTSynchBlock {
public:
    MTSynchBlock(MTLock &lock) : m_lock(lock) {
        m_lock.lock();
    }
    ~MTSynchBlock() {
        m_lock.unlock();
    }
private:
    MTLock& m_lock;
};


#define MT_SYNCHRONIZED_BLOCK(lock)  MTSynchBlock __synch_block_temp__(lock);

class Base {
public:
    Base(ahm::Logging *pLogging) : m_pLogging(pLogging) {}
    void addLogText(iop::string szText);
private:
    ahm::Logging *m_pLogging;
};












#endif // STAGECONTROLLER_H
