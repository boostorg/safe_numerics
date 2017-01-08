#use -r switch on fop for relaxed validation
xsltproc --xinclude --nonet bb2db.xsl accu.xml \
| fop -r -dpi 300 -xsl db2fo.xsl -xml - -pdf accu.pdf
