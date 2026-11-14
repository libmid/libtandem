#ifndef TD_CORO_H
#define TD_CORO_H

#include "common.h"

TD_API td_rt *td_init();
TD_API void td_free(td_rt*);

// Spawn a coroutine
TD_API td_coro *td_spawn(td_rt*, void (*fn)(td_rt*), size_t argc, void *argv, size_t stack_size);

// Suspend a coroutine back to the main coroutine where `td_init` was called.
// Nothing happens when called from the main coroutine.
TD_API void td_suspend(td_rt*);

// Resume a paused, waiting coroutine.
TD_API void td_resume(td_rt*, td_coro*);

// When called from inside a coroutine they retrieve the argc and argv.
TD_API size_t td_argc(td_rt*);
TD_API void *td_argv(td_rt*);

// Set the return value
TD_API void td_return(td_rt*, void *result);

// Consume the return value.
// Also marks the coroutine as DEAD.
TD_API void* td_consume(td_rt*, td_coro*);

#endif
