Content Lock
======================

The purpose of this module is to avoid the situation where two people
are editing a single node at the same time. This module implements a
pessimistic locking strategy, which means that content will be
exclusively locked whenever a user starts editing it.

The lock will be automatically released when the user
submits the form or navigates away from the edit page. Content locks
that have been "forgotten" can be automatically releasedafter a configurable
time span using the bundled content_lock_timeout sub module.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://docs.backdropcms.org/documentation/extend-with-modules.

- Visit the configuration page under Administration > Configuration > Content >
  Content lock (admin/config/content/content_lock) and enter the required information.

Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/foo-project/issues).

Current Maintainers <!-- This section is required. -->
-------------------

- [Robert Garrigós](https://github.com/robertgarrigos).

Credits
-------

- Ported to Backdrop CMS by [Robert Garrigós](https://github.com/robertgarrigos).
- Originally written for Drupal by [EugenMayer](https://www.drupal.org/u/eugenmayer).


License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
