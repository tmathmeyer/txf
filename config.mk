VERSION = 1.0

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# Xft, comment if you don't want it
XFTINC = /usr/include/freetype2
XFTLIBS  = -lXft -lXrender -lfreetype -lz -lfontconfig

#lxcb libraries
LXCBLIBS = -lxcb -lxcb-icccm -lxcb-ewmh

# includes and libs
INCS = -I${X11INC} -I${XFTINC}
LIBS = -L${X11LIB} -lX11 ${XFTLIBS} ${LXCBLIBS} -lpthread -ldl

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\"
CFLAGS   = -std=c11 -pedantic -rdynamic -Wextra -Wall ${INCS} ${CPPFLAGS} -g
LDFLAGS  = ${LIBS}

# compiler and linker
CC = gcc
