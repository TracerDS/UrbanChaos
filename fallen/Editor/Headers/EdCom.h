// EdCom.h
// Guy Simmons, 16th March 1998.

#ifndef ED_COM_H
#define ED_COM_H

//---------------------------------------------------------------
// Editor condition stuff.

#define MAX_EDIT_CONDITIONS 1000

struct EditCondition {
    bool Used;

    std::uint16_t Flags,
        ConditionType,
        Data1,
        Data2,
        Data3,
        GroupRef;

    EditCondition *Next,
        *Prev;
};

extern std::uint32_t ed_condition_count;
extern EditCondition edit_conditions[MAX_EDIT_CONDITIONS];

//---------------------------------------------------------------

void init_ed_conditions();
EditCondition *alloc_ed_condition();
void free_ed_condition(EditCondition *the_condition);

//---------------------------------------------------------------
// Editor condition list stuff.

#define MAX_EDIT_CLISTS 100

struct EditCondList {
    bool Used;
    char CListName[32];

    EditCondition *ConditionList,
        *ConditionListEnd;

    std::uint32_t ConditionCount;
    EditCondList *Next,
        *Prev;
};

extern std::uint32_t ed_clist_count;
extern EditCondList edit_clists[MAX_EDIT_CLISTS],
    *clists,
    *clists_end,
    *win_conditions,
    *lose_conditions;

#define ED_CONLIST_NUMBER(c) (std::uint16_t) (c - edit_clists)

//---------------------------------------------------------------

void init_ed_clists();
EditCondList *alloc_ed_clist();
void free_ed_clist(EditCondList *the_clist);
void add_condition(EditCondList *the_clist, EditCondition *the_condition);
void remove_condition(EditCondList *the_clist, EditCondition *the_condition);
void move_condition(EditCondList *the_clist, EditCondition *insert_point, EditCondition *the_condition);

//---------------------------------------------------------------
// Editor command stuff.

#define MAX_EDIT_COMMANDS 2000

struct EditCommand {
    bool Used;

    std::uint16_t Flags,
        CommandType,
        Data1,
        Data2,
        Data3,
        GroupRef;

    EditCommand *Next,
        *Prev;
};

extern std::uint32_t ed_command_count;
extern EditCommand edit_commands[MAX_EDIT_COMMANDS];

//---------------------------------------------------------------

void init_ed_commands();
EditCommand *alloc_ed_command();
void free_ed_command(EditCommand *the_command);

//---------------------------------------------------------------
// Editor command list stuff.

#define MAX_EDIT_COMLISTS 200

struct EditComList {
    bool Used;
    char ComListName[32];

    EditCommand *CommandList,
        *CommandListEnd;

    std::uint32_t CommandCount;
    EditComList *Next,
        *Prev;
};

extern std::uint32_t ed_comlist_count;
extern EditComList edit_comlists[MAX_EDIT_COMLISTS],
    *comlists,
    *comlists_end;

#define ED_COMLIST_NUMBER(c) (std::uint16_t) (c - edit_comlists)

//---------------------------------------------------------------

void init_ed_comlists();
EditComList *alloc_ed_comlist();
void free_ed_comlist(EditComList *the_clist);
void add_command(EditComList *the_clist, EditCommand *the_command);
void remove_command(EditComList *the_comlist, EditCommand *the_command);
void move_command(EditComList *the_comlist, EditCommand *insert_point, EditCommand *the_command);

//---------------------------------------------------------------

#endif
