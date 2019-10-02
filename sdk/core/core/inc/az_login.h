// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_LOGIN_H
#define AZ_LOGIN_H

#include <az_result.h>
#include <az_span.h>

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

az_result az_login(az_login_info const * const p_login_info);

#include <_az_cfg_suffix.h>

#endif
