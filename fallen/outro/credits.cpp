//
// Draws the credits
//

#include "always.h"
#include "key.h"
#include "font.h"
#include "os.h"

//
// The credits for each section.
//

char* CREDITS_muckyfoot[] =
    {
        "Mucky Foot are Ashley Hampton, Barry Meade, Chris Knott, Eddie",
        "Edwards, Fin McGechie, Gary Carr, Guy Simmons, James 'Dudley'",
        "Watson, Jan Svarovsky, John Steels, Junior Walker, Justin Amore,",
        "Mark Smart, Mark Adami, Martin Oliver, Matthew Rosenfeld, Mike",
        "Burnham, Mike Diskett, Ollie Shaw, Richard Franke, Simon 'Grimmy'",
        "Keating, Stuart Black, Tom Forsyth, Tom Ireland and Wayne Imlach.",
        nullptr,
        "Here's an effort to divide up what we did to make Urban Chaos!",
        nullptr,

        "~BProgramming",
        "\tMike Diskett",
        "\tMark Adami",
        "\tMatthew Rosenfeld",
        "\tJames 'Dudley' Watson",
        "\tEddie Edwards",
        "\tGuy Simmons",
        "\tTom Forsyth",
        "~I\tAdditional programming",
        "\t\tJeremy Longley",
        nullptr,

        "~BArt Direction",
        "\tFin McGechie",
        nullptr,

        "~BArt",
        "\tStuart Black",
        "\tRichard Franke",
        "\tOllie Shaw",
        "\tFin McGechie",
        "\tJunior Walker",
        "\tChris Knott",
        "\tGary Carr",
        "~I\tAdditional art",
        "\t\tTerry Catrell",
        "\t\tJoe Rider",
        "\t\tSteve Brown",
        nullptr,

        "~BAnimation",
        "\tOllie Shaw",
        "\tJunior Walker",
        "\tChris Knott",
        nullptr,

        "~BLevel Design",
        "\tSimon 'Grimmy' Keating",
        "\tBarry Meade",
        nullptr,

        "~BSound and Music",
        "\tMartin Oliver",
        nullptr,

        "~BScripting",
        "\tBarry Meade",
        "\tSimon 'Grimmy' Keating",
        "\tFin McGechie",
        nullptr,

        "~BTesting",
        "\tJustin (mucky)hands Amore",
        "~I\tAdditional testing",
        "\t\tSean Lamacraft",
        "\t\tMarie Colwell",
        "\t\tMark Rose",
        "\t\tDavid Harlow",
        "~I\tFurther testing",
        "\t\tChristopher Absolom",
        "\t\tMark Baker",
        "\t\tAlex Blackwood",
        "\t\tDahman Coombes",
        "\t\tTom Everard",
        "\t\tEamon Meadows",
        "\t\tAnthony Nicholson",
        "\t\tTom Patterson",
        "\t\tLawrence Phillips",
        "\t\tDaniel Purvis",
        "\t\tGary Reed",
        "\t\tAmy Ross",
        "\t\tPeter Ruscoe",
        "\t\tKraig Stone",
        "\t\tLorne Tietjen",
        "\t\tDavid Walker",
        "\t\tDavid Wright",
        nullptr,
        "\tMichael Burnham",
        "\tTom Forsyth",
        "\tGary Carr",
        "\tJohn Steels",
        "\tTom Ireland",
        "\tJan Svarovsky",
        "\tand everyone at MuckyFoot",
        nullptr,

        "~BIT and Mucky Website",
        "\tMichael Burnham",
        nullptr,

        "~BPR",
        "\tCathy Campos at PanachePR",
        nullptr,

        "~BFurry Friends",
        "\tSam",
        "\tLisa",
        "\tBarney",
        nullptr,

        "Special thanks to Glenn Corpes and a big mucky \"Thank You\" from",
        "Mucky Foot to everyone at Eidos. Hello to the Wednesday night",
        "footballers!",
        "!"};

char* CREDITS_eidos_uk[] =
    {
        "~BSenior Producer",
        "\tDarren Hedges",
        nullptr,
        "~BLocalisation Team",
        "\tHolly Andrews",
        "\tFlavia Timiani",
        nullptr,

        "~BMarketing",
        "\tKaren Ridley",
        "\tLorna Evans",
        nullptr,

        "~BPR",
        "\tJonathan Rosenblatt",
        "\tSteve Starvis",
        nullptr,

        "~BCreative Services",
        "\tCaroline Simon",
        nullptr,

        "~BLocalisation QA",
        "\tMichael Weissmuller",
        "\tAlex Lepoureau",
        "\tAlessandro Mantelli",
        nullptr,

        "~BQA Manager",
        "\tTony Bourne",
        nullptr,

        "~BLead QA",
        "\tJean Duret",
        nullptr,

        "~BQA",
        "\tLinus Dominique (Assistant Lead)",
        "\tJason Walker (Hardware Specialist)",
        "\tMichael Hanley",
        "\tAndrew Christopher",
        "\tAnthony Fretwell",
        "\tBJ Samuel Kil",
        "\tChris Ince",
        "\tClint Nembhard",
        "\tEsmond Ferns",
        "\tGuy Cooper",
        "\tLouis Farnham",
        "\tMarlon Grant",
        "\tNoel Cowan",
        "\tPatrick Cowan",
        "\tTyrone O'Neil",
        nullptr,

        "~BWith thanks for the last leg",
        "\tIain McNeil",
        "\tGrant Dean",
        "!"};

