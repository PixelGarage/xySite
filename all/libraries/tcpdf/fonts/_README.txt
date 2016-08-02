TCPDF custom fonts
==================
To add a custom font, the following steps has to be performed.

1) Convert the font.ttf file with the online TCPDF font converter: http://fonts.snm-portal.com
2) Copy the generated files to the default font folder in sites/all/libraries/tcpdf/fonts.
3) Add your font into the document and set the font, see example below:

  $tcpdf_obj->addFont('museosans_900', '', 'museosans_900');
  $tcpdf_obj->setFont('museosans_900');//, null, 'museosans_900', false);
  $tcpdf_obj->setFontSize(22);
  $tcpdf_obj->SetTextColor(255, 255, 255);
  $tcpdf_obj->setXY(0,2);
  // Format: Cell($w, $h=0, $txt='', $border=0, $ln=0, $align='', $fill=false, $link='', $stretch=0, $ignore_min_height=false, $calign='T', $valign='M')
  $tcpdf_obj->Cell(0, 0, $slogan, 0, 0, 'C', false);

