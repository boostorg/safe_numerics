safe_numerics
=============

Arithmetic operations in C++ are NOT guarenteed to yield a correct mathematical result. This feature is inherited from the early days of C. The behavior of int, unsigned int and others were designed to map closely to the underlying hardware. Computer hardware implements these types as a fixed number of bits. When the result of arithmetic operations exceeds this number of bits, the result is undefined and usually not what the programmer intended. It is incumbent up the C++ programmer to guarentee that this behavior does not result in incorrect behavior of the program. This library implements special versions of these data types which behave exactly like the original ones EXCEPT that the results of these operations are checked to be sure that an exception will be thrown anytime an attempt is made to store the result of an undefined operation.

Additionally, we define data types safe_signed_range<MIN, MAX> and safe_unsigned_range<MIN, MAX> which will throw an exception if an attempt is made to store a result which is outside the closed range [MIN, MAX]

Browse documentation <a href=http://htmlpreview.github.com/?https://raw.github.com/robertramey/safe_numerics/master/doc/html/index.html>here</a>
