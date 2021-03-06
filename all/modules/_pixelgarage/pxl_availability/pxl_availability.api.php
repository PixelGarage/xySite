<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 27.01.17
 * Time: 11:22
 */

/**
 * Hook to alter the contextual filter for the specified calendar view (context).
 * The returned contextual filter has to be a valid product id or NULL to prevent filtering.
 *
 * @param $filter  array
 *    The views contextual filter array containing valid product id's as contextual filter.
 * @param $context array
 *    Associative array with the following keys:
 *      - block_id: the id of the availability block.
 *      - view_name: name of view
 *      - view_display_id:  the display id of view
 */
function hook_pxl_availability_filter_by_product_alter(&$filter, $context) {
  if ($context['view_name'] == 'calendar_view' || $context['view_display_id'] == 'block_availability') {
    $filter = array(null); // no contextual filter
  }
}
