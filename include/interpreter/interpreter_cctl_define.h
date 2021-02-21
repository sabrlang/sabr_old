#ifndef __INTERPRETER_CCTL_DEFINE_H__
#define __INTERPRETER_CCTL_DEFINE_H__

#include <stdint.h>

#include "cctl/vector.h"

#include "rbt.h"
#include "value.h"

cctl_ptr_def(rbt);

vector_fd(value);
vector_fd(size_t);
vector_fd(cctl_ptr(rbt));

vector_imp_h(value);
vector_imp_h(size_t);
vector_imp_h(cctl_ptr(rbt));

#endif