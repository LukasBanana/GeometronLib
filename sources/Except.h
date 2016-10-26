/*
 * Except.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_EXCEPT_H
#define GM_EXCEPT_H


#include <stdexcept>
#include <string>


#ifdef _MSC_VER
#   define GM_EXCEPT_INFO(INFO) (__FUNCTION__ ": " INFO)
#else
#   define GM_EXCEPT_INFO(INFO) (std::string(__FUNCTION__) + std::string(": ") + std::string(INFO))
#endif


#endif



// ================================================================================
