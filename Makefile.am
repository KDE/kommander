SUBDIRS = plugin widget factory pluginmanager widgets editor executor

mimetypedata_DATA = x-kommander.desktop
mimetypedatadir = $(kde_mimedir)/application

messages:
	extractrc `find . -name "*.ui"` >> rc.cpp ; \
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	       $(XGETTEXT) $$LIST -o $(podir)/kommander.pot; \
	fi; 


