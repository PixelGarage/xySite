<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 11.10.17
 * Time: 15:39
 */

/**
 * Allows to alter the given chart options, before the chart is drawn.
 *
 * @param $options    array     The associated array of all chart options to be altered.
 * @param $field_id   string    The field id of the chart to be displayed.
 */
function hook_chart_options_alter(&$options, $field_id) {
  // alter or enhance the given drawing options for the chart
  $options['width'] = 500;

}
