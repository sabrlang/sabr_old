#ifndef __CCTL_CCTL_H__
#define __CCTL_CCTL_H__

#define cctl_concat_(A, B) A##B
#define cctl_concat(A, B) cctl_concat_(A, B)
#define cctl_join(PREFIX, NAME) cctl_concat(PREFIX, cctl_concat(_, NAME))

#define cctl_ptr(TYPE) cctl_join(TYPE, ptr)
#define cctl_ptr_def(TYPE) \
	typedef TYPE *cctl_ptr(TYPE)

#endif