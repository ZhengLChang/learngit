# DO NOT EDIT! GENERATED AUTOMATICALLY!
# Copyright (C) 2002-2009 Free Software Foundation, Inc.
#
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# This file represents the compiled summary of the specification in
# gnulib-cache.m4. It lists the computed macro invocations that need
# to be invoked from configure.ac.
# In projects using CVS, this file can be treated like other built files.


# This macro should be invoked from ./configure.ac, in the section
# "Checks for programs", right after AC_PROG_CC, and certainly before
# any checks for libraries, header files, types and library functions.
AC_DEFUN([md5_EARLY],
[
  m4_pattern_forbid([^gl_[A-Z]])dnl the gnulib macro namespace
  m4_pattern_allow([^gl_ES$])dnl a valid locale name
  m4_pattern_allow([^gl_LIBOBJS$])dnl a variable
  m4_pattern_allow([^gl_LTLIBOBJS$])dnl a variable
  AC_REQUIRE([AC_PROG_RANLIB])
])

# This macro should be invoked from ./configure.ac, in the section
# "Check for header files, types and library functions".
AC_DEFUN([md5_INIT],
[
  AM_CONDITIONAL([GL_COND_LIBTOOL], [false])
  gl_cond_libtool=false
  gl_libdeps=
  gl_ltlibdeps=
  m4_pushdef([AC_LIBOBJ], m4_defn([md5_LIBOBJ]))
  m4_pushdef([AC_REPLACE_FUNCS], m4_defn([md5_REPLACE_FUNCS]))
  m4_pushdef([AC_LIBSOURCES], m4_defn([md5_LIBSOURCES]))
  m4_pushdef([md5_LIBSOURCES_LIST], [])
  m4_pushdef([md5_LIBSOURCES_DIR], [])
  gl_COMMON
  gl_source_base='md5'
  gl_MD5
  gl_MULTIARCH
  gl_STDDEF_H
  gl_STDINT_H
  gl_WCHAR_H
  m4_ifval(md5_LIBSOURCES_LIST, [
    m4_syscmd([test ! -d ]m4_defn([md5_LIBSOURCES_DIR])[ ||
      for gl_file in ]md5_LIBSOURCES_LIST[ ; do
        if test ! -r ]m4_defn([md5_LIBSOURCES_DIR])[/$gl_file ; then
          echo "missing file ]m4_defn([md5_LIBSOURCES_DIR])[/$gl_file" >&2
          exit 1
        fi
      done])dnl
      m4_if(m4_sysval, [0], [],
        [AC_FATAL([expected source file, required through AC_LIBSOURCES, not found])])
  ])
  m4_popdef([md5_LIBSOURCES_DIR])
  m4_popdef([md5_LIBSOURCES_LIST])
  m4_popdef([AC_LIBSOURCES])
  m4_popdef([AC_REPLACE_FUNCS])
  m4_popdef([AC_LIBOBJ])
  AC_CONFIG_COMMANDS_PRE([
    md5_libobjs=
    md5_ltlibobjs=
    if test -n "$md5_LIBOBJS"; then
      # Remove the extension.
      sed_drop_objext='s/\.o$//;s/\.obj$//'
      for i in `for i in $md5_LIBOBJS; do echo "$i"; done | sed "$sed_drop_objext" | sort | uniq`; do
        md5_libobjs="$md5_libobjs $i.$ac_objext"
        md5_ltlibobjs="$md5_ltlibobjs $i.lo"
      done
    fi
    AC_SUBST([md5_LIBOBJS], [$md5_libobjs])
    AC_SUBST([md5_LTLIBOBJS], [$md5_ltlibobjs])
  ])
  gltests_libdeps=
  gltests_ltlibdeps=
  m4_pushdef([AC_LIBOBJ], m4_defn([md5tests_LIBOBJ]))
  m4_pushdef([AC_REPLACE_FUNCS], m4_defn([md5tests_REPLACE_FUNCS]))
  m4_pushdef([AC_LIBSOURCES], m4_defn([md5tests_LIBSOURCES]))
  m4_pushdef([md5tests_LIBSOURCES_LIST], [])
  m4_pushdef([md5tests_LIBSOURCES_DIR], [])
  gl_COMMON
  gl_source_base='tests'
  m4_ifval(md5tests_LIBSOURCES_LIST, [
    m4_syscmd([test ! -d ]m4_defn([md5tests_LIBSOURCES_DIR])[ ||
      for gl_file in ]md5tests_LIBSOURCES_LIST[ ; do
        if test ! -r ]m4_defn([md5tests_LIBSOURCES_DIR])[/$gl_file ; then
          echo "missing file ]m4_defn([md5tests_LIBSOURCES_DIR])[/$gl_file" >&2
          exit 1
        fi
      done])dnl
      m4_if(m4_sysval, [0], [],
        [AC_FATAL([expected source file, required through AC_LIBSOURCES, not found])])
  ])
  m4_popdef([md5tests_LIBSOURCES_DIR])
  m4_popdef([md5tests_LIBSOURCES_LIST])
  m4_popdef([AC_LIBSOURCES])
  m4_popdef([AC_REPLACE_FUNCS])
  m4_popdef([AC_LIBOBJ])
  AC_CONFIG_COMMANDS_PRE([
    md5tests_libobjs=
    md5tests_ltlibobjs=
    if test -n "$md5tests_LIBOBJS"; then
      # Remove the extension.
      sed_drop_objext='s/\.o$//;s/\.obj$//'
      for i in `for i in $md5tests_LIBOBJS; do echo "$i"; done | sed "$sed_drop_objext" | sort | uniq`; do
        md5tests_libobjs="$md5tests_libobjs $i.$ac_objext"
        md5tests_ltlibobjs="$md5tests_ltlibobjs $i.lo"
      done
    fi
    AC_SUBST([md5tests_LIBOBJS], [$md5tests_libobjs])
    AC_SUBST([md5tests_LTLIBOBJS], [$md5tests_ltlibobjs])
  ])
  LIBMD5_LIBDEPS="$gl_libdeps"
  AC_SUBST([LIBMD5_LIBDEPS])
  LIBMD5_LTLIBDEPS="$gl_ltlibdeps"
  AC_SUBST([LIBMD5_LTLIBDEPS])
])

