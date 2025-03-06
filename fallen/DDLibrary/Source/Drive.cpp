// drive.cpp
//
// CD-ROM drives

#include "ddlib.h"
#include "drive.h"
#include "..\headers\env.h"

static char Path[MAX_PATH]; // CD-ROM path
static bool TexturesCD;     // textures on CD?
static bool SFX_CD;         // sound effects on CD?
static bool MoviesCD;       // movies on CD?
static bool SpeechCD;       // speech on CD?

// Exit
//
// exit on failure

static void Exit() {
#ifndef TARGET_DC
    MessageBox(nullptr, "Cannot locate Urban Chaos CD-ROM", nullptr, MB_ICONERROR);
#else
    ASSERT(false);
#endif
    exit(1);
}

// LocateCDROM
//
// locate the CD drive containing the Urban Chaos CD

#ifdef TARGET_DC

void LocateCDROM() {
    // Er... I know where it is on the DreamCast :-)
#ifdef FILE_PC
#error SPONG!
    // It's on the PC.
    strcpy(Path, "\\PC\\");
#else
    // It's on the DC itself.
    strcpy(Path, "\\CD-ROM\\");
#endif
}

#else // #ifdef TARGET_DC

void LocateCDROM() {
    TexturesCD = ENV_get_value_number("textures", 1, "LocalInstall") ? false : true;
    SFX_CD = ENV_get_value_number("sfx", 1, "LocalInstall") ? false : true;
    MoviesCD = ENV_get_value_number("movies", 1, "LocalInstall") ? false : true;
    SpeechCD = ENV_get_value_number("speech", 1, "LocalInstall") ? false : true;

    if (!TexturesCD && !SFX_CD && !MoviesCD && !SpeechCD) return; // don't need CD-ROM

    char strings[256];

    if (!GetLogicalDriveStrings(255, strings)) {
        MessageBox(nullptr, "Cannot locate system devices - serious internal error", nullptr, MB_ICONERROR);
        exit(0);
    }

    for (;;) {
        char* sptr = strings;

        while (*sptr) {
            if (GetDriveType(sptr) == DRIVE_CDROM) {
                char filename[MAX_PATH];

                sprintf(filename, "%sclumps\\mib.txc", sptr); // fallen.ex_ doesnt exist on eidos funny fanny builds

                FILE* fd = MF_Fopen(filename, "rb");

                if (fd) {
                    // found it!
                    MF_Fclose(fd);
                    strcpy(Path, sptr);
                    return;
                }
            }

            sptr += strlen(sptr) + 1;
        }

        if (MessageBox(nullptr, "Cannot locate Urban Chaos CD-ROM", nullptr, MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL) {
            break;
        }
    }
    exit(0);
}

#endif // #else //#ifdef TARGET_DC

// Get*Path

static inline char* GetPath(bool on_cd) {
    return on_cd ? Path : ".\\";
}

char* GetCDPath() {
    return Path;
}
char* GetTexturePath() {
    return GetPath(TexturesCD);
}
char* GetSFXPath() {
    return GetPath(SFX_CD);
}
char* GetMoviesPath() {
    return GetPath(MoviesCD);
}
char* GetSpeechPath() {
    return GetPath(SpeechCD);
}
