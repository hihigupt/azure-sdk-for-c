// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_curl_adapter.h>

typedef struct {
  struct curl_slist * p_list;
} az_curl_headers;

az_result az_curl_headers_append(az_curl_headers * const p_state, az_pair const header) {
  const az_span_seq token_seq = az_build_header_to_callback(&header);
  char * str_header;
  AZ_RETURN_IF_FAILED(az_span_seq_to_new_str(token_seq, &str_header));

  p_state->p_list = curl_slist_append(p_state->p_list, str_header);

  free(str_header);
  return AZ_OK;
}

AZ_CALLBACK_IMPL(az_curl_headers_append, az_curl_headers *, az_pair_visitor)

az_result az_build_headers(az_http_request const * const p_request, az_curl_headers * curl_headers) {
  az_pair_visitor const pair_visitor = az_curl_headers_append_to_callback(curl_headers);

  az_pair_seq const request_headers_seq = p_request->headers;
  AZ_RETURN_IF_FAILED(request_headers_seq.func(request_headers_seq.data, pair_visitor));

  return AZ_OK;
}

az_result az_send_request_impl(az_http_request const * const p_request) {
  az_curl p_curl;
  AZ_RETURN_IF_FAILED(az_curl_init(&p_curl));
  az_result result = az_curl_send_request(&p_curl, p_request);
  az_curl_done(&p_curl);
  return result;
}

az_result az_curl_send_request(az_curl * const p_curl, az_http_request const * const p_request) {
  // creates a slist for bulding curl headers
  az_curl_headers headers = {
    .p_list = NULL,
  };
  // build headers into a slist as curl is expecting
  AZ_RETURN_IF_FAILED(az_build_headers(p_request, &headers));
  // set all headers from slist
  curl_easy_setopt(p_curl->p_curl, CURLOPT_HTTPHEADER, headers.p_list);

  // set URL
  char * url;
  AZ_RETURN_IF_FAILED(az_http_url_to_new_str(p_request, &url));
  curl_easy_setopt(p_curl->p_curl, CURLOPT_URL, url);
  free(url);

  CURLcode res = curl_easy_perform(p_curl->p_curl);
  if (res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

  // TODO: would this be part of done function instead?
  curl_slist_free_all(headers.p_list);

  return AZ_OK;
}
