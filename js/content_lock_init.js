/**
 * @file
 *   Initialize onUnload scripts.
 */

Drupal.behaviors.content_lock = function() {
  window.content_lock_onleave = function  () {
    var nid = Drupal.settings.content_lock.nid;
    var ajax_key = Drupal.settings.content_lock.ajax_key;
    $.ajax({
      url: Drupal.settings.basePath + 'ajax/content_lock/'+nid+'/canceledit',
      data: {k: ajax_key, token: Drupal.settings.content_lock.token},
      async: false,
      cache: false
    });
  }

  window.content_lock_confirm = function () {
    return Drupal.t(Drupal.settings.content_lock.unload_js_message);
  }

  /* Prevent submitting the node form from being interpreted as "leaving the page" */
  $('form#node-form').submit(function () {
    userMovingWithinSite();
  });

  $(document).ready(function() {
    $().onUserExit( {
      execute: content_lock_onleave,
      executeConfirm: content_lock_confirm,
      internalURLs: 'node/' + Drupal.settings.content_lock.nid + '/edit'
    });
  });
};
