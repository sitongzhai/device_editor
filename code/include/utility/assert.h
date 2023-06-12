//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef UTILITY_ASSERT_H
#define UTILITY_ASSERT_H

#include <assert.h>
#include "utility/log.h"
#include "utility/exception.h"

#ifndef _EDA_FUNC_
#ifdef WIN32
#define _EDA_FUNC_  __FUNCTION__
#else
#define _EDA_FUNC_  __func__
#endif
#endif

namespace eda {
#define eda_assert(exp) { \
  if(!(exp)){ \
    eda_info("%s:%d:%s\n\"%s\"\n", __FILE__, __LINE__, _EDA_FUNC_, #exp); \
    eda::dump_stack(); \
    throw eda::Exception("assert error!"); \
  } \
}

}


#endif
