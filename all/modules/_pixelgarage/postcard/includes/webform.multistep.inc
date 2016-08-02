<?php
/**
 * Enhances the webform module with the possibility to define multi-step webforms.
 *
 * The following functionality is available:
 *    - definition of multi-step webforms can be done with hooks
 *    - automatic creation of webform sequence according to the definition (redirect_url)
 *    - automatic creation of Back-buttons in webforms, if a previous webform exists.
 *    - muti-step webform editing by anonymous user supported (access via token)
 *    - submission_presave hook is multi-step enabled
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 20.07.16
 * Time: 17:36
 */


/* --------------------------------------------------
 * Webform hooks
 * --------------------------------------------------*/

/**
 * Called for each webform submission before it is saved.
 *
 * Implements a multi-step form submission presave.
 */
function postcard_webform_submission_presave($node, &$submission) {
  //
  // return, if form is not part of a mult-step form
  if (!_is_multistep_webform($node)) return;


  //
  // perform the submission presave for the specific step (webform node)
  // and the session data exchanged between steps
  $master = postcard_webform_master_form($node);

  //
  // handle step options in session
  $session_data = &postcard_session_data();
  if (_is_first_step($master)) {
    $step_options = array(
      'step_key' => _get_step_key($master),
      'step_info' => _get_step_info($master),
    );
  }
  else {
    $step_options = $session_data['step_options'];
    $step_options['step_key'] = _get_step_key($master);
    $step_options['step_info'] = _get_step_info($master);
  }

  //
  // alter submission before saving
  drupal_alter('postcard_multi_step_submission_presave', $step_options, $node, $submission);
  $session_data['step_options'] = $step_options;

  //
  // cleanup session data
  if (_is_last_step($master)) {
    unset($session_data['step_options']);
  }

}

/**
 * Implements hook_webform_submission_insert().
 * Called after the submission has been saved.
 */
function postcard_webform_submission_insert($node, $submission) {
  //
  // return, if form is not part of a mult-step form
  if (!_is_multistep_webform($node)) return;


  //
  // create multi-step submission chain in db for later use
  _save_step_submission($node, $submission);
}

/**
 * Menu title callback; Return the submission step as a title.
 */
function postcard_webform_submission_title($node, $submission) {
  return t('Edit @title', array('@title' => $node->title));
}


/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Extends the webform client form with a back button, if a previous step exists.
 */

function postcard_form_webform_client_form_alter (&$form, &$form_state) {
  //
  // return, if form is not part of a mult-step form
  $node = $form['#node'];
  if (!_is_multistep_webform($node)) return;

  //
  // cleanup session data at the opening step of a multi-step process
  // do not clean up during editing (submission exists)
  $session_data = &postcard_session_data();
  $master = postcard_webform_master_form($node);
  $submission = $form['#submission'];
  if (!$submission && _is_first_step($master)) {
    unset($session_data['multistep']);
  }

  //
  // find the previous / next step (webform) of the multi-step webform
  $prev_step = false;
  $next_step = false;
  if ($submission) {
    //
    // OPENED VIA ANONYMOUS ACCESS LINK (submission available)
    // get previous and next step from database, if available
    $prev_step = _get_previous_step_submission($master->nid, $submission->sid);
    $next_step = _get_next_step_submission($master->nid, $submission->sid);

    //
    // mark form as not finished to edit step with submission values set
    // This flag has to be false, that the redirect url is not overridden
    $form['details']['finished']['#value'] = false;

    //
    // set submission page title
    drupal_set_title(postcard_webform_submission_title($node, $submission));
  }
  else if (isset($session_data['multistep']['last_step'])) {
    //
    // CALLED DURING MULTI-STEP WEBFORM CREATION PROCESS
    // get the previous step from session (no db entry yet), no next step available
    $last_step = $session_data['multistep']['last_step'];
    $prev_step = $session_data['multistep'][$last_step];
  }

  //
  // define the back link, if a previous webform exists in the multi-step process
  if ($prev_step) {
    $button_title = t('Edit postcard');
    $form['actions']['back'] = array(
      '#markup' => postcard_create_anonym_submission_link($prev_step['nid'], $prev_step['sid'], 'anonym_edit', true, $button_title),
    );
  }

  //
  // set the language dependent redirect url of the form. Set anonymous link, if next step is known!
  if ($next_step) {
    $form['#node']->webform['redirect_url'] =
      postcard_create_anonym_submission_link($next_step['nid'], $next_step['sid'], 'anonym_edit');

  }
  else if ($next_step = _get_step_info($master, 'next')) {
    global $language;

    $next_form = node_load($next_step['nid']);
    $next_master = postcard_webform_master_form($next_form);
    $transl_arr = translation_node_get_translations($next_master->nid);
    $lang_nid = !empty($transl_arr) ? $transl_arr[$language->language]->nid : $next_master->nid;

    $form['#node']->webform['redirect_url'] = "node/{$lang_nid}";

  }
}


