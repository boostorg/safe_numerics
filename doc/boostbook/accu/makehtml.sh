if test x = x$BOOST_ROOT 
then
    echo BOOST_ROOT not set
fi
xsltproc --nonet db2html.xsl accudocbook4.xml
cp ../pre-boost.jpg ../html
cp $BOOST_ROOT/doc/src/boostbook.css ../html
cp -R $BOOST_ROOT/doc/html/images ../html
