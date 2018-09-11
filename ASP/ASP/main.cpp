

#include <nanogui/nanogui.h>
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

//using namespace std;
using namespace nanogui;
using namespace ahm;


// ITK HYDRA

					// tools
					   // findUnit: tool function to find a unit with a given type id in the unit tree
Unit *findUnit(Unit *pUnit, ahm::TypeId typeId) {
	// test unit's type for given typeId
	if (pUnit && pUnit->type()) {
		if (pUnit->type()->isA(typeId)) {
			return pUnit; // ok it is!
		}
		else {
			if (pUnit->units()) {
				// recursively find unit in child units
				for (iop::int32 i = 0; i<pUnit->units()->numUnits(); i++) {
					Unit *pDeepUnit = findUnit(pUnit->units()->getUnit(i), typeId);
					if (pDeepUnit) {
						return pDeepUnit; // stop recursion
					}
				}
			}
		}
	}
	return 0; // unit with type id was not found
}

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

iop::string safe(iop::string sz) { return sz ? sz : ""; }

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



class MTPrinter {
public:
	MTPrinter() {

	}

	void print(std::strstream & sstream, bool nl = false) {
		std::string str;
		sstream << '\0';
		str = (iop::string) sstream.str();
		print(str, nl);
	}

	void print(const std::string& str, bool nl = false) {
		MT_SYNCHRONIZED_BLOCK(m_lock);
		std::cout << str.c_str();
		if (nl) std::cout << std::endl;
	}
private:
	MTLock m_lock;
};



class Base {
public:
	Base(ahm::Logging *pLogging) : m_pLogging(pLogging) {}
	void addLogText(iop::string szText) {
		if (m_pLogging) {
			m_pLogging->addText(szText);
		}
	}
private:
	ahm::Logging *m_pLogging;
};


void printCheck(iop::string text, void *ptr) {
	std::cout << "\t" << text << "\t" << (ptr != NULL ? "[X]" : "[ ]") << std::endl;
}

// end tools



