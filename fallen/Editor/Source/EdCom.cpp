// EdCom.cpp
// Guy Simmons, 16th March 1998

#include "Editor.hpp"
#include "EdCom.h"

//---------------------------------------------------------------
//	Editor condition list stuff.
//---------------------------------------------------------------

std::uint32_t ed_clist_count = 0;
EditCondList edit_clists[MAX_EDIT_CLISTS],
    *clists,
    *clists_end,
    *win_conditions,
    *lose_conditions;

void add_clist(EditCondList* the_clist);
void remove_clist(EditCondList* the_clist);

//---------------------------------------------------------------

void init_ed_clists() {
    ed_clist_count = 0;
    memset(edit_clists, 0, sizeof(edit_clists));
    clists = nullptr;
    clists_end = nullptr;

    init_ed_conditions();
}

//---------------------------------------------------------------

EditCondList* alloc_ed_clist() {
    std::uint16_t c0;
    EditCondList* the_clist;

    for (c0 = 1; c0 < MAX_EDIT_CLISTS; c0++) {
        if (!edit_clists[c0].Used) {
            the_clist = &edit_clists[c0];
            the_clist->Used = true;
            sprintf(the_clist->CListName, "Condition List %d", c0);

            the_clist->ConditionCount = 0;
            the_clist->ConditionList = nullptr;
            the_clist->ConditionListEnd = nullptr;

            add_clist(the_clist);
            ed_clist_count++;

            return the_clist;
        }
    }
    return nullptr;
}

//---------------------------------------------------------------

void free_ed_clist(EditCondList* the_clist) {
    EditCondition* current_condition;

    //	Firstly remove all conditions.
    current_condition = the_clist->ConditionList;
    while (current_condition) {
        remove_condition(the_clist, current_condition);
        free_ed_condition(current_condition);

        current_condition = current_condition->Next;
    }

    //	Now free the condition list.
    remove_clist(the_clist);

    the_clist->Used = false;
    ed_clist_count--;
}

//---------------------------------------------------------------

void add_clist(EditCondList* the_clist) {
    //	Add to list.
    the_clist->Prev = clists_end;
    the_clist->Next = nullptr;

    //	Update list end.
    if (clists_end)
        clists_end->Next = the_clist;
    clists_end = the_clist;

    //	Update list.
    if (!clists)
        clists = the_clist;
}

//---------------------------------------------------------------

void remove_clist(EditCondList* the_clist) {
    EditCondList *next_clist,
        *prev_clist;

    // Remove from list
    prev_clist = the_clist->Prev;
    next_clist = the_clist->Next;

    if (prev_clist)
        prev_clist->Next = next_clist;
    else
        clists = next_clist;

    if (next_clist)
        next_clist->Prev = prev_clist;
    else
        clists_end = prev_clist;
}

//---------------------------------------------------------------

void add_condition(EditCondList* the_clist, EditCondition* the_condition) {
    if (!the_clist || !the_condition) {
        // Error.
        return;
    }

    //	Add to list.
    the_condition->Prev = the_clist->ConditionListEnd;
    the_condition->Next = nullptr;

    //	Update list end.
    if (the_clist->ConditionListEnd)
        the_clist->ConditionListEnd->Next = the_condition;
    the_clist->ConditionListEnd = the_condition;

    //	Update list.
    if (!the_clist->ConditionList)
        the_clist->ConditionList = the_condition;

    the_clist->ConditionCount++;
}

//---------------------------------------------------------------

void remove_condition(EditCondList* the_clist, EditCondition* the_condition) {
    EditCondition *next_condition,
        *prev_condition;

    // Remove from list
    prev_condition = the_condition->Prev;
    next_condition = the_condition->Next;

    if (prev_condition)
        prev_condition->Next = next_condition;
    else
        the_clist->ConditionList = next_condition;

    if (next_condition)
        next_condition->Prev = prev_condition;
    else
        the_clist->ConditionListEnd = prev_condition;

    the_clist->ConditionCount--;
}

//---------------------------------------------------------------

void move_condition(EditCondList* the_clist, EditCondition* insert_point, EditCondition* the_condition) {
}

//---------------------------------------------------------------
//	Editor condition stuff.
//---------------------------------------------------------------

std::uint32_t ed_condition_count = 0;
EditCondition edit_conditions[MAX_EDIT_CONDITIONS];

//---------------------------------------------------------------

void init_ed_conditions() {
    ed_condition_count = 0;
    memset(edit_conditions, 0, sizeof(edit_conditions));
}

//---------------------------------------------------------------

EditCondition* alloc_ed_condition() {
    std::uint16_t c0;
    EditCondition* the_condition;

    for (c0 = 1; c0 < MAX_EDIT_CONDITIONS; c0++) {
        if (edit_conditions[c0].Used == false) {
            the_condition = &edit_conditions[c0];
            the_condition->Used = true;

            the_condition->Flags = 0;
            the_condition->ConditionType = 0;
            the_condition->Data1 = 0;
            the_condition->Data2 = 0;
            the_condition->Data3 = 0;

            ed_condition_count++;

            return the_condition;
        }
    }
    return nullptr;
}

//---------------------------------------------------------------

