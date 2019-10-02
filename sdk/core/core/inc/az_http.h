// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_HTTP_H
#define AZ_HTTP_H

#include <az_result.h>

#include <_az_cfg_prefix.h>

typedef struct {

} az_http_request;

typedef struct {

} az_http_response;

az_result az_http_send(az_http_request const request, az_http_response * out_response);

#include <_az_cfg_suffix.h>

#endif
