#include "interpreter_cctl_define.h"

deque_imp_c(value);
deque_imp_c(size_t);
deque_imp_c(cctl_ptr(rbt));

vector_imp_c(value);
deque_imp_c(cctl_ptr(vector(value)));

vector_imp_c(uint64_t);
vector_imp_c(cctl_ptr(vector(uint64_t)));