/* ********************************************************************
 * Header File: get_arguments.h
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

#ifndef GET_ARGUMENTS_H
#define GET_ARGUMENTS_H

#include "platform_os.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /* The get_arguments function either returns an option
     * character or one of these values. */
    typedef enum
    {
        ARG_ERROR_INVALID_SWITCH = -2,
        ARG_ERROR_MISSING_SWITCH_ARGUMENT = -1,
        ARG_NO_MORE_ARGUMENTS = 0,
        ARG_POSITIONAL_ARGUMENT = 0x7FFFFFFF
    } ArgOptionChar_t;

    /* Used to set whether an option requires a parameter. */
    typedef enum
    {
        OPTION_NO_PARAMETER,
        OPTION_REQUIRES_PARAMETER
    } ArgOptionParameter_t;

    /* The option structure.
     * Element c stores the short option character, i.e. "b" for "-b".
     *   name is the long option name string, "build" for "--build".
     * requires_params is set to one of the ArgOptionParameter_t
     *   enum values to indicate whether the option requires a parameter. */
    typedef struct
    {
        int c;
        TCHAR* long_name;
        ArgOptionParameter_t requires_param;
    } ArgOption_t;

    /* ********************************************************************
    * Function: get_arguments
    *
    * Abstract:
    *
    *  The get_arguments function was written to provide argument
    *  parsing code for multiple platforms.  Also, the code can
    *  be used under the terms of the MIT license.
    *
    *  The get_arguments function allows both short and long
    *  optional switches, and will work with the ASCII, UTF-8, and
    *  Unicode character sets.
    *
    *  Both single character and long optional argument switch
    *  named are allowed.
    *
    *  Optional arguments and positional arguments can be interspersed.
    *
    *  The benefit of allowing interspersed optional and positional
    *  arguments comes at the cost of requiring that an optional argument
    *  either does, or does not, take an additional parameter, but not both.
    *
    *  Also, optional argument names cannot start with a digit
    *  character.  This simplified parsing as a dash followed by
    *  a digit character could also be the start of a numeric
    *  positional argument.
    *
    *  Input:
    *
    *    The first two arguments are passed into the 'main' function of
    *    a C or C++ console program.
    *
    *    argc                      - The number of arguments.
    *
    *    argv                      - An array of pointers to argument strings.
    *
    *    argument_option_array_ptr - A pointer to an array of ArgOption_t
    *                                structures that specifies that attributes
    *                                of each optional argument.
    *
    *                                An example declaration for the array might be:
    *
    *                                static const ArgOption_t argument_option_array[] =
    *                                {
    *                                    { 'x', NULL, OPTION_NO_PARAMETER, },
    *                                    { 'f', NULL, OPTION_REQUIRES_PARAMETER, },
    *                                    { 'b', "build", OPTION_REQUIRES_PARAMETER, },
    *                                    { 'h', "help", OPTION_NO_PARAMETER, },
    *                                    {   0, NULL, OPTION_NO_PARAMETER }
    *                                };
    *
    *                                This would allow optional arguments of the form:
    *
    *                                -x
    *                                -f <somestring>
    *                                -b <somestring>
    *                                --build <somestring>
    *                                -h
    *                                --help
    *
    *                                A string must be supplied after the -s parameter
    *                                because OPTION_REQUIRES_PARAMETER is used.
    *
    *                                The final line must always be supplied as written
    *                                to terminate the data.
    *
    *                                Either a short argument name, a long
    *                                argument name, or both must be specified.
    *                                If only a long name is specified, the first
    *                                field of the ArgOption_t structure must be
    *                                set to an out-of-band integer value, which
    *                                for either ASCII or Unicode character sets
    *                                can be any value above 0x011FFFF.
    *
    *
    *    arg_index_ptr               A pointer to an 'int' value that is used
    *                                to index into the argv[] array. The value
    *                                pointed to by arg_index_ptr specifies either
    *                                the index of a positional argument or the
    *                                index of the required parameter for an
    *                                optional argument that has the
    *                                OPTION_REQUIRES_PARAMETER attribute.
    *
    *  Returns:
    *
    *    An integer value that is either the option character for an
    *    optional argument, which is the first field in OptionsArg_t
    *    structure that matches the optional argument passed on the
    *    command line, or, one of the following values.
    *
    *    ARG_POSITIONAL_ARGUMENT  - Specified that argument in the argv
    *                               array, indexed by the index value
    *                               pointed to by arg_index_ptr, is a
    *                               positional argument.
    *
    *    ARG_NO_MORE_ARGUMENTS    - Specifies that there are no more
    *                               arguments to parse.
    *
    *    ARG_ERROR_INVALID_SWITCH - Invalid switch specified on the command
    *                               line.
    *
    *    ARG_ERROR_MISSING_SWITCH_ARGUMENT - A dash character for an
    *                                        optional argument was found
    *                                        but is missing required
    *                                        characters following the dash.
    *
    *  Any returned character value, represented as an integer, and
    *  the value of ARG_POSITIONAL_ARGUMENT, are always greater than
    *  zero.  ARG_NO_MORE_ARGUMENTS equals zero, and the error return
    *  values are negative numbers, so parsing should be done until
    *  the return value of this function is not greater than
    *  ARG_NO_MORE_ARGUMENTS, or zero.
    *
    ******************************************************************* */

    int get_arguments(int argc,
        TCHAR* argv[],
        const ArgOption_t* argument_option_array_ptr,
        int* arg_index_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GET_ARGUMENTS_H */
