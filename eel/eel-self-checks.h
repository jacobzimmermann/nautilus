/*
   eel-self-checks.h: The self-check framework.
 
   Copyright (C) 1999 Eazel, Inc.
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
  
   You should have received a copy of the GNU Library General Public
   License along with this program; if not, see <http://www.gnu.org/licenses/>.
  
   Author: Darin Adler <darin@eazel.com>
*/

#pragma once

#include <glib.h>

#define EEL_CHECK_RESULT(type, expression, expected_value) \
G_STMT_START { \
    eel_before_check (#expression, __FILE__, __LINE__); \
    eel_check_##type##_result (expression, expected_value); \
} G_STMT_END

#define EEL_CHECK_BOOLEAN_RESULT(expression, expected_value) \
    EEL_CHECK_RESULT(boolean, expression, expected_value)
#define EEL_CHECK_INTEGER_RESULT(expression, expected_value) \
    EEL_CHECK_RESULT(integer, expression, expected_value)
#define EEL_CHECK_STRING_RESULT(expression, expected_value) \
    EEL_CHECK_RESULT(string, expression, expected_value)

void eel_exit_if_self_checks_failed (void);
void eel_before_check_function      (const char    *name);
void eel_after_check_function       (void);
void eel_before_check               (const char    *expression,
                                     const char    *file_name,
                                     int            line_number);
void eel_after_check                (void);

/* Both 'result' and 'expected' get freed with g_free */
void eel_report_check_failure       (char          *result,
                                     char          *expected);
void eel_check_boolean_result       (gboolean       result,
                                     gboolean       expected_value);
void eel_check_integer_result       (long           result,
                                     long           expected_value);
void eel_check_string_result        (char          *result,
                                     const char    *expected_value);

#define EEL_SELF_CHECK_FUNCTION_PROTOTYPE(function) \
    void function (void);

#define EEL_CALL_SELF_CHECK_FUNCTION(function) \
    eel_before_check_function (#function); \
    function (); \
    eel_after_check_function ();
