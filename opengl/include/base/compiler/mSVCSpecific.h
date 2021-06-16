#pragma once

#ifndef _MSC_VER
	#error This file should only be included on MSVC compiler
#endif


//! Barrier to prevent R/W reordering by the compiler.
//! Note: This does not emit any instruction, and it does not prevent CPU reordering!
#define MEMORY_RW_REORDERING_BARRIER _ReadWriteBarrier()
