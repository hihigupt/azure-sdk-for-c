// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_json_read.h>

#include <ctype.h>
#include <math.h>

inline bool az_json_is_white_space(az_result const c) {
  switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      return true;
  }
  return false;
}

inline bool az_json_is_esc(az_result const c) {
  switch (c) {
    case '\\':
    case '"':
    case '/':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
      return true;
  }
  return false;
}

inline bool az_json_is_e(az_result const c) {
  switch (c) {
    case 'e':
    case 'E':
      return true;
  }
  return false;
}

inline az_result az_json_error_unexpected(az_result const c) {
  return az_failed(c) ? c : AZ_JSON_ERROR_UNEXPECTED_CHAR;
}

inline bool az_json_stack_is_empty(az_json_state const *const p) { return p->stack == 1; }

inline az_json_stack_item az_json_stack_last(az_json_state const *const p) { return p->stack & 1; }

inline az_result az_json_stack_push(az_json_state *const p_state, az_json_stack const stack) {
  if (p_state->stack >> AZ_JSON_STACK_SIZE != 0) {
    return AZ_JSON_ERROR_STACK_OVERFLOW;
  }
  p_state->stack = (p_state->stack << 1) | stack;
  return AZ_OK;
}

inline az_result az_json_stack_pop(az_json_state *const p_state) {
  if (p_state->stack <= 1) {
    return AZ_JSON_ERROR_INVALID_STATE;
  }
  p_state->stack >>= 1;
  return AZ_OK;
}

az_json_state az_json_state_create(az_iter const i) {
  return (az_json_state){
      .i = i,
      .stack = 1,
  };
}

az_result az_json_read_expected_char(az_span_iter *const p_i, uint8_t const expected) {
  az_result const c = az_span_iter_current(p_i);
  if (c != expected) {
    return az_json_error_unexpected(c);
  }
  az_span_iter_next(p_i);
  return AZ_OK;
}

void az_json_read_white_space(az_span_iter *const p_iter) {
  while (az_json_is_white_space(az_span_iter_current(p_iter))) {
    az_span_iter_next(p_iter);
  }
}

az_result az_json_read_keyword_rest(az_span_iter *const p_iter, az_const_span const keyword) {
  az_span_iter_next(p_iter);
  az_span_iter k = az_span_iter_create(keyword);
  while (true) {
    az_result const ko = az_span_iter_current(&k);
    if (az_failed(ko)) {
      return AZ_OK;
    }
    az_result const o = az_span_iter_current(p_iter);
    if (o != ko) {
      return az_json_error_unexpected(o);
    }
    az_span_iter_next(p_iter);
    az_span_iter_next(&k);
  }
}

// 18 decimal digits. 10^18 - 1.
//                        0         1
//                        012345678901234567
#define AZ_DEC_NUMBER_MAX 999999999999999999ull

typedef struct {
  int sign;
  uint64_t value;
  bool remainder;
  int16_t exp;
} az_dec_number;

double az_json_number_to_double(az_dec_number const *p) {
  return p->value * pow(10, p->exp) * p->sign;
}

az_result az_json_number_int_parse(
    az_iter *const p_i,
    az_dec_number *const p_n,
    int16_t const e_offset,
    az_result const first) {
  az_result c = first;
  // read an integer part of the number
  while (true) {
    int d = c - '0';
    if (p_n->value <= (AZ_DEC_NUMBER_MAX - d) / 10) {
      p_n->value = p_n->value * 10 + d;
      p_n->exp += e_offset;
    } else {
      if (d != 0) {
        p_n->remainder = true;
      }
      p_n->exp += e_offset + 1;
    }
    az_iter_next(p_i);
    c = az_iter_current(p_i);
    if (!isdigit(c)) {
      return AZ_OK;
    }
  };
}

