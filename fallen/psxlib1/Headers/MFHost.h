// MFHost.h		-	Windows.
// Guy Simmons, 1st February 1997.

#define H_CREATE_LOG (1 << 0)

#ifdef _MF_WINDOWS

extern int iGlobalCmdShow;
extern HINSTANCE hGlobalInstance,
    hGlobalPrevInstance;
extern LPSTR szGlobalCmdLine;

#define main(ac, av) MF_main(ac, av)

std::int32_t main(std::uint16_t argc, char** argv);

#endif

struct MFTime {
    std::int32_t Hours,
        Minutes,
        Seconds,
        MSeconds;
    std::int32_t DayOfWeek, //	0 - 6;		Sunday		=	0
        Day,
        Month, //	1 - 12;		January		=	1
        Year;
    std::int32_t Ticks; // Number of ticks(milliseconds) since windows started.
};

bool SetupHost(std::uint32_t flags);
void ResetHost();
void LogText(char* error, ...);
void Time(struct MFTime* the_time);
