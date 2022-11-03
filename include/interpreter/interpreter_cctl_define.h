#ifndef __INTERPRETER_CCTL_DEFINE_H__
#define __INTERPRETER_CCTL_DEFINE_H__

#include <stdint.h>

#include "cctl/deque.h"
#include "cctl/vector.h"

#include "rbt.h"
#include "value.h"


deque_fd(value);
deque_fd(size_t);
cctl_ptr_def(rbt);
deque_fd(cctl_ptr(rbt));

vector_fd(value);
cctl_ptr_def(vector(value));
deque_fd(cctl_ptr(vector(value)));

vector_fd(uint64_t);
cctl_ptr_def(vector(uint64_t));
vector_fd(cctl_ptr(vector(uint64_t)));

deque_imp_h(value);
deque_imp_h(size_t);
deque_imp_h(cctl_ptr(rbt));

vector_imp_h(value);
deque_imp_h(cctl_ptr(vector(value)));

vector_imp_h(uint64_t);
vector_imp_h(cctl_ptr(vector(uint64_t)));


#endif