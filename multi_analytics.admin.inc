<?php

function multi_analytics_admin_list() {
  $headers = array(
    'ua_code' => t('Google UA Code'),
    'actions' => t('Actions'),
  );
  $rows = array();
  $rs = db_query('SELECT * FROM {multi_analytics_uas} ORDER BY ua_code');
  foreach ($rs as $row) {
    $rows[] = array(
      'ua_code' => $row->ua_code,
      'actions' => implode(' | ', array(
        l('edit', 'admin/config/system/multianalytics/' . $row->ua_code),
        l('delete', 'admin/config/system/multianalytics/' . $row->ua_code . '/delete'),
      )),
    );
  }
  return theme('table', array(
    'header' => $headers,
    'rows' => $rows,
    'empty' => t('No UA codes registered'),
  ));
}

function multi_analytics_admin_settings_form($form, &$form_state, $ua_object = NULL) {
  module_load_include('inc', 'googleanalytics', 'googleanalytics.admin');
  $form = googleanalytics_admin_settings_form($form_state);
  if (!empty($ua_object)) {
    $ua_object = (array) $ua_object;
    $form = multi_analytics_admin_update_form($form, $ua_object);
  }
  $form['#submit'] = array('multi_analytics_admin_settings_form_submit');
  return $form;
}

function multi_analytics_admin_update_form($form, $ua_object) {
  foreach ($form as $key => $value) {
    if (substr($key, 0, 1) !== '#') {
      if (isset($ua_object['ua_values'][$key])) {
        $form[$key]['#default_value'] = $ua_object['ua_values'][$key];
      }
      $form[$key] = multi_analytics_admin_update_form($form[$key], $ua_object);
    }
  }
  return $form;
}

function multi_analytics_admin_settings_form_submit($form, &$form_state) {
  $record = array();
  $record['ua_code'] = $form_state['values']['googleanalytics_account'];
  $record['ua_values'] = serialize($form_state['values']);
  db_merge('multi_analytics_uas')
  ->fields($record)
  ->key(array('ua_code' => $record['ua_code']))
  ->execute();
  drupal_set_message(t('Profile saved'));
  $form_state['redirect'] = 'admin/config/system/multianalytics';
}


function multi_analytics_delete_settings_form($form, &$form_state, $ua_object) {
  $form['ua_object'] = array(
    '#type' => 'value',
    '#value' => $ua_object,
  );
  return confirm_form(
      $form,
      t('Are you sure you want to delete this GA tracking code?'),
      'admin/config/system/multianalytics');
}

function multi_analytics_delete_settings_form_submit($form, &$form_state) {
  db_delete('multi_analytics_uas')
      ->condition('ua_code', $form_state['values']['ua_object']['ua_code'])
      ->execute();
  drupal_set_message(t('Profile deleted'));
  $form_state['redirect'] = 'admin/config/system/multianalytics';
}