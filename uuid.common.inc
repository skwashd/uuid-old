<?php
/**
 * Return schema for a uuid table.
 *
 * @param $key
 *   Name of key field, e.g. nid for nodes.
 *
 * @return
 *   Array with table structure definition (schema).
 */
function uuid_table_schema($table, $key = 'key') {
  return array(
    'fields' => array(
      $key => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0,
        'description' => 'The primary key of the record which this UUID was generated for.',
      ),
      'uuid' => array(
        'type' => 'char',
        'length' => 36,
        'not null' => TRUE,
        'default' => '',
        'description' => 'The Universally Unique Identifier.',
      ),
    ),
    'primary key' => array($key),
    'unique keys' => array(
      'uuid_'. $table .'_uuid_key' => array('uuid'),
    ),
  );
}

/**
 * Determines if a UUID is valid.
 */
function uuid_is_valid($uuid) {
  return (boolean) preg_match('/^[A-Fa-f0-9]{8}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{12}$/', $uuid);
}

/**
 * Generate and return an Universally Unique IDentifier (UUID).
 *
 * @see http://www.ietf.org/rfc/rfc4122.txt
 * @see http://php.net/manual/en/function.uniqid.php#65879
 *
 * @return
 *   An UUID, made up of 32 hex digits and 4 hyphens.
 */
function uuid_uuid() {
  // If we're not using postgres, the database will give us a UUID.
  if (Database::getConnection()->databaseType() == 'mysql') {
    return db_result(db_query('SELECT UUID()'));
  }

  // If the function provided by the uuid pecl extension is available, use that.
  if(function_exists('uuid_create')) {
    return uuid_create(UUID_TYPE_DEFAULT);
  }

  // Fall back to generating a Universally Unique IDentifier version 4.
  // Base on http://php.net/manual/en/function.uniqid.php#65879
  // The field names refer to RFC 4122 section 4.1.2.
  return sprintf('%04x%04x-%04x-%03x4-%04x-%04x%04x%04x',
    // 32 bits for "time_low".
    mt_rand(0, 65535), mt_rand(0, 65535),
    // 16 bits for "time_mid".
    mt_rand(0, 65535),
    // 12 bits before the 0100 of (version) 4 for "time_hi_and_version".
    mt_rand(0, 4095),
    bindec(substr_replace(sprintf('%016b', mt_rand(0, 65535)), '01', 6, 2)),
    // 8 bits, the last two of which (positions 6 and 7) are 01, for "clk_seq_hi_res"
    // (hence, the 2nd hex digit after the 3rd hyphen can only be 1, 5, 9 or d)
    // 8 bits for "clk_seq_low" 48 bits for "node".
    mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(0, 65535)
  );
}

/**
 * Token integration. UUID module provides the following tokens to be used on
 * the node, user, comment and taxonomy contexts.
 *   - Global:
 *     - uuid: an arbitrary uuid generated on demand not linked with any object.
 *   - Node:
 *     - uuid: the node's uuid.
 *     - revision-uuid: the node revision's uuid
 *     - author-uuid: the node author's uuid
 *   - User:
 *     - uuid: the user's uuid.
 *
 * @todo Still to complete taxonomy and comment tokens.
 */

/**
 * Implementation of hook_token_values().
 */
function uuid_token_values($type, $object = NULL, $options = array()) {
  $tokens = array();

  switch ($type) {
    case 'global':
      $tokens['uuid'] = uuid_uuid();
      break;
    case 'user':
      $tokens['uuid'] = isset($object->uuid) ? $object->uuid : NULL;
      break;
    case 'node':
      $tokens['uuid'] = isset($object->uuid) ? $object->uuid : NULL;
      $tokens['revision-uuid'] = isset($object->revision_uuid) ? $object->revision_uuid : NULL;
      $tokens['author-uuid'] = db_result(db_query("SELECT uuid FROM {uuid_users} WHERE uid = %d", $object->uid));
      break;
  }

  return $tokens;
}

/**
 * Implementation of hook_token_list().
 */
function uuid_token_list($type = 'all') {
  $tokens = array();

  if ($type == 'global' || $type == 'all') {
    $tokens['global']['uuid'] = t("An arbitrary UUID value");
  }
  if ($type == 'node' || $type == 'all') {
    $tokens['node']['uuid'] = t("The node UUID value");
    $tokens['node']['revision_uuid'] = t("The node revision UUID value");
    $tokens['node']['author-uuid'] = t("The node author UUID value");
  }
  if ($type == 'user' || $type == 'all') {
    $tokens['user']['uuid'] = t("The user UUID value");
  }

  return $tokens;
}
