<?php

/**
 * @file
 * content_lock.func.inc
 */

/**
 * Form alter handler callback.
 */
function _content_lock_form_handler(&$form, &$form_state, $form_id, $callback = '') {
  // Load module inc file.
  module_load_include('inc', 'content_lock', 'includes/content_lock.forms');

  if (!empty($callback) && function_exists($callback)) {
    $callback($form, $form_state, $form_id);
  }
}

/**
 * Add unload js.
 *
 * @param array $form
 *   Form.
 * @param array $form_state
 *   Form state.
 *
 * @return mixed
 *   Form array
 */
function _content_lock_add_unload_js(&$form, $form_state) {
  $m = drupal_get_path('module', 'content_lock');
  drupal_add_js("$m/js/content_lock.url.js", array('group' => JS_LIBRARY));
  drupal_add_js("$m/js/content_lock.core.js", array('group' => JS_LIBRARY));
  drupal_add_js("$m/js/content_lock.init.js");

  $nid = empty($form['nid']['#value']) ? NULL : $form['nid']['#value'];
  $internal_urls = array();
  $internal_form_selectors = array();

  // We're on a locked revision reversion page.
  if (!empty($form['#node_revision']->nid)) {
    $nid = $form['#node_revision']->nid;
    // Don't ask the user if he wants to leave the page when
    // cancelling a reversion.
    $internal_urls[] = $form['actions']['cancel']['#href'];
    $internal_form_selectors[] = '.confirmation';
  }

  $internal_urls[] = 'node/' . $nid . '/edit';
  $internal_form_selectors[] = 'form.node-form';

  // Check lock status.
  $lock = content_lock_fetch_lock($nid);
  if ($lock) {
    $lock_ajax_key = $lock->ajax_key;
  }
  else {
    $lock_ajax_key = FALSE;
  }

  // Get tokens.
  $token = content_lock_get_release_token($nid);

  // Prepare settings.
  $settings = array(
    'nid' => $nid,
    'ajax_key' => $lock_ajax_key,
    'token' => $token,
    'unload_js_message_enable' => variable_get('content_lock_unload_js_message_enable', TRUE),
    'internal_urls' => implode('|', $internal_urls),
    'internal_forms' => implode(', ', $internal_form_selectors),
  );
  if ($settings['unload_js_message_enable']) {
    $settings['unload_js_message'] = variable_get('content_lock_unload_js_message', 'If you proceed, ALL of your changes will be lost.');
  }
  /*
   * Workaround for http://drupal.org/node/1525784 where this function
   * is called multiple times when doing a file field AJAX upload and
   * array_merge_recursive() is used instead of
   * drupal_array_merge_deep_array() to construct the Drupal.settings
   * value. Not calling drupal_add_js() multiple times deprives
   * file_ajax_upload() of the ability to mess up here ;-).
   */
  $called =& drupal_static(__FUNCTION__ . '__called');
  if (!empty($called)) {
    $called++;

    return $form;
  }
  $called = 1;

  drupal_add_js(array('content_lock' => $settings), 'setting');

  return $form;
}
