// MFHost.h		-	Windows.
// Guy Simmons, 1st February 1997.

#define H_CREATE_LOG (1 << 0)

#define PAUSE_TIMEOUT 500
#define PAUSE (1 << 0)
#define PAUSE_ACK (1 << 1)

extern int iGlobalCmdShow;
extern HINSTANCE hGlobalInstance,
    hGlobalPrevInstance;
extern LPSTR szGlobalCmdLine;

#define main(ac, av) MF_main(ac, av)

std::int32_t main(std::uint16_t argc, char **argv);

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
// void	LogText(char* error, ...);
// void	Time(struct MFTime *the_time);
bool LibShellActive();
bool LibShellChanged();
bool LibShellMessage(const char *pMessage, const char *pFile, std::uint32_t dwLine);
void ShellPaused();
void ShellPauseOn();
void ShellPauseOff();
