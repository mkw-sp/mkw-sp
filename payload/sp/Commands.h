#pragma once

#include <Common.h>

#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#define PRAGMA_SECTION(s)
#else
#define PRAGMA_SECTION(s) __attribute__((section(s)))
#endif

// Matches "/command", "/command arg", but not "/command2"
bool StringStartsWithCommand(const char *line, const char *cmd);

typedef struct Command {
    const char *match;
    const char *desc;
    void (*onBegin)(const char *tmp);
} Command;

#define SP_COMMAND(Match, Desc, Exec) \
    PRAGMA_SECTION("commands") \
    Command MACRO_CONCAT(command_, Exec) = { \
            .match = Match, \
            .desc = Desc, \
            .onBegin = Exec, \
    }

#define sp_define_command_internal(Counter, Match, Desc, Args) \
    void MACRO_CONCAT(cmd, Counter)(Args); \
    SP_COMMAND(Match, Desc, MACRO_CONCAT(cmd, Counter)); \
    void MACRO_CONCAT(cmd, Counter)(Args)

#define sp_define_command(Match, Desc, Args) sp_define_command_internal(__LINE__, Match, Desc, Args)

void Commands_init(void);
const Command *Commands_match(const char *tmp);
void Commands_lineCallback(const char *buf, size_t len);
