/**
 * 2mm.h: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#ifndef _2MM_H
# define _2MM_H

/* Default to STANDARD_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET) && !defined(SUPREMELARGE_DATASET) && !defined(SUPREME_DATASET)
#	define STANDARD_DATASET
# endif

/* Do not define anything if the user manually defines the size. */
# if !defined(NI) && !defined(NJ) && !defined(NK)
/* Define the possible dataset sizes. */
#	ifdef MINI_DATASET
#	 define NI 256
#	 define NJ 256
#	 define NK 256
#	 define NL 256
#	 define SIZE 256
#	endif

#	ifdef SMALL_DATASET
#	 define NI 512
#	 define NJ 512
#	 define NK 512
#	 define NL 512
#	 define SIZE 512
#	endif

#	ifdef STANDARD_DATASET
#	 define NI 8
#	 define NJ 8
#	 define NK 8
#	 define NL 8
#	 define SIZE 8
#	endif

#	ifdef LARGE_DATASET
#	 define NI 2000
#	 define NJ 2000
#	 define NK 2000
#	 define NL 2000
#	 define SIZE 2000
#	endif

#	ifdef EXTRALARGE_DATASET
#	 define NI 4000
#	 define NJ 4000
#	 define NK 4000
#	 define NL 4000
#	 define SIZE 4000
#	endif

#	ifdef SUPREME_DATASET
#	 define NI 6000
#	 define NJ 6000
#	 define NK 6000
#	 define NL 6000
#	 define SIZE 6000
#	endif

#	ifdef SUPREMELARGE_DATASET
#	 define NI 8000
#	 define NJ 8000
#	 define NK 8000
#	 define NL 8000
#	 define SIZE 8000
#   endif
# endif /* !N */

# define _PB_NI POLYBENCH_LOOP_BOUND(NI,ni)
# define _PB_NJ POLYBENCH_LOOP_BOUND(NJ,nj)
# define _PB_NK POLYBENCH_LOOP_BOUND(NK,nk)
# define _PB_NL POLYBENCH_LOOP_BOUND(NL,nl)
# define MAXTHREADS 4

# ifndef DATA_TYPE
#  define DATA_PRINTF_MODIFIER "%0.2lf "
# endif


#endif /* !_2MM */
