/**
 * @file option_result.h
 * @author Basit Ayantunde <rlamarrr@gmail.com>
 * @version  0.0.1
 * @date 2020-06-05
 *
 * @copyright MIT License
 *
 * Copyright (c) 2020 Basit Ayantunde
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

/// @file
///
/// Error Propagation Macros
///
///

#pragma once

// Do remember that Result's value type can not be a reference

#define TRY_OK(identifier, result_expr)                                 \
  static_assert(                                                        \
      !std::is_lvalue_reference_v<decltype((result_expr))>,             \
      "'TRY_OK' is for value forwarding of r-values or temporaries, "   \
      "consider explicitly calling 'std::move' if the l-value "         \
      "(reference) is no longer needed");                               \
  decltype((result_expr)) stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh =       \
      (result_expr);                                                    \
  if (stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh.is_err())                   \
    return Err<decltype((result_expr))::error_type>(                    \
        std::move(stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh).unwrap_err()); \
  decltype((result_expr))::value_type identifier =                      \
      std::move(stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh).unwrap();

#define TRY_SOME(identifier, option_expr)                                  \
  static_assert(                                                           \
      !std::is_lvalue_reference_v<decltype((option_expr))>,                \
      "'CO_TRY_SOME' is for value forwarding of r-values or temporaries, " \
      "consider explicitly calling 'std::move' if the l-value "            \
      "(reference) is no longer needed");                                  \
  decltype((option_expr)) stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh =          \
      (option_expr);                                                       \
  if (stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh.is_none()) return stx::None;   \
  decltype((option_expr))::value_type identifier =                         \
      std::move(stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh).unwrap();

// Coroutines
// not tested yet

#if defined(__cpp_coroutines) || defined(__cpp_lib_coroutine)

#define CO_TRY_OK(identifier, result_expr)                               \
  static_assert(                                                         \
      !std::is_lvalue_reference_v<decltype((result_expr))>,              \
      "'CO_TRY_OK' is for value forwarding of r-values or temporaries, " \
      "consider explicitly calling 'std::move' if the l-value "          \
      "(reference) is no longer needed");                                \
  decltype((result_expr)) stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh =        \
      (result_expr);                                                     \
  if (stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh.is_err())                    \
    co_return Err<decltype((result_expr))::error_type>(                  \
        std::move(stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh).unwrap_err());  \
  decltype((result_expr))::value_type identifier =                       \
      std::move(stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh).unwrap();

#define CO_TRY_SOME(identifier, option_expr)                                \
  static_assert(                                                            \
      !std::is_lvalue_reference_v<decltype((option_expr))>,                 \
      "'CO_TRY_SOME' is for value forwarding of r-values or temporaries, "  \
      "consider explicitly calling 'std::move' if the l-value "             \
      "(reference) is no longer needed");                                   \
  decltype((option_expr)) stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh =           \
      (option_expr);                                                        \
  if (stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh.is_none()) co_return stx::None; \
  decltype((option_expr))::value_type identifier =                          \
      std::move(stx_TmpVaRYoUHopEfUllYwOnTcoLlidEwiTh).unwrap();

#endif
