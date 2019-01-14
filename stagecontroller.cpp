#include <stagecontroller.h>


// ITK HYDRA

// tools
// findUnit: tool function to find a unit with a given type id in the unit tree
ahm::Unit *findUnit(ahm::Unit *pUnit, ahm::TypeId typeId) {
    // test unit's type for given typeId
    if (pUnit && pUnit->type()) {
        if (pUnit->type()->isA(typeId)) {
            return pUnit; // ok it is!
        }
        else {
            if (pUnit->units()) {
                // recursively find unit in child units
                for (iop::int32 i = 0; i<pUnit->units()->numUnits(); i++) {
                    ahm::Unit *pDeepUnit = findUnit(pUnit->units()->getUnit(i), typeId);
                    if (pDeepUnit) {
                        return pDeepUnit; // stop recursion
                    }
                }
            }
        }
    }
    return 0; // unit with type id was not found
}

iop::string safe(iop::string sz) { return sz ? sz : ""; }


void MTPrinter::print(std::strstream & sstream, bool nl) {
    std::string str;
    sstream << '\0';
    str = (iop::string) sstream.str();
    print(str, nl);
}
void MTPrinter::print(const std::string& str, bool nl) {
    MT_SYNCHRONIZED_BLOCK(m_lock);
    std::cout << str.c_str();
    if (nl) std::cout << std::endl;
}



void Base::addLogText(iop::string szText) {
    if (m_pLogging) {
        m_pLogging->addText(szText);
    }
}

extern void printCheck(iop::string text, void *ptr) {
    std::cout << "\t" << text << "\t" << (ptr != NULL ? "[X]" : "[ ]") << std::endl;
}

// end tools



// Stage axis wrapper class

