// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_contract_internal.h>
#include <az_credentials_internal.h>
#include <az_http.h>
#include <az_http_internal.h>
#include <az_http_transport.h>
#include <az_json.h>
#include <az_storage_blobs.h>
#include <az_config_internal.h>

#include <stddef.h>

#include <_az_cfg.h>

static az_span const AZ_STORAGE_BLOBS_BLOB_HEADER_X_MS_BLOB_TYPE
    = AZ_SPAN_LITERAL_FROM_STR("x-ms-blob-type");

static az_span const AZ_STORAGE_BLOBS_BLOB_TYPE_BLOCKBLOB = AZ_SPAN_LITERAL_FROM_STR("BlockBlob");

static az_span const AZ_HTTP_HEADER_CONTENT_LENGTH = AZ_SPAN_LITERAL_FROM_STR("Content-Length");
static az_span const AZ_HTTP_HEADER_CONTENT_TYPE = AZ_SPAN_LITERAL_FROM_STR("Content-Type");

AZ_NODISCARD az_storage_blobs_blob_client_options az_storage_blobs_blob_client_options_default(
    az_http_transport_options const * http_transport_options) {

    return (az_storage_blobs_blob_client_options) {
    ._internal = { .http_transport_options = *http_transport_options,
                   .api_version = (_az_http_policy_apiversion_options) { 
                        ._internal = { 
                            .option_location = _az_http_policy_apiversion_option_location_header,
                            .name = AZ_SPAN_FROM_STR("x-ms-version"),
                            .version = AZ_STORAGE_API_VERSION,
                            }
                        },
                   ._telemetry_options = _az_http_policy_telemetry_options_default() },
    .retry = az_http_policy_retry_options_default(),
  };

}

AZ_NODISCARD az_result az_storage_blobs_blob_client_init(
    az_storage_blobs_blob_client * self,
    az_span uri,
    void * credential,
    az_storage_blobs_blob_client_options * options) {
  AZ_CONTRACT_ARG_NOT_NULL(self);
  AZ_CONTRACT_ARG_NOT_NULL(options);

  _az_credential * const cred = (_az_credential *)credential;

  *self = (az_storage_blobs_blob_client) {
    ._internal = {
      .uri = AZ_SPAN_FROM_BUFFER(self->_internal.url_buffer),
      .options = *options,
      .credential = cred,
      .pipeline = (_az_http_pipeline){
        ._internal = {
          .p_policies = {
            {
              ._internal = {
                .process = az_http_pipeline_policy_apiversion,
                .p_options= &self->_internal.options._internal.api_version,
              },
            },
            {
              ._internal = {
                .process = az_http_pipeline_policy_uniquerequestid,
                .p_options = NULL,
              },
            },
            {
              ._internal = {
                .process = az_http_pipeline_policy_telemetry,
                .p_options = &self->_internal.options._internal._telemetry_options,
              },
            },
            {
              ._internal = {
                .process = az_http_pipeline_policy_retry,
                .p_options = &self->_internal.options.retry,
              },
            },
            {
              ._internal = {
                .process = az_http_pipeline_policy_credential,
                .p_options = cred,
              },
            },
            {
              ._internal = {
                .process = az_http_pipeline_policy_logging,
                .p_options = NULL,
              },
            },
            {
              ._internal = {
                .process = az_http_pipeline_policy_bufferresponse,
                .p_options = NULL,
              },
            },
            {
              ._internal = {
                .process= az_http_pipeline_policy_distributedtracing,
                .p_options = NULL,
              },
            },
            {
              ._internal = {
                .process = az_http_pipeline_policy_transport,
                .p_options= &self->_internal.options._internal.http_transport_options,
              },
            },
          },
        }
      }
    }
  };

  // Copy url to client buffer so customer can re-use buffer on his/her side
  AZ_RETURN_IF_FAILED(az_span_copy(self->_internal.uri, uri, &self->_internal.uri));

  AZ_RETURN_IF_FAILED(
      _az_credential_set_scopes(cred, AZ_SPAN_FROM_STR("https://storage.azure.com/.default")));

  return AZ_OK;
}

AZ_NODISCARD az_result az_storage_blobs_blob_upload(
    az_storage_blobs_blob_client * client,
    az_span content, /* Buffer of content*/
    az_storage_blobs_blob_upload_options * options,
    az_http_response * response) {

  az_storage_blobs_blob_upload_options opt;
  if (options == NULL) {
    opt = az_storage_blobs_blob_upload_options_default();
  } else {
    opt = *options;
  }
  (void)opt;

  // Request buffer
  // create request buffer TODO: define size for a blob upload
  uint8_t url_buffer[AZ_HTTP_REQUEST_URL_BUF_SIZE];
  az_span request_url_span = AZ_SPAN_FROM_BUFFER(url_buffer);
  // copy url from client
  AZ_RETURN_IF_FAILED(az_span_copy(request_url_span, client->_internal.uri, &request_url_span));
  uint8_t headers_buffer[AZ_HTTP_REQUEST_HEADER_BUF_SIZE];
  az_span request_headers_span = AZ_SPAN_FROM_BUFFER(headers_buffer);

  // create request
  _az_http_request hrb;
  AZ_RETURN_IF_FAILED(az_http_request_init(
      &hrb, az_http_method_put(), request_url_span, request_headers_span, content));

  // add blob type to request
  AZ_RETURN_IF_FAILED(az_http_request_append_header(
      &hrb, AZ_STORAGE_BLOBS_BLOB_HEADER_X_MS_BLOB_TYPE, AZ_STORAGE_BLOBS_BLOB_TYPE_BLOCKBLOB));

  //
  uint8_t content_length[_az_INT64_AS_STR_BUF_SIZE] = { 0 };
  az_span content_length_builder = AZ_SPAN_FROM_BUFFER(content_length);
  AZ_RETURN_IF_FAILED(az_span_append_int64(&content_length_builder, az_span_length(content)));

  // add Content-Length to request
  AZ_RETURN_IF_FAILED(
      az_http_request_append_header(&hrb, AZ_HTTP_HEADER_CONTENT_LENGTH, content_length_builder));

  // add blob type to request
  AZ_RETURN_IF_FAILED(az_http_request_append_header(
      &hrb, AZ_HTTP_HEADER_CONTENT_TYPE, AZ_SPAN_FROM_STR("text/plain")));

  // start pipeline
  return az_http_pipeline_process(&client->_internal.pipeline, &hrb, response);
}
