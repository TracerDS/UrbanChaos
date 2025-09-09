//
// Quick load and save from within a level.
//

#include "game.h"

//
// The filename we use as our savegame.
//

#define QLS_FNAME "data\\quicksave.dat"

void QLS_init() {
    //
    // Get rid of the file.
    //

    FileDelete(QLS_FNAME);
}

void QLS_available() {
    FILE* handle = MF_Fopen(QLS_FNAME, "rb");

    if (handle) {
        MF_Fclose(handle);

        return true;
    } else {
        return false;
    }
}

void QLS_save() {
    FILE* handle = MF_Fopen(QLS_FNAME);

    if (!handle) {
        return;
    }

    //
    // Save out the version number.
    //

    std::int32_t version = 1;

    if (fwrite(&version, sizeof(std::int32_t), 1, handle) != 1)
        goto file_error;

    //
    // Save out the the_game structure.
    //

    if (fwrite(&the_game, sizeof(the_game), 1, handle) != 1)
        goto file_error;

    MF_Fclose(handle);

    return;

file_error:;

    MF_Fclose(handle);

    return;
}

std::int32_t QLS_load() {
}

#endif