void Axis::printWhatIsSupported() {
        std::cout << (isX(m_pAxisUnit) ? 'X' : (isY(m_pAxisUnit) ? 'Y' : '?')) << " Axis" << std::endl;
        printCheck("BasicControlValue", m_pBasicControlValue);
        printCheck("BasicControlValueAsync", m_pBasicControlValueAsync);
        printCheck("HaltControlValue", m_pHaltControlValue);
        printCheck("DirectedControlValueAsync", m_pDirectedControlValueAsync);
        printCheck("BasicControlValueVelocity", m_pBasicControlValueVelocity);
        printCheck("DirectedControlValueAsyncVelocity", m_pDirectedControlValueAsyncVelocity);
        printCheck("EventSource", m_pEventSource);
        printCheck("MicronsConverter", m_pMicronsConverter);
        printCheck("MicronsPerSecondConverter", m_pMicronsPerSecondConverter);
    }


    // BasicControlValue - sync control

    // move to position in microsteps
    bool Axis::moveTo(iop::int32 pos) {
        if (m_pBasicControlValue) {
            try {
                m_pBasicControlValue->setControlValue(pos);
                return true;
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while moving to position" << pos << ": " << ex.errorText() << std::endl;
            }
        }
        return false;
    }

    iop::int32 Axis::getCurrentPosition() { // current position in micro steps
        if (m_pBasicControlValue) {
            try {
                return m_pBasicControlValue->getControlValue();
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while reading current position" << std::endl;
            }
        }
        return 0;
    }

    iop::int32 Axis::getMinPosition() { // min position in micro steps
        if (m_pBasicControlValue) {
            try {
                return m_pBasicControlValue->minControlValue();
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while reading min position" << std::endl;
            }
        }
        return 0;
    }
    iop::int32 Axis::getMaxPosition() { // min position in micro steps
        if (m_pBasicControlValue) {
            try {
                return m_pBasicControlValue->maxControlValue();
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while reading max position" << std::endl;
            }
        }
        return 0;
    }

    // BasicControlState
    bool Axis::isMoving() {
        if (m_pBasicControlState) {
            try {
                return m_pBasicControlState->isChanging();
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while reading state" << std::endl;
            }
        }
        return 0;
    }

    // BasicControlValueAsync
    ahm::AsyncResult* Axis::moveToAsync(iop::int32 pos) {
       if (m_pBasicControlValueAsync) {
            try {
                return m_pBasicControlValueAsync->setControlValueAsync(pos);
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while starting async move" << pos << std::endl;
            }
        }
        return 0;
    }

    // DirectedControlValueAsync
    void Axis::moveAsync(iop::int32 direction) { // <0 => to min >=0 => to max
        if (m_pDirectedControlValueAsync) {
            try {
                if (direction<0) {

                    m_pDirectedControlValueAsync->startTowardsMin();
                }
                else {
                    m_pDirectedControlValueAsync->startTowardsMax();
                }
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while starting async move to " << (direction<0 ? "min" : "max") << std::endl;
            }
        }
    }

    // DirectedControlValueAsyncVelocity
    void Axis::moveAsyncVel(iop::int32 speed) { // <0 => to min >=0 => to max
        if (m_pDirectedControlValueAsyncVelocity) {
            try {
                if (speed<0) {
                    m_pDirectedControlValueAsyncVelocity->startTowardsMinVelocity(abs(speed));
                }
                else {
                    m_pDirectedControlValueAsyncVelocity->startTowardsMaxVelocity(speed);
                }
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while starting async move to " << (speed <0 ? "min" : "max") << " with speed" << speed << std::endl;
            }
        }
    }

    // BasicControlValueVelocity - current speed
    bool Axis::setCurrentSpeed(iop::int32 speed) {// set current native speed
        if (m_pBasicControlValueVelocity) {
            try {
                m_pBasicControlValueVelocity->setControlValue(speed);
                return true;
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while setting speed" << speed << ": " << ex.errorText() << std::endl;
            }
        }
        return false;
    }

    iop::int32 Axis::getCurrentSpeed() { // retrieve current native speed
        if (m_pBasicControlValueVelocity) {
            try {
                return m_pBasicControlValueVelocity->getControlValue();
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while reading current speed" << std::endl;
            }
        }
        return 0;
    }

    iop::int32 Axis::getMinSpeed() { // min native speed
        if (m_pBasicControlValueVelocity) {
            try {
                return m_pBasicControlValueVelocity->minControlValue();
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while reading min speed" << std::endl;
            }
        }
        return 0;
    }
    iop::int32 Axis::getMaxSpeed() { // min position in micro steps
        if (m_pBasicControlValueVelocity) {
            try {
                return m_pBasicControlValueVelocity->maxControlValue();
            }
            catch (ahm::Exception & ex) {
                std::cout << "exception while reading max speed" << std::endl;
            }
        }
        return 0;
    }

    // what is supported
    bool Axis::isAsyncSupported() const {
        return m_pBasicControlValueAsync != 0;
    }

    bool Axis::isDirectedAsyncVelSupported() const {
        return m_pDirectedControlValueAsyncVelocity != 0;
    }

    bool Axis::eventsSupported() const {
        return m_pEventSource != 0;
    }
    // converters
    iop::float64 Axis::toMicrons(iop::int32 pos) { // convert native steps to microns
        return m_pMicronsConverter != 0 ? m_pMicronsConverter->getMetricsValue(pos) : 0.0;
    }

    iop::int32 Axis::toNativePosition(iop::float64 dMicrons) {// convert microns to native steps
        return m_pMicronsConverter ? m_pMicronsConverter->getControlValue(dMicrons) : 0;
    }

    iop::float64 Axis::toMicronsPerSecond(iop::int32 speed) { // convert native speed to microns/s
        return m_pMicronsPerSecondConverter != 0 ? m_pMicronsPerSecondConverter->getMetricsValue(speed) : 0.0;
    }

    iop::int32 Axis::toNativeSpeed(iop::float64 dMicronsPerSecond) {// convert microns/s to native speed
        return m_pMicronsConverter ? m_pMicronsConverter->getControlValue(dMicronsPerSecond) : 0;
    }


    // events
    void Axis::subscribeEvents(ahm::EventSink *pEventSink) {
        if (m_pEventSource) {
            try {
                m_pEventSource->subscribe(pEventSink);
            }
            catch (ahm::Exception &ex) {
                std::cout << "error subscribing events" << ex.errorText() << std::endl;
            }
        }
    }

    void Axis::unsubscribeEvents(ahm::EventSink *pEventSink) {
        if (m_pEventSource) {
            try {
                m_pEventSource->unsubscribe(pEventSink);
            }
            catch (ahm::Exception &ex) {
                std::cout << "error unsubscribing events" << ex.errorText() << std::endl;
            }
        }
    }

    bool Axis::isX(ahm::Unit *pUnit) {
        return pUnit != 0 && pUnit->type()->isA(ahm::MICROSCOPE_X_UNIT);
    }

    bool Axis::isY(ahm::Unit *pUnit) {
        return pUnit != 0 && pUnit->type()->isA(ahm::MICROSCOPE_Y_UNIT);
    }






        bool Stage::isInProgress(ahm::AsyncResult* pAsyncResult) {
            return pAsyncResult != NULL && pAsyncResult->state() == ahm::AsyncResult::INPROGRESS;
        }

        bool Stage::isMoving() {
            return m_XAxis.isMoving() || m_YAxis.isMoving();
        }

        void Stage::moveToAsync(iop::int32 posx, iop::int32 posy, bool flagWaitMovements)
        {
            ahm::AsyncResult *pAsyncResultX = m_XAxis.moveToAsync(posx);
            ahm::AsyncResult *pAsyncResultY = m_YAxis.moveToAsync(posy);
            while (flagWaitMovements && (isInProgress(pAsyncResultX) || isInProgress(pAsyncResultY)))
            {
                ::Sleep(50);
            }

            // discard Asyncresult objects
            if (pAsyncResultX != NULL)
            {
                pAsyncResultX->dispose();
            }
            if (pAsyncResultY != NULL)
            {
                pAsyncResultY->dispose();
            }
        }

        void Stage::PositionRecorder::onEvent(ahm::Unit* pSender, ahm::Event* pEvent) {

                if (pEvent && pEvent->eventType() == ahm::ValueChangedEvent::EVENT_TYPE) {
                    ahm::ValueChangedEvent *pValueChangedEvent = (ahm::ValueChangedEvent *)pEvent;

                    if (pValueChangedEvent->interfaceId() == ahm::IID_BASIC_CONTROL_VALUE) { // position event
                        char chWho = '?';
                        if (Axis::isX(pSender)) {
                            chWho = 'X';
                        }
                        else if (Axis::isY(pSender)) {
                            chWho = 'Y';
                        }
                        Record record;
                        record.who = chWho;
                        record.pos = pValueChangedEvent->value();// native position value
                        record.time = ::GetTickCount();

                        {
                            MT_SYNCHRONIZED_BLOCK(m_lock);
                            m_records.push_back(record);
                        }
                    }
                }
            }

            void Stage::PositionRecorder::onShutdown(ahm::Unit* sender) {}

            void Stage::PositionRecorder::clear() {
                MT_SYNCHRONIZED_BLOCK(m_lock);
                m_records.clear();
            }

            bool Stage::PositionRecorder::getRecords(Records & result, bool flagClear) {
                MT_SYNCHRONIZED_BLOCK(m_lock);
                result = m_records;
                if (flagClear) m_records.clear();
                return !result.empty();
            }

        void Stage::print(std::ostream & out, const PositionRecorder::Record& record, DWORD t0) {
            out << record.time - t0 << " millis " << record.who << "  pos: " << record.pos;
            Axis & axis = m_XAxis;
            if (record.who == 'y') axis = m_YAxis;

            out << " (" << axis.toMicrons(record.pos) << ")";
        }
        void Stage::subscribe(ahm::EventSink* pEventSink) {
            m_XAxis.subscribeEvents(pEventSink);
            m_YAxis.subscribeEvents(pEventSink);
        }

        void Stage::unsubscribe(PositionRecorder& recorder) {
            m_XAxis.unsubscribeEvents(&recorder);
            m_YAxis.unsubscribeEvents(&recorder);
        }
