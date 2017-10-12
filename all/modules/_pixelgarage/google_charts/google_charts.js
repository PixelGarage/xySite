/**
 * This file contains behaviors for the google charts.
 * The goofle chart library has to be loaded in order to use the functionality in this file.
 *
 * Created by ralph on 10.10.2017.
 */

(function ($) {

  /**
   * Transpose a DataTable and returns it.
   * @param dt DataTable The table to be transposed.
   * @param hasHeaderRow bool
   *    True, if the first row is a header row
   * @returns {google.visualization.DataTable}
   */
  var transposeDT = function(dt, hasHeaderRow) {
    // set default parameters
    hasHeaderRow = (typeof hasHeaderRow !== 'undefined') ?  hasHeaderRow : false;

    var nColumn = dt.getNumberOfColumns(),
        nRows = dt.getNumberOfRows(),
        firstColIsStr = dt.getColumnType(0) === 'string',
        newDT = new google.visualization.DataTable;

    //
    // create first column of type string and fill with old column labels or header row
    var rLabel = hasHeaderRow ? dt.getValue(0,0).toString() : dt.getColumnLabel(0);
    newDT.addColumn ('string', rLabel);
    for (var i=1; i < nColumn; i++) {
      // create empty rows with the column/header labels at the first position
      rLabel = hasHeaderRow ? dt.getValue(0,i).toString() : dt.getColumnLabel(i);
      newDT.addRow ([rLabel]);
    }

    //
    // Transpose table without header row, if existing
    // If first column of dt is of type string, then use these values for the column labels of newDT
    var cLabel = 0,
        ciStart = firstColIsStr ? 1 : 0,
        riStart = hasHeaderRow ? 1 : 0;
    for (var ri=riStart; ri < nRows; ri++) {
      // create column with label of old first column (if string column) or row index
      cLabel = firstColIsStr ? dt.getValue(ri,0).toString() : ri.toString();
      newDT.addColumn ('number', cLabel);
      for (var ci=ciStart; ci < nColumn; ci++)
        newDT.setValue (ci-ciStart, ri-riStart+1, dt.getValue (ri,ci));
    }
    return newDT;
  };

  /**
   * Draws all google charts defined by link fields. Each link defines a data set for a chart.
   */
  Drupal.behaviors.drawGoogleCharts = {
    attach: function () {
      var drawCharts = function() {
        // Iterate through all defined google chart link fields and draw it with its settings
        $.each(Drupal.settings.google_charts, function (field_id, settings) {
          var query = new google.visualization.Query(settings['chart_data_source']),
              doTranspose = settings['chart_switch_column_row'],
              hasHeaderRow = settings['chart_has_header_row'],
              handleQueryResponse = function(response) {
                if (response.isError()) {
                  alert('Error in query: ' + response.getMessage() + ' ' + response.getDetailedMessage());
                  return;
                }

                // get data and switch column/row if requested
                var dataTable = response.getDataTable();

                if (doTranspose) {
                  dataTable = transposeDT(dataTable, hasHeaderRow);
                }
                else if (hasHeaderRow) {
                  // remove header row (no data row)
                  dataTable.removeRow(0);
                }

                var chartWrapper = new google.visualization.ChartWrapper({
                  'containerId': field_id,
                  'chartType': settings['chart_type'],
                  'dataTable': dataTable,
                  'options': settings['chart_options']
                });

                // draw chart
                chartWrapper.draw();
              };

          // send query
          query.send(handleQueryResponse);
        });
      };

      //
      // load core charts and assign callback
      google.charts.load('current', {packages: ['corechart']});
      google.charts.setOnLoadCallback(drawCharts);
    }
  }

})(jQuery);

