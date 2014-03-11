AC_INIT([ts],[0.1],[ilya.sumb@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign silent-rules])
AM_SILENT_RULES([yes])

AC_PROG_CXX([mpic++])

CXXFLAGS="-O0 -g -std=c++11 -I/usr/local/include -Wall"
LDFLAGS="-L/usr/local/lib"
LIBS="-lts"

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
