<?php
// $Id: uuid.admin.inc,v 1.3 2010/03/19 07:18:18 recidive Exp $

/**
 * Menu callback: options for UUID.
 */
function uuid_admin() {
  $form = array();

  $form['content'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content UUID Settings'),
  );
  $types = node_get_types('names');
  $form['content']['uuid_automatic_for_nodes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content Types With Automatic UUIDs'),
    '#default_value' => variable_get('uuid_automatic_for_nodes', array()),
    '#options' => $types,
    '#description' => t('Selected content types to have UUIDs automatically generated.'),
    '#required' => FALSE,
  );

  $form['user'] = array(
    '#type' => 'fieldset',
    '#title' => t('User UUID Settings'),
  );
  $form['user']['uuid_automatic_for_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatic UUIDs For Users?'),
    '#default_value' => variable_get('uuid_automatic_for_users', FALSE),
    '#description' => t('Should UUIDs be created automatically for every existing, and new user?'),
    '#required' => FALSE,
  );
  
  $form['taxonomy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy UUID Settings'),
  );
  
  if ($options = array_map(create_function('$voc', 'return $voc->name;'), taxonomy_get_vocabularies())) {
    $form['taxonomy']['uuid_automatic_for_taxonomy'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Automatic UUIDs for taxonomy'),
      '#default_value' => variable_get('uuid_automatic_for_taxonomy', array()),
      '#options' => $options,
      '#description' => t('Selected vocabularies to have UUIDs automatically generated.'),
      '#required' => FALSE,
    );
  }
  else {
    $form['taxonomy']['uuid_automatic_for_taxonomy'] = array(
      '#type' => 'item',
      '#value' => t('There is currently no vocabulary defined.'),
    );
  }

  // TODO: Is this "sync" button really necessary?  Handle this better.
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
  );
  $form['settings']['sync'] = array(
    '#type' => 'submit',
    '#value' => t('Create Missing UUIDs'),
    '#submit' => array('uuid_sync'),
    '#weight' => 10,
  );
  $form['settings']['rebuild_help'] = array(
    '#value' => '<div class="description">' . t("If UUID settings have changed recently some content or users may be missing their UUIDs.") . '</div>',
    '#weight' => 11,
  );

  return system_settings_form($form);
}

/**
 * Ensure all content and users have UUIDs, if they are supposed to.
 */
function uuid_sync() {
  if (variable_get('uuid_automatic_for_users', FALSE)) {
    db_query("INSERT INTO {uuid_users} SELECT u.uid, UUID() FROM {users} AS u WHERE NOT EXISTS (SELECT uid FROM {uuid_users} WHERE uid = u.uid)");
  }

  foreach (variable_get('uuid_automatic_for_nodes', array()) as $type) {
    db_query("INSERT INTO {uuid_node} SELECT n.nid, UUID() FROM {node} AS n WHERE n.type = '%s' AND NOT EXISTS (SELECT nid FROM {uuid_node} WHERE nid = n.nid)", $type);
    db_query("INSERT INTO {uuid_node_revisions} SELECT nr.vid, UUID() FROM {node_revisions} AS nr INNER JOIN {node} n ON nr.nid = n.nid WHERE n.type = '%s' AND NOT EXISTS (SELECT vid FROM {uuid_node_revisions} WHERE vid = nr.vid)", $type);
  }
  
  $vids = variable_get('uuid_automatic_for_taxonomy', array());
  if ($vids) {
    $placehoders = implode(', ', array_fill(0, count($vids), '%d'));
    // Vocabularies
    db_query("INSERT INTO {uuid_vocabulary} SELECT v.vid, UUID() FROM {vocabulary} AS v WHERE v.vid IN ($placehoders) AND NOT EXISTS (SELECT vid FROM {uuid_vocabulary} WHERE vid = v.vid)", $vids);
    // Terms
    db_query("INSERT INTO {uuid_term_data} SELECT td.tid, UUID() FROM {term_data} AS td WHERE td.vid IN ($placehoders) AND NOT EXISTS (SELECT tid FROM {uuid_term_data} WHERE tid = td.tid)", $vids);
  }

  drupal_set_message(t("UUID tables have been updated."));
}
