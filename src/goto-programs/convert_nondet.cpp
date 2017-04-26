/*******************************************************************\

Module: Convert side_effect_expr_nondett expressions

Author: Reuben Thomas, reuben.thomas@diffblue.com

\*******************************************************************/

#include "goto-programs/convert_nondet.h"
#include "goto-programs/goto_convert.h"
#include "goto-programs/goto_model.h"

#include "java_bytecode/java_object_factory.h" // gen_nondet_init

#include "util/irep_ids.h"

/*******************************************************************\

Function: insert_nondet_init_code

  Inputs:
    goto_program: The goto program to modify.
    target: One of the steps in that goto program.
    symbol_table: The global symbol table.
    message_handler: Handles logging.
    max_nondet_array_length: Maximum size of new nondet arrays.

  Outputs: The next instruction to process with this function.

 Purpose: Checks an instruction to see whether it contains an assignment
          from side_effect_expr_nondet.  If so, replaces the instruction
          with a range of instructions to properly nondet-initialize
          the lhs.

\*******************************************************************/

static goto_programt::const_targett insert_nondet_init_code(
  goto_programt &goto_program,
  const goto_programt::const_targett &target,
  symbol_tablet &symbol_table,
  message_handlert &message_handler,
  size_t max_nondet_array_length)
{
  // Return if the instruction isn't an assignment
  const auto next_instr=std::next(target);
  if(!target->is_assign())
  {
    return next_instr;
  }

  // Return if the rhs of the assignment isn't a side effect expression
  const auto &assign=to_code_assign(target->code);
  if(assign.rhs().id()!=ID_side_effect)
  {
    return next_instr;
  }

  // Return if the rhs isn't nondet
  const auto &side_effect=to_side_effect_expr(assign.rhs());
  if(side_effect.get_statement()!=ID_nondet)
  {
    return next_instr;
  }

  const auto lhs=assign.lhs();
  // If the lhs type doesn't have a subtype then I guess it's primitive and
  // we want to bail out now
  if(!lhs.type().has_subtype())
  {
    return next_instr;
  }

  // Although, if the type is a ptr-to-void then we also want to bail
  if(lhs.type().subtype().id()==ID_empty)
  {
    return next_instr;
  }

  // Check whether the nondet object may be null
  const auto nullable=to_side_effect_expr_nondet(side_effect).get_nullable();
  // Get the symbol to nondet-init
  const auto source_loc=target->source_location;

  // Erase the nondet assignment
  const auto after_erased=goto_program.instructions.erase(target);

  // Generate nondet init code
  code_blockt init_code;
  gen_nondet_init(
    lhs,
    init_code,
    symbol_table,
    source_loc,
    false,
    true,
    !nullable,
    max_nondet_array_length);

  // Convert this code into goto instructions
  goto_programt new_instructions;
  goto_convert(init_code, symbol_table, new_instructions, message_handler);

  // Insert the new instructions into the instruction list
  goto_program.destructive_insert(after_erased, new_instructions);
  goto_program.update();

  return after_erased;
}

/*******************************************************************\

Function: convert_nondet

  Inputs:
    goto_program: The goto program to modify.
    symbol_table: The global symbol table.
    message_handler: Handles logging.
    max_nondet_array_length: Maximum size of new nondet arrays.

 Purpose: For each instruction in the goto program, checks if it is
          an assignment from nondet and replaces it with the appropriate
          composite initialization code if so.

\*******************************************************************/

static void convert_nondet(
  goto_programt &goto_program,
  symbol_tablet &symbol_table,
  message_handlert &message_handler,
  size_t max_nondet_array_length)
{
  for(auto instruction_iterator=goto_program.instructions.cbegin(),
        end=goto_program.instructions.cend();
        instruction_iterator!=end;)
  {
    instruction_iterator=insert_nondet_init_code(
      goto_program,
      instruction_iterator,
      symbol_table,
      message_handler,
      max_nondet_array_length);
  }
}



void convert_nondet(
  goto_functionst &goto_functions,
  symbol_tablet &symbol_table,
  message_handlert &message_handler,
  size_t max_nondet_array_length)
{
  for(auto &goto_program : goto_functions.function_map)
  {
    convert_nondet(
      goto_program.second.body,
      symbol_table,
      message_handler,
      max_nondet_array_length);
  }

  goto_functions.compute_location_numbers();
}
