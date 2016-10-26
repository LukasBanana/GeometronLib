/*
 * Macros.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_MACROS_H
#define GM_MACROS_H


#include <type_traits>


#define GM_ASSERT_FLOAT_TYPE(NAME) \
    static_assert(std::is_floating_point<T>::value, NAME " class only allows floating point types")


#endif



// ================================================================================
