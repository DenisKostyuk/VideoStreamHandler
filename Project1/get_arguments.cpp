/* ********************************************************************
 * Header File: get_arguments.c
 * Author: Bill Hallahan
 * Date: September 26, 2013
 *
 * Copyright (C) 2013 William Hallahan
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************* */
#include "stdafx.h"
#include "get_arguments.h"

int get_arguments(int argc,
    TCHAR* argv[],
    const ArgOption_t* argument_option_array_ptr,
    int* arg_index_ptr)
{
    int arg_length = 0;
    int option_param_type = OPTION_NO_PARAMETER;
    const ArgOption_t* arg_option_ptr = argument_option_array_ptr;
    TCHAR* argument_ptr = NULL;
    int option_character = ARG_NO_MORE_ARGUMENTS;

    /* Increment the index to point to the next argument.
     * On the first call to this function, this increment
     * skips the program name. */
    ++(*arg_index_ptr);

    /* Are there more arguments? */
    if (*arg_index_ptr < argc)
    {
        /* There are more arguments.  Get the current argument string
         * and the number of characters in the argument string. */
        argument_ptr = argv[*arg_index_ptr];
        arg_length = _tcslen(argument_ptr);

        /* Check for the dash character.  The dash character starts either
         * an optional argument or a number. */
        if (argument_ptr[0] == _T('-'))
        {
            /* Optional arguments always have at least one non-digit character
             * after the dash character. */
            if (arg_length > 1)
            {
                /* A dash followed by a digit, period, or a plus sign is not a
                 * valid switch name and is considered to be a negative number. */
                if ((_istdigit(argument_ptr[1]))
                    || (argument_ptr[1] == _T('.'))
                    || (argument_ptr[1] == _T('+')))
                {
                    option_character = ARG_POSITIONAL_ARGUMENT;
                }
                else
                {
                    /* This is an optional argument.  Long optional arguments
                     * start with two dash characters followed by at least one
                     * character.  Check for a second dash character. */
                    if ((arg_length > 2) && (argument_ptr[1] == _T('-')))
                    {
                        /* This is a long optional argument of the form "--name".
                         * Skip the second '_' character. */
                        argument_ptr += 2;

                        /* Search for the argument long name in the argument options array */
                        while (arg_option_ptr->c != _T('\0'))
                        {
                            if ((arg_option_ptr->long_name != NULL)
                                && (_tcscmp(arg_option_ptr->long_name, argument_ptr) == 0))
                            {
                                option_character = arg_option_ptr->c;
                                option_param_type = arg_option_ptr->requires_param;
                                break;
                            }

                            /* Point to the next ArgOption_t instance in the array. */
                            ++arg_option_ptr;
                        }
                    }
                    else
                    {
                        /* This is a short optional argument of the form "-n".
                         * Skip the '_' character. */
                        ++argument_ptr;

                        /* This is a short optional argument of the form "-n".
                         * Search for the argument character in the argument options array */
                        while (arg_option_ptr->c != _T('\0'))
                        {
                            if (argument_ptr[0] == arg_option_ptr->c)
                            {
                                option_character = arg_option_ptr->c;
                                option_param_type = arg_option_ptr->requires_param;
                                break;
                            }

                            /* Point to the next ArgOption_t instance in the array. */
                            ++arg_option_ptr;
                        }
                    }

                    /* Check to see if the argument option matched any ArgOption_t entry
                     * in the array pointed to by 'argument_option_array_ptr'. */
                    if (option_character != ARG_NO_MORE_ARGUMENTS)
                    {
                        /* An option switch was found.  Does the switch require an argument? */
                        if (option_param_type == OPTION_REQUIRES_PARAMETER)
                        {
                            /* Increment the index to point to the switch argument. */
                            ++(*arg_index_ptr);

                            if (*arg_index_ptr >= argc)
                            {
                                /* The option parameter is missing.  Return an error. */
                                --(*arg_index_ptr);
                                option_character = ARG_ERROR_MISSING_SWITCH_ARGUMENT;
                            }
                        }
                    }
                    else
                    {
                        /* No option matched.  Return an error. */
                        option_character = ARG_ERROR_INVALID_SWITCH;
                    }
                }
            }
            /* A single dash character is not a valid argument. */
            else if (arg_length == 1)
            {
                option_character = ARG_ERROR_MISSING_SWITCH_ARGUMENT;
            }
            else
            {
                option_character = ARG_POSITIONAL_ARGUMENT;
            }
        }
        else
        {
            option_character = ARG_POSITIONAL_ARGUMENT;
        }
    }
    else
    {
        option_character = ARG_NO_MORE_ARGUMENTS;
    }

    return option_character;
}