void free_ed_condition(EditCondition* the_condition) {
    the_condition->Used = false;
    ed_condition_count--;
}

//---------------------------------------------------------------
//	Editor command list stuff.
//---------------------------------------------------------------

std::uint32_t ed_comlist_count = 0;
EditComList edit_comlists[MAX_EDIT_COMLISTS],
    *comlists,
    *comlists_end;

void add_comlist(EditComList* the_comlist);
void remove_comlist(EditComList* the_comlist);

//---------------------------------------------------------------

void init_ed_comlists() {
    ed_comlist_count = 0;
    comlists = nullptr;
    comlists_end = nullptr;
    memset(edit_comlists, 0, sizeof(edit_comlists));

    init_ed_commands();
}

//---------------------------------------------------------------

EditComList* alloc_ed_comlist() {
    std::uint16_t c0;
    EditComList* the_comlist;

    for (c0 = 1; c0 < MAX_EDIT_COMLISTS; c0++) {
        if (!edit_comlists[c0].Used) {
            the_comlist = &edit_comlists[c0];
            the_comlist->Used = true;
            sprintf(the_comlist->ComListName, "Command List %d", c0);

            the_comlist->CommandCount = 0;
            the_comlist->CommandList = nullptr;
            the_comlist->CommandListEnd = nullptr;

            add_comlist(the_comlist);
            ed_comlist_count++;

            return the_comlist;
        }
    }
    return nullptr;
}

//---------------------------------------------------------------

void free_ed_comlist(EditComList* the_comlist) {
    EditCommand* current_command;

    //	Firstly remove all commands.
    current_command = the_comlist->CommandList;
    while (current_command) {
        remove_command(the_comlist, current_command);
        free_ed_command(current_command);

        current_command = current_command->Next;
    }

    //	Now free the command list.
    remove_comlist(the_comlist);

    the_comlist->Used = false;
    ed_comlist_count--;
}

//---------------------------------------------------------------

void add_comlist(EditComList* the_comlist) {
    //	Add to list.
    the_comlist->Prev = comlists_end;
    the_comlist->Next = nullptr;

    //	Update list end.
    if (comlists_end)
        comlists_end->Next = the_comlist;
    comlists_end = the_comlist;

    //	Update list.
    if (!comlists)
        comlists = the_comlist;
}

//---------------------------------------------------------------

void remove_comlist(EditComList* the_comlist) {
    EditComList *next_comlist,
        *prev_comlist;

    // Remove from list
    prev_comlist = the_comlist->Prev;
    next_comlist = the_comlist->Next;

    if (prev_comlist)
        prev_comlist->Next = next_comlist;
    else
        comlists = next_comlist;

    if (next_comlist)
        next_comlist->Prev = prev_comlist;
    else
        comlists_end = prev_comlist;
}

//---------------------------------------------------------------

void add_command(EditComList* the_comlist, EditCommand* the_command) {
    if (!the_comlist || !the_command) {
        // Error.
        return;
    }

    //	Add to list.
    the_command->Prev = the_comlist->CommandListEnd;
    the_command->Next = nullptr;

    //	Update list end.
    if (the_comlist->CommandListEnd)
        the_comlist->CommandListEnd->Next = the_command;
    the_comlist->CommandListEnd = the_command;

    //	Update list.
    if (!the_comlist->CommandList)
        the_comlist->CommandList = the_command;

    the_comlist->CommandCount++;
}

//---------------------------------------------------------------

void remove_command(EditComList* the_comlist, EditCommand* the_command) {
    EditCommand *next_command,
        *prev_command;

    // Remove from list
    prev_command = the_command->Prev;
    next_command = the_command->Next;

    if (prev_command)
        prev_command->Next = next_command;
    else
        the_comlist->CommandList = next_command;

    if (next_command)
        next_command->Prev = prev_command;
    else
        the_comlist->CommandListEnd = prev_command;

    the_comlist->CommandCount--;
}

//---------------------------------------------------------------

void move_command(EditComList* the_comlist, EditCommand* insert_point, EditCommand* the_command) {
}

//---------------------------------------------------------------
//	Editor command stuff.
//---------------------------------------------------------------

std::uint32_t ed_command_count = 0;
EditCommand edit_commands[MAX_EDIT_COMMANDS];

//---------------------------------------------------------------

void init_ed_commands() {
    ed_command_count = 0;
    memset(edit_commands, 0, sizeof(edit_commands));
}

//---------------------------------------------------------------

EditCommand* alloc_ed_command() {
    std::uint16_t c0;
    EditCommand* the_command;

    for (c0 = 1; c0 < MAX_EDIT_COMMANDS; c0++) {
        if (edit_commands[c0].Used == false) {
            the_command = &edit_commands[c0];
            the_command->Used = true;

            the_command->Flags = 0;
            the_command->CommandType = 0;
            the_command->Data1 = 0;
            the_command->Data2 = 0;
            the_command->Data3 = 0;

            ed_command_count++;

            return the_command;
        }
    }
    return nullptr;
}

//---------------------------------------------------------------

void free_ed_command(EditCommand* the_command) {
    the_command->Used = false;
    ed_command_count--;
}

//---------------------------------------------------------------
