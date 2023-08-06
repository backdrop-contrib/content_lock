/**
 * @file
 * Core behavior for Content locking module.
 */

(function ($) {
  'use strict';
  Backdrop.behaviors.content_lock = {
    attach: function (context) {
      window.content_lock_onleave = function () {
        var nid = Backdrop.settings.content_lock.nid;
        var ajax_key = Backdrop.settings.content_lock.ajax_key;
        jQuery.ajax({
          url: Backdrop.settings.basePath + 'ajax/content_lock/' + nid + '/canceledit',
          data: {k: ajax_key, token: Backdrop.settings.content_lock.token},
          async: false,
          cache: false
        });
      };

      window.content_lock_confirm = function () {
        if (Backdrop.settings.content_lock.unload_js_message_enable) {
          return Backdrop.t(Backdrop.settings.content_lock.unload_js_message);
        }

      };

      // Prevent submitting the node form from being interpreted as "leaving the page".
      jQuery(Backdrop.settings.content_lock.internal_forms).submit(function () {
        userMovingWithinSite();
      });

      jQuery().onUserExit({
        execute: content_lock_onleave,
        executeConfirm: content_lock_confirm,
        internalURLs: Backdrop.settings.content_lock.internal_urls
      });
    }
  };

}(jQuery));
