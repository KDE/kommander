SUBDIRS =  factory icons plugin pluginmanager widget \
	widgets editor executor part

mimetypedata_DATA = x-kommander.desktop
mimetypedatadir = $(kde_mimedir)/application

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui` >> rc.cpp ; \
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	       $(XGETTEXT) $$LIST -o $(podir)/kommander.pot; \
	fi; 


