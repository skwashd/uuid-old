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

  return system_settings_form($form);
}

/**
 * Prepares a list of UUID generators for select #options in settings form.
 *
 * @return 
 *   List of available UUID generators.
 */
function _uuid_admin_generators() {
  $options = array();
  $implementations = uuid_info();
  foreach ($implementations as $name => $data) {
    $options[$name] = $data['title'];
  }

  return $options;
}
