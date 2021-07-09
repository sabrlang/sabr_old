#ifndef __INTERPRETER_CCTL_DEFINE_H__
#define __INTERPRETER_CCTL_DEFINE_H__

#include <stdint.h>

#include "cctl/deque.h"

#include "rbt.h"
#include "value.h"

cctl_ptr_def(rbt);

deque_fd(value);
deque_fd(size_t);
deque_fd(cctl_ptr(rbt));

deque_imp_h(value);
deque_imp_h(size_t);
deque_imp_h(cctl_ptr(rbt));

#endif