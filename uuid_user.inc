<?php
// $Id: uuid_user.inc,v 1.1 2008/10/21 16:37:06 recidive Exp $

/**
 * Implementation of hook_user().
 */
function uuid_user($op, &$edit, &$user, $category = NULL) {
  switch ($op) {
    case 'load':
      $user->uuid = db_result(db_query('SELECT uuid FROM {uuid_users} WHERE uid = %d', $user->uid));
    case 'insert':
      db_query('INSERT INTO {uuid_users} (uid, uuid) VALUES (%d, UUID())', $user->uid);
      break;
    case 'delete':
      db_query('DELETE FROM {uuid_users} WHERE uid = %d', $node->uid);
      break;
  }
}