az_result az_json_read_number_digit_rest(az_iter *const p_i, double *const out_value) {
  az_dec_number i = {
      .sign = 1,
      .value = 0,
      .remainder = false,
      .exp = 0,
  };

  // integer part
  {
    az_result o = az_iter_current(p_i);
    if (o == '-') {
      i.sign = -1;
      az_iter_next(p_i);
      o = az_iter_current(p_i);
      if (!isdigit(o)) {
        return az_json_error_unexpected(o);
      }
    }
    if (o != '0') {
      AZ_RETURN_IF_NOT_OK(az_json_number_int_parse(p_i, &i, 0, o));
    } else {
      az_iter_next(p_i);
    }
  }

  // fraction
  if (az_iter_current(p_i) == '.') {
    az_iter_next(p_i);
    az_result o = az_iter_current(p_i);
    if (!isdigit(o)) {
      return az_json_error_unexpected(o);
    }
    AZ_RETURN_IF_NOT_OK(az_json_number_int_parse(p_i, &i, -1, o));
  }

  // exp
  if (az_json_is_e(az_iter_current(p_i))) {
    // skip 'e' or 'E'
    az_iter_next(p_i);
    az_result c = az_iter_current(p_i);

    // read sign, if any.
    int8_t e_sign = 1;
    switch (c) {
      case '-':
        e_sign = -1;
      case '+':
        az_iter_next(p_i);
        c = az_iter_current(p_i);
    }

    // expect at least one digit.
    if (!isdigit(c)) {
      return az_json_error_unexpected(c);
    }

    uint16_t e_int = 0;
    do {
      e_int = e_int * 10 + (uint16_t)(c - '0');
      az_iter_next(p_i);
      c = az_iter_current(p_i);
    } while (isdigit(c));
    i.exp += e_int * e_sign;
  }

  *out_value = az_json_number_to_double(&i);
  return AZ_OK;
}

az_result az_json_read_string_rest(az_iter *const p_reader, az_const_span *const string) {
  // skip '"'
  size_t const begin = p_reader->i;
  while (true) {
    az_result c = az_iter_current(p_reader);
    switch (c) {
        // end of the string
      case '"': {
        *string = az_const_span_sub(p_reader->span, begin, p_reader->i);
        az_span_iter_next(p_reader);
        return AZ_OK;
      }
        // escape sequence
      case '\\': {
        az_span_iter_next(p_reader);
        c = az_span_iter_current(p_reader);
        if (az_json_is_esc(c)) {
          az_span_iter_next(p_reader);
        } else if (c == 'u') {
          az_span_iter_next(p_reader);
          for (size_t i = 0; i < 4; ++i, az_span_iter_next(p_reader)) {
            c = az_span_iter_current(p_reader);
            if (!isxdigit(c)) {
              return az_json_error_unexpected(c);
            }
          }
        } else {
          return az_json_error_unexpected(c);
        }
        break;
      }
      default: {
        if (c < 0x20) {
          return az_json_error_unexpected(c);
        }
        az_span_iter_next(p_reader);
      }
    }
  }
}

// _value_
az_result az_json_read_value(az_json_state *const p_state, az_json_value *const out_value) {
  az_iter *const p_i = &p_state->i;
  az_result const c = az_iter_current(p_i);
  if (isdigit(c)) {
    out_value->kind = AZ_JSON_VALUE_NUMBER;
    return az_json_read_number_digit_rest(p_i, &out_value->data.number);
  }
  switch (c) {
    case 't':
      out_value->kind = AZ_JSON_VALUE_BOOLEAN;
      out_value->data.boolean = true;
      return az_json_read_keyword_rest(p_i, AZ_STR("rue"));
    case 'f':
      out_value->kind = AZ_JSON_VALUE_BOOLEAN;
      out_value->data.boolean = false;
      return az_json_read_keyword_rest(p_i, AZ_STR("alse"));
    case 'n':
      out_value->kind = AZ_JSON_VALUE_NULL;
      return az_json_read_keyword_rest(p_i, AZ_STR("ull"));
    case '"':
      out_value->kind = AZ_JSON_VALUE_STRING;
      az_span_iter_next(p_i);
      return az_json_read_string_rest(p_i, &out_value->data.string);
    case '-':
      out_value->kind = AZ_JSON_VALUE_NUMBER;
      return az_json_read_number_digit_rest(p_i, &out_value->data.number);
    case '{':
      out_value->kind = AZ_JSON_VALUE_OBJECT;
      az_span_iter_next(p_i);
      return az_json_stack_push(p_state, AZ_JSON_STACK_OBJECT);
    case '[':
      out_value->kind = AZ_JSON_VALUE_ARRAY;
      az_span_iter_next(p_i);
      return az_json_stack_push(p_state, AZ_JSON_STACK_ARRAY);
  }
  return az_json_error_unexpected(c);
}

