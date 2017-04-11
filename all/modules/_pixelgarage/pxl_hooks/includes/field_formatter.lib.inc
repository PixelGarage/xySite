<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 07.04.17
 * Time: 19:29
 */

/* --------------------------------------------------
 * Themes and field formatters for date-time ranges
 * --------------------------------------------------*/
/**
 * Implements hook_field_formatter_info().
 */
function pxl_hooks_field_formatter_info() {
  return array(
    // the key must be unique, so it's best to prefix with your module's name.
    'pxl_hooks_date_von_bis' => array(
      // the label is is what is displayed in the select box in the UI.
      'label' => t('Date Von Bis'),
      // field types is the important bit!! List the field types your formatter is for.
      'field types' => array('datetime'),
      // You can usually leave 'multiple values' as follows:
      'multiple values' => FIELD_BEHAVIOR_DEFAULT,
    ),
    'pxl_hooks_date_start_ende' => array(
      // the label is is what is displayed in the select box in the UI.
      'label' => t('Date Start Ende'),
      // field types is the important bit!! List the field types your formatter is for.
      'field types' => array('datetime'),
      // You can usually leave 'multiple values' as follows:
      'multiple values' => FIELD_BEHAVIOR_DEFAULT,
    ),
  );
}
/**
 * Implements hook_field_formatter_view().
 */
function pxl_hooks_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $elements = array();
  foreach ($items as $delta => $item) {
    $elements[$delta] = array(
      '#markup' => theme($display['type'] . '_formatter', array('element' => $item, 'field' => $instance)),
    );
  }
  return $elements;
}

function theme_pxl_hooks_date_von_bis_formatter($element) {
  $from_date = strtotime($element['element']['value']);
  $date_start = explode(' ', $element['element']['value']);
  $date_end = explode(' ', $element['element']['value2']);
  $to_date = strtotime($element['element']['value2']);
  $has_end_date = $from_date != $to_date && !empty($date_end[0]);

  if ($has_end_date) {
    $string = '<strong>' . t('Von') . '</strong> ';
  } else {
    $string = '<strong>' . t('Am') . '</strong> ';
  }
  if (date('H:i', $from_date) == '00:00') {
    $string .= date('d.m.Y', $from_date);
  } else {
    $string .= date('d.m.Y, H:i', $from_date);
  }
  if ($has_end_date) {
    $string .= ' <strong>' . t('bis') . '</strong> ' . date('d.m.Y', strtotime($element['element']['value2']));
  }
  return $string;
}

function theme_pxl_hooks_date_start_ende_formatter($element) {
  $from_date = strtotime($element['element']['value']);
  $string = t('Start') . ': ';
  if (date('H:i', $from_date) == '00:00') {
    $string .= date('d.m.Y', $from_date);
  } else {
    $string .= date('d.m.Y, H:i', $from_date);
  }
  if ($element['element']['value2']) {
    $end_date = strtotime($element['element']['value2']);
    if ($from_date != $end_date) {
      $string .= '<br />' . t('Ende') . ': ' . date('d.m.Y', $end_date);
    }
  }
  return $string;
}


