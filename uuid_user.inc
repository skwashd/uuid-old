<?php
// $Id: uuid_user.inc,v 1.2 2008/10/29 17:51:26 recidive Exp $

/**
 * Implementation of hook_user().
 */
function uuid_user($op, &$edit, &$user, $category = NULL) {
  switch ($op) {
    case 'load':
      $user->uuid = db_result(db_query('SELECT uuid FROM {uuid_users} WHERE uid = %d', $user->uid));
      break;
    case 'insert':
      db_query('INSERT INTO {uuid_users} (uid, uuid) VALUES (%d, UUID())', $user->uid);
      break;
    case 'delete':
      db_query('DELETE FROM {uuid_users} WHERE uid = %d', $node->uid);
      break;
  }
}