/* --------------------------------------------------
 * Multi-step info helper
 * --------------------------------------------------*/

/**
 * Load postcard multi-step webform info from 3rd party modules.
 */
function _multi_step_webform_info() {
  $info = &drupal_static(__FUNCTION__);
  if (!isset($info)) {
    $info = array();

    foreach (module_implements('postcard_multi_step_webform_info') as $module) {
      $steps = module_invoke($module, 'postcard_multi_step_webform_info');
      foreach ($steps as $key => $step) {
        $steps[$key]['module'] = $module;
      }
      $info = array_merge($info, $steps);
    }
    drupal_alter('postcard_multi_step_webform_info', $info);

  }
  return $info;
}

/**
 * Returns the tnid's from all multi-step webforms.
 * @return array
 *    An array of all tnid's.
 */
function _is_multistep_webform($node) {
  $master = postcard_webform_master_form($node);
  $steps = _multi_step_webform_info();
  $tnids = array();
  foreach($steps as $key => $step_info) {
    // load step webform master
    $step_form = node_load($step_info['nid']);
    $step_master = postcard_webform_master_form($step_form);
    $tnids[] = $step_master->nid;
  }
  return in_array($master->nid, $tnids);

}

/**
 * Gets the step key of the given master node (language independent)
 *
 * @param $master   object
 *    The master webform node (tnid)
 * @param $step   string
 *     Step key to return, e.g. 'current' | 'prev' | 'next', default is 'current'
 * @return string
 *    The step key string.
 */
function _get_step_key($master, $step = 'current') {
  $steps = _multi_step_webform_info();
  foreach($steps as $key => $step_info) {
    // load step webform master
    $step_form = node_load($step_info['nid']);
    $step_master = postcard_webform_master_form($step_form);

    if ($master->nid == $step_master->nid) {
      switch ($step) {
        case 'current':
          return $key;
        case 'prev':
          $key = $step_info['prev step'];
          return $key ? $key : false;
        case 'next':
          $key = $step_info['next step'];
          return $key ? $key : false;
      }
    }
  }
  return false;
}

/**
 * Gets the step info array of the given master node (language independent)
 *
 * @param $master   object
 *    The master webform node (tnid)
 * @param $step   string
 *     Step info to return, e.g. 'current' | 'prev' | 'next', default is 'current'
 * @return bool|mixed
 */
function _get_step_info($master, $step = 'current') {
  $steps = _multi_step_webform_info();
  foreach($steps as $key => $step_info) {
    // load step webform master
    $step_form = node_load($step_info['nid']);
    $step_master = postcard_webform_master_form($step_form);

    if ($master->nid == $step_master->nid) {
      switch ($step) {
        case 'current':
          return $step_info;
        case 'prev':
          $key = $step_info['prev step'];
          return $key ? $steps[$key] : false;
        case 'next':
          $key = $step_info['next step'];
          return $key ? $steps[$key] : false;
      }
    }
  }
  return false;
}

/**
 * Returns true, if given webform is first step of a multi-step webform. False otherwise.
 */
