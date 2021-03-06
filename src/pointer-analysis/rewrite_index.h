/*******************************************************************\

Module: Pointer Dereferencing

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_POINTER_ANALYSIS_REWRITE_INDEX_H
#define CPROVER_POINTER_ANALYSIS_REWRITE_INDEX_H

class dereference_exprt;
class index_exprt;

// rewrite a[i] to *(a+i)

dereference_exprt rewrite_index(const index_exprt &index_expr);

#endif // CPROVER_POINTER_ANALYSIS_REWRITE_INDEX_H