az_result az_json_read_value_space(az_json_state *const p_state, az_json_value *const out_value) {
  AZ_RETURN_IF_NOT_OK(az_json_read_value(p_state, out_value));
  az_json_read_white_space(&p_state->i);
  return AZ_OK;
}

az_result az_json_read(az_json_state *const p_state, az_json_value *const out_value) {
  if (!az_json_stack_is_empty(p_state)) {
    return AZ_JSON_ERROR_INVALID_STATE;
  }
  az_json_read_white_space(&p_state->i);
  AZ_RETURN_IF_NOT_OK(az_json_read_value_space(p_state, out_value));
  bool const is_empty = az_span_iter_is_empty(&p_state->i);
  switch (out_value->kind) {
    case AZ_JSON_VALUE_ARRAY:
    case AZ_JSON_VALUE_OBJECT:
      return is_empty ? AZ_JSON_ERROR_UNEXPECTED_END : AZ_OK;
  }
  return is_empty ? AZ_OK : AZ_JSON_ERROR_UNEXPECTED_CHAR;
}

inline uint8_t az_json_stack_item_to_close(az_json_stack_item const item) {
  return item == AZ_JSON_STACK_OBJECT ? '}' : ']';
}

az_result az_json_read_comma_or_close(az_json_state *const p_state) {
  az_result const c = az_span_iter_current(&p_state->i);
  if (c == ',') {
    // skip ',' and read all whitespaces.
    az_span_iter_next(&p_state->i);
    az_json_read_white_space(&p_state->i);
    return AZ_OK;
  }
  uint8_t const close = az_json_stack_item_to_close(az_json_stack_last(p_state));
  if (c != close) {
    return az_json_error_unexpected(c);
  }
  return AZ_OK;
}

az_result az_json_check_item_begin(
    az_json_state *const p_state,
    az_json_stack_item const stack_item) {
  if (az_json_stack_is_empty(p_state) || az_json_stack_last(p_state) != stack_item) {
    return AZ_JSON_ERROR_INVALID_STATE;
  }
  az_result const c = az_span_iter_current(&p_state->i);
  if (c != az_json_stack_item_to_close(stack_item)) {
    return AZ_OK;
  }
  // c == close
  AZ_RETURN_IF_NOT_OK(az_json_stack_pop(p_state));
  az_span_iter_next(&p_state->i);
  az_json_read_white_space(&p_state->i);
  if (!az_json_stack_is_empty(p_state)) {
    AZ_RETURN_IF_NOT_OK(az_json_read_comma_or_close(p_state));
  }
  return AZ_JSON_NO_MORE_ITEMS;
}

az_result az_json_check_item_end(az_json_state *const p_state, az_json_value const value) {
  switch (value.kind) {
    case AZ_JSON_VALUE_OBJECT:
    case AZ_JSON_VALUE_ARRAY:
      return AZ_OK;
  }
  return az_json_read_comma_or_close(p_state);
}

az_result az_json_read_object_member(
    az_json_state *const p_state,
    az_json_member *const out_member) {
  AZ_RETURN_IF_NOT_OK(az_json_check_item_begin(p_state, AZ_JSON_STACK_OBJECT));
  AZ_RETURN_IF_NOT_OK(az_json_read_expected_char(&p_state->i, '"'));
  AZ_RETURN_IF_NOT_OK(az_json_read_string_rest(&p_state->i, &out_member->name));
  az_json_read_white_space(&p_state->i);
  AZ_RETURN_IF_NOT_OK(az_json_read_expected_char(&p_state->i, ':'));
  az_json_read_white_space(&p_state->i);
  AZ_RETURN_IF_NOT_OK(az_json_read_value_space(p_state, &out_member->value));
  return az_json_check_item_end(p_state, out_member->value);
}

az_result az_json_read_array_element(
    az_json_state *const p_state,
    az_json_value *const out_element) {
  AZ_RETURN_IF_NOT_OK(az_json_check_item_begin(p_state, AZ_JSON_STACK_ARRAY));
  AZ_RETURN_IF_NOT_OK(az_json_read_value_space(p_state, out_element));
  return az_json_check_item_end(p_state, *out_element);
}

az_result az_json_state_done(az_json_state const *const p_state) {
  if (!az_span_iter_is_empty(&p_state->i) || !az_json_stack_is_empty(p_state)) {
    return AZ_JSON_ERROR_INVALID_STATE;
  }
  return AZ_OK;
}
