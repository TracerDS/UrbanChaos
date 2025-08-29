//
// Loads in 32-bit RGBA uncompressed TGAs.
//

#include <MFStdLib.h>
#include "Tga.h"
#include "FileClump.h"

#ifdef TARGET_DC

#ifdef DEBUG
// For quick loading
// #define BOGUS_TGAS_PLEASE_BOB I have been defined
// Do a downsample of textures to minimise memory use for now.
// This is a two-to-the-power one, so "2" means quarter the size.
// #define DOWNSAMPLE_PLEASE_BOB_AMOUNT 2
#else
// For quick loading
// #define BOGUS_TGAS_PLEASE_BOB I have been defined
// Do a downsample of textures to minimise memory use for now.
// This is a two-to-the-power one, so "2" means quarter the size.
// #define DOWNSAMPLE_PLEASE_BOB_AMOUNT 2
#endif

// Don't downsample any smaller than this.
#define DOWNSAMPLE_MINIMUM_SIZE 16

#endif

static FileClump* tclump = nullptr;
static bool writing;
static bool init_convert = false;

TGA_Info TGA_load_from_file(const char* file, std::int32_t max_width, std::int32_t max_height, TGA_Pixel* data, bool bCanShrink = true);

#ifndef TARGET_DC
static void TGA_make_conversion_tables();
static void TGA_write_compressed(const TGA_Info& ti, TGA_Pixel* data, std::uint32_t id);
static TGA_Info TGA_read_compressed(TGA_Pixel* data, std::uint32_t id, std::int32_t max_width, std::int32_t max_height);
#endif

#ifndef TARGET_DC
// OpenTGAClump
//
// create a texture clump

void OpenTGAClump(const char* clumpfn, size_t maxid, bool readonly) {
    if (!init_convert) {
        TGA_make_conversion_tables();
        init_convert = true;
    }

    delete tclump;
    tclump = nullptr;

    writing = !readonly;

    tclump = new FileClump(clumpfn, maxid, readonly);
}

// CloseTGAClump
//
// close a texture clump

void CloseTGAClump() {
    delete tclump;
    tclump = nullptr;
}

// GetTGAClump
//
// return tclump

FileClump* GetTGAClump() {
    ASSERT(tclump);
    return tclump;
}

#endif // #ifndef TARGET_DC

// DoesTGAExist
//
// look for a TGA

bool DoesTGAExist(const char* filename, std::uint32_t id) {
    if (writing) {
        FILE* fd = MF_Fopen(filename, "rb");
        if (fd) {
            MF_Fclose(fd);
            return true;
        }
        return false;
    } else {
        return tclump->Exists(id);
    }
}

// TGA_load
//
// loads a TGA

std::int32_t tga_width;
std::int32_t tga_height;

TGA_Info TGA_load(const char* file, std::int32_t max_width, std::int32_t max_height, TGA_Pixel* data, std::uint32_t id, bool bCanShrink) {
#ifdef TARGET_DC
    ASSERT(!tclump || (id == -1));
#else
    if (!tclump || (id == -1))
#endif
    {
        // read directly from file
        return TGA_load_from_file(file, max_width, max_height, data, bCanShrink);
    }

#ifndef TARGET_DC
    if (writing) {
        // read from file, then write compressed
        TGA_Info ti = TGA_load_from_file(file, max_width, max_height, data);
        TGA_write_compressed(ti, data, id);
        return ti;
    }

    // read compressed
    return TGA_read_compressed(data, id, max_width, max_height);
#endif
}

// TGA_load_from_file
//
// load a TGA from file

