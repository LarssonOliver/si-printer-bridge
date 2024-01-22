#pragma once

void console_init(void);
void console_tick(void);

// Thread safe print to console.
int console_printf(const char *__restrict, ...)
    __attribute__((__format__(__printf__, 1, 2)));
