<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 12.04.17
 * Time: 17:15
 */

/**
 * This hook is called during the assembly notification (webhook)
 * of a Transloadit asynchronous request. The standard implementation calls the
 * hook_transloadit_api_context_alter() alter hook with the corresponding
 * context and conversion results and therefore has normally not to be overridden.
 *
 * @param array $results
 *    An associative array containing the resulting files of a transloadit
 *    assembly response.
 * @param array $data
 *    The complete data array of a transloadit assembly response.
 *
 * @see hook_transloadit_api_context_alter().
 */
function hook_transloadit_api_notification($results, $data) {
  //
  // store the resulting files somewhere
}

/**
 * This hook can be used to alter the stored context for an asynchronous
 * Transloadit request.
 *
 * @param array $context
 *    The context array to be altered during the asynchronous assembly
 *    notification.
 * @param array $results
 *    An associative array containing the resulting files of a transloadit
 *    assembly.
 * @param array $data
 *    The complete data array of a transloadit assembly response.
 *
 * @see transloadit_api_execute_assembly()
 */
function hook_transloadit_api_context_alter(&$context, $results, $data) {
  //
  // fill results into the node stored as context during asynchronous assembly notification
  $node = node_load($context['nid']);
  $node->converted_file_url[LANGUAGE_NONE][0]['url'] = $results['step_x']['file/path'];
  node_save($node);
}