TGA_Info TGA_load_from_file(const char* file, std::int32_t max_width, std::int32_t max_height, TGA_Pixel* data, bool bCanShrink) {
    std::int32_t i;
    std::int32_t x;
    std::int32_t y;
    std::int32_t y1;
    std::int32_t y2;

    // std::uint8_t red;
    // std::uint8_t green;
    // std::uint8_t blue;

    std::int32_t tga_pixel_depth;
    std::int32_t tga_image_type;
    std::int32_t tga_id_length;

    std::int32_t tga_pal;

    std::uint8_t header[18];
    std::uint8_t definitely_no_alpha;

    FILE* fd;

    TGA_Info ans;
    std::uint8_t pal[256 * 3];

#ifdef BOGUS_TGAS_PLEASE_BOB
    if (bCanShrink) {
        // Fast dodgy textures that don't need loading.
        ans.valid = 1;
        ans.width = 16;
        ans.height = 16;
        ans.contains_alpha = 1;
        ASSERT(max_width >= ans.width);
        ASSERT(max_height >= ans.height);

        TGA_Pixel* pdest = data;
        DWORD junk;
        while (*file != '\0') {
            junk ^= *file++;
            junk ^= (junk >> 2) ^ (junk << 1);
        }
        junk ^= (junk << 3) ^ (junk >> 2) ^ (junk >> 13) ^ (junk >> 21) ^ (junk >> 16);
        for (int i = 0; i < ans.height; i++) {
            for (int j = 0; j < ans.width; j++) {
                int num = (i & 1) | ((j & 1) << 1);
                pdest->alpha = (std::uint8_t)(junk >> (num));
                pdest->red = (std::uint8_t)(junk >> (num + 1));
                pdest->green = (std::uint8_t)(junk >> (num + 2));
                pdest->blue = (std::uint8_t)(junk >> (num + 3));
                pdest++;
            }
        }

        return (ans);
    }

#endif

    //
    // Open the file.
    //

    fd = MF_Fopen(file, "rb");
    if (!fd) {
        ans.valid = 0;
        return ans;
    }

    //
    // Read the header.
    //

    if (fread(header, 1, 18, fd) != 18)
        goto file_error;

    //
    // Extract info from the header.
    //

    tga_id_length = header[0x0];
    tga_pal = header[1];
    tga_image_type = header[0x2];
    tga_width = header[0xc] + header[0xd] * 256;
    tga_height = header[0xe] + header[0xf] * 256;
    tga_pixel_depth = header[0x10];

    //
    // Is this a valid tga file?
    //

    ans.valid = 0;
    ans.width = tga_width;
    ans.height = tga_height;
    ans.contains_alpha = 0;

    if (tga_image_type != 2 && tga_pal != 1) {
        TRACE("Tga must be true colour and uncompressed.\n");
        MF_Fclose(fd);
        return ans;
    }

    if (tga_pixel_depth != 32 && tga_pixel_depth != 24 && tga_pal != 1) {
        TRACE("Tga must be 32-bit or 24-bit (24-bit + 8-bit alpha channel)\n");
        MF_Fclose(fd);
        return ans;
    }

    if (tga_width > max_width ||
        tga_height > max_height) {
        TRACE("Invalid dimensions:\n\tWanted <= %d x %d\n\tGot       %d x %d\n", max_width, max_height, tga_width, tga_height);
        MF_Fclose(fd);
        return ans;
    }

    //
    // The tga file is valid...
    //

    ans.valid = 1;

    //
    // Skip past the image identification field.
    //

    fseek(fd, tga_id_length, SEEK_CUR);

    if (header[0x1]) {
        std::uint8_t col;
        //
        // The file has colour map data- but how much?
        //

        std::int32_t entries = header[5] + header[6] * 256;
        std::int32_t bitsperentry = header[7];
        std::uint32_t length;

        length = (entries * bitsperentry + 7) >> 3;
        ASSERT(length <= 256 * 3);

        if (fread(&pal[0], 1, length, fd) != length)
            goto file_error;

        std::uint8_t* buffer = new std::uint8_t[tga_width * tga_height];
        if (fread(buffer, 1, tga_width * tga_height, fd) != tga_width * tga_height)
            goto file_error;
        std::uint8_t* bp = buffer;

        for (i = 0; i < tga_width * tga_height; i++) {
            col = *bp++;

            data[i].red = pal[col * 3 + 2];
            data[i].green = pal[col * 3 + 1];
            data[i].blue = pal[col * 3 + 0];
            data[i].alpha = 255;

            definitely_no_alpha = 1;
        }

        delete[] buffer;
    } else {
        //
        // Load in the data.
        //

        if (tga_pixel_depth == 32) {
            if (fread(data, 1, sizeof(TGA_Pixel) * tga_width * tga_height, fd) != sizeof(TGA_Pixel) * tga_width * tga_height)
                goto file_error;

            //
            // Might be alpha channel data...
            //

            definitely_no_alpha = 0;
        } else {
            //
            // We have to load a pixel in at a time to add the nullptr alpha channel.
            //

            std::uint8_t* buffer = new std::uint8_t[3 * tga_width * tga_height];
            if (fread(buffer, 1, 3 * tga_width * tga_height, fd) != 3 * tga_width * tga_height)
                goto file_error;
            std::uint8_t* bp = buffer;

            for (i = 0; i < tga_width * tga_height; i++) {
                data[i].blue = *bp++;
                data[i].green = *bp++;
                data[i].red = *bp++;
                data[i].alpha = 255;
            }

            delete[] buffer;

            //
            // There is no alpha channel data.
            //

            definitely_no_alpha = 1;
        }
    }

    MF_Fclose(fd);

    //
    // All ok. Does this tga contain any alpha?
    //

    if (definitely_no_alpha == 0) {
        for (i = 0; i < tga_width * tga_height; i++) {
            if (data[i].alpha != 255) {
                //
                // Found alpha channel data.
                //

                ans.contains_alpha = 1;

                break;
            }
        }
    } else {
        ans.contains_alpha = 0;
    }

    //
    // TGAs are stored upside down to what we expect.
    //

    TGA_Pixel spare;

    for (y = 0; y < tga_height / 2; y++) {
        y1 = y;
        y2 = tga_height - 1 - y;

        for (x = 0; x < tga_width; x++) {
            spare = data[x + y1 * tga_width];
            data[x + y1 * tga_width] = data[x + y2 * tga_width];
            data[x + y2 * tga_width] = spare;
        }
    }

#ifdef DOWNSAMPLE_PLEASE_BOB_AMOUNT
    // OK, now downsample the image
    if (bCanShrink) {
        int iPasses = DOWNSAMPLE_PLEASE_BOB_AMOUNT;
        while (iPasses-- > 0) {
            if ((tga_width <= DOWNSAMPLE_MINIMUM_SIZE) || (tga_height <= DOWNSAMPLE_MINIMUM_SIZE)) {
                // Stop!
                break;
            }
            tga_width >>= 1;
            tga_height >>= 1;
            ans.width = tga_width;
            ans.height = tga_height;

            // Just a cheesy in-place box filter.
            TGA_Pixel* psrc1 = data;
            TGA_Pixel* psrc2 = data + (tga_width << 1);
            TGA_Pixel* pdest = data;

            for (int j = tga_height; j > 0; j--) {
                for (int k = tga_width; k > 0; k--) {
                    pdest->alpha = (std::uint8_t)(((DWORD) psrc1[0].alpha + (DWORD) psrc1[1].alpha + (DWORD) psrc2[0].alpha + (DWORD) psrc2[1].alpha) >> 2);
                    pdest->red = (std::uint8_t)(((DWORD) psrc1[0].red + (DWORD) psrc1[1].red + (DWORD) psrc2[0].red + (DWORD) psrc2[1].red) >> 2);
                    pdest->green = (std::uint8_t)(((DWORD) psrc1[0].green + (DWORD) psrc1[1].green + (DWORD) psrc2[0].green + (DWORD) psrc2[1].green) >> 2);
                    pdest->blue = (std::uint8_t)(((DWORD) psrc1[0].blue + (DWORD) psrc1[1].blue + (DWORD) psrc2[0].blue + (DWORD) psrc2[1].blue) >> 2);
                    pdest++;
                    psrc1 += 2;
                    psrc2 += 2;
                }
                psrc1 += tga_width << 1;
                psrc2 += tga_width << 1;
            }
        }
    }
#endif

    return ans;

file_error:;

    //
    // Error!
    //

    TRACE("File error loading TGA file %s\n", file);
    MF_Fclose(fd);
    ans.valid = 0;

    return ans;
}

