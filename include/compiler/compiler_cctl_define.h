#ifndef __COMPILER_CCTL_DEFINE_H__
#define __COMPILER_CCTL_DEFINE_H__

#include <stdint.h>

#include "cctl/vector.h"

#include "value.h"

#include "control.h"

cctl_ptr_def(char);
vector_fd(cctl_ptr(char));
vector_fd(uint8_t);
vector_fd(control_data);
cctl_ptr_def(vector(control_data));
vector_fd(cctl_ptr(vector(control_data)));
vector_fd(value);
vector_fd(uint64_t);

vector_imp_h(cctl_ptr(char));
vector_imp_h(uint8_t);
vector_imp_h(control_data);
vector_imp_h(cctl_ptr(vector(control_data)));
vector_imp_h(value);
vector_imp_h(uint64_t);

#endif