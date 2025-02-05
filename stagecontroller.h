#ifndef STAGECONTROLLER_H
#define STAGECONTROLLER_H
#include <Windows.h>
#include <iostream>
#include <string>
#include <strstream>
#include <ahm.h>  // include AHM header
#include <ahwbasic.h>
#include <ahwmic.h>
#include <ahwmicpropid.h>
#include <ahwprop2.h>
#include <reuse/proptools.h>

ahm::Unit *findUnit(ahm::Unit *pUnit, ahm::TypeId typeId);

template <class clazz> clazz* find_itf(ahm::Unit *pUnit, iop::int32 iid) {
    if (pUnit && pUnit->interfaces()) {
        ahm::Interface* pInterface = pUnit->interfaces()->findInterface(iid);
        return pInterface ? (clazz*)pInterface->object() : 0;
    }
    return 0;
}

template <class clazz> clazz* find_itf_version(ahm::Unit *pUnit, iop::int32 iid, iop::int32 minimumVersion) {
    if (pUnit && pUnit->interfaces()) {
        ahm::Interface* pInterface = pUnit->interfaces()->findInterface(iid);
        return pInterface && pInterface->interfaceVersion() >= minimumVersion ? (clazz*)pInterface->object() : 0;
    }
    return 0;
}



class MTLock {
public:
    MTLock() { ::InitializeCriticalSection(&m_cs);
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

class MTPrinter {
public:
    MTPrinter() {
    }
    void print(std::strstream & sstream, bool nl = false);

    void print(const std::string& str, bool nl = false);
private:
    MTLock m_lock;
};

class Base {
public:
    Base(ahm::Logging *pLogging) : m_pLogging(pLogging) {}
    void addLogText(iop::string szText);
private:
    ahm::Logging *m_pLogging;
};

class Axis : Base {

public:
    Axis(ahm::Unit *pAxisUnit, ahm::Logging *pLogging = 0) : Base(pLogging), m_pAxisUnit(pAxisUnit) {
        m_pBasicControlValue = find_itf<ahm::BasicControlValue>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE);
        m_pBasicControlValueAsync = find_itf<ahm::BasicControlValueAsync>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE_ASYNC);
        m_pHaltControlValue = find_itf<ahm::HaltControlValue>(m_pAxisUnit, ahm::IID_HALT_CONTROL_VALUE);
        m_pDirectedControlValueAsync = find_itf<ahm::DirectedControlValueAsync>(m_pAxisUnit, ahm::IID_DIRECTED_CONTROL_VALUE_ASYNC);
        m_pBasicControlValueVelocity = find_itf<ahm::BasicControlValueVelocity>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE_VELOCITY);
        m_pBasicControlValueAcceleration = find_itf<ahm::BasicControlValueAcceleration>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE_ACCELERATION);
        m_pDirectedControlValueAsyncVelocity = find_itf<ahm::DirectedControlValueAsyncVelocity>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE_ASYNC);
        m_pBasicControlState = find_itf<ahm::BasicControlState>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_STATE);
        m_pEventSource = find_itf<ahm::EventSource>(m_pAxisUnit, ahm::IID_EVENT_SOURCE);
        m_pAutoCalibration = find_itf<ahm::AutoCalibration>(m_pAxisUnit, ahm::IID_AUTO_CALIBRATION);
        m_pMicronsConverter = m_pBasicControlValue != 0 && m_pBasicControlValue->metricsConverters() != 0 ? m_pBasicControlValue->metricsConverters()->findMetricsConverter(ahm::METRICS_MICRONS) : 0;
        m_pMicronsPerSecondConverter = m_pBasicControlValueVelocity != 0 && m_pBasicControlValueVelocity->metricsConverters() != 0 ? m_pBasicControlValueVelocity->metricsConverters()->findMetricsConverter(ahm::METRICS_MICRONS_PER_SECOND) : 0;
        m_pMicronsPerSecondSquaredConverter = m_pBasicControlValueAcceleration != 0 && m_pBasicControlValueAcceleration->metricsConverters() != 0 ? m_pBasicControlValueVelocity->metricsConverters()->findMetricsConverter(ahm::METRICS_MICRONS_PER_SECOND_SQUARED) : 0;
    }

    void printWhatIsSupported();

    // BasicControlValue - sync control

    // move to position in microsteps
    bool moveTo(iop::int32 pos);

    void autocalibration() {
            m_pAutoCalibration->calibrate();
        }

    iop::int32 getCurrentPosition() ;


    iop::int32 getMinPosition(); // min position in micro steps


    iop::int32 getMaxPosition(); // min position in micro steps

    // BasicControlState
    bool isMoving();

    // BasicControlValueAsync
    ahm::AsyncResult*  moveToAsync(iop::int32 pos) ;

    // DirectedControlValueAsync
    void moveAsync(iop::int32 direction) ; // <0 => to min >=0 => to max

    // DirectedControlValueAsyncVelocity
    void moveAsyncVel(iop::int32 speed) ;// <0 => to min >=0 => to max

    // BasicControlValueVelocity - current speed
    bool setCurrentSpeed(iop::int32 speed) ;// set current native speed



    iop::int32 getCurrentSpeed();// retrieve current native speed

    iop::int32 getMinSpeed() ; // min native speed

    iop::int32 getMaxSpeed() ; // min position in micro steps

    bool setCurrentAcceleration(iop::int32 accel);

    iop::int32 getCurrentAcceleration();

    // what is supported
    bool isAsyncSupported() const ;

    bool isDirectedAsyncVelSupported() const;

    bool eventsSupported() const ;

    // converters
    iop::float64 toMicrons(iop::int32 pos) ; // convert native steps to microns

    iop::int32 toNativePosition(iop::float64 dMicrons) ;// convert microns to native steps

    iop::float64 toMicronsPerSecond(iop::int32 speed) ; // convert native speed to microns/s

    iop::int32 toNativeSpeed(iop::float64 dMicronsPerSecond);// convert microns/s to native speed

    // events
    void subscribeEvents(ahm::EventSink *pEventSink) ;

    void unsubscribeEvents(ahm::EventSink *pEventSink) ;

    static bool isX(ahm::Unit *pUnit);

    static bool isY(ahm::Unit *pUnit);

private:
    ahm::Unit *m_pAxisUnit;
    ahm::BasicControlValue *m_pBasicControlValue;
    ahm::BasicControlValueAsync *m_pBasicControlValueAsync;
    ahm::HaltControlValue *m_pHaltControlValue;
    ahm::MetricsConverter *m_pMicronsConverter, *m_pMicronsPerSecondConverter, *m_pMicronsPerSecondSquaredConverter;
    ahm::DirectedControlValueAsync *m_pDirectedControlValueAsync;
    ahm::DirectedControlValueAsyncVelocity *m_pDirectedControlValueAsyncVelocity;
    ahm::BasicControlValueVelocity *m_pBasicControlValueVelocity;
    ahm::BasicControlValueAcceleration *m_pBasicControlValueAcceleration;
    ahm::BasicControlState *m_pBasicControlState;
    ahm::EventSource *m_pEventSource;
    ahm::AutoCalibration *m_pAutoCalibration;
};

class Stage : public Base {
public:
    Stage(ahm::Unit *pStageUnit, ahm::Logging *pLogging = 0) : Base(pLogging)
        , m_XAxis(findUnit(pStageUnit, ahm::MICROSCOPE_X_UNIT))
        , m_YAxis(findUnit(pStageUnit, ahm::MICROSCOPE_Y_UNIT)) {}

    void printWhatIsSupported() {
        m_XAxis.printWhatIsSupported();
        m_YAxis.printWhatIsSupported();
    }

    Axis& XAxis() { return m_XAxis; }
    Axis& YAxis() { return m_YAxis; }

    static bool isInProgress(ahm::AsyncResult* pAsyncResult) ;

    bool isMoving() ;

    void moveToAsync(iop::int32 posx, iop::int32 posy, bool flagWaitMovements);


class PositionRecorder : public ahm::EventSink {
public:
    struct Record {
        char who;
        iop::int32 pos;
        DWORD time;
    };

    typedef std::vector<Record> Records;

    virtual void onEvent(ahm::Unit* pSender, ahm::Event* pEvent);

    virtual void onShutdown(ahm::Unit* sender) ;

    void clear();

    bool getRecords(Records & result, bool flagClear = false);

private:
    Records m_records;
    MTLock m_lock;
};


void print(std::ostream & out, const PositionRecorder::Record& record, DWORD t0) ;

void subscribe(ahm::EventSink* pEventSink) ;

void unsubscribe(PositionRecorder& recorder);

private:
    Axis m_XAxis;
    Axis m_YAxis;
};



#endif // STAGECONTROLLER_H
