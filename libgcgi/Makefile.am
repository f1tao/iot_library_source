auxdir     = @ac_aux_dir@
AUX_DIST   = $(ac_aux_dir)/config.guess \
	     $(ac_aux_dir)/config.sub \
	     $(auxdir)/install-sh \
	     $(ac_aux_dir)/ltconfig \
	     $(ac_aux_dir)/ltmain.sh \
	     $(ac_aux_dir)/mdate-sh \
	     $(auxdir)/missing \
	     $(auxdir)/mkinstalldirs

SUBDIRS    = src examples doc
EXTRA_DIST = bootstrap

ACLOCAL    = aclocal -I $(auxdir)

dist-hook:
	(cd $(distdir) && mkdir $(auxdir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	  cp $$file $(distdir)/$$file; \
	done
