// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_login.h>

#include <stddef.h>

#include <_az_cfg_warn.h>

az_result az_login(az_login_info login_info, az_session * const out_session) {
  switch (login_info.kind) {
    case AZ_LOGIN_INFO_KIND_USERNAME_AND_PASSWORD:
      if (login_info.data.username_and_password.username.size == 0
          || login_info.data.username_and_password.password.size == 0) {
        return AZ_ERROR_ARG;
      }

      return AZ_ERROR_NOT_IMPLEMENTED;
    default:
      return AZ_ERROR_ARG;
  }

  return AZ_OK;
}

az_result az_logout(az_session * const p_session) {
  if (p_session == NULL) {
    return AZ_ERROR_ARG;
  }

  return AZ_ERROR_NOT_IMPLEMENTED;
}
