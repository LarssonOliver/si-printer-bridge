#pragma once

void console_init(void);
void console_tick(void);

int console_printf(const char *__restrict, ...)
    _ATTRIBUTE((__format__(__printf__, 1, 2)));
