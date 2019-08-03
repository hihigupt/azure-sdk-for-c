// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include "../inc/az_core.h"

az_string const s = AZ_STRING("1234567890");

int const array[] = { 1, 2, 3 };

AZ_DEFINE_RANGE(int const, int_range);

int_range const slice = AZ_RANGE(array);
