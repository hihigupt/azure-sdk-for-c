// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_HTTP_RESPONSE_H
#define AZ_HTTP_RESPONSE_H

#include <az_span_reader.h>

#include <_az_cfg_prefix.h>

typedef struct {
  az_span_reader reader;
} az_http_response_state;

static inline az_http_response_state az_http_response_state(az_const_span const buffer) {
  return (az_http_response_state){ .reader = az_span_reader_create(buffer) };
}

az_result az_http_response_read_status_code(
    az_http_response_state * const p_state,
    uint16_t * const out);

#include <_az_cfg_suffix.h>

#endif
