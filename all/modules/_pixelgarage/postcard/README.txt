
-- SUMMARY --
This module allows to create a PDF postcard (tcpdf needed) and to convert
this pdf in a high resolution PNG file (ImageMagick needed):


-- REQUIREMENTS ImageMagick--

* ImageMagick (http://www.imagemagick.org) needs to be installed on your server
  and the convert binary needs to be accessible and executable from PHP.

* The PHP configuration must allow invocation of proc_open() (which is
  security-wise identical to exec()).

Consult your server administrator or hosting provider if you are unsure about
these requirements.


-- REQUIREMENTS tcpdf_document --
The tcpdf_document module contains more information about requirements.


-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.


