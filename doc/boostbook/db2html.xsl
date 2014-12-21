<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:import href="http://www.boost.org/tools/boostbook/xsl/html.xsl"/>

<!-- The root of the Boost directory - not used here! -->
<!-- <xsl:param name="boost.root" select="'file:///c:/BoostRelease/'"/> -->
<!-- use boost directorys for css stylesheets and images, etc - again not used here -->
<!-- <xsl:param name="boost.defaults" select="'Boost'"/> -->

<!-- use the images inside of the html directory
<xsl:param name="boost.graphics.root" select="images/" />
-->
<!-- direct "libraries" to boost page
<xsl:param name="boost.libraries">http://www.boost.org/doc/libs</xsl:param>
<xsl:param name="boost.home_link">file:///index.html</xsl:param>
-->

<!-- use stylesheet inside of the html directory -->
<xsl:param name="html.stylesheet" select="'boostbook.css'" />

<!-- enable spirit type nav bar which is the current boost design -->
<xsl:param name="nav.layout">horizontal</xsl:param>

<!-- How far down we chunk nested sections, basically all of them: -->
<xsl:param name="chunk.section.depth" select="1" />
<!-- don't make first sections special - leave TOC in different file -->
<xsl:param name="chunk.first.sections" select="1" />

<!-- How far down sections get TOC's -->
<xsl:param name = "toc.section.depth" select="1" />

<!-- Max depth in each TOC: -->
<xsl:param name = "toc.max.depth" select="3" />

<!-- How far down we go with TOC's -->
<xsl:param name="generate.section.toc.level" select="10" />

<!-- remove "Chapter 1" from first page -->
<xsl:param name="chapter.autolabel" select="0"/>
<!-- leave the html files in the directory ../html -->
<xsl:param name="base.dir" select="'../html/'"/>
<!-- substitute the pre-boost logo for the boost one -->
<xsl:param name = "boost.image.src">pre-boost.jpg</xsl:param>
<xsl:param name = "boost.image.alt">pre-boost</xsl:param>
<xsl:param name = "boost.image.w">30%</xsl:param>
<xsl:param name = "boost.image.h">30%</xsl:param>

<!--
BoostBook takes a section node id like safe_numeric.safe_cast
and renders it as safe_numeric/safe_cast. Presumably they do this
so they can make a huge "book" with all the libraries in subdirectories.
But we want something different.  To my mind, this should have been
done using the library "directory" attribute.  But of course that
doesn't matter now.  We'll just re-hack the path to eliminate
the "safe_numeric/" from the above example.
-->

<xsl:template match="*" mode="recursive-chunk-filename">
    <xsl:variable name="their">
        <xsl:apply-imports mode="recursive-chunk-filename" select="."/>
    </xsl:variable>
    <xsl:choose>
    <xsl:when test="contains($their, '/')">
        <xsl:value-of select="substring-after($their, '/')" />
    </xsl:when>
    <xsl:otherwise>
        <xsl:value-of select="$their"/>
    </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>


