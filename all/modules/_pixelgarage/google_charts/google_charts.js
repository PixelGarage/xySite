/**
 * This file contains behaviors for the google charts.
 *
 * Created by ralph on 10.10.2017.
 */

(function ($) {

  /**
   * Draws all google charts defined by link fields. Each link defines a data set for a chart.
   */
  Drupal.behaviors.drawGoogleCharts = {
    attach: function () {
      var drawCharts = function() {
        // Iterate through all defined google chart link fields and draw it with its settings
        $.each(Drupal.settings.google_charts, function (field_id, settings) {
          var chartWrapper = new google.visualization.ChartWrapper({
            'containerId': field_id,
            'chartType': settings['chart_type'],
            'dataSourceUrl': settings['chart_data_source'],
            'options': settings['chart_options']
          });

          // draw chart
          chartWrapper.draw();
        });
      };

      //
      // load core charts and assign callback
      google.charts.load('current', {packages: ['corechart']});
      google.charts.setOnLoadCallback(drawCharts);
    }
  }

})(jQuery);

