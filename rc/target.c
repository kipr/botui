#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// QProcess does not correctly emulate a terminal, so output is not flushed by newline.
// This sets flush after every write, which is unfortunately slower.
__attribute__((constructor))
static void __set_no_stdout_buffer() {
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
}

#ifdef __cplusplus
}
#endif