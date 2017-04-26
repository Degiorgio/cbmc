/*******************************************************************\

Module: Handling of functions without body

Author: Michael Tautschnig

Date: July 2016

\*******************************************************************/

#ifndef CPROVER_UNDEFINED_FUNCTIONS_H
#define CPROVER_UNDEFINED_FUNCTIONS_H

#include <iosfwd>

class goto_functionst;

void list_undefined_functions(
  const goto_functionst &goto_functions,
  const namespacet &ns,
  std::ostream &os);

void undefined_function_abort_path(goto_functionst &goto_functions);

#endif
