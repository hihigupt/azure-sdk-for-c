// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_ITER_FROM_SPAN_H
#define AZ_ITER_FROM_SPAN_H

#include <az_iter.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Note: it's a non-movable structure.
 */
typedef struct {
  az_const_span span;
  az_iter iter;
} az_iter_from_span;

az_next_span_result az_iter_from_span_func(az_const_span const* p_span) {
  if (az_span_is_empty(*p_span)) {
    return (az_next_span_result){.result = AZ_ERROR_NO_MORE_ITEMS, .span= az_const_span_empty()};
  }
  az_next_span_result const result = {.result = AZ_OK, .span = *p_span};
  *p_span = az_const_span_empty();
  return result;
}

inline void az_iter_from_span_init(az_const_span const span, az_iter_from_span *const out_i) {
  out_i->span = span;
  out_i->iter = az_iter_create((az_next_span){ .context = &out_i->span, .func = az_iter_from_span_func });
}

#ifdef __cplusplus
}
#endif

#endif