function _is_first_step($master) {
  $steps = _multi_step_webform_info();
  foreach($steps as $key => $step_info) {
    // load step webform master
    $step_form = node_load($step_info['nid']);
    $step_master = postcard_webform_master_form($step_form);

    if ($master->nid == $step_master->nid) {
      return !isset($step_info['prev step']);
    }
  }
  return false;
}

/**
 * Returns true, if given webform is last step of a multi-step webform. False otherwise.
 */
function _is_last_step($master) {
  $steps = _multi_step_webform_info();
  foreach($steps as $key => $step_info) {
    // load step webform master
    $step_form = node_load($step_info['nid']);
    $step_master = postcard_webform_master_form($step_form);

    if ($master->nid == $step_master->nid) {
      return !isset($step_info['next step']);
    }
  }
  return false;
}


/* --------------------------------------------------
 * Multi-step db access helper
 * --------------------------------------------------*/

/**
 * Gets the node and submission id of the previous step in a multi-step webform from the database.
 * REMARK: The database contains the translation set master id's of a webform (tnid) in a multi-lingual site.
 *
 * @param $nid  int   Node id of current webform (tnid).
 * @param $sid  int   Submission id of current submission.
 * @return array mixed
 *    Associated array with nid and sid of previous step or false, if no result has been found.
 */
function _get_previous_step_submission($nid, $sid) {
  //
  // Query the previous node and submission id.
  $query = db_select('postcard_webform_multistep', 'wm');
  $query
    ->fields('wm', array('prev_nid', 'prev_sid'))
    ->condition('wm.nid', $nid)
    ->condition('wm.sid', $sid)
    ->range(0, 1);
  $result = $query->execute();

  $prev_step_submission = array();
  foreach ($result as $row) {
    $prev_step_submission['nid'] = $row->prev_nid;
    $prev_step_submission['sid'] = $row->prev_sid;
  }

  return !empty($prev_step_submission) ? $prev_step_submission : false;
}

/**
 * Gets the node and submission id of the next step in a multi-step webform from the database.
 * REMARK: The database contains the translation set master id's of a webform (tnid) in a multi-lingual site.
 *
 * @param $nid  int   Node id of current webform (tnid).
 * @param $sid  int   Submission id of current submission.
 * @return array mixed
 *    Associated array with nid and sid of next step or false, if no result has been found.
 */
function _get_next_step_submission($nid, $sid) {
  //
  // Query the next node and submission id.
  $query = db_select('postcard_webform_multistep', 'wm');
  $query
    ->fields('wm', array('nid', 'sid'))
    ->condition('wm.prev_nid', $nid)
    ->condition('wm.prev_sid', $sid)
    ->range(0, 1);
  $result = $query->execute();

  $next_step_submission = array();
  foreach ($result as $row) {
    $next_step_submission['nid'] = $row->nid;
    $next_step_submission['sid'] = $row->sid;
  }

  return !empty($next_step_submission) ? $next_step_submission : false;
}

/**
 * Saves the submission (webform) of a new step in a multi-step webform permanently.
 * This allows to recreate the multi-step form with all submissions later.
 *
 * REMARK: In a multi-lingual site, the translation set master id's of a webform (tnid) are stored.
 *
 * @param $node
 * @param $sid
 */
function _save_step_submission($node, $submission) {
  //
  // perform the submission insert for the specific step (webform node)
  $session_data = &postcard_session_data();
  $master = postcard_webform_master_form($node);
  $step_key = _get_step_key($master);
  $step_info = _get_step_info($master);

  //
  // save previous submission in database, if any
  $prev_step = $step_info['prev step'];
  if ($prev_step && isset($session_data['multistep'][$prev_step])) {
    $record  = array(
      'nid' => $master->nid,
      'sid' => $submission->sid,
      'prev_nid' => $session_data['multistep'][$prev_step]['nid'],
      'prev_sid' => $session_data['multistep'][$prev_step]['sid'],
    );
    drupal_write_record('postcard_webform_multistep', $record);
  }

  //
  // save webform and submission id in session (to be saved in next step)
  // cleanup in last step
  if ($step_info['next step']) {
    // save webform and submission id for current step in session
    $session_data['multistep'][$step_key] = array(
      'nid' => $master->nid,
      'sid' => $submission->sid,
    );
    $session_data['multistep']['last_step'] = $step_key;

  }
  else {
    // cleanup session data in last step
    unset($session_data['multistep']);

  }

}



