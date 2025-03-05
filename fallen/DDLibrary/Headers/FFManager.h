// FFManager.h
// 18th Sept 98
//
// Limitations: Currently assumes first joystick

#include "MFStdLib.h"
#include "DIManager.h"

#ifdef TARGET_DC
#error Silly person! DC has no force feedback.
#endif

#if 0

PC doesn't have DIDeviceInfo's any more

class FFManager {
	private:
	  std::int32_t  ForceFeedback;
	  LPDIRECTINPUTEFFECT testeffect;
	  DIDeviceInfo* DeviceInfo;
	  LPDIRECTINPUTDEVICE2	lpdiInputDevice;			// DI Device.
	public:
		FFManager();
		~FFManager();
		
		bool FFSupported(LPDIRECTINPUTDEVICE2 device);
		void ReleaseFX();
		bool Test();
};

extern FFManager* the_ff_manager;

#endif