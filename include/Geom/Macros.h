/*
 * Macros.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_MACROS_H__
#define __GM_MACROS_H__


#include <type_traits>


#define __GM_ASSERT_FLOAT_TYPE__(NAME) \
    static_assert(std::is_floating_point<T>::value, NAME " class only allows floating point types")


#endif



// ================================================================================
