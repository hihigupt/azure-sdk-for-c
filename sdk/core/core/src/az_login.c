// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_login.h>

#include <cstddef.h>

#include <_az_cfg_warn.h>

az_result az_login(az_login_info const * const login_info, az_cookie * const out_cookie) {
  if (login_info == NULL) {
    return AZ_ERROR_ARG;
  }

  switch (login_info->kind) {
    case AZ_LOGIN_INFO_KIND_USERNAME_AND_PASSWORD:
      return AZ_ERROR_NOT_IMPLEMENTED;
    default:
      return AZ_ERROR_ARG;
  }

  return AZ_OK;
}

az_result az_logout(az_cookie const * const out_cookie) {
  if (out_cookie == NULL) {
    return AZ_ERROR_ARG;
  }

  return AZ_ERROR_NOT_IMPLEMENTED;
}
