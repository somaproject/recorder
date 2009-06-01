SRC=../docs

dbusxx-xml2cpp $SRC/manager-introspect.xml --proxy=dbusrecorderproxy.h
dbusxx-xml2cpp $SRC/experiment-introspect.xml --adaptor=dbusexperimentadaptor.h
dbusxx-xml2cpp $SRC/epoch-introspect.xml --adaptor=dbusepochadaptor.h

