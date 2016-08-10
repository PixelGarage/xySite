<?php
/**
 * Contains all postcard module specific hooks.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 20.07.16
 * Time: 20:00
 */

/* --------------------------------------------------
 * Postcard multi-step webform hooks
 * --------------------------------------------------*/
/**
 * Defines the multi-step webform process to create and deliver a postcard.
 * BE AWARE that the webform redirect url's are automatically set for each
 * webform in the process with a successor webform (url points to successor webform).
 *
 * The info array is an associated array defining all steps of the multi-step webform.
 *
 * A step info array must contain at least the following keys:
 *    'nid':          the webform node id
 *    'prev step':    step key of the previous step. Is null for the first step.
 *    'next step':    step key of the next step. IS null for the last step.
 */
function hook_postcard_multi_step_webform_info() {
  $info = array(
    'step1' => array(
      // set webform node id of step 1
      'nid' =>  1,
      // previous step
      'prev step' => null,
      // next step
      'next step' => null,
    ),
  );

  return $info;
}

/**
 * Alter the multi-step webform info to create and deliver a postcard.
 * @param $info array
 *    The multi-step webform info array to be altered.
 */
function hook_postcard_multi_step_webform_info_alter(&$info) {
  // set the node id of the first step
  $info['step 1']['nid'] = 345;

  // set a custom preview step after step 1
  $info['step 1']['next step'] = 'preview';
  $info['preview'] = array(
    // preview webform node id
    'nid' =>  346,
    // previous step
    'prev step' => 'step 1',
    // next step
    'next step' => null,
  );
}

/**
 * Alter a specific step submission of a multi-step webform,  prior to saving it in the database.
 *
 * @param $step_options array
 *    An array holding all step options to be transferred between the steps. This array can be altered between steps.
 *    The initial array holds two parameters:
 *      step_key: The key of the step.
 *      step_info: The step info array.
 * @param $node     object
 *    The webform node of the particular step
 * @param $submission   object
 *    The submission of the particular step
 */
function hook_postcard_multi_step_submission_presave_alter(&$step_options, $node, &$submission) {
  
}

/**
 * Alter a specific step removal of a multi-step webform, prior of the submission deletion, e.g. delete objects
 * you have created in the submission presave step.
 *
 * @param $step_key   string
 *    The step key indicating which submission of the multi-step webform is going to be deleted
 * @param $node   object
 *    The webform node (tnid)
 * @param $submission  object
 *    The submission object.
 */
function hook_postcard_multi_step_submission_remove_alter($step_key, $node, $submission) {
  //
  // cleanup the step before submission is deleted
}


/* --------------------------------------------------
 * Postcard creation alter hooks
 *
 * The following hooks are called inside of the library
 * function postcard_create_pdf().
 * --------------------------------------------------*/
/**
 * Allows to draw to a fully initialised pdf. Only the postcard itself has to be drawn here.
 * The initialisation of the TCPDF object has already been done.
 * (See https://tcpdf.org for more details about PDF creation.)
 *
 * Additionally the PDF postcard can be converted (ImageMagick need to be installed) to a high resolution image,
 * if an image file path with an image type extension is returned.
 *
 * @param $tcpdf_obj  object
 *    A fully initialized TCPDF object. Use this object to draw your pdf.
 * @param $pdf_uri string
 *    Alter the predefined pdf uri (schema://target.pdf), if needed.
 * @param $options array
 *    The options array passed to the postcard_create_pdf() function.
 *
 *    The options array has an additional parameter added.
 *    options['postcard_image_type']:
 *      Indicates the file type of the high resolution postcard image to be created from the pdf (default .png).
 *      You can alter the predefined image type, e.g. '.jpg', '.png' etc. or set it to FALSE to prevent image creation.
 *
 *    Image creation prerequisites:
 *      ImageMagick has to be installed (convert binary)
 */
function hook_postcard_draw_pdf_alter (&$tcpdf_obj, &$pdf_uri, &$options) {
  // alter pdf uri
  $pdf_uri = 'private://postcards/test.pdf';

  // disable high resolution image creation
  $options['postcard_image_type'] = false;

  //
  // draw the postcard
  $bg_color = array(155, 195, 27); // #9bc31b
  $tcpdf_obj->Rect(0, 0, 105, 148, 'F', array(), $bg_color);
}