char* CREDITS_eidos_usa[] =
    {
        "~BAssociate Producer",
        "\tEric Adams",
        nullptr,

        "~BQA Manager",
        "\tMike McHale",
        nullptr,

        "~BProduct Manager",
        "\tJennifer Fitzsimmons",
        nullptr,

        "~BMarketing Manager",
        "\tSusan Boshkoff",
        nullptr,

        "~BPR",
        "\tBrian Kemp",
        "\tGreg Rizzer",
        nullptr,

        "~BTesting",
        "\tCorey Fong",
        "\tKjell Vistad",
        "\tRalph Ortiz",
        "\tJohn Arvay",
        "!"};

char* CREDITS_eidos_france[] =
    {
        "~BChef de produit",
        "\tOlivier Salomon",
        nullptr,

        "~BResponsable localisation",
        "\tSt�phan Gonizzi", // This has got an accent
        nullptr,

        "~BResponsable RP",
        "\tPriscille Demoly",
        nullptr,

        "~BTesteur de la VF",
        "\tGuillaume Mahouin",
        nullptr,

        "~BTraduction",
        "\tAround the Word, Paris",
        nullptr,

        "~BEnregistrement des voix fran�aises", // This has got an accent in it!
        "\tLe Lotus Rose, Paris",
        "!"};

/*

//
// The Translated version...
//

{
        "~BMarketing Manager",
        "\tOlivier Salomon",
        nullptr,

        "~BLocalisation Manager",
        "\tSt�phan Gonizzi",
        nullptr,

        "~BPR Manager",
        "\tPriscille Demoly",
        nullptr,

        "~BLocalisation Testing",
        "\tGuillaume Mahouin",
        "!"
};

*/

char* CREDITS_eidos_germany[] =
    {
        "~BLeiter Produktentwicklung",
        "\tBeco Mulderij",
        nullptr,

        "~BProdukt-Manager",
        "\tLars Wittkuhn",
        nullptr,

        "~BLeiter Marketing",
        "\tKnut-Jochen Bergel",
        nullptr,

        "~BPR-Manager",
        "\tSascha Denise Green-Kaiser",
        nullptr,

        "~BLokalisierungs-Manager",
        "\tHeidi Maria Kohne",
        nullptr,

        "~BQA-Manager",
        "S�ren Winterfeldt", // Accent!
        nullptr,

        "~B�bersetzung", // Accent!
        "\tViolet Media, Isabel Sterner",
        nullptr,

        "~BAudio",
        "\tHastings International Audio Network",
        nullptr,

        "~BTonmeister",
        "\tCedric Hopf",
        "!"

};

char* CREDITS_voice_production[] =
    {
        "~BCasting",
        "\tPhil Morris at AllintheGame",
        nullptr,

        "~BVoice Production",
        "\tBarry Meade",
        "\tMartin Oliver",
        "\tChris O'Saughanessy",
        "\tPhil Morris",
        nullptr,

        "~BVoice Actors",
        "\tJohnnie Fiori",
        "\tDan Russell",
        "\tSharon Holm",
        "\tKerry Shale",
        "\tJulienne Davis",
        "\tColin McFarlane",
        "\tTed Maynard",
        "\tBrad Lavelle",
        "\tTogo Igawa",
        nullptr,

        "~BTranslation",
        "\tAround the Word, Paris",
        nullptr,

        "~BFrench Voices",
        "\tLe Lotus Rose, Paris",
        "!"};

char* CREDITS_bands[] =
    {
        "Way Out West - Urban Chaos",
        "The 3 Jays - Feeling it too",
        "Tour De Force ",
        "Asian Dub Foundation and Audioactive - Psycho Buds",
        "Infidels - Everything",
        "Infidels - Ooma Gabba",
        "Special Forces 'Something Else... The bleep tune' courtesy of Photek Productions",
        nullptr,
        "Many Thanks to Miles Jacobson at Anglo.",
        "!"};

//
// The credits grouped into sections.
//

