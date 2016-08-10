<?php
/**
 * This library contains all functionality to create a postcard.
 * The postcard is created as PDF (tcpdf needed) and can be converted to a PNG in high resolution (ImageMagick needed).
 * 
 * 
 * Created by PhpStorm.
 * User: ralph
 * Date: 28.07.16
 * Time: 12:37
 */

/**
 * Performs a PDF postcard creation.
 * The function calls the hook_postcard_draw_pdf_alter() hook where the PDF can be drawn.
 *
 * This function needs the tcpdf_document module installed for PDF creation (TCPDF library wrapper).
 *
 * @param $pdf_name string
 *    The name of the pdf to be created.
 * @param $options array
 *    An array of caller options to be used in the postcard hooks. This array is passed to all postcard hooks.
 *
 *    The options array has additional parameters added during PDF creation.
 *    postcard_image_type:
 *      Indicates the file type of the high resolution postcard image to be created from the pdf (default .png).
 *      You can alter the predefined image type in the hook_postcard_draw_pdf_alter() function to define the image type.
 *    postcard_image_uri:
 *      Holds the high resolution image uri after the file has been created.
 *
 * @return string
 *    Returns the pdf uri of the created postcard pdf.
 * @throws Exception
 *
 * @see tcpdf_document_instance()
 */