#ifndef TARGET_DC

// WriteSquished
//
// write out a file, squished

static void WriteSquished(std::uint16_t* buffer, size_t nwords, std::uint32_t id) {
    std::uint16_t used[65536];
    std::uint16_t mapping[65536];

    memset(used, 0, 65536 * 2);

    std::uint16_t total = 0;

    for (size_t ii = 3; ii < nwords; ii++) {
        if (!used[buffer[ii]]) {
            used[buffer[ii]] = total + 1;
            mapping[total] = buffer[ii];
            total++;
        }
    }

    int bits = 1;

    while (total > (1 << bits)) bits++;

    int bits_required = bits * (nwords - 3) + (5 + total) * 16;
    int bits_normal = 16 * nwords;

    if (bits_required < bits_normal) {
        std::uint16_t* squished = new std::uint16_t[(bits_required + 15) / 16];
        std::uint16_t* sptr = squished;

        *sptr++ = 0xFFFF; // marker for compressed file
        *sptr++ = buffer[0];
        *sptr++ = buffer[1];
        *sptr++ = buffer[2];
        *sptr++ = total;

        for (size_t ii = 0; ii < total; ii++) {
            *sptr++ = mapping[ii];
        }

        // write out bit-encoded data
        // 10-bit A,B,C,D into 16 bits goes:
        // 0xAAAAAAAAAABBBBBB 0xBBBBCCCCCCCCCCDD 0xDDDDDDDD00000000

        std::uint16_t cword = 0; // current word
        std::uint16_t cbits = 0; // # bits in current word

        for (size_t ii = 3; ii < nwords; ii++) {
            std::uint16_t encoded = used[buffer[ii]] - 1;

            ASSERT(encoded < total);

            if (cbits + bits > 16) {
                // add (16 - cbits) bits and write out
                cword <<= (16 - cbits);
                cword |= encoded >> (bits - (16 - cbits));
                *sptr++ = cword;
                // store low bits and remember how many
                cword = encoded;
                cbits = bits - (16 - cbits);
            } else {
                // add (bits) bits
                cword <<= bits;
                cword |= encoded;
                cbits += bits;
            }
        }

        if (cbits) *sptr++ = (cword << (16 - cbits));

        ASSERT((sptr - squished) == ((bits_required + 15) / 16));

        tclump->Write(squished, 2 * (sptr - squished), id);
        delete[] squished;
    } else {
        tclump->Write(buffer, 2 * nwords, id);
    }
}

