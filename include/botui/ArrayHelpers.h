#ifndef _ARRAYHELPERS_H_
#define _ARRAYHELPERS_H_

// This is derived from:
// http://stackoverflow.com/questions/3368883/how-does-this-size-of-array-template-function-work
// This method has the advantage of being a constant expression

#include <cstddef>

template <std::size_t n> struct type_of_size { typedef char type[n]; };

template <typename T, std::size_t size>
typename type_of_size<size>::type &sizeof_array_h(T (&)[size]);

#define sizeof_array(a) (sizeof(sizeof_array_h(a)))

#endif
