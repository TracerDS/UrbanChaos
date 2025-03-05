//
// The entrypoint for muckyBASIC
//

#include "always.h"
#include "cg.h"
#include "console.h"
#include "font.h"
#include "key.h"
#include "link.h"
#include "mem.h"
#include "parse.h"
#include "os.h"
#include "st.h"
#include "sysvar.h"
#include "vm.h"

//
// Compiles the given file. Returns true on success.
//

std::int32_t MAIN_compile(char* input, char* output) {
    FILE* handle;

    PARSE_do(input);

    if (PARSE_error_upto) {
        //
        // There are errors!
        //

        std::int32_t i;

        for (i = 0; i < PARSE_error_upto; i++) {
            OS_string("%s\n", PARSE_error[i]);
        }

        return false;
    }

    CG_do(output, CG_OUTPUT_OBJECT_FILE);

    return true;
}

void MAIN_main() {
    FONT_init();
    ST_init();

    //
    // Compile.
    //

    if (!MAIN_compile("test.mbs", "MuckyBASIC objects\\test.mbo")) return;
    if (!MAIN_compile("test2.mbs", "MuckyBASIC objects\\test2.mbo")) return;

    //
    // Link.
    //

    char* link[] =
        {
            "MuckyBASIC objects\\test.mbo",
            "MuckyBASIC objects\\test2.mbo",
        };

    LINK_do(link, 2, "MuckyBASIC executables\\test.mbe");

    //
    // Run.
    //

    VM_run("MuckyBASIC executables\\test.mbe");

    //
    // Wait for a key...
    //

    while (1) {
        if (KEY_on[KEY_ESCAPE]) {
            KEY_on[KEY_ESCAPE] = 0;

            break;
        }
    }
}
