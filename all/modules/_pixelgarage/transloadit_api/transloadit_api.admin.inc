<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 11.04.17
 * Time: 14:28
 */

/**
 * Stripe API admin settings form.
 */
function transloadit_api_admin_settings_form($form, $form_state) {
  $form['transloadit_api_links'] = array(
    '#markup' => t('!templates | !api_credentials | !docs<br /><br />', array(
      '!templates' => l(t('Templates'), 'https://transloadit.com/templates', array('attributes' => array('target' => '_blank'))),
      '!api_credentials' => l(t('API Credentials'), 'https://transloadit.com/accounts/credentials', array('attributes' => array('target' => '_blank'))),
      '!docs' => l(t('Docs'), 'https://transloadit.com/docs/api-docs/', array('attributes' => array('target' => '_blank'))),
    )),
  );

  //
  // Transloadit request
  $form['transloadit_api_request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Transloadit Request Parameters'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 10,
  );
  $form['transloadit_api_request']['transloadit_api_auth_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Transloadit authentication key'),
    '#default_value' => variable_get('transloadit_api_auth_key', ''),
  );
  $form['transloadit_api_request']['transloadit_api_auth_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Transloadit authentication secret'),
    '#default_value' => variable_get('transloadit_api_auth_secret', ''),
  );
  $form['transloadit_api_request']['transloadit_api_template_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Transloadit default template id'),
    '#default_value' => variable_get('transloadit_api_template_id', ''),
    '#description' => t('The default template to be used when processing files. The default template can be overridden in the API calls.'),
  );
  if (variable_get('transloadit_api_auth_secret', '')) {
    $form['transloadit_api_request']['transloadit_api_test'] = array(
      '#type' => 'button',
      '#value' => t('Test Transloadit Connection'),
      '#ajax' => array(
        'callback' => '_transloadit_api_admin_test_connection',
        'wrapper' => 'transloadit-connect-results',
        'method' => 'append',
      ),
      '#suffix' => '<div id="transloadit-connect-results"></div>',
    );
  }

  $form['transloadit_api_webhook'] = array(
    '#type' => 'fieldset',
    '#title' => t('Transloadit Webhook'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 20,
  );
  $form['transloadit_api_webhook']['transloadit_api_localtunnel_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Localtunnel URL'),
    '#default_value' => variable_get('transloadit_api_localtunnel_url', ''),
    '#description' => t('The Localtunnel URL used for testing the webhook locally. If you have !localtunnel installed, start a tunnel with the command "lt --port 80" in a terminal and copy the returned Localtunnel URL here.', array(
      '!localtunnel' => l(t('Localtunnel'), 'https://localtunnel.github.io/www/', array('attributes' => array('target' => '_blank'))),
    )),
  );
  if ($lt_url = variable_get('transloadit_api_localtunnel_url', false)) {
    $ide_key = $_COOKIE['XDEBUG_SESSION'];
    $debug_param = $ide_key ? '?XDEBUG_SESSION_START=' . $ide_key : '';
    $notification_url = $lt_url . '/' . TRANSLOADIT_NOTIFY_WEBHOOK . $debug_param;
  }
  else {
    $notification_url = url(TRANSLOADIT_NOTIFY_WEBHOOK, array('absolute' => TRUE));
  }
  $form['transloadit_api_webhook']['transloadit_api_webhook_url'] = array(
    '#type' => 'textfield',
    '#disabled' => TRUE,
    '#title' => t('Notification URL (webhook)'),
    '#default_value' => $notification_url,
    '#size' => 100,
    '#description' => t('The notification URL used for all transloadit asynchronous responses.'),
  );

  //
  // Transloadit export ftp account
  $form['transloadit_api_ftp_account'] = array(
    '#type' => 'fieldset',
    '#title' => t('Transloadit export FTP account'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('The FTP account is used by Transloadit to export the conversion results. For a localhost server (see Localtunnel above), no export step is performed. In this case the returned file URLs are valid for 24 hours.'),
    '#weight' => 30,
  );
  $form['transloadit_api_ftp_account']['transloadit_api_ftp_user'] = array(
    '#type' => 'textfield',
    '#title' => t('FTP user'),
    '#description' => t('The FTP user used for the Transloadit file export step.'),
  );
  $form['transloadit_api_ftp_account']['transloadit_api_ftp_password'] = array(
    '#type' => 'password_confirm',
    '#title' => t('FTP password'),
    '#description' => t('The FTP password used for thenTransloadit file export step.'),
  );


  return system_settings_form($form);
}

/**
 * AJAX callback to test the Transloadit connection.
 */
function _transloadit_api_admin_test_connection($form, $form_state) {
  $path = '/bill/' . date('Y-m');
  $request = transloadit_api_get_instance();
  $response = $request->request(array('method' => 'GET', 'path'   => $path), true);

  if ($response->data['ok'] === 'BILL_FOUND') {
    $you = $response->data['to'] ? $response->data['to'] : 'You';
    $gb = $response->data['used_gb'];
    return t('Successful connection! @you used @gb GB this month.', array('@you' => $you, '@gb' => $gb));
  }
  else {
    return t('Error! @err', array('@err' => $response->error()));
  }
}
