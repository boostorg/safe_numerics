if test x = x$BOOST_ROOT 
then
    echo BOOST_ROOT not set
    exit 1
fi
xsltproc --nonet --xinclude bb2db.xsl safe_numerics.xml | xsltproc --nonet db2epub.xsl -
#cp pre-boost.jpg ../html
#cp $BOOST_ROOT/doc/src/boostbook.css ../html
#cp -R $BOOST_ROOT/doc/html/images ../html
