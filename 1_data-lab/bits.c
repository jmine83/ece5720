/* 
 * CS:APP Data Lab 
 * 
 * Joel Meine, A01764207
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
	// A bitwise truth table yields the result.
	return ~(~x | ~y);
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
	// Shift right n bytes then filter word to get lowest byte.
	int shift = n << 3;
	int filter = 0xFF;
	return (x >> shift) & filter;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
	// In arithmetical shift, most significant bits are 1.
	// In logical shift, most significant bits are 0.
	// Thus, the most significant bits are filtered out.
	x = x >> n;
	int filter = 1 << 31;
	filter = filter >> n;
	filter = filter << 1;
	return ~filter & x;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
	// Counting the 1 bits in every 2^n bits.
	int filter_a = 0xFF + (0xFF << 8); // 2^4 = 16
	int filter_b = 0xFF; // 2^3 = 8
	int filter_c = 0x0F + (0x0F << 8); // 2^2 = 4
	int filter_d = 0x33 + (0x33 << 8); // 2^1 = 2
	int filter_e = 0x55 + (0x55 << 8); // 2^0 = 1
	int end_a = filter_a & (x >> 16);
	int end_b = filter_a & x;
	end_a = (filter_e & end_a) + ((~filter_e & end_a) >> 1);
	end_a = (filter_d & end_a) + ((~filter_d & end_a) >> 2);
	end_b = (filter_e & end_b) + ((~filter_e & end_b) >> 1);
	end_b = (filter_d & end_b) + ((~filter_d & end_b) >> 2);
	int sum;
	sum = end_a + end_b;
	sum = (filter_c & sum) + ((~filter_c & sum) >> 4);
	return (filter_b & sum) + (sum >> 8);
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
	// Set most significant bit to a 1 bit if a 1 bit exists in x.
	// Shift right 31 bits input x to get -1 if a 1 bit exists.
	// Add 1 to result to get 0 if 1 bit exists in x.
	x = (x << 1) | x;
	x = (x << 2) | x;
	x = (x << 4) | x;
	x = (x << 8) | x;
	x = (x << 16) | x;
	return 1 + (x >> 31);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
	return (1 << 31);
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
	// Returns 1 only if higher (32-n+1) bits are all 1 bits of input x.
	int minus1 = ~0 + n;
	x = x >> minus1;
	return !(~x) | !x;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
	// Shift right input x by n bits if lower n bits are all 0 bits or if x is positive.
	// If lower n bits are not all 0 bits, then add 1 after the shift right.
	int all0_upper = !!(x & ~(~0 << n));
	int neg = 1 & (x >> 31);
	return (neg & all0_upper) + (x >> n);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
	return 1 + ~x;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
	// Isolate and filter input x to its sign bit.
	int filter_sign = 1 << 31;
	int minus1 = ~0 + x;
	int sign_a = x & filter_sign;
	int sign_b = minus1 & filter_sign;
	return !(sign_a | sign_b);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
	int xMin = !(x ^ (1 << 31)); // x = minimum
	int xNyP = 1 & (x >> 31) & (~(y >> 31)); // x is negative, y is positive
	int xPyN = 1 & (y >> 31) & (~(x >> 31)); // x is positive, y is negative
	int minus_a = (1 + ~x) + y; // y-x
	int minus_b = 1 & (minus_a >> 31); // y-x >= 0
	return xMin | xNyP | ((!xPyN) & (!minus_b));
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
	// Set as 1 bits to all bits lower than highest 1 bit; see bang() function.
	// Count number of 1 bits in input x; see bitCount() function.
	// Shift right input x from center position.
	// Number of bits right of highest 1 bit in input x is the answer.
	int filter_a = 0xFF + (0xFF << 8);
	int filter_b = 0xFF + (0xFF << 16);
	int filter_c = 0x0F + (0x0F << 8) + (0x0F << 16) + (0x0F << 24);
	int filter_d = 0x33 + (0x33 << 8) + (0x33 << 16) + (0x33 << 24);
	int filter_e = 0x55 + (0x55 << 8) + (0x55 << 16) + (0x55 << 24);
	x = (x >> 1) | x;
	x = (x >> 2) | x;
	x = (x >> 4) | x;
	x = (x >> 8) | x;
	x = (x >> 16) | x;
	x = x >> 1;
	x = (filter_e & x) + ((~filter_e & x) >> 1);
	x = (filter_d & x) + ((~filter_d & x) >> 2);
	x = (filter_c & x) + ((~filter_c & x) >> 4);
	x = (filter_b & x) + ((~filter_b & x) >> 8);
	return (filter_a & x) + (x >> 16);
}