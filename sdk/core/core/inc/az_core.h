// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_CORE_H
#define AZ_CORE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AZ_STRING_LITERAL_SIZE(S) (sizeof(S "") - 1)

typedef struct {
  char const *p;
  size_t size;
} az_cstr;

#define AZ_DEFINE_CSTR(NAME, STR) az_cstr const NAME = { .p = (STR), size = AZ_STRING_LITERAL_SIZE(STR) };

inline char const *az_cstr_end(az_cstr a) {
  return a.p + a.size;
}

// Static assert

#define AZ_STATIC_ASSERT(C) typedef int _az_static_assert[(C) ? 1 : -1];

AZ_STATIC_ASSERT(true);

#ifdef __cplusplus
}
#endif

#endif
