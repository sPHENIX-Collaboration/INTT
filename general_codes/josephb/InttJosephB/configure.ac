dnl Minimal autogen file as per sPHENIX wiki:
dnl https://wiki.sphenix.bnl.gov/index.php/Autotools#configure.ac

dnl NOTE:
dnl You will need to change the package name below (all lowercase):
AC_INIT(intt_joseph_b, [1.00])
dnl This should be the only line which requires editing

AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE

AC_PROG_CXX(CC g++)
LT_INIT([disable-static])

if test $ac_cv_prog_gxx = yes; then
   CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wshadow -Werror"
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
