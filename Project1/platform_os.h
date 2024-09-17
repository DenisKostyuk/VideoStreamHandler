/* ********************************************************************
 * Header File: platform_os.h
 * Author: Bill Hallahan
 * Date: September 26, 2013
 *
 * Platform and operating system specific definitions.
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

#ifndef PLATFORM_OS_H
#define PLATFORM_OS_H

 //#include <stddef.h>

#ifdef WIN32
#include <tchar.h>
#ifdef UNICODE
#include <wctype.h>
#else  /* Not UNICODE */
#include <ctype.h>
#endif /* UNICODE */
#endif /* WIN32 */

#ifdef __linux__
#include <ctype.h>
#include <stddef.h>
#include <string.h>

typedef char TCHAR;

#define SET_STDOUT_MODE
#define STD_COUT std::cout
#define _T(X) X
#define _tmain main
#define _tprintf printf
#define _istdigit isdigit
#define _tcslen strlen
#define _tcscmp strcmp
#define _tcstol strtol
#define _tcstod strtod
#endif /* __linux__ */

#define FALSE (0)
#define TRUE (1)

typedef int BOOL;

#endif /* PLATFORM_OS_H */
