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
  $path = drupal_get_path('module', 'content_lock');
  drupal_add_js($path . '/js/content_lock.url.js', array('group' => JS_LIBRARY));
  drupal_add_js($path . '/js/content_lock.core.js', array('group' => JS_LIBRARY));
  drupal_add_js($path . '/js/content_lock.init.js', array('group' => JS_DEFAULT));

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

/**
 * Check lock status.
 *
 * @param int $uid
 *    User ID.
 * @param int $nid
 *    Node ID.
 *
 * @return bool
 *    Return TRUE OR FALSE.
 */
function _content_lock_still_locked($uid, $nid) {
  $query = db_select('content_lock', 'c')
    ->condition('nid', $nid)
    ->condition('uid', $uid)
    ->countQuery();
  $result = $query->execute();

  return (bool) $result->fetchField();
}

/**
 * Release all locks.
 */
function _content_lock_release_all_user_locks($uid) {
  db_delete('content_lock')
    ->condition('uid', $uid)
    ->execute();
}

/**
 * Save lock warning.
 *
 * @param string $message
 *    Message string.
 * @param int $nid
 *    Node id.
 */
function _content_lock_save_lock_warning($message, $nid) {
  if (empty($_SESSION['content_lock'])) {
    $_SESSION['content_lock'] = '';
  }
  $data = unserialize($_SESSION['content_lock']);
  if (!is_array($data)) {
    $data = array();
  }

  if (array_key_exists($nid, $data)) {
    return;
  }

  $data[$nid] = $message;
  $_SESSION['content_lock'] = serialize($data);
}

/**
 * Show warnings.
 */
function _content_lock_show_warnings() {
  global $user;
  if (empty($_SESSION['content_lock'])) {
    return;
  }
  $data = unserialize($_SESSION['content_lock']);
  if (!is_array($data) || count($data) == 0) {
    return;
  }
  foreach ($data as $nid => $messsage) {
    if (_content_lock_still_locked($user->uid, $nid)) {
      drupal_set_message($messsage, 'warning', FALSE);
    }
  }
  $_SESSION['content_lock'] = '';
}

/**
 * Warn pending locks.
 *
 * For every lock a user current have on any nodes, print a warning message
 * with an link to release this node.
 *
 * @param int $uid
 *    User ID.
 */
function _content_lock_warn_pending_locks($uid) {
  // Cache.
  static $warned_nodes = array();
  static $content_lock_messages_printed = FALSE;
  if ($content_lock_messages_printed) {
    return;
  }

  if (array_key_exists($uid, $warned_nodes)) {
    // Placeholder.
  }
  else {
    // Load form db.
    $warned_nodes[$uid] = array();
    $query = db_select('content_lock', 'c')
      ->fields('c', array('nid'))
      ->condition('c.uid', $uid);
    $n = $query->leftJoin('node', 'n', '%alias.nid = c.nid');
    $query->fields($n, array('title'));

    foreach ($query->execute() as $lock) {
      $warned_nodes[$uid][] = $lock;
    }
  }

  // Load module inc file.
  module_load_include('inc', 'content_lock', 'includes/content_lock.func');

  foreach ($warned_nodes[$uid] as $lock) {
    $nodetitle_link = l($lock->title, "node/{$lock->nid}");
    $token = content_lock_get_release_token($lock->nid);
    $releasethelock_link = l(t('release the lock'), "admin/content/{$lock->nid}/content_lock/releaseown", array('query' => array('token' => $token)));
    _content_lock_save_lock_warning(t("The node '!nodetitle_link' is locked by you. You may want to '!releasethelock_link' in order to allow others to edit.", array(
      '!nodetitle_link' => $nodetitle_link,
      '!releasethelock_link' => $releasethelock_link,
    )), $lock->nid);
  }
  $content_lock_messages_printed = TRUE;
}
