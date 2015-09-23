The Flexpaper2 module provides a formatter for the File field which is used for
showing pdf-files using the Devaldi FlexPaper pdf-reader.

See also http://flexpaper.devaldi.com

-----------------------------------------------------------------------------
VERSION INFO
-----------------------------------------------------------------------------
Version 1.x supports Flexpaper Zine viewer and Flexpaper Desktop Publisher Zine publishing file structure.
The actual version of Flexpaper Desktop Publisher Zine at development time was Version 2.3.8.

-----------------------------------------------------------------------------
ABOUT THIS MODULE
-----------------------------------------------------------------------------
This module provides a field formatter for the File field. This formatter allows
to display PDF files as beautiful adaptive flipbooks, using the Devaldi Flexpaper
Viewer. You can see demos of published PDF's at http://flexpaper.devaldi.com/demo.jsp

The module supports both publishing methods (client and server):
1) Server publishing: Upload a pdf and process it on the server, so that the Flexpaper Viewer can
   display it. This variant needs specific conversion tools installed on the server,
   e.g. pdf2swf, pdf2json, swfrender and pdftk.

2) Client publishing: Publish a PDF locally with the "FlexPaper Desktop Publisher Zine" application (Mac and Windows).
   Archive the resulting PDF file structure and upload the archive instead of the pdf (all Drupal archive formats
   are supported). This possibility has no server requirements and therefore can be used on all web servers.

The commercial version of the Flexpaper viewer supports 3 publishing formats: html5,
html4 and Flash. All three formats are supported, but the Adaptive UI can only be used in html5/html4 modes.

Actually HTML5 version requires nothing else than a pdf file, except in case the pdf
should be split into pages for better performance. The split mode splits pdf's into single pages which then can
be loaded seperately. Flash mode uses specially prepared .swf files to operate. And html4 needs .png files.


-----------------------------------------------------------------------------
REQUIREMENTS
-----------------------------------------------------------------------------
General requirements
The module requires the FlexPaper Webserver package available as drupal library. See below for installation
instructions.

These tools are needed for server publishing.
1. pdf2swf and swfrender: these tools support server publishing for the Flash and HTML4 modes.
2. pdf2json: To provide a correct way to search the text in the split mode your server
   should also have pdf2json tool installed and available on the command line.
3. pdftk (optional): To provide highly performing FlipBooks in HTML5, the pdftk command line tool has to be
   installed on the server.
4. Your server should allow PHP exec() and shell_exec() to run the specific command line tools.


-----------------------------------------------------------------------------
INSTALLATION
-----------------------------------------------------------------------------
Flexpaper Library:

1. Create a flexpaper folder in the libraries directory.
2. Download the needed Flexpaper Webserver package from the download page http://flexpaper.devaldi.com/download.
3. There are many folders in this package. You need only js, css and locale folders and FlexPaperViewer.swf file.
4. Copy these items into the flexpaper folder.

Flexpaper2 module:
See official Drupal documentation for module installation instructions.



Sponsored by Pixelgarage (http://www.pixelgarage.ch)
