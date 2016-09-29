<?php
/**
 * Contains the admin form to administer the postcard creation.
 * 
 * Created by PhpStorm.
 * User: ralph
 * Date: 29.07.16
 * Time: 22:40
 */

/**
 * Settings admin form for the postcard creation process.
 */
function postcard_settings_form($form, &$form_state) {
  $form['postcard_enable_functionality'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable postcard functionality'),
    '#default_value' => variable_get('postcard_enable_functionality', 0),
    '#description' => t('Set flag to enable the postcard and preview functionality.'),
  );
  $form['postcard_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Postcard PDF and preview image quality parameter settings'),
    '#collapsible' => FALSE,
    '#description' => t('Set specifc pdf and image quality parameters for the postcard and preview functionality.'),
    '#states' => array(
      // Hide the settings when the cancel notify checkbox is disabled.
      'visible' => array(
        ':input[name="postcard_enable_functionality"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['postcard_fieldset']['postcard_image_text'] = array(
    '#type' => 'item',
    '#title' => t('ImageMagick setup'),
    '#markup' => t('<p>ImageMagick is used for the preview function. ImageMagick is a stand-alone program for image manipulation. It must be installed on the server and you need to know where it is located. Consult your server administrator or hosting provider for details.</p>'),
    '#weight' => -20,
  );
  $form['postcard_fieldset']['postcard_convert_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to the ImageMagick binary (convert)'),
    '#default_value' => variable_get('postcard_convert_path', '/usr/local/bin/convert'),
    '#required' => TRUE,
    '#element_validate' => array('postcard_element_validate_path'),
    '#weight' => -10,
    '#description' => t('The complete path and filename of the ImageMagick <kbd>convert</kbd> binary. For example: <kbd>/usr/bin/convert</kbd> or <kbd>C:\Program Files\ImageMagick-6.3.4-Q16\convert.exe</kbd>'),
  );

  $form['postcard_fieldset']['postcard_pdf_dpi'] = array(
    '#type' => 'textfield',
    '#title' => t('Postcard pdf quality (dpi)'),
    '#size' => 10,
    '#maxlength' => 3,
    '#default_value' => (int)variable_get('postcard_pdf_dpi', 150),
    '#field_suffix' => 'dpi',
    '#element_validate' => array('postcard_element_validate_pdf_dpi'),
    '#description' => t('Define the pdf quality in dpi. Higher values mean better pdf quality but bigger files.'),
  );
  $form['postcard_fieldset']['postcard_img_quality'] = array(
    '#type' => 'textfield',
    '#title' => t('Postcard image quality'),
    '#size' => 10,
    '#maxlength' => 3,
    '#default_value' => variable_get('postcard_img_quality', 75),
    '#field_suffix' => '%',
    '#element_validate' => array('postcard_element_validate_quality'),
    '#description' => t('Define the preview image quality. Ranges from 0 to 100. Higher values mean better image quality but bigger files.'),
  );
  $form['postcard_fieldset']['postcard_img_sharpen'] = array(
    '#type' => 'textfield',
    '#title' => t('Postcard image sharpen'),
    '#size' => 10,
    '#maxlength' => 10,
    '#default_value' => variable_get('postcard_img_sharpen', '0X1.0'),
    '#field_suffix' => '[radiusxsigma]',
    '#description' => t('Define a Gaussian operator of the given radius and standard deviation (sigma).'),
  );
  $form['postcard_fieldset']['postcard_img_resize'] = array(
    '#type' => 'textfield',
    '#title' => t('Postcard image resize'),
    '#size' => 10,
    '#maxlength' => 10,
    '#default_value' => variable_get('postcard_img_resize', 100),
    '#field_suffix' => '%',
    '#description' => t('Define a resize factor in percentage for the preview image.'),
  );

  $form['postcard_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display debugging information'),
    '#default_value' => variable_get('postcard_debug', 0),
    '#description' => t('Shows ImageMagick commands and their output to users with the %permission permission.', array(
      '%permission' => t('administer postcard'),
    )),
  );


  return system_settings_form($form);
}

/**
 * Form element validation handler for convert executable path setting.
 */
function postcard_element_validate_path($element, &$form_state) {
  $status = _check_imagemagick_path($element['#value']);
  if ($status['errors']) {
    // Form API allows only one error per element, so we concatenate possibly multiple errors.
    form_error($element, implode('<br />', $status['errors']));
  }
}

/**
 * Form element validation handler for pdf quality settings field.
 */
function postcard_element_validate_pdf_dpi($element, &$form_state) {
  if (!is_numeric($element['#value']) || $element['#value'] < 0 || $element['#value'] > 500) {
    form_error($element, t('!name must be an integer between 0 and 500.', array('!name' => $element['#title'])));
  }
}

/**
 * Form element validation handler for image quality settings field.
 */
function postcard_element_validate_quality($element, &$form_state) {
  if (!is_numeric($element['#value']) || $element['#value'] < 0 || $element['#value'] > 100) {
    form_error($element, t('!name must be a value between 0 and 100.', array('!name' => $element['#title'])));
  }
}


/* =============================================================================
 *   ImageMagick convert path checker
 * ========================================================================== */

/**
 * Verifies file path of ImageMagick convert binary by checking its version.
 *
 * @param $file
 *   The user-submitted file path to the convert binary.
 *
 * @return
 *   An associative array containing:
 *   - output: The shell output of 'convert -version', if any.
 *   - errors: A list of error messages indicating whether ImageMagick could not
 *     be found or executed.
 */
function _check_imagemagick_path($file) {
  $status = array(
    'output' => '',
    'errors' => array(),
  );

  // If only the name of the executable is given, we only check whether it is in
  // the path and can be invoked.
  if ($file != 'convert') {
    // Check whether the given file exists.
    if (!is_file($file)) {
      $status['errors'][] = t('The specified ImageMagick file path %file does not exist.', array('%file' => $file));
    }
    // If it exists, check whether we can execute it.
    elseif (!is_executable($file)) {
      $status['errors'][] = t('The specified ImageMagick file path %file is not executable.', array('%file' => $file));
    }
  }
  // In case of errors, check for open_basedir restrictions.
  if ($status['errors'] && ($open_basedir = ini_get('open_basedir'))) {
    $status['errors'][] = t('The PHP <a href="@php-url">open_basedir</a> security restriction is set to %open-basedir, which may prevent to locate ImageMagick.', array(
      '%open-basedir' => $open_basedir,
      '@php-url' => 'http://php.net/manual/en/ini.core.php#ini.open-basedir',
    ));
  }

  // Unless we had errors so far, try to invoke convert.
  if (!$status['errors']) {
    $result = _imagemagick_convert_exec('-version', $status['output'], $error, $file);
    // _imagemagick_convert_exec() triggers a user error upon failure, but
    // during form validation all errors need to be reported.
    if ($error !== '') {
      // $error normally needs check_plain(), but file system errors on Windows
      // use a unknown encoding. check_plain() would eliminate the entire string.
      $status['errors'][] = $error;
    }
  }

  return $status;
}

