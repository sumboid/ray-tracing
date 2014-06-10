AC_INIT([rs],[0.1],[ilya.sumb@gmail.com])
AM_INIT_AUTOMAKE([foreign silent-rules])
AM_SILENT_RULES([yes])

AC_PROG_CXX([mpic++])

CXXFLAGS="-O3 -std=c++11 -I$TS/include -Wall -Wextra -Werror"
LDFLAGS="-L$TS/lib"
LIBS="-lts"

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