// ReadSquished
//
// read in a squished file

static std::uint8_t* ReadSquished(std::uint32_t id) {
    // read squished file
    std::uint8_t* buffer = tclump->Read(id);
    if (!buffer) return nullptr;

    std::uint16_t* bptr = (std::uint16_t*) buffer;

    if (*bptr != 0xFFFF) return buffer; // unsquished

    bptr++;

    // allocate output buffer
    size_t nwords = bptr[1] * bptr[2];

    std::uint8_t* output = new std::uint8_t[2 * (nwords + 3)];
    std::uint16_t* optr = (std::uint16_t*) output;

    // copy header
    *optr++ = *bptr++;
    *optr++ = *bptr++;
    *optr++ = *bptr++;

    // read mapping
    std::uint16_t total = *bptr++;
    std::uint16_t mapping[65536];

    for (size_t ii = 0; ii < total; ii++) {
        mapping[ii] = *bptr++;
    }

    int bits = 1;

    while (total > (1 << bits)) bits++;

    // read bit-encoded data
    int cbits = 16;
    std::uint16_t cword = *bptr++;

    for (size_t ii = 0; ii < nwords; ii++) {
        std::uint16_t encoded;

        if (cbits > bits) {
            // read (bits) bits
            encoded = cword >> (16 - bits);
            cword <<= bits;
            cbits -= bits;
        } else {
            // read (16 - cbits) bits
            int xbits = bits - cbits;
            encoded = cword >> (16 - bits);
            cword = *bptr++;
            cbits = 16;
            encoded |= (cword >> (16 - xbits));
            cword <<= xbits;
            cbits -= xbits;
        }

        ASSERT(encoded < total);

        *optr++ = mapping[encoded];
    }

    delete[] buffer;

    return output;
}

// TGA_make_conversion_tables
//
// make conversion tables

static std::uint8_t C8to4[256];
static std::uint8_t C8to5[256];
static std::uint8_t C8to6[256];
static std::uint8_t C4to8[16];
static std::uint8_t C5to8[32];
static std::uint8_t C6to8[64];

