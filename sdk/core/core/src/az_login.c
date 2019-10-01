// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_login.h>

az_result az_login(az_login_info const * const p_login_info) {
  if (p_login_info == NULL) {
    return AZ_LOGIN_ERROR_INPUT_PARAM;
  }

  switch (p_login_info->kind) {
    case AZ_LOGIN_INFO_KIND_USERNAME_AND_PASSWORD:
      return AZ_ERROR_NOT_IMPLEMENTED;
    default:
      return AZ_ERROR_ARG;
  }

  return AZ_OK;
}