/* ---------------------------------------------------------------------
 * Multi-step webform submission access by anonymous user (offer actions, access rights)
 * ---------------------------------------------------------------------*/

/**
 * Implements hook_webform_webform_submission_actions().
 *
 * Adds action to allow deletion of the corresponding submission by an anonymous user (token enabled access).
 */
function postcard_webform_submission_actions($node, $submission) {
  $actions = array();
  $token_access = $submission && isset($_GET['token']) && $_GET['token'] == webform_get_submission_access_token($submission);

  //
  // show submission actions only for anonymous users with token access
  // and secure the action callbacks with token too
  if ($token_access) {
    $actions['remove'] = array(
      'title' => t('Remove post'),
      'href' => 'postcard/' . $node->nid . '/submission/' . $submission->sid . '/anonym_remove',
      'query' => drupal_get_query_parameters(),
    );
  }

  return $actions;
}

/**
 * Confirm form to delete a single form submission.
 *
 * @param $form
 *   The new form array.
 * @param $form_state
 *   The current form state.
 * @param $master
 *   The master webform of a translation set.
 * @param $submission
 *   The submission to be deleted (including the attached offer).
 * @return mixed The confirmation form render array.
 */
function postcard_webform_submission_remove_form($form, $form_state, $master, $submission) {
  // Keep the NID and SID in the same location as the webform_client_form().
  // This helps mollom identify the same fields when deleting a submission.
  $form['#tree'] = TRUE;
  $form['details']['nid'] = array(
    '#type' => 'value',
    '#value' => $master->nid,
  );
  $form['details']['sid'] = array(
    '#type' => 'value',
    '#value' => $submission->sid,
  );

  $question = t('Are you sure you want to remove this submission?');
  $query = drupal_http_build_query(array('token' => $_GET['token']));
  $cancel_redirect = "postcard/{$master->nid}/submission/{$submission->sid}/anonym_view?" . $query;

  return confirm_form($form, NULL, $cancel_redirect, $question, t('Remove'), t('Cancel'));
}

function postcard_webform_submission_remove_form_submit($form, &$form_state) {
  $master = node_load($form_state['values']['details']['nid']);
  $submission = webform_get_submission($form_state['values']['details']['nid'], $form_state['values']['details']['sid']);
  $post_nid = _webform_submission_value($master, 'post_nid', $submission);

  // TODO: remove all submissions of the multi-step webform

  // unpublish post
  $post = node_load($post_nid);
  $post->status = 0;
  node_save($post);

  drupal_set_message(t('Post successfully removed.'));

  // redirect to home
  $form_state['redirect'] = '/';
}


/* --------------------------------------------------
 * Webform submission helpers
 * --------------------------------------------------*/
/**
 * Gets the submitted value for a specific webform component.
 *
 * @param object $master
 *    The webform master node containing the component to be read.
 * @param string $form_key
 *    The defined form key of the component.
 * @param object $submission
 *    The form submission with the submitted values.
 * @return mixed
 *    The value of the webform component or false, if not set.
 */
function _webform_submission_value($master, $form_key, $submission) {
  foreach ($master->webform['components'] as $key => $data) {
    if ($data['form_key'] == $form_key) {
      return !empty($submission->data[$key]) ? $submission->data[$key][0] : false;
    }
  }
  return false;
}

/**
 * Sets a submission value for the given form key.
 *
 * @param object $master
 *    The webform master node containing the component to be updated.
 * @param $submission    object
 *    The submission object
 * @param $form_key      string
 *    The form key of the component
 * @param $value         mixed
 *    The value to be set on submission for specified component
 */
function _webform_set_submission_value($master, $form_key, $submission, $value) {
  foreach ($master->webform['components'] as $key => $data) {
    if ($data['form_key'] == $form_key) {
      $submission->data[$key][0] = $value;
      break;
    }
  }
}


