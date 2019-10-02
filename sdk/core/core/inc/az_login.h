// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_LOGIN_H
#define AZ_LOGIN_H

#include <az_result.h>
#include <az_span.h>

#include <cstddef.h>

#include <_az_cfg_prefix.h>

typedef enum {
  AZ_LOGIN_INFO_KIND_USERNAME_AND_PASSWORD = 1,
} az_login_info_kind;

typedef struct {
  az_const_span username;
  az_const_span password;
} az_login_info_username_and_password;

typedef struct {
  az_login_info_kind kind;
  union {
    az_login_info_username_and_password username_and_password;
  } data;
} az_login_info;

static inline az_result az_login_info_init_username_and_password(
    az_const_span const username,
    az_const_span const password,
    az_login_info * const out_login_info) {
  if (username == NULL || password == NULL || out_login_info == NULL) {
    return AZ_ERROR_ARG;
  }

  *out_login_info = { .kind = AZ_LOGIN_INFO_KIND_USERNAME_AND_PASSWORD,
                      .data = { .username = username, .password = password } };

  return AZ_OK;
}

typedef struct {
} az_cookie;

az_result az_login(az_login_info const * const login_info, az_cookie * const out_cookie);
az_result az_logout(az_cookie const * const cookie);

#include <_az_cfg_suffix.h>

#endif
