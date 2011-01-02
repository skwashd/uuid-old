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

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
  );

  $form['settings']['uuid_generator'] = array(
    '#type' => 'select',
    '#title' => t('UUID Generator'),
    '#default_value' => variable_get('uuid_generator', array()),
    '#description' => t('The method used to generate the UUID'),
    '#options' => _uuid_admin_generators()
  );

  $entities = uuid_list_fieldable_entities();
  foreach ($entities as $entity => $data) {
    $translations = array('@entity' => $data['label']);
    $variable = "uuid_automatic_for_{$entity}";
    $title = format_plural(count($data['bundles']),
      '@entity type with automatic UUID generation',
      '@entity types with automatic UUID generation',
      $translations);

    $form[$entity] = array(
      '#type' => 'fieldset',
      '#title' => t('@entity settings', $translations),
    );

    $form[$entity][$variable] = array(
      '#type' => 'checkboxes',
      '#title' => $title,
      '#default_value' => variable_get($variable, array()),
      '#options' => $data['bundles'],
      '#description' => t('@entity of these types will have UUIDs automatically generated.', $translations),
      '#required' => FALSE,
    );
  }

  $form['synchronization'] = array(
    '#type' => 'fieldset',
    '#title' => t('Synchronization'),
  );

  $form['synchronization']['sync'] = array(
    '#type' => 'submit',
    '#value' => t('Create missing UUIDs'),
    '#submit' => array('uuid_sync'),
    '#weight' => 10,
  );

  $form = system_settings_form($form);

  $form['#submit'][] = 'uuid_settings_form_submit';

  return $form;
}

/**
 * Implements hook_settings_form_submit().
 */
function uuid_settings_form_submit($form, &$form_state) {
  //field_delete_field('uuid');

  // Handle fields
  $values = $form_state['values'];
  $entities = entity_get_info();
  foreach ($entities as $entity => $data) {
    // we only care about fieldable entities
    if (!$data['fieldable']) { 
      continue;
    }
    foreach ($data['bundles'] as $bundle => $info) {
      $fields = field_info_instances($entity, $bundle);
      if (isset($fields['uuid'])) {
         if (!_uuid_request_enabled($values, $entity, $bundle)) {
           $message = 'Disabled UUID field for @bundle.';
           if (!uuid_disable_field($entity, $bundle)) {
             $message = 'Failed to disable UUID field for @bundle.';
           }
           drupal_set_message(t($message, array('@bundle' => $bundle)));
         }
      }
      else {
         if (_uuid_request_enabled($values, $entity, $bundle)) {
           $message = 'Enabled UUID field for @bundle.';
           if (!uuid_enable_field($entity, $bundle)) {
             $message = 'Failed to enable UUID field for @bundle.';
           }
           drupal_set_message(t($message, array('@bundle' => $bundle)));
         }
      }
    }
  }  
}

/**
 * Check if UUID should be enabled for specified entity bundle.
 *
 * @param Array $values The form values.
 * @param String $entity The name of the entity to check.
 * @param String $bundle The name of the bundle to check.
 *
 * @return bool Should UUID be enabled?
 */
function _uuid_request_enabled($values, $entity, $bundle) {
  return !empty($values["uuid_automatic_for_{$entity}"][$bundle]);
}

/**
 * Prepare a list of UUID generators for select #options in settings form.
 *
 * @returns Array List of available UUID generators.
 */
function _uuid_admin_generators() {
  $options = array();
  $implementations = uuid_info();
  foreach ($implementations as $name => $data) {
    $options[$name] = $data['title'];
  }

  return $options;
}
