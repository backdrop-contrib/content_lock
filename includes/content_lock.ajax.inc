<?php

/**
 * @file
 * content_lock.ajax.inc
 */

/**
 * AJAX callback to lock a node manually.
 *
 * @param object $node
 *   The node to lock.
 * @param string $token
 *   The CSRF token.
 */
function content_lock_node_ajax_callback($node, $token) {
  global $user;

  // Load module inc file.
  module_load_include('inc', 'content_lock', 'includes/content_lock.func');

  // Only lock the node if we have a valid CSRF token.
  if (drupal_valid_token($token, $node->nid)) {
    content_lock_node($node->nid, $user->uid);

    // Add the javascript that unlocks the node when the user navigates away
    // from the page.
    $form = array('nid' => array('#value' => $node->nid));
    _content_lock_add_unload_js($form, array());
  }
  else {
    drupal_set_message(t('The content could not be locked.'));
  }
  $commands = array();
  $commands[] = ajax_command_remove('div.messages');
  $commands[] = ajax_command_before('#block-system-main', theme('status_messages'));
  ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
}