typedef struct
{
    char* title;
    char** line;

} CREDITS_Section;

#define CREDITS_NUM_SECTIONS 7

CREDITS_Section CREDITS_section[CREDITS_NUM_SECTIONS] =
    {
        {"MUCKYFOOT",
         CREDITS_muckyfoot},

        {"EIDOS UK",
         CREDITS_eidos_uk},

        {"EIDOS USA",
         CREDITS_eidos_usa},

        {"EIDOS FRANCE",
         CREDITS_eidos_france},

        {"EIDOS GERMANY",
         CREDITS_eidos_germany},

        {"VOICE PRODUCTION",
         CREDITS_voice_production},

        {"ORIGINAL CD MUSIC",
         CREDITS_bands}};

//
// The current section and y-offset.
//

std::int32_t CREDITS_current_section;
float CREDITS_current_y;
float CREDITS_current_end_y;

std::int32_t CREDITS_last;
std::int32_t CREDITS_now;

void CREDITS_init() {
    CREDITS_current_section = 0;
    CREDITS_current_y = 1.0F;
    CREDITS_now = 0;
    CREDITS_last = 0;
}

void CREDITS_draw() {
    std::int32_t i;
    std::int32_t dont_draw;
    std::int32_t flag;
    float x;
    float y;
    float shimmer;
    float scale;

    CREDITS_Section* cs;

    CREDITS_now = OS_ticks();

    //
    // Never be more than 1/2 a second behind...
    //

    if (CREDITS_last < CREDITS_now - 500) {
        CREDITS_last = CREDITS_now - 500;
    }

    if (KEY_on[KEY_S]) {
        CREDITS_last -= 200;
    }

    //
    // Scroll upwards...
    //

    CREDITS_current_y -= (CREDITS_now - CREDITS_last) * 0.00005F;

    //
    // Draw the title of the section.
    //

    ASSERT(WITHIN(CREDITS_current_section, 0, CREDITS_NUM_SECTIONS - 1));

    cs = &CREDITS_section[CREDITS_current_section];

    //
    // Draw the title.
    //

    if (CREDITS_current_y > 0.6F) {
        shimmer = 1.0F - (1.0F - CREDITS_current_y) * (1.0F / 0.4F);
    } else if (CREDITS_current_end_y < 0.50F) {
        shimmer = 1.0F - (CREDITS_current_end_y - 0.10F) * (1.0F / 0.4F);
    }

    SATURATE(shimmer, 0.0F, 1.0F);

    FONT_draw(
        FONT_FLAG_JUSTIFY_LEFT,
        0.03F, 0.05F,
        0xffffff,
        1.5F,
        -1,
        shimmer,
        cs->title);

    //
    // Draw all the text in the current section.
    //

    y = CREDITS_current_y;

    for (i = 0; cs->line[i] == nullptr || cs->line[i][0] != '!'; i++) {
        flag = FONT_FLAG_JUSTIFY_LEFT;
        scale = 0.6F;
        dont_draw = false;

        if (!cs->line[i]) {
            //
            // Blank line.
            //

            dont_draw = true;
        } else if (y < 0.10F) {
            //
            // Dont draw...
            //

            dont_draw = true;
        } else if (y < 0.30F) {
            shimmer = 1.0F - (y - 0.10F) * (1.0F / 0.2F);
        } else if (y < 0.75F) {
            shimmer = 0.0F;
        } else if (y < 0.95F) {
            shimmer = (y - 0.75F) * (1.0F / 0.2F);
        } else {
            //
            // Don't draw...
            //

            dont_draw = true;
        }

        if (cs->line[i]) {
            char* text = cs->line[i];

            //
            // What style do we draw the text?
            //

            if (text[0] == '~') {
                switch (text[1]) {
                    case 'B':
                        scale = 1.0F;
                        break;

                    case 'I':
                        flag |= FONT_FLAG_ITALIC;
                        break;

                    default:
                        ASSERT(0);
                        break;
                }

                text += 2;
            }

            //
            // Whats the tabbing?
            //

            x = 0.06F;

            while (*text == '\t') {
                text += 1;
                x += 0.03F;
            }

            if (!dont_draw) {
                //
                // Draw the text!
                //

                FONT_draw(
                    flag,
                    x, y,
                    0xffffff,
                    scale,
                    -1,
                    shimmer,
                    text);
            }
        }

        y += 0.05F * scale;
    }

    CREDITS_current_end_y = y;

    if (y < 0.10F) {
        //
        // Time to move onto the next set of credits!
        //

        CREDITS_current_section += 1;
        CREDITS_current_y = 1.0F;

        if (CREDITS_current_section >= CREDITS_NUM_SECTIONS) {
            CREDITS_current_section = 0;
        }
    }

    CREDITS_last = CREDITS_now;
}
