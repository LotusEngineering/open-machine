// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_ASSERT_H_
#define OM_ASSERT_H_


/**
 * @brief Om Assert Handler that must be defined by the application
 * 
 * @param file_name 
 * @param line 
 * 
 * @note This should be called by the OM_ASSERT macro
 */
void om_assert_handler(const char * file_name, int line);


/**
 * @brief Macro that sets the file name, only once per module
 * 
 * Place this at the top of your file
 */
#ifdef __FILE_NAME__
#define OM_ASSERT_SET_FILE_NAME() static char const * om_file_name = __FILE_NAME__
#else
#define OM_ASSERT_SET_FILE_NAME() static char const * om_file_name = __FILE__
#endif
/**
 * @brief Assert macro
 * 
 */
#define OM_ASSERT(expression_)  (expression_) ? ((void)0) : om_assert_handler(om_file_name, __LINE__)


#endif// OM_ASSERT_H_