static void TGA_make_conversion_tables() {
    int ii;

    for (ii = 0; ii < 256; ii++) {
        C8to4[ii] = (((ii * 30) / 256) + 1) / 2;
        C8to5[ii] = (((ii * 62) / 256) + 1) / 2;
        C8to6[ii] = (((ii * 126) / 256) + 1) / 2;
    }

    for (ii = 0; ii < 16; ii++) C4to8[ii] = (ii << 4) | ii;
    for (ii = 0; ii < 32; ii++) C5to8[ii] = (ii << 3) | (ii >> 2);
    for (ii = 0; ii < 64; ii++) C6to8[ii] = (ii << 2) | (ii >> 4);
}

// TGA_write_compressed
//
// write a TGA out compressed

static void TGA_write_compressed(const TGA_Info& ti, TGA_Pixel* data, std::uint32_t id) {
    if (!ti.valid) return;
    if (tclump->Exists(id)) return;

    std::uint16_t* buffer = new std::uint16_t[ti.width * ti.height + 3];
    std::uint16_t* bptr = buffer;

    *bptr++ = (std::uint16_t) ti.contains_alpha;
    *bptr++ = (std::uint16_t) ti.width;
    *bptr++ = (std::uint16_t) ti.height;

    if (ti.contains_alpha) {
        // compress to 4:4:4:4
        for (int ii = 0; ii < ti.width * ti.height; ii++) {
            std::uint16_t pix;

            pix = C8to4[data->blue];
            pix |= C8to4[data->green] << 4;
            pix |= C8to4[data->red] << 8;
            pix |= C8to4[data->alpha] << 12;

            *bptr++ = pix;
            data++;
        }
    } else {
        // compress to 5:6:5
        for (int ii = 0; ii < ti.width * ti.height; ii++) {
            std::uint16_t pix;

            pix = C8to5[data->blue];
            pix |= C8to6[data->green] << 5;
            pix |= C8to5[data->red] << 11;

            *bptr++ = pix;
            data++;
        }
    }

    WriteSquished(buffer, ti.width * ti.height + 3, id);
    delete[] buffer;
}

// TGA_read_compressed
//
// read a TGA in compressed

static TGA_Info TGA_read_compressed(TGA_Pixel* data, std::uint32_t id, std::int32_t max_width, std::int32_t max_height) {
    TGA_Info ti;

    ti.valid = 0;

    if (!tclump->Exists(id)) return ti;

    std::uint8_t* buffer = ReadSquished(id);
    if (!buffer) return ti;
    std::uint16_t* bptr = (std::uint16_t*) buffer;

    ti.valid = 1;
    ti.contains_alpha = *bptr++;
    ti.width = *bptr++;
    ti.height = *bptr++;

    ASSERT(ti.width <= max_width);
    ASSERT(ti.height <= max_height);

    if (ti.contains_alpha) {
        // compress to 4:4:4:4
        for (int ii = 0; ii < ti.width * ti.height; ii++) {
            std::uint16_t pix = *bptr++;

            data->blue = C4to8[pix & 0xF];
            data->green = C4to8[(pix >> 4) & 0xF];
            data->red = C4to8[(pix >> 8) & 0xF];
            data->alpha = C4to8[(pix >> 12) & 0xF];
            data++;
        }
    } else {
        // compress to 5:6:5
        for (int ii = 0; ii < ti.width * ti.height; ii++) {
            std::uint16_t pix = *bptr++;

            data->blue = C5to8[pix & 0x1F];
            data->green = C6to8[(pix >> 5) & 0x3F];
            data->red = C5to8[(pix >> 11) & 0x1F];
            data->alpha = 0;
            data++;
        }
    }

    delete[] buffer;
    return ti;
}

