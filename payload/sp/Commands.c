#include "Commands.h"

#include "sp/StringView.h"

#include <game/system/SaveManager.h>
#include <game/ui/SectionManager.h>
#include <revolution.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Command __start_commands;
extern Command __stop_commands;

// Matches "/command", "/command arg", but not "/command2"
bool StringStartsWithCommand(const char *line, const char *cmd) {
    const size_t cmdLen = strlen(cmd);
    if (strncmp(line, cmd, cmdLen)) {
        return false;
    }
    return line[cmdLen] == '\0' || line[cmdLen] == ' ';
}

static int Commands_compare(const Command *lhs, const Command *rhs) {
    assert(lhs->match);
    assert(rhs->match);
    return strcmp(lhs->match, rhs->match);
}
static int Commands_compareFn(const void *lhs, const void *rhs) {
    return Commands_compare((const Command *)lhs, (const Command *)rhs);
}

void Commands_init(void) {
    qsort(&__start_commands, &__stop_commands - &__start_commands, sizeof(__start_commands),
            Commands_compareFn);
}

const Command *Commands_match(const char *tmp) {
    assert(tmp);
    assert(tmp[0] != '\0');

    for (const Command *it = &__start_commands; it < &__stop_commands; ++it) {
        assert(it->match);
        if (StringStartsWithCommand(tmp, it->match)) {
            return it;
        }
    }

    return NULL;
}

void Commands_lineCallback(const char *buf, size_t len) {
    StringView view = (StringView){.s = buf, .len = len};
    const char *tmp = sv_as_cstr(view, 64);

    OSReport("[SP] Line submitted: %s\n", tmp);

    const Command *it = Commands_match(tmp);
    if (it == NULL) {
        OSReport("&aUnknown command \"%s\"\n", tmp);
        return;
    }
    if (it->onBegin == NULL) {
        OSReport("&aCommand not implemented \"%s\"\n", tmp);
        return;
    }
    it->onBegin(tmp);
}

static void Commands_printHelp(size_t page) {
    const size_t numCommands = &__stop_commands - &__start_commands;
    const size_t commandsPerPage = 9;
    const size_t numPages = (numCommands + commandsPerPage - 1) / commandsPerPage;

    OSReport(" &e---- &6Help &e-- &6Page &c%u&6/&c%u &e----\n", (unsigned)page + 1,
            (unsigned)numPages);

    for (size_t i = page * commandsPerPage; i < MIN((page + 1) * commandsPerPage, numCommands);
            ++i) {
        const Command *command = &__start_commands + i;
        OSReport("&6%s&f: %s\n", command->match, command->desc);
    }
}

sp_define_command("/help", "Views a list of all available commands.", const char *tmp) {
    int page = 1;
    sscanf(tmp, "/help %i", &page);
    Commands_printHelp(MAX(page - 1, 0));
}
