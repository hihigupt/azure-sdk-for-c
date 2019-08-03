#include "../inc/az_json.h"

az_string const null_string = AZ_STRING("null");

az_string const true_string = AZ_STRING("true");

az_string const false_string = AZ_STRING("false");

static inline az_error write_string(az_json_write_string const f, az_string const s) {
  return f.write(f.context, s);
}

static inline az_error write_char(az_json_write_string const f, char const c) {
  az_string const s = AZ_STRING(&c);
  return write_string(f, s);
}

static inline az_error write_json_number(az_json_write_string const f, double const number) {
  // TODO:
  return write_char(f, '0');
}

#define RETURN_ON_ERROR(E) { az_error const result = (E); if (result != AZ_OK) { return result; } }

static inline az_error write_json_string(az_json_write_string const f, az_string const s) {
  RETURN_ON_ERROR(write_char(f, '"'));
  // TODO:
  RETURN_ON_ERROR(write_char(f, '"'));
  return AZ_OK;
}

static inline az_error write_json_property(az_json_write_string const f, az_json_property const p) {
  RETURN_ON_ERROR(write_json_string(f, p.name));
  RETURN_ON_ERROR(write_char(f, ':'));
  RETURN_ON_ERROR(az_json_write(f, p.value));
  return AZ_OK;
}

static inline az_error write_json_object(az_json_write_string const f, az_json_object a) {
  RETURN_ON_ERROR(write_char(f, '{'));
  if (a.begin != a.end) {
    RETURN_ON_ERROR(write_json_property(f, *a.begin));
    for (++a.begin; a.begin != a.end; ++a.begin) {
      RETURN_ON_ERROR(write_char(f, ','));
      RETURN_ON_ERROR(write_json_property(f, *a.begin));
    }
  }
  RETURN_ON_ERROR(write_char(f, '}'));
  return AZ_OK;
}

static inline az_error write_json_array(az_json_write_string const f, az_json_array a) {
  RETURN_ON_ERROR(write_char(f, '['));
  if (a.begin != a.end) {
    RETURN_ON_ERROR(az_json_write(f, *a.begin));
    for (++a.begin; a.begin != a.end; ++a.begin) {
      RETURN_ON_ERROR(write_char(f, ','));
      RETURN_ON_ERROR(az_json_write(f, *a.begin));
    }
  }
  RETURN_ON_ERROR(write_char(f, ']'));
  return AZ_OK;
}

static inline az_error write_json_null(az_json_write_string const f) {
  return write_string(f, null_string);
}

static inline az_error write_json_boolean(az_json_write_string const f, bool const b) {
  return write_string(f, b ? true_string : false_string);
}

az_error az_json_write(az_json_write_string const f, az_json const json) {
  switch (json.type) {
    case AZ_JSON_TYPE_BOOLEAN:
      return write_json_boolean(f, json.boolean);
    case AZ_JSON_TYPE_NUMBER:
      return write_json_number(f, json.number);
    case AZ_JSON_TYPE_STRING:
      return write_json_string(f, json.string);
    case AZ_JSON_TYPE_OBJECT:
      return write_json_object(f, json.object);
    case AZ_JSON_TYPE_ARRAY:
      return write_json_array(f, json.array);
    case AZ_JSON_TYPE_NULL:
    default:
      return write_json_null(f);
  }
}