TGA_Info TGA_load_remap(const char* file, const char* pname, std::int32_t max_width, std::int32_t max_height, TGA_Pixel* data) {
    std::int32_t i;
    std::int32_t x;
    std::int32_t y;
    std::int32_t y1;
    std::int32_t y2;

    std::int32_t tga_pixel_depth;
    std::int32_t tga_image_type;
    std::int32_t tga_id_length;

    std::int32_t tga_pal;

    std::uint8_t header[18];
    std::uint8_t junk;
    std::uint8_t definitely_no_alpha;

    FILE *handle, *phandle;

    TGA_Info ans;
    std::uint8_t pal[256 * 3];
    std::uint8_t remap_pal[256 * 4];

    //
    // Open the file.
    //

    phandle = MF_Fopen(pname, "rb");
    if (!phandle) {
        ans.valid = 0;
        return ans;
    }

    if (fread(&pal[0], 1, 24, phandle) != 24)
        goto file_error;

    if (fread(&remap_pal[0], 1, 256 * 4, phandle) != 256 * 4)
        goto file_error;

    MF_Fclose(phandle);

    {
        std::int32_t pal1 = 0, pal2 = 0, c0;
        for (c0 = 0; c0 < 16 * 4; c0++)
            pal1 += remap_pal[c0];

        for (c0 = 240 * 4; c0 < 256 * 4; c0++)
            pal2 += remap_pal[c0];

        if (pal2 > pal1) {
            //
            // shunt the pallette down
            //
            for (c0 = 0; c0 < 16 * 4; c0++) {
                remap_pal[c0] = remap_pal[240 * 4 + c0];
            }
        }
    }

    handle = MF_Fopen(file, "rb");

    if (!handle) {
        ans.valid = 0;
        return ans;
    }

    //
    // Read the header.
    //

    if (fread(header, sizeof(std::uint8_t), 18, handle) != 18)
        goto file_error;

    //
    // Extract info from the header.
    //

    tga_id_length = header[0x0];
    tga_pal = header[1];
    tga_image_type = header[0x2];
    tga_width = header[0xc] + header[0xd] * 256;
    tga_height = header[0xe] + header[0xf] * 256;
    tga_pixel_depth = header[0x10];

    //
    // Is this a valid tga file?
    //

    ans.valid = 0;
    ans.width = tga_width;
    ans.height = tga_height;
    ans.contains_alpha = 0;

    if (tga_pal != 1) {
        TRACE("Tga must be true colour and uncompressed.\n");
        MF_Fclose(handle);
        return ans;
    }

    if (tga_width > max_width ||
        tga_height > max_height) {
        TRACE("Invalid dimensions:\n\tWanted <= %d x %d\n\tGot       %d x %d\n", max_width, max_height, tga_width, tga_height);
        MF_Fclose(handle);
        return ans;
    }

    //
    // The tga file is valid...
    //

    ans.valid = 1;

    //
    // Skip past the image identification field.
    //

    for (i = 0; i < tga_id_length; i++) {
        if (fread(&junk, sizeof(std::uint8_t), 1, handle) != 1)
            goto file_error;
    }

    if (header[0x1]) {
        std::uint8_t col;
        //
        // The file has colour map data- but how much?
        //

        std::int32_t entries = header[5] + header[6] * 256;
        std::int32_t bitsperentry = header[7];
        std::int32_t length;

        length = (entries * bitsperentry + 7) >> 3;
        ASSERT(length <= 256 * 3);

        {
            if (fread(&pal[0], sizeof(std::uint8_t), length, handle) != length)
                goto file_error;
        }
        for (i = 0; i < tga_width * tga_height; i++) {
            if (fread(&col, sizeof(std::uint8_t), 1, handle) != 1)
                goto file_error;
            col = 255 - col;

            col &= 15;

            data[i].red = remap_pal[col * 4 + 0];
            data[i].green = remap_pal[col * 4 + 1];
            data[i].blue = remap_pal[col * 4 + 2];
            data[i].alpha = 255;
        }
        definitely_no_alpha = 1;
    }

    MF_Fclose(handle);

    //
    // All ok. Does this tga contain any alpha?
    //

    {
        ans.contains_alpha = 0;
    }

    //
    // TGAs are stored upside down to what we expect.
    //

    TGA_Pixel spare;

    for (y = 0; y < tga_height / 2; y++) {
        y1 = y;
        y2 = tga_height - 1 - y;

        for (x = 0; x < tga_width; x++) {
            spare = data[x + y1 * tga_width];
            data[x + y1 * tga_width] = data[x + y2 * tga_width];
            data[x + y2 * tga_width] = spare;
        }
    }

    return ans;

file_error:;

    //
    // Error!
    //

    TRACE("File error loading TGA file %s\n", file);
    MF_Fclose(handle);
    ans.valid = 0;

    return ans;
}

