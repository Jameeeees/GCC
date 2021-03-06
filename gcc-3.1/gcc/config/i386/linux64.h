/* Definitions for AMD x86-64 running Linux-based GNU systems with ELF format.
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
   Contributed by Jan Hubicka <jh@suse.cz>, based on linux.h.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#define LINUX_DEFAULT_ELF

#undef TARGET_VERSION
#define TARGET_VERSION fprintf (stderr, " (x86-64 Linux/ELF)");

#undef CPP_PREDEFINES
#define CPP_PREDEFINES "-D__ELF__ -Dunix -D__gnu_linux__ -Dlinux -Asystem(posix)"

#undef CPP_SPEC
#define CPP_SPEC "%(cpp_cpu) %{fPIC:-D__PIC__ -D__pic__} %{fpic:-D__PIC__ -D__pic__} %{posix:-D_POSIX_SOURCE} %{pthread:-D_REENTRANT} %{!m32:-D__LONG_MAX__=9223372036854775807L}"

/* Provide a LINK_SPEC.  Here we provide support for the special GCC
   options -static and -shared, which allow us to link things in one
   of these three modes by applying the appropriate combinations of
   options at link-time.

   When the -shared link option is used a final link is not being
   done.  */

#undef	LINK_SPEC
#define LINK_SPEC "%{!m32:-m elf_x86_64 -Y P,/usr/lib64} %{m32:-m elf_i386} \
  %{shared:-shared} \
  %{!shared: \
    %{!static: \
      %{rdynamic:-export-dynamic} \
      %{m32:%{!dynamic-linker:-dynamic-linker /lib/ld-linux.so.2}} \
      %{!m32:%{!dynamic-linker:-dynamic-linker /lib64/ld-linux-x86-64.so.2}}} \
    %{static:-static}}"

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC \
  "%{m32:%{!shared: \
       %{pg:gcrt1.o%s} %{!pg:%{p:gcrt1.o%s} \
       %{!p:%{profile:gcrt1.o%s} %{!profile:crt1.o%s}}}} \
     crti.o%s %{static:crtbeginT.o%s}\
     %{!static:%{!shared:crtbegin.o%s} %{shared:crtbeginS.o%s}}} \
   %{!m32:%{!shared: \
       %{pg:/usr/lib64/gcrt1.o%s} %{!pg:%{p:/usr/lib64/gcrt1.o%s} \
       %{!p:%{profile:/usr/lib64/gcrt1.o%s} %{!profile:/usr/lib64/crt1.o%s}}}}\
     /usr/lib64/crti.o%s %{static:crtbeginT.o%s} \
     %{!static:%{!shared:crtbegin.o%s} %{shared:crtbeginS.o%s}}}"

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC "\
  %{m32:%{!shared:crtend.o%s} %{shared:crtendS.o%s} crtn.o%s} \
  %{!m32:%{!shared:crtend.o%s} %{shared:crtendS.o%s} /usr/lib64/crtn.o%s}"

#define MULTILIB_DEFAULTS { "m64" }

/* Do code reading to identify a signal frame, and set the frame
   state data appropriately.  See unwind-dw2.c for the structs.  */

#ifdef IN_LIBGCC2
#include <signal.h>
#include <sys/ucontext.h>
#endif

#define MD_FALLBACK_FRAME_STATE_FOR(CONTEXT, FS, SUCCESS)		\
  do {									\
    unsigned char *pc_ = (CONTEXT)->ra;					\
    struct sigcontext *sc_;						\
    long new_cfa_;							\
									\
    /* movq __NR_rt_sigreturn, %rax ; syscall  */			\
    if (*(unsigned char *)(pc_+0) == 0x48				\
	&& *(unsigned long *)(pc_+1) == 0x050f0000000fc0c7)		\
      {									\
	struct ucontext *uc_ = (CONTEXT)->cfa;				\
	sc_ = (struct sigcontext *) &uc_->uc_mcontext;			\
      }									\
    else								\
      break;								\
									\
    new_cfa_ = sc_->rsp;						\
    (FS)->cfa_how = CFA_REG_OFFSET;					\
    /* Register 7 is rsp  */						\
    (FS)->cfa_reg = 7;							\
    (FS)->cfa_offset = new_cfa_ - (long) (CONTEXT)->cfa;		\
									\
    /* The SVR4 register numbering macros aren't usable in libgcc.  */	\
    (FS)->regs.reg[0].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[0].loc.offset = (long)&sc_->rax - new_cfa_;		\
    (FS)->regs.reg[1].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[1].loc.offset = (long)&sc_->rbx - new_cfa_;		\
    (FS)->regs.reg[2].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[2].loc.offset = (long)&sc_->rcx - new_cfa_;		\
    (FS)->regs.reg[3].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[3].loc.offset = (long)&sc_->rdx - new_cfa_;		\
    (FS)->regs.reg[4].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[4].loc.offset = (long)&sc_->rbp - new_cfa_;		\
    (FS)->regs.reg[5].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[5].loc.offset = (long)&sc_->rsi - new_cfa_;		\
    (FS)->regs.reg[6].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[6].loc.offset = (long)&sc_->rdi - new_cfa_;		\
    (FS)->regs.reg[8].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[8].loc.offset = (long)&sc_->r8 - new_cfa_;		\
    (FS)->regs.reg[9].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[9].loc.offset = (long)&sc_->r9 - new_cfa_;		\
    (FS)->regs.reg[10].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[10].loc.offset = (long)&sc_->r10 - new_cfa_;		\
    (FS)->regs.reg[11].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[11].loc.offset = (long)&sc_->r11 - new_cfa_;		\
    (FS)->regs.reg[12].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[12].loc.offset = (long)&sc_->r12 - new_cfa_;		\
    (FS)->regs.reg[13].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[13].loc.offset = (long)&sc_->r13 - new_cfa_;		\
    (FS)->regs.reg[14].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[14].loc.offset = (long)&sc_->r14 - new_cfa_;		\
    (FS)->regs.reg[15].how = REG_SAVED_OFFSET;				\
    (FS)->regs.reg[15].loc.offset = (long)&sc_->r15 - new_cfa_;		\
    (FS)->retaddr_column = 16;						\
    goto SUCCESS;							\
  } while (0)
