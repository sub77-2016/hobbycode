SUBDIRS = . 

#
#  core libraries
#

LuaLBLibs =			\
	../binarymix/liblbLandau.a	\
	../graphics/liblbgraphics.a \
	../core/liblbcore.a		
	
if LB_ENABLE_MPI
lb_lua_wrap = lb-lua_wrap-mpi.c
else
lb_lua_wrap = lb-lua_wrap.c
endif

bin_PROGRAMS = binarymix

binarymix_SOURCES =		\
	lb-lua.c	\
	$(lb_lua_wrap)

binarymix_LDADD = $(LuaLBLibs) $(HDF5_LIBS) $(PNG_LIBS) $(LUA5_LIBS) -ldl -lreadline -lhistory -lncurses

INCLUDES = -I../core -I../graphics -I../binarymix -I$(top_srcdir) $(HDF5_CFLAGS) $(LUA5_CFLAGS)

EXTRA_binarymix_SOURCES = 	\
		lb-lua.i	

EXTRA_DIST = README
