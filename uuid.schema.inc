<?php

/**
 * @file
 * Installer helper functions for adding UUIDs to schemas.
 */

/**
 * Processes the new fields and indicies for a table.
 *
 * @param
 *   $schema The schema for the new fields.
 *
 *  @param
 *   $table The name of the database table to alter.
 *
 *  @param
 *    $return The return values from adding the fields.
 */
function uuid_add_field($schema, $table) {
  foreach ($schema[$table]['fields'] as $name => $field) {
    db_add_field($table, $name, $field);
  }

  foreach ($schema[$table]['indexes'] as $name => $fields) {
    db_add_index($table, $name, $fields);
  }
}

/**
 * Removes the UUID fields from the specified table.
 */
function uuid_drop_field($schema, $table) {
  foreach (array_keys($schema[$table]['indexes']) as $index) {
    db_drop_index($table, $index);
  }

  foreach (array_keys($schema[$table]['fields']) as $field) {
    db_drop_field($table, $field);
  }
}