function postcard_create_pdf($pdf_name, &$options) {
  //
  // create default pdf and pdf-image path and make sure the directory exists and is writable
  $pdf_dir = 'private://postcards';
  file_prepare_directory($pdf_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
  $pdf_name = _file_check_name($pdf_name);
  $pdf_uri = $pdf_dir . '/' . $pdf_name;

  try {
    //
    // create postcard tcpdf object
    $pdf = tcpdf_document_instance(array(), 'P', 'mm', 'A6'); //105 x 148 mm

    //
    // allow others to draw a custom pdf, pass options
    $options['postcard_image_type'] = '.jpg';
    drupal_alter('postcard_draw_pdf', $pdf, $pdf_uri, $options);

    // finish pdf writing and save pdf to given location
    if ($pdf_sw = file_stream_wrapper_get_instance_by_uri($pdf_uri)) {
      $pdf_path = $pdf_sw->realpath();
    }
    else {
      $pdf_path = $pdf_uri;
    }

    $pdf->lastPage();
    $pdf->Output($pdf_path, 'F');

    //
    // convert postcard pdf to high resolution image, if requested
    if ($options['postcard_image_type']) {
      $img_dir = 'public://postcards';
      file_prepare_directory($img_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      $img_uri = str_replace('.pdf', $options['postcard_image_type'], $pdf_uri);
      $img_uri = str_replace('private://', 'public://', $img_uri);

      if ($img_sw = file_stream_wrapper_get_instance_by_uri($img_uri)) {
        $img_path = $img_sw->realpath();
      }
      else {
        $img_path = $img_uri;
      }
      if (_postcard_convert_to_image($pdf_path, $img_path)) {
        $options['postcard_image_uri'] = $img_uri;
      }
    }

  }
  catch (Exception $e) {
    watchdog('Postcard PDF creation', 'Postcard PDF creation error: @err_msg.', array('@err_msg' => $e->getMessage()), WATCHDOG_ERROR);
    $pdf_uri = false;
  }

  //
  // return the pdf path
  return $pdf_uri;
}

/**
 * Helper function to created a valid file name from a user input name.
 */
function _file_check_name($name) {
  // Punctuation characters that are allowed, but not as first/last character.
  $punctuation = '-_.';
  $map = array(
    // Replace (groups of) whitespace characters.
    '!\s+!' => ' ',
    // Replace multiple dots.
    '!\.+!' => '.',
    // Remove characters that are not alphanumeric or the allowed punctuation.
    "![^0-9A-Za-z$punctuation]!" => '',
  );

  // Apply the regex replacements. Remove any leading or hanging punctuation.
  return trim(preg_replace(array_keys($map), array_values($map), $name), $punctuation);
}

/**
 * Converts a pdf file into a high resolution image file at the given path.
 * The image extension defines the type of the created image, e.g. '.jpg', '.png'...
 *
 * @param $pdf_path string
 *    The path of the pdf file to be converted
 * @param $img_path string
 *    The image file path, where the created image is stored.
 * @return mixed
 *    True, if conversion was successful, false or an error code otherwise.
 */
function _postcard_convert_to_image($pdf_path, $img_path) {
  //
  // set conversion parameter
  $args['quality'] = '-quality ' . escapeshellarg(variable_get('postcard_img_quality', 75));
  $args['sharpen'] = '-sharpen ' . escapeshellarg(variable_get('postcard_img_sharpen', '0x1.0'));
  $args['resize'] = '-resize ' . escapeshellarg(variable_get('postcard_img_resize', 100) . '%');

  //
  // set the convert arguments for a high quality conversion
  // and execute command (ImageMagick convert command)
  //
  // ImageMagick arguments: convert { [-setting]... "image"|-operation }... "output_image"
  // @see http://www.imagemagick.org/Usage/basics/#cmdline
  array_unshift($args, escapeshellarg($pdf_path));
  array_unshift($args, '-density ' . escapeshellarg(variable_get('postcard_pdf_dpi', 150)));
  $args[] = escapeshellarg($img_path);
  $cmd_args = implode(' ', $args);
  return _imagemagick_convert_exec($cmd_args);
}


/* =============================================================================
 *   ImageMagick convert wrapper
 * ========================================================================== */

/**
 * Executes the ImageMagick convert executable as shell command.
 *
 * @param $command_args
 *   A string containing arguments to pass to the convert command, which must
 *   have been passed through escapeshellarg() already.
 * @param &$output
 *   (optional) A variable to assign the shell stdout to, passed by reference.
 * @param &$error
 *   (optional) A variable to assign the shell stderr to, passed by reference.
 * @param $convert_path
 *   (optional) A custom file path to the convert binary. Internal use only.
 *
 * @return mixed
 *   The return value depends on the shell command result:
 *   - Boolean TRUE if the command succeeded.
 *   - Boolean FALSE if the shell process could not be executed.
 *   - Error exit status code integer returned by the executable.
 */
function _imagemagick_convert_exec($command_args, &$output = NULL, &$error = NULL, $convert_path = NULL) {
  // $convert_path is only passed from the system-wide image toolkit form, on
  // which the path to convert is configured.
  // @see _imagemagick_check_path()
  if (!isset($convert_path)) {
    // By using a default of NULL, we force users to setup the toolkit through
    // the image toolkit administration UI. Sites enforcing a path via
    // settings.php should know what they are doing.
    $convert_path = variable_get('postcard_convert_path', NULL);
    if (!isset($convert_path)) {
      return FALSE;
    }
  }

  // Use Drupal's root as working directory to resolve relative paths correctly.
  $drupal_path = DRUPAL_ROOT;

  if (strstr($_SERVER['SERVER_SOFTWARE'], 'Win32') || strstr($_SERVER['SERVER_SOFTWARE'], 'IIS')) {
    // Use Window's start command with the /B flag to make the process run in
    // the background and avoid a shell command line window from showing up.
    // @see http://us3.php.net/manual/en/function.exec.php#56599
    // Use /D to run the command from PHP's current working directory so the
    // file paths don't have to be absolute.
    $convert_path = 'start "ImageMagick" /D ' . escapeshellarg($drupal_path) . ' /B ' . escapeshellarg($convert_path);
  }

  // make sure the path of the convert command is in $PATH environment variable (for dependencies like gs, libxml etc.)
  $env_path = getenv("PATH");        // save $PATH environment variable
  $new_env_path = str_replace('/convert', '', $convert_path);
  if ($new_env_path) {
    $new_env_path .= ":$env_path";
    putenv("PATH=$new_env_path");
  }

  // perform command
  $command = $convert_path . ' ' . $command_args;

  $descriptors = array(
    // stdin
    0 => array('pipe', 'r'),
    // stdout
    1 => array('pipe', 'w'),
    // stderr
    2 => array('pipe', 'w'),
  );
  if ($h = proc_open($command, $descriptors, $pipes, $drupal_path)) {
    $output = '';
    while (!feof($pipes[1])) {
      $output .= fgets($pipes[1]);
    }
    $error = '';
    while (!feof($pipes[2])) {
      $error .= fgets($pipes[2]);
    }

    fclose($pipes[0]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    $return_code = proc_close($h);

    // Display debugging information to authorized users.
    if (variable_get('postcard_debug', FALSE) && user_access('administer postcard')) {
      debug($command, t('ImageMagick command'), TRUE);
      if ($output !== '') {
        debug($output, t('ImageMagick output'), TRUE);
      }
      if ($error !== '') {
        debug($error, t('ImageMagick error'), TRUE);
      }
    }

    // If ImageMagick returned a non-zero code, trigger a PHP error that will
    // be caught by Drupal's error handler, logged to the watchdog and
    // eventually displayed to the user if configured to do so.
    if ($return_code != 0) {
      // If there is no error message, clarify this.
      if ($error === '') {
        $error = t('No error message.');
      }
      watchdog('Postcard image creation', 'ImageMagick error @code: !error', array('@code' => $return_code, '!error' => $error), WATCHDOG_ERROR);
      return FALSE;
    }
    // The shell command was executed successfully.
    return TRUE;
  }
  // The shell command could not be executed.
  watchdog('Postcard image creation', 'The ImageMagick command could not be executed due to an unknown error.', array(), WATCHDOG_ERROR);
  return FALSE;
}



/* =============================================================================
 *   Helper functions to show pdf in browser
 * ========================================================================== */

/**
 * Menu callback of a PDF creation request with one menu parameter.
 *
 * @param $menu_param int
 *    First menu parameter.
 */
function _postcard_show_pdf($menu_param)
{
  // load post node
  $post = node_load($menu_param);

  // create the pdf from the node
  $pdf_name = 'test_pdf_' . time() . '.pdf';
  $options = array('post_node' => $post);
  $pdf_uri = postcard_create_pdf($pdf_name, $options);

  // send pdf to client
  if ($pdf_uri) {
    postcard_transfer_pdf($pdf_uri, $pdf_name, true);
  }

  // file not found
  drupal_not_found();
  drupal_exit();
}

/**
 * Downloads / shows the pdf with the given file name from the private folder.
 *
 * @param string $pdf_uri
 *    A file uri in the format "scheme://target".
 * @param string $filename
 *    The filename of the file.
 * @param bool $show_file_inline
 *    Flag, if TRUE shows the file inline (in the browser), otherwise downloads the file to the client.
 * @param int $valid_duration
 *    Duration during which the file is valid (default: -1 meaning no time limit)
 */
function postcard_transfer_pdf($pdf_uri, $filename, $show_file_inline = false, $valid_duration = -1)
{
  // get stream wrapper instance for pdf
  $stream_wrapper = file_stream_wrapper_get_instance_by_uri($pdf_uri);
  $pdf_path = $stream_wrapper->realpath();

  if (file_exists($pdf_path)) {
    if ($valid_duration < 0 || filemtime($pdf_path) >= time() - $valid_duration) {
      $content_dispo = $show_file_inline ? 'inline' : 'attachment';
      $content_dispo .= '; filename="' . $filename . '"';
      file_transfer($pdf_uri, array(
        'Cache-control' => 'private',
        'Content-Type' => file_get_mimetype($pdf_uri), //'application/pdf',  'application/octet-stream'
        'Content-Disposition' => $content_dispo,
        'Content-Length' => filesize($pdf_path)
      ));
    }
  }
  drupal_not_found();
  drupal_exit();
}



