/*

Copyright (c) 2009, 2010, 2011, 2012 STMicroelectronics
Written by Christophe Lyon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#if defined(__arm__) || defined(__aarch64__)
#include <arm_neon.h>
#else
#include "stm-arm-neon.h"
#endif

#include "stm-arm-neon-ref.h"

#define INSN vqdmull
#define TEST_MSG "VQDMULL"
#define FNNAME1(NAME) void exec_ ## NAME (void)
#define FNNAME(NAME) FNNAME1(NAME)

FNNAME (INSN)
{
  /* Basic test: y=vqdmull(x,x), then store the result.  */
#define TEST_VQDMULL2(INSN, T1, T2, W, W2, N)			\
  Set_Neon_Cumulative_Sat(0, VECT_VAR(vector_res, T1, W2, N));	\
  VECT_VAR(vector_res, T1, W2, N) =				\
    INSN##_##T2##W(VECT_VAR(vector, T1, W, N),			\
		   VECT_VAR(vector2, T1, W, N));		\
  vst1q_##T2##W2(VECT_VAR(result, T1, W2, N),			\
		 VECT_VAR(vector_res, T1, W2, N));		\
  dump_neon_cumulative_sat(TEST_MSG, xSTR(INSN##_##T2##W),	\
			   xSTR(T1), W, N)

  /* Two auxliary macros are necessary to expand INSN */
#define TEST_VQDMULL1(INSN, T1, T2, W, W2, N)	\
  TEST_VQDMULL2(INSN, T1, T2, W, W2, N)

#define TEST_VQDMULL(T1, T2, W, W2, N)		\
  TEST_VQDMULL1(INSN, T1, T2, W, W2, N)


  /* With ARM RVCT, we need to declare variables before any executable
     statement */
  DECL_VARIABLE(vector, int, 16, 4);
  DECL_VARIABLE(vector, int, 32, 2);
  DECL_VARIABLE(vector2, int, 16, 4);
  DECL_VARIABLE(vector2, int, 32, 2);
  DECL_VARIABLE(vector_res, int, 32, 4);
  DECL_VARIABLE(vector_res, int, 64, 2);

  clean_results ();

  VLOAD(vector, buffer, , int, s, 16, 4);
  VLOAD(vector, buffer, , int, s, 32, 2);
  VLOAD(vector2, buffer, , int, s, 16, 4);
  VLOAD(vector2, buffer, , int, s, 32, 2);

  fprintf(ref_file, "\n%s cumulative saturation output:\n", TEST_MSG);
  TEST_VQDMULL(int, s, 16, 32, 4);
  TEST_VQDMULL(int, s, 32, 64, 2);

  /* FIXME: only a few result buffers are used, but we output all of them */
  dump_results_hex (TEST_MSG);


  VDUP(vector, , int, s, 16, 4, 0x8000);
  VDUP(vector2, , int, s, 16, 4, 0x8000);
  VDUP(vector, , int, s, 32, 2, 0x80000000);
  VDUP(vector2, , int, s, 32, 2, 0x80000000);
  fprintf(ref_file, "\n%s cumulative saturation output:\n",
	  TEST_MSG " (check mul cumulative saturation)");
  TEST_VQDMULL(int, s, 16, 32, 4);
  TEST_VQDMULL(int, s, 32, 64, 2);
  dump_results_hex2 (TEST_MSG, " (check mul cumulative saturation)");
}
