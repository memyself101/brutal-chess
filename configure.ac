#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.58)
AC_INIT(brutalchess, 0.5, brutalchess-devel@lists.sourceforge.net)
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([src/config.h])

# Check the host to determine OS
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE


case $host_os in
	mingw21* | cygwin* | windows* )
		# Include files aren't found with MDL_HAVE_OPENGL
		# Libs have different names
		LIBS="$LIBS -lOpenGl32 -lGlu32"
		;;
	darwin* )
		CFLAGS="$CFLAGS -I/System/Library/Frameworks/OpenGL.framework/Headers"
		CXXFLAGS="$CXXFLAGS -I/System/Library/Frameworks/OpenGL.framework/Headers"
		LIBS="$LIBS -framework OpenGL"
		# OSX wants gl.h not GL/gl.h
		AC_CHECK_HEADERS([gl.h glu.h glx.h])
		;;
	* )
		MDL_HAVE_OPENGL
		CFLAGS="$CFLAGS $GL_CFLAGS"
		CXXFLAGS="$CXXFLAGS $GL_CFLAGS"
		LIBS="$LIBS $GL_LIBS"
		AC_CHECK_HEADERS([GL/glx.h])
		;;
esac

SDL_VERSION=1.2.7
AM_PATH_SDL($SDL_VERSION, :,
		AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
CFLAGS="$CFLAGS $SDL_CFLAGS"
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_FT2
CFLAGS="$CFLAGS $FT2_CFLAGS"
CXXFLAGS="$CXXFLAGS $FT2_CFLAGS"
LIBS="$LIBS $FT2_LIBS"

dnl Checks for libraries. 
dnl Replace `main' with a function in -lSDL_image: 
AC_CHECK_LIB(SDL_image, 
		IMG_Load, 
		LIBS="$LIBS -lSDL_image", 
		AC_MSG_ERROR([*** SDL_image library not found!]) 
)
# Checks for library functions.
#AC_CHECK_FUNCS([sqrt])

# Directory where resources are installed
AC_SUBST(bcdatadir, "${datadir}/brutalchess/", "Prefix")
AC_SUBST(modelsdir, "${bcdatadir}/models/", "Models")
AC_SUBST(artdir, "${bcdatadir}/art/", "Art")
AC_SUBST(fontsdir, "${bcdatadir}/fonts/", "Fonts")

AC_CONFIG_FILES([Makefile 
			src/Makefile
			art/Makefile
			fonts/Makefile
			models/Makefile])
AC_OUTPUT