// Stage axis wrapper class
class Axis : Base {
public:
	Axis(ahm::Unit *pAxisUnit, ahm::Logging *pLogging = 0) : Base(pLogging), m_pAxisUnit(pAxisUnit) {
		m_pBasicControlValue = find_itf<ahm::BasicControlValue>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE);
		m_pBasicControlValueAsync = find_itf<ahm::BasicControlValueAsync>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE_ASYNC);
		m_pHaltControlValue = find_itf<ahm::HaltControlValue>(m_pAxisUnit, ahm::IID_HALT_CONTROL_VALUE);
		m_pDirectedControlValueAsync = find_itf<ahm::DirectedControlValueAsync>(m_pAxisUnit, ahm::IID_DIRECTED_CONTROL_VALUE_ASYNC);
		m_pBasicControlValueVelocity = find_itf<ahm::BasicControlValueVelocity>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE_VELOCITY);
		m_pDirectedControlValueAsyncVelocity = find_itf<ahm::DirectedControlValueAsyncVelocity>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_VALUE_ASYNC);
		m_pBasicControlState = find_itf<ahm::BasicControlState>(m_pAxisUnit, ahm::IID_BASIC_CONTROL_STATE);
		m_pEventSource = find_itf<ahm::EventSource>(m_pAxisUnit, ahm::IID_EVENT_SOURCE);
		m_pMicronsConverter = m_pBasicControlValue != 0 && m_pBasicControlValue->metricsConverters() != 0 ? m_pBasicControlValue->metricsConverters()->findMetricsConverter(ahm::METRICS_MICRONS) : 0;
		m_pMicronsPerSecondConverter = m_pBasicControlValueVelocity != 0 && m_pBasicControlValueVelocity->metricsConverters() != 0 ? m_pBasicControlValueVelocity->metricsConverters()->findMetricsConverter(ahm::METRICS_MICRONS_PER_SECOND) : 0;
	}

	void printWhatIsSupported() {
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
	bool moveTo(iop::int32 pos) {
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

	iop::int32 getCurrentPosition() { // current position in micro steps
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

	iop::int32 getMinPosition() { // min position in micro steps
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
	iop::int32 getMaxPosition() { // min position in micro steps
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
	bool isMoving() {
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
	ahm::AsyncResult *moveToAsync(iop::int32 pos) {
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
	void moveAsync(iop::int32 direction) { // <0 => to min >=0 => to max
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
	void moveAsyncVel(iop::int32 speed) { // <0 => to min >=0 => to max
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
	bool setCurrentSpeed(iop::int32 speed) {// set current native speed
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

	iop::int32 getCurrentSpeed() { // retrieve current native speed
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

	iop::int32 getMinSpeed() { // min native speed
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
	iop::int32 getMaxSpeed() { // min position in micro steps
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
	bool isAsyncSupported() const {
		return m_pBasicControlValueAsync != 0;
	}

	bool isDirectedAsyncVelSupported() const {
		return m_pDirectedControlValueAsyncVelocity != 0;
	}
	bool eventsSupported() const {
		return m_pEventSource != 0;
	}
	// converters
	iop::float64 toMicrons(iop::int32 pos) { // convert native steps to microns
		return m_pMicronsConverter != 0 ? m_pMicronsConverter->getMetricsValue(pos) : 0.0;
	}

	iop::int32 toNativePosition(iop::float64 dMicrons) {// convert microns to native steps
		return m_pMicronsConverter ? m_pMicronsConverter->getControlValue(dMicrons) : 0;
	}

	iop::float64 toMicronsPerSecond(iop::int32 speed) { // convert native speed to microns/s
		return m_pMicronsPerSecondConverter != 0 ? m_pMicronsPerSecondConverter->getMetricsValue(speed) : 0.0;
	}

	iop::int32 toNativeSpeed(iop::float64 dMicronsPerSecond) {// convert microns/s to native speed
		return m_pMicronsConverter ? m_pMicronsConverter->getControlValue(dMicronsPerSecond) : 0;
	}


	// events
	void subscribeEvents(ahm::EventSink *pEventSink) {
		if (m_pEventSource) {
			try {
				m_pEventSource->subscribe(pEventSink);
			}
			catch (ahm::Exception &ex) {
				std::cout << "error subscribing events" << ex.errorText() << std::endl;
			}
		}
	}

	void unsubscribeEvents(ahm::EventSink *pEventSink) {
		if (m_pEventSource) {
			try {
				m_pEventSource->unsubscribe(pEventSink);
			}
			catch (ahm::Exception &ex) {
				std::cout << "error unsubscribing events" << ex.errorText() << std::endl;
			}
		}
	}

	static bool isX(ahm::Unit *pUnit) {
		return pUnit != 0 && pUnit->type()->isA(ahm::MICROSCOPE_X_UNIT);
	}

	static bool isY(ahm::Unit *pUnit) {
		return pUnit != 0 && pUnit->type()->isA(ahm::MICROSCOPE_Y_UNIT);
	}
private:
	ahm::Unit *m_pAxisUnit;
	ahm::BasicControlValue *m_pBasicControlValue;
	ahm::BasicControlValueAsync *m_pBasicControlValueAsync;
	ahm::HaltControlValue *m_pHaltControlValue;
	ahm::MetricsConverter *m_pMicronsConverter, *m_pMicronsPerSecondConverter;
	ahm::DirectedControlValueAsync *m_pDirectedControlValueAsync;
	ahm::DirectedControlValueAsyncVelocity *m_pDirectedControlValueAsyncVelocity;
	ahm::BasicControlValueVelocity *m_pBasicControlValueVelocity;
	ahm::BasicControlState *m_pBasicControlState;
	ahm::EventSource *m_pEventSource;
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

	static bool isInProgress(ahm::AsyncResult* pAsyncResult) {
		return pAsyncResult != NULL && pAsyncResult->state() == ahm::AsyncResult::INPROGRESS;
	}

	bool isMoving() {
		return m_XAxis.isMoving() || m_YAxis.isMoving();
	}

	void moveToAsync(iop::int32 posx, iop::int32 posy, bool flagWaitMovements)
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



	class PositionRecorder : public ahm::EventSink {
	public:
		struct Record {
			char who;
			iop::int32 pos;
			DWORD time;
		};
		typedef std::vector<Record> Records;

		virtual void onEvent(ahm::Unit* pSender, ahm::Event* pEvent) {

			if (pEvent && pEvent->eventType() == ValueChangedEvent::EVENT_TYPE) {
				ValueChangedEvent *pValueChangedEvent = (ValueChangedEvent *)pEvent;

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
		virtual void onShutdown(ahm::Unit* sender) {}

		void clear() {
			MT_SYNCHRONIZED_BLOCK(m_lock);
			m_records.clear();
		}

		bool getRecords(Records & result, bool flagClear = false) {
			MT_SYNCHRONIZED_BLOCK(m_lock);
			result = m_records;
			if (flagClear) m_records.clear();
			return !result.empty();
		}

	private:
		Records m_records;
		MTLock m_lock;
	};

	void print(std::ostream & out, const PositionRecorder::Record& record, DWORD t0) {
		out << record.time - t0 << " millis " << record.who << "  pos: " << record.pos;
		Axis & axis = m_XAxis;
		if (record.who == 'y') axis = m_YAxis;

		out << " (" << axis.toMicrons(record.pos) << ")";
	}
	void subscribe(ahm::EventSink* pEventSink) {
		m_XAxis.subscribeEvents(pEventSink);
		m_YAxis.subscribeEvents(pEventSink);
	}

	void unsubscribe(PositionRecorder& recorder) {
		m_XAxis.unsubscribeEvents(&recorder);
		m_YAxis.unsubscribeEvents(&recorder);
	}

private:
	Axis m_XAxis, m_YAxis;
};


// stage sample procedure
void stage_sample(ahm::Unit *pRootUnit) {

	// search necessary units:
	ahm::Unit *pStageUnit = findUnit(pRootUnit, ahm::MICROSCOPE_STAGE);


	if (!pStageUnit) {
		std::cout << "stage_sample: no stage found!" << std::endl;
		return;
	}

	Stage stage(pStageUnit);
	stage.printWhatIsSupported();

	std::cout << "current position: " << stage.XAxis().toMicrons(stage.XAxis().getCurrentPosition()) << ";" << stage.YAxis().toMicrons(stage.YAxis().getCurrentPosition()) << std::endl;



	iop::int32 x0 = stage.XAxis().getMinPosition();
	iop::int32 x1 = stage.XAxis().getMaxPosition();

	iop::int32 y0 = stage.YAxis().getMinPosition();
	iop::int32 y1 = stage.YAxis().getMaxPosition();
	std::cout << "moving to minimum" << std::endl;
	stage.moveToAsync(x0, y0, true); // move to min corner and wait

	std::cout << "current position: " << stage.XAxis().toMicrons(stage.XAxis().getCurrentPosition()) << ";" << stage.YAxis().toMicrons(stage.YAxis().getCurrentPosition()) << std::endl;

	iop::int32 x = x0 + (iop::int32) ((x1 - x0) / 2.0);
	iop::int32 y = y0 + (iop::int32) ((y1 - y0) / 2.0);

	std::cout << "moving to center " << x << "," << y << std::endl;


	Stage::PositionRecorder recorder;

	stage.subscribe(&recorder);

	DWORD t0 = ::GetTickCount();

	stage.moveToAsync(x, y, false);

	::Sleep(1000);

	Stage::PositionRecorder::Records records;
	recorder.getRecords(records, true);

	bool flagMoving = stage.isMoving();

	std::cout << "after one second the stage is " << (flagMoving ? "still" : "not") << " moving" << std::endl;
	std::cout << "in this time " << records.size() << " events were received" << std::endl;

	for (size_t i = 0; i<records.size(); i++) {
		std::cout << "[" << i << "] - ";
		stage.print(std::cout, records[i], t0);
		std::cout << std::endl;
	}

	iop::int32 maxSpeedX = stage.XAxis().getMaxSpeed();
	iop::int32 curSpeedX = stage.XAxis().getCurrentSpeed();

	std::cout << "maximum speed is " << stage.XAxis().toMicronsPerSecond(maxSpeedX) << " microns/sec" << std::endl;
	std::cout << "current speed is " << stage.XAxis().toMicronsPerSecond(curSpeedX) << " microns/sec" << std::endl;

	iop::int32 oldSpeedX = curSpeedX; // save original speed

	curSpeedX = curSpeedX / 2;

	std::cout << "setting half speed" << std::endl;
	stage.XAxis().setCurrentSpeed(curSpeedX);
	std::cout << "current speed is " << stage.XAxis().toMicronsPerSecond(stage.XAxis().getCurrentSpeed()) << " microns/sec" << std::endl;

	// lets move back to min

	std::cout << "now moving X back to minimum.." << std::endl;

	t0 = ::GetTickCount();

	ahm::AsyncResult *pAsyncResult = stage.XAxis().moveToAsync(x0);
	if (pAsyncResult) {// discard AsyncResult
		pAsyncResult->dispose();
		pAsyncResult = 0;
	}
	::Sleep(1000);

	recorder.getRecords(records, true);

	flagMoving = stage.isMoving();

	std::cout << "after one second the stage is " << (flagMoving ? "still" : "not") << " moving" << std::endl;
	std::cout << "in this time " << records.size() << " events were received" << std::endl;

	for (size_t i = 0; i<records.size(); i++) {
		std::cout << "[" << i << "] - ";
		stage.print(std::cout, records[i], t0);
		std::cout << std::endl;
	}


	stage.XAxis().setCurrentSpeed(oldSpeedX); // restore original speed
	std::cout << "current speed is " << stage.XAxis().toMicronsPerSecond(stage.XAxis().getCurrentSpeed()) << " microns/sec" << std::endl;
}


//GUI 
enum test_enum {
	Item1 = 0,
	Item2,
	Item3
};

bool bvar = true;
int ivar = 12345678;
double dvar = 3.1415926;
float fvar = (float)dvar;
std::string strval = "A string";
test_enum enumval = Item2;
Color colval(0.5f, 0.5f, 0.7f, 1.f);





int main(int /* argc */, char ** /* argv */) {
	try {
		if (theHardwareModel) {
			Unit *pRootUnit = theHardwareModel()->getUnit("");
			if (pRootUnit) {
				std::cout << "ITK HYDRA IS INITAILAZED READY TO FUCK " << std::endl;
				//pRootUnit->dispose();
			}
			else {
				std::cout << "NOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
			}
		}
	}
	catch (ahm::Exception & ex) {
		// a hardware model exception occured:
		std::cout << "a hardware model exception occurred: error code: " << ex.errorClass() << ", error code: " << ex.errorCode() << ", text: " << ex.errorText() << std::endl;
		std::cout << "youre fucking  destroyed by leica shit..." << std::endl;
		std::cout << "press any key to undestroy";
		getchar();

	}
	
	
	nanogui::init();
	/* scoped variables */ {
		bool use_gl_4_1 = false;// Set to true to create an OpenGL 4.1 context.
		Screen *screen = nullptr;

		if (use_gl_4_1) {
			// NanoGUI presents many options for you to utilize at your discretion.
			// See include/nanogui/screen.h for what all of these represent.
			screen = new Screen(Vector2i(350, 500), "ASP_Controller [GL 4.1]",
				/*resizable*/true, /*fullscreen*/false, /*colorBits*/8,
				/*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
				/*nSamples*/0, /*glMajor*/4, /*glMinor*/1);
		}
		else {
			screen = new Screen(Vector2i(350, 500), "ASP_Controller");
		}

		bool enabled = true;
		FormHelper *gui = new FormHelper(screen);
		ref<Window> asd = gui->addWindow(Eigen::Vector2i(10, 10), "Pipette Controller");

		gui->addVariable("setX", ivar)->setSpinnable(true);
		gui->addVariable("setY", ivar)->setSpinnable(true);
		gui->addVariable("setZ", ivar)->setSpinnable(true);
		
		gui->addButton("X- button", []() { std::cout << "pipetteX- pressed." << std::endl; });
		gui->addButton("X+ button", []() { std::cout << "pipetteX+ pressed." << std::endl; });
		gui->addButton("Y- button", []() { std::cout << "pipetteY- pressed." << std::endl; });
		gui->addButton("Y+ button", []() { std::cout << "pipetteY+ pressed." << std::endl; });
		gui->addButton("Z- button", []() { std::cout << "pipetteY- pressed." << std::endl; });
		gui->addButton("Z+ button", []() { std::cout << "pipetteY+ pressed." << std::endl; });


		ref<Window> window = gui->addWindow(Eigen::Vector2i(175, 10), "Stage Controller");
//		gui->addGroup("");
		gui->addVariable("setX", ivar)->setSpinnable(true);
		gui->addVariable("setY", ivar)->setSpinnable(true);

		gui->addButton("X- button", []() { std::cout << "stageX- pressed." << std::endl; });
		gui->addButton("X+ button", []() { std::cout << "stageX+ pressed." << std::endl; });
		gui->addButton("Y- button", []() { std::cout << "stageY- pressed." << std::endl; });
		gui->addButton("Y+ button", []() { std::cout << "stageY+ pressed." << std::endl; });
		ref<Window> PC = gui->addWindow(Eigen::Vector2i(10, 320), "Pressure Controller");
		gui->addVariable("Requested pressure (mBar)", ivar)->setSpinnable(true);
		gui->addButton("atmosphere", []() { std::cout << "atmosphere pressed" << std::endl; });



		screen->setVisible(true);
		screen->performLayout();
		nanogui::mainloop();
	}

	nanogui::shutdown();
	return 0;
}
