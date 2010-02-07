bin_PROGRAMS = main

AM_CPPFLAGS = \
	-DPACKAGE_DATADIR=\""$(datadir)/gmc"\" \
	$(GMC_CFLAGS)

LDADD = \
	$(GMC_LIBS)

main_SOURCES = \
	main.c \
	gmc-button.c \
	gmc-video-model.c