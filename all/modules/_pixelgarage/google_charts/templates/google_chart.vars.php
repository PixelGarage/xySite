<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 29.09.17
 * Time: 18:55
 */
function template_preprocess_google_chart(&$vars) {
  // define chart options
  $chart_settings = $vars['chart_settings'];
  $chart_type = $chart_settings['chart_type'];
  $switch_column_row = $chart_settings['chart_switch_column_row'];
  $has_header_row = $chart_settings['chart_has_header_row'];
  $show_diff_chart = $chart_settings['chart_show_diff'] ?
    ($chart_type == 'PieChart' || $chart_type == 'BarChart' || $chart_type == 'ColumnChart' || $chart_type == 'Scatter') : false;
  $chart_options = _google_chart_prepare_chart_options($vars['link_id'], $chart_settings);

  // define link classes and attributes
  $classes = 'chart ' . strtolower($chart_type);
  $classes .= $chart_options['is3D'] ? ' chart_3d' : '';
  $vars['link_classes'] = $classes;

  $width = $chart_options['width'];
  $height = $chart_options['height'];
  $vars['link_attributes'] = "style='width:{$width}; height:{$height};'";

  // add google chart js settings
  $js_settings = array(
    $vars['link_id'] => array(
      'chart_type' => $chart_type,
      'chart_data_source' => $vars['chart_data_source'],
      'chart_switch_column_row' => $switch_column_row,
      'chart_has_header_row' => $has_header_row,
      'chart_show_diff' => $show_diff_chart,
      'chart_options' => $chart_options,
    ),
  );
  _google_chart_add_js_settings($js_settings);
}

/**
 * Add js files and settings for the specific chart.
 */
function _google_chart_add_js_settings($link_settings) {
  $path = &drupal_static(__FUNCTION__);
  if (!isset($path)) {
    // add the Stripe and the stripe_button javascript
    $path = drupal_get_path('module', 'google_charts');
    drupal_add_js(GOOGLE_CHARTS_LIBRARY_PATH, 'external');
    drupal_add_js($path . '/google_charts.js');

    // add google charts js settings (Drupal settings are merged for multiple links)
    $js_settings = array (
      'google_charts' => array(),
    );
  }
  $js_settings['google_charts'] = $link_settings;
  drupal_add_js($js_settings, 'setting');
}

/**
 * Prepare the chart options defining the look and feel of the chart.
 *
 * @return array An array of all options defining the chart type
 */
function _google_chart_prepare_chart_options($link_id, $settings) {
  $chart_type = $settings['chart_type'];
  $style_axis = $settings['chart_style_axis'];
  $show_legend = $settings['chart_show_legend'];
  $show_trendline = $settings['chart_show_trendline'];
  $show_crosshair = $settings['chart_show_crosshair'];
  $chart_width = $settings['chart_width'];
  $chart_height = $settings['chart_height'];
  $font_name = $settings['chart_font_name'];
  $font_size = intval($settings['chart_font_size']);
  $is_3D = $settings['chart_is_3d'];
  $do_animate = $settings['chart_do_animate'];

  $chart_options = array(
    'title' => '',
    'titlePosition' => 'out',
    'titleTextStyle' => array(
      'color' => 'black',
      'fontSize' => $font_size + 2,
      'bold' => false,
      'italic' => false
    ),
    'width' => $chart_width,
    'height' => $chart_height,
    'fontName' => $font_name,
    'fontSize' => $font_size,
    'is3D' => $is_3D,
    'dataOpacity' => 1.0,
    'focusTarget' => 'datum',
    'enableInteractivity' => true,
    'backgroundColor' => array(
      'stroke' => '#666',
      'strokeWidth' => 0,
      'fill' => 'white'
    ),
    'chartArea' => array(
      'top' => 'auto',
      'left' => 'auto',
      'width' => 'auto',
      'height' => 'auto',
      'backgroundColor' => 'white'
    ),
    'legend' => array(
      'position' => $show_legend ? 'right' : 'none',
      'alignment' => 'center',
      'textStyle' => array(
        'color' => 'black',
        'bold' => false,
        'italic' => false
      )
    ),
    'tooltip' => array(
      'ignoreBounds' => false,
      'isHtml' => false,
      'textStyle' => array(
        'color' => 'black',
        'bold' => false,
        'italic' => false
      ),
      'trigger' => 'focus'
    )
  );

  // add animation options
  if ($do_animate) {
    $chart_options['animation'] = array(
      'duration' => 1000,
      'easing' => 'linear',
      'startup' => false,
    );
  }

  // type specific settings

  switch ($chart_type) {
    case 'AreaChart':
      $chart_options = mergeAreaChartOptions($chart_options, $style_axis, $show_crosshair);
      break;
    case 'BarChart':
    default:
      $chart_options = mergeBarChartOptions($chart_options, $style_axis, $show_trendline);
      break;
    case 'BubbleChart':
      $chart_options = mergeBubbleChartOptions($chart_options);
      break;
    case 'Calendar':
      //$chart_options = mergeCalendarOptions($chart_options);
      break;
    case 'CandlestickChart':
      //$chart_options = mergeCandlestickChartOptions($chart_options);
      break;
    case 'ColumnChart':
      $chart_options = mergeColumnChartOptions($chart_options, $style_axis, $show_trendline);
      break;
    case 'Gantt':
      //$chart_options = mergeGanttOptions($chart_options);
      break;
    case 'Gauge':
      //$chart_options = mergeGaugeOptions($chart_options);
      break;
    case 'GeoChart':
      //$chart_options = mergeGeoChartOptions($chart_options);
      break;
    case 'Histogram':
      //$chart_options = mergeHistogramOptions($chart_options);
      break;
    case 'LineChart':
      //$chart_options = mergeLineChartOptions($chart_options);
      break;
    case 'Map':
      //$chart_options = mergeMapOptions($chart_options);
      break;
    case 'OrgChart':
      //$chart_options = mergeOrgChartOptions($chart_options);
      break;
    case 'PieChart':
      $chart_options = mergePieChartOptions($chart_options);
      break;
    case 'Sankey':
      //$chart_options = mergeSankeyOptions($chart_options);
      break;
    case 'Scatter':
      //$chart_options = mergeScatterOptions($chart_options);
      break;
    case 'SteppedAreaChart':
      //$chart_options = mergeSteppedAreaChartOptions($chart_options);
      break;
    case 'Table':
      //$chart_options = mergeTableOptions($chart_options);
      break;
    case 'Timeline':
      //$chart_options = mergeTimelineOptions($chart_options);
      break;
    case 'TreeMap':
      //$chart_options = mergeTreeMapOptions($chart_options);
      break;
    case 'WordTree':
      //$chart_options = mergeWordTreeOptions($chart_options);
      break;
  }

  //
  // let other users alter the chart options
  drupal_alter('chart_options', $chart_options, $link_id);

  return $chart_options;
}

function _get_axis_options($title) {
  return array(
    'title' => $title,
    'titleTextStyle' => array(
      'color' => 'black',
      'bold' => false,
      'italic' => false
    ),
    'baselineColor' => 'black',
    'direction' => 1,
    'format' => 'auto',
    'gridlines' => array(
      'color' => '#ccc',
      'count' => 5,
    ),
    'minorGridlines' => null,
    'logScale' => false,
    'textStyle' => array(
      'color' => 'black',
      'bold' => false,
      'italic' => false
    ),
    'textPosition' => 'out',
    'ticks' => 'auto',
    'viewWindowMode' => 'pretty',
    'viewWindow' => array(
      'min' => 'auto',
      'max'=> 'auto'
    ),
  );
}

