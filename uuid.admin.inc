<?php
// $Id: uuid.admin.inc,v 1.1.2.9 2010/10/09 01:43:23 recidive Exp $

/**
 * @file
 * Administration functions for the uuid module.
 */

/**
 * Menu callback: options for UUID.
 */
function uuid_admin() {
  $form = array();

  $form['content'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content settings'),
  );
  $types = node_get_types('names');
  $form['content']['uuid_automatic_for_nodes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types with automatic UUID generation'),
    '#default_value' => variable_get('uuid_automatic_for_nodes', array()),
    '#options' => $types,
    '#description' => t('Content of these types will have UUIDs automatically generated.'),
    '#required' => FALSE,
  );

  $form['user'] = array(
    '#type' => 'fieldset',
    '#title' => t('User settings'),
  );
  $form['user']['uuid_automatic_for_users'] = array(
    '#type' => 'radios',
    '#title' => t('Automatic UUID generation for users'),
    '#default_value' => variable_get('uuid_automatic_for_users', FALSE),
    '#options' => array(
      TRUE => t('Enabled'),
      FALSE => t('Disabled'),
    ),
    '#description' => t('Should UUIDs be created automatically for users?'),
  );

  if (module_exists('taxonomy')) {
    $form['taxonomy'] = array(
      '#type' => 'fieldset',
      '#title' => t('Taxonomy settings'),
    );

    if ($options = array_map(create_function('$voc', 'return $voc->name;'), taxonomy_get_vocabularies())) {
      $form['taxonomy']['uuid_automatic_for_taxonomy'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Automatic UUID generation for taxonomy'),
        '#default_value' => variable_get('uuid_automatic_for_taxonomy', array()),
        '#options' => $options,
        '#description' => t("UUIDs will be created for the vocabulary and its terms."),
        '#required' => FALSE,
      );
    }
    else {
      $form['taxonomy']['uuid_automatic_for_taxonomy'] = array(
        '#type' => 'item',
        '#value' => t('There is currently no vocabulary defined.'),
      );
    }
  }

  $form['comment'] = array(
    '#type' => 'fieldset',
    '#title' => t('Comment settings'),
  );
  $form['comment']['uuid_automatic_for_comments'] = array(
    '#type' => 'radios',
    '#title' => t('Automatic UUID generation for comments'),
    '#default_value' => variable_get('uuid_automatic_for_comments', FALSE),
    '#options' => array(
      TRUE => t('Enabled'),
      FALSE => t('Disabled'),
    ),
    '#description' => t('Should UUIDs be created automatically for comments?'),
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Synchronization'),
  );
  $form['settings']['sync'] = array(
    '#type' => 'submit',
    '#value' => t('Create missing UUIDs'),
    '#submit' => array('uuid_sync'),
    '#weight' => 10,
  );

  return system_settings_form($form);
}

/**
 * Ensure all content and users have UUIDs, if they are supposed to.
 */
function uuid_sync() {

  module_invoke_all('uuid_sync');

  drupal_set_message(t("UUID tables have been updated."));
}
