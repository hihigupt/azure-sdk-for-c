// SPDX-License-Identifier: MIT

#ifndef AZ_ITER_H
#define AZ_ITER_H

#include <az_span_iter.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  az_result result;
  az_const_span span;
} az_next_span_result;

typedef struct {
  void *context;
  az_next_span_result (*func)(void *const context);
} az_next_span;

inline az_next_span_result az_next_span_call(az_next_span const next_span) {
  return next_span.func(next_span.context);
}

typedef struct {
  az_result result;
  az_span_iter span_i;
} az_iter_state;

inline az_iter_state az_iter_state_create(az_next_span const next_span) {
  az_next_span_result const r = az_next_span_call(next_span);
  if (az_failed(r.result)) {
    return (az_iter_state){.result = r.result,
                           .span_i = az_span_iter_create(az_const_span_empty())};
  }
  az_span_iter const span_i = az_span_iter_create(r.span);
  return (az_iter_state){.result = az_span_iter_current(&span_i), .span_i = span_i};
}

typedef struct {
  az_next_span next_span;
  az_iter_state state;
} az_iter;

inline az_iter az_iter_create(az_next_span const next_span) {
  return (az_iter){.next_span = next_span, .state = az_iter_state_create(next_span)};
}

inline bool az_iter_is_empty(az_iter const *const p_i) { return az_failed(p_i->state.result); }

inline az_result az_iter_current(az_iter const *const p_i) { return p_i->state.result; }

inline void az_iter_next(az_iter *const p_i) {
  if (az_iter_is_empty(p_i)) {
    return;
  }
  az_span_iter_next(&p_i->state.span_i);
  az_result const result = az_span_iter_current(&p_i->state.span_i);
  if (az_succeeded(result)) {
    p_i->state.result = result;
    return;
  }
  p_i->state = az_iter_state_create(p_i->next_span);
}

#ifdef __cplusplus
}
#endif

#endif