#endif // #ifndef TARGET_DC

#ifndef TARGET_DC

//
// expects to load a 16 colour indexed palletised tga
//
extern volatile HWND hDDLibWindow;

void psx_load_error(char* err, const char* fname) {
#ifndef TARGET_DC
    char title[256];

    //
    // Tell the user about not loading the waypoint.
    //

    sprintf(title, "err >%s<  fname >%s<", err, fname);

    MessageBox(
        hDDLibWindow,
        title,
        "psx texture error",
        MB_OK | MB_ICONERROR | MB_APPLMODAL);
#endif
}

TGA_Info TGA_load_psx(const char* file, std::int32_t max_width, std::int32_t max_height, std::uint8_t* data, std::uint8_t* pal) {
    std::int32_t i;
    std::int32_t x;
    std::int32_t y;
    std::int32_t y1;
    std::int32_t y2;

    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;

    std::int32_t tga_pixel_depth;
    std::int32_t tga_image_type;
    std::int32_t tga_id_length;

    std::int32_t tga_pal;

    std::uint8_t header[18];
    std::uint8_t junk;
    std::uint8_t definitely_no_alpha;

    FILE* handle;

    TGA_Info ans;

    //
    // Open the file.
    //

    handle = MF_Fopen(file, "rb");

    if (!handle) {
        psx_load_error("dont exist", file);
        DebugText(" %s dont exist \n", file);
        ans.valid = 0;
        return ans;
    } else
        DebugText(" file %s exists \n", file);

    //
    // Read the header.
    //

    if (fread(header, sizeof(std::uint8_t), 18, handle) != 18)
        goto file_error;

    //
    // Extract info from the header.
    //

    tga_id_length = header[0x0];
    tga_pal = header[1];
    tga_image_type = header[0x2];
    tga_width = header[0xc] + header[0xd] * 256;
    tga_height = header[0xe] + header[0xf] * 256;
    tga_pixel_depth = header[0x10];

    //
    // Is this a valid tga file?
    //

    ans.valid = 0;
    ans.width = tga_width;
    ans.height = tga_height;
    ans.contains_alpha = 0;

    if (tga_pal != 1 && tga_image_type != 3) {
        psx_load_error("no pal/greyscale", file);
        TRACE("missing pal\n");
        MF_Fclose(handle);
        return ans;
    }

    if (tga_width > max_width ||
        tga_height > max_height) {
        psx_load_error("funny size", file);

        TRACE("Invalid dimensions:\n\tWanted <= %d x %d\n\tGot       %d x %d\n", max_width, max_height, tga_width, tga_height);
        MF_Fclose(handle);
        return ans;
    }

    //
    // The tga file is valid...
    //

    ans.valid = 1;

    //
    // Skip past the image identification field.
    //

    for (i = 0; i < tga_id_length; i++) {
        if (fread(&junk, sizeof(std::uint8_t), 1, handle) != 1)
            goto file_error;
    }

    if (tga_pal) {
        std::uint8_t col, palpos = 0;
        //
        // The file has colour map data- but how much?
        //

        std::int32_t entries = header[5] + header[6] * 256;
        std::int32_t bitsperentry = header[7];
        std::int32_t length;

        length = entries * bitsperentry + 7 >> 3;
        ASSERT(length <= 256 * 3);

        if (fread(&pal[i], sizeof(std::uint8_t), length, handle) != length)
            goto file_error;

        if (fread(data, sizeof(std::uint8_t), tga_width * tga_height, handle) != tga_width * tga_height)
            goto file_error;

        //
        // make data compatable with alternative palettes
        //
        if (tga_width == 64) {
            std::int32_t temp;

            for (i = 0; i < tga_width * tga_height; i++) {
                data[i] = 255 - data[i];
                data[i] &= 15;
            }

            for (i = 0; i < length >> 1; i++) {
                temp = pal[i];
                pal[i] = pal[length - i - 1];
                pal[length - i - 1] = temp;
            }
        } else {
            for (i = 0; i < (length / 3); i++) {
                std::int32_t t;

                t = pal[i * 3 + 0];
                pal[i * 3 + 0] = pal[i * 3 + 2];
                pal[i * 3 + 2] = t;
            }
        }

        {
            std::int32_t pal1 = 0, pal2 = 0, c0;
            if (length >= 255 * 3) {
                for (c0 = 0; c0 < 16 * 3; c0++)
                    pal1 += pal[c0];
                for (c0 = 240 * 3; c0 < 256 * 3; c0++)
                    pal2 += pal[c0];
                if (pal2 > pal1) {
                    //
                    // shunt the pallette down
                    //
                    for (c0 = 0; c0 < 16 * 3; c0++) {
                        pal[c0] = pal[240 * 3 + c0];
                    }
                }
            }
        }

        //
        // read the pal indexes straight in, no conversion to RGB
        //

    } else {
        if (tga_image_type != 3) {
            psx_load_error("not gray scale", file);

            TRACE(" not gray scale!\n");
            return ans;
        }
        std::int32_t c0;
        std::uint8_t remap[256];
        std::int32_t next_pal = 0;
        memset(remap, 0, 255);
        if (fread(data, sizeof(std::uint8_t), tga_width * tga_height, handle) != tga_width * tga_height)
            goto file_error;

        for (c0 = 0; c0 < tga_width * tga_height; c0++) {
            if (remap[data[c0]]) {
                data[c0] = remap[data[c0]] - 1;
            } else {
                pal[next_pal * 3 + 0] = data[c0];
                pal[next_pal * 3 + 1] = data[c0];
                pal[next_pal * 3 + 2] = data[c0];

                remap[data[c0]] = next_pal + 1;

                data[c0] = next_pal;

                next_pal += 1;
            }
        }
    }

    MF_Fclose(handle);

    ans.contains_alpha = 0;

    //
    // TGAs are stored upside down to what we expect.
    //

    std::uint8_t spare;

    for (y = 0; y < tga_height / 2; y++) {
        y1 = y;
        y2 = tga_height - 1 - y;

        for (x = 0; x < tga_width; x++) {
            spare = data[x + y1 * tga_width];
            data[x + y1 * tga_width] = data[x + y2 * tga_width];
            data[x + y2 * tga_width] = spare;
        }
    }

    return ans;

file_error:;
    psx_load_error("Mystery file error", file);

    //
    // Error!
    //

    TRACE("File error loading TGA file %s\n", file);
    MF_Fclose(handle);
    ans.valid = 0;

    return ans;
}

