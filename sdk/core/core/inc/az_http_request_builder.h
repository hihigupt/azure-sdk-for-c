// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_HTTP_REQUEST_BUILDER_H
#define AZ_HTTP_REQUEST_BUILDER_H

#include <az_result.h>
#include <az_span.h>

#include <stdint.h>

#include <_az_cfg_prefix.h>

typedef struct {
  az_span buffer;
  az_const_span method_verb;
  az_span url;
  uint16_t max_url_size;
  uint16_t max_headers;
  uint16_t retry_headers_start;
  uint16_t headers_end;
} az_http_request_builder;

extern az_const_span const AZ_HTTP_METHOD_VERB_GET;
extern az_const_span const AZ_HTTP_METHOD_VERB_HEAD;
extern az_const_span const AZ_HTTP_METHOD_VERB_POST;
extern az_const_span const AZ_HTTP_METHOD_VERB_PUT;
extern az_const_span const AZ_HTTP_METHOD_VERB_DELETE;
extern az_const_span const AZ_HTTP_METHOD_VERB_TRACE;
extern az_const_span const AZ_HTTP_METHOD_VERB_OPTIONS;
extern az_const_span const AZ_HTTP_METHOD_VERB_CONNECT;
extern az_const_span const AZ_HTTP_METHOD_VERB_PATCH;

az_result az_http_request_builder_init(
    az_http_request_builder * const p_hrb,
    az_span const buffer,
    az_const_span const method_verb,
    az_const_span const initial_url,
    uint16_t const max_url_size,
    uint16_t const max_headers);

az_result az_http_request_builder_set_query_parameter(
    az_http_request_builder * const p_hrb,
    az_const_span const name,
    az_const_span const value);

az_result az_http_request_builder_append_header(
    az_http_request_builder * const p_hrb,
    az_const_span const name,
    az_const_span const value);

az_result az_http_request_builder_mark_retry_headers_start(az_http_request_builder * const p_hrb);

az_result az_http_request_builder_remove_retry_headers(az_http_request_builder * const p_hrb);

#include <_az_cfg_suffix.h>

#endif