# Like AC_LIBOBJ, except that the module name goes
# into md5_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([md5_LIBOBJ], [
  AS_LITERAL_IF([$1], [md5_LIBSOURCES([$1.c])])dnl
  md5_LIBOBJS="$md5_LIBOBJS $1.$ac_objext"
])

# Like AC_REPLACE_FUNCS, except that the module name goes
# into md5_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([md5_REPLACE_FUNCS], [
  m4_foreach_w([gl_NAME], [$1], [AC_LIBSOURCES(gl_NAME[.c])])dnl
  AC_CHECK_FUNCS([$1], , [md5_LIBOBJ($ac_func)])
])

# Like AC_LIBSOURCES, except the directory where the source file is
# expected is derived from the gnulib-tool parameterization,
# and alloca is special cased (for the alloca-opt module).
# We could also entirely rely on EXTRA_lib..._SOURCES.
AC_DEFUN([md5_LIBSOURCES], [
  m4_foreach([_gl_NAME], [$1], [
    m4_if(_gl_NAME, [alloca.c], [], [
      m4_define([md5_LIBSOURCES_DIR], [md5])
      m4_append([md5_LIBSOURCES_LIST], _gl_NAME, [ ])
    ])
  ])
])

# Like AC_LIBOBJ, except that the module name goes
# into md5tests_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([md5tests_LIBOBJ], [
  AS_LITERAL_IF([$1], [md5tests_LIBSOURCES([$1.c])])dnl
  md5tests_LIBOBJS="$md5tests_LIBOBJS $1.$ac_objext"
])

# Like AC_REPLACE_FUNCS, except that the module name goes
# into md5tests_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([md5tests_REPLACE_FUNCS], [
  m4_foreach_w([gl_NAME], [$1], [AC_LIBSOURCES(gl_NAME[.c])])dnl
  AC_CHECK_FUNCS([$1], , [md5tests_LIBOBJ($ac_func)])
])

# Like AC_LIBSOURCES, except the directory where the source file is
# expected is derived from the gnulib-tool parameterization,
# and alloca is special cased (for the alloca-opt module).
# We could also entirely rely on EXTRA_lib..._SOURCES.
AC_DEFUN([md5tests_LIBSOURCES], [
  m4_foreach([_gl_NAME], [$1], [
    m4_if(_gl_NAME, [alloca.c], [], [
      m4_define([md5tests_LIBSOURCES_DIR], [tests])
      m4_append([md5tests_LIBSOURCES_LIST], _gl_NAME, [ ])
    ])
  ])
])

# This macro records the list of files which have been installed by
# gnulib-tool and may be removed by future gnulib-tool invocations.
AC_DEFUN([md5_FILE_LIST], [
  build-aux/link-warning.h
  lib/dummy.c
  lib/md5.c
  lib/md5.h
  lib/stddef.in.h
  lib/stdint.in.h
  lib/wchar.in.h
  m4/00gnulib.m4
  m4/gnulib-common.m4
  m4/include_next.m4
  m4/longlong.m4
  m4/md5.m4
  m4/multiarch.m4
  m4/stddef_h.m4
  m4/stdint.m4
  m4/wchar.m4
  m4/wchar_t.m4
  m4/wint_t.m4
])