std::uint8_t TGA_header[18] =
    {
        0, 0, 2, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, // Width  LSB:MSB
        0, 1, // Height LSB:MSB
        24,   // Pixel depth
        0};

void TGA_save(
    const char* file,
    std::int32_t width,
    std::int32_t height,
    TGA_Pixel* data,
    std::int32_t contains_alpha) {
    std::int32_t x;
    std::int32_t y;

    std::int32_t num_pixels;
    std::uint8_t header[18];
    std::int32_t bpp;

    FILE* handle;

#ifndef TARGET_DC

    handle = fopen(file, "wb");

#else

    handle = MF_Fopen(file, "wb");

#endif

    if (!handle) {
        TRACE("Cannot open TGA file %s\n", file);

        return;
    }

    //
    // Create the header.
    //

    std::int32_t i;

    for (i = 0; i < 18; i++) {
        header[i] = TGA_header[i];
    }

    header[0xc] = width & 0xff;
    header[0xd] = width >> 8;
    header[0xe] = height & 0xff;
    header[0xf] = height >> 8;

    header[0x10] = (contains_alpha) ? 32 : 24;

    //
    // Write out the header.
    //

    fwrite(&header, sizeof(std::uint8_t), 18, handle);

    //
    // Write out the pixel data.
    //

    for (y = height - 1; y >= 0; y--)
        for (x = 0; x < width; x++) {
            if (contains_alpha) {
                fwrite(&data[x + y * width].alpha, sizeof(std::uint8_t), 1, handle);
            }

            fwrite(&data[x + y * width].blue, sizeof(std::uint8_t), 1, handle);
            fwrite(&data[x + y * width].green, sizeof(std::uint8_t), 1, handle);
            fwrite(&data[x + y * width].red, sizeof(std::uint8_t), 1, handle);
        }

    MF_Fclose(handle);
}

#endif // #ifndef TARGET_DC
