
#pragma once

#ifndef __clang__
	#error This file should only be included on Clang compiler
#endif


//! Barrier to prevent R/W reordering by the compiler.
//! Note: This does not emit any instruction, and it does not prevent CPU reordering!
#define MEMORY_RW_REORDERING_BARRIER asm volatile ("" ::: "memory")