function _get_trendline_options() {
  return array(
    'type' => 'linear',
    'degree' => 3,
    'color' => 'blue',
    'lineWidth' => 2,
    'opacity' => 1.0,
    'labelInLegend' => 'Trend line',
    'visibleInLegend' => false,
    'showR2' => false,
  );
}

function _get_crosshair_options() {
  return array(
    'color' => 'blue',
    'focused' => array(
      'color' => 'darkblue',
      'opacity' => 0.8
    ),
    'opacity' => 1.0,
    'orientation' => 'both',
    'selected' => array(
      'color' => 'lightblue',
      'opacity' => 1.0
    ),
    'trigger' => 'both'
  );
}


function mergeAreaChartOptions($chart_options, $style_axis, $show_crosshair) {
  $options = array(
    'areaOpacity' => 0.8,
    'interpolateNulls' => false,
    'isStacked' => false,
    'orientation' => false,
    'lineDashStyle' => null,
    'lineWidth' => 2,
    'pointShape' => 'circle',
    'pointSize' => 0,
    'pointsVisible' => true,
    'reverseCategories' => false,
    'axisTitlesPosition' => 'out',
    'selectionMode' => 'single',
    'series' => array()
  );
  // add axis options
  if ($style_axis) {
    $options['hAxis'] = _get_axis_options('x-Axis');
    $options['vAxis'] = _get_axis_options('y-Axis');
  }

  if ($show_crosshair) {
    $options['crosshair'] = _get_crosshair_options();
  }

  return array_merge($chart_options, $options);
}

function mergeBarChartOptions($chart_options, $style_axis, $show_trendline) {
  $options = array(
    'isStacked' => false,
    'reverseCategories' => false,
    'orientation' => false,
    'bar' => array(
      'groupWidth' => '61.8%'
    ),
    'axisTitlesPosition' => 'out'
  );

  // add axis options
  if ($style_axis) {
    $options['hAxis'] = _get_axis_options('x-Axis');
    $options['vAxis'] = _get_axis_options('y-Axis');
  }

  // add trendline options
  if ($show_trendline) {
    $options['trendlines'] = array(
      _get_trendline_options()
    );
  }

  return array_merge($chart_options, $options);
}

function mergeBubbleChartOptions($chart_options) {
  $options = array(
    'axisTitlesPosition' => 'out',
    'bubble' => array(
      'opacity' => 0.8,
      'stroke' => '#ccc',
      'textStyle' => array(
        'color' => 'black',
        'bold' => false,
        'italic' => false
      )
    ),
    'sizeAxis' => array(
      'maxSize' => 30,
      'minSize' => 5
    ),
    'sortBubblesBySize' => true,
    'selectionMode' => 'single'
  );
  return array_merge($chart_options, $options);
}

function mergeColumnChartOptions($chart_options, $style_axis, $show_trendline) {
  $options = array(
    'isStacked' => true,
    'reverseCategories' => false,
    'orientation' => false,
    'bar' => array(
      'groupWidth' => '61.8%'
    ),
    'axisTitlesPosition' => 'out'
  );

  // add axis options
  if ($style_axis) {
    $options['hAxis'] = _get_axis_options('x-Axis');
    $options['vAxis'] = _get_axis_options('y-Axis');
  }

  // add trendline options
  if ($show_trendline) {
    $options['trendlines'] = array(
      _get_trendline_options()
    );
  }

  return array_merge($chart_options, $options);
}

function mergePieChartOptions($chart_options) {
  $options = array(
    'pieHole' => 0,
    'pieSliceBorderColor' => 'white',
    'pieSliceText' => 'percentage',
    'pieSliceTextStyle' => array(
      'color' => 'black',
      'bold' => false,
      'italic' => false
    ),
    'pieStartAngle' => 0,
    'reverseCategories' => false,
    'pieResidueSliceColor' => '#ccc',
    'pieResidueSliceLabel' => 'other',
    'sliceVisibilityThreshold' => 0.0014,
    'slices' => array()
  );
  return array_merge($chart_options, $options);
}
