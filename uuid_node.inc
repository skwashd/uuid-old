<?php
// $Id: uuid_node.inc,v 1.1 2008/10/21 16:37:06 recidive Exp $

/**
 * Implementation of hook_nodeapi().
 */
function uuid_nodeapi(&$node, $op, $teaser, $page) {
  switch ($op) {
    case 'load':
      return db_fetch_array(db_query('SELECT uuid FROM {uuid_node} WHERE nid = %d', $node->nid));
    case 'insert':
      db_query('INSERT INTO {uuid_node} (nid, uuid) VALUES (%d, UUID())', $node->nid);
      break;
    case 'delete':
      db_query('DELETE FROM {uuid_node} WHERE nid = %d', $node->nid);
      break;
  }
}
