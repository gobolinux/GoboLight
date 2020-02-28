
sbin_PROGRAMS = gobolight

gobolight_SOURCES = gobolight.c

if SETUID_INSTALL
install-exec-hook:
	chmod 4755 $(DESTDIR)$(sbindir)/gobolight
endif

