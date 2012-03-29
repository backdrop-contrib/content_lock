/** -*- indent-tabs-mode: nil; js-indent-level: 2; -*-
 * @file
 *   Initialize onUnload scripts.
 */

Drupal.behaviors.content_lock_admin = function() {
  var js_message_obj = jQuery('input[name="content_lock_unload_js_message"]');
  var js_message_enable_obj = jQuery('input[name="content_lock_unload_js_message_enable"]');

  function check_js_message_enable() {
    if (js_message_enable_obj.is(':checked')) {
      js_message_obj.removeAttr('disabled');
    } else {
      js_message_obj.attr('disabled', 'disabled');
    }
  }

  /* Do not lose the existing message when the message is disabled */
  js_message_obj.parents('form').submit(function() {
    js_message_obj.removeAttr('disabled');
  });

  js_message_enable_obj.click(check_js_message_enable);
  check_js_message_enable();
};
