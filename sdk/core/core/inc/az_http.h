// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef AZ_HTTP_H
#define AZ_HTTP_H

#include <az_result.h>
#include <az_span.h>

#include <_az_cfg_prefix.h>

/*
typedef struct {
  az_const_span method;
  az_const_span url;
  az_const_span body;
} az_http_request;

typedef struct {
  int code;
  az_const_span body;
} az_http_response;

az_result az_http_send(az_http_request const request, az_span const response_buffer, az_http_response * out_response);
*/

// 1.

IEnumerable send1(IEnumarable c);

az_result az_tcp_send(az_ipaddress const address, az_iter const iter, az_iter * const output);

foreach (output) {....}

// 2.

void send2(IEnumerable c, Action<string> action) {
  foreach (s in send1(c)) { action(s); }
}

struct x {};

az_result write(az_const_span const x) {

}

az_result az_tcp_send(az_ipaddress const address, az_iter const iter, az_write const write) {
  .....for () { result = write(); if (write()) { close(); return result; }
}

#include <_az_cfg_suffix.h>

#endif
