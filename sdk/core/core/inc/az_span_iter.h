// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_SPAN_ITER_H
#define AZ_SPAN_ITER_H

#include <az_result.h>
#include <az_span.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  az_const_span span;
  size_t i;
} az_span_iter;

inline az_span_iter az_span_iter_create(az_const_span const span) {
  return (az_span_iter){.span = span, .i = 0};
}

inline bool az_span_iter_is_empty(az_span_iter const *const p_iter) {
  return p_iter->span.size <= p_iter->i;
}

inline az_result az_span_iter_current(az_span_iter const *const p_iter) {
  return az_const_span_get(p_iter->span, p_iter->i);
}

inline void az_span_iter_next(az_span_iter *const p_iter) {
  if (az_span_iter_is_empty(p_iter)) {
    return;
  }
  p_iter->i += 1;
}

#ifdef __cplusplus
}
#endif

#endif
