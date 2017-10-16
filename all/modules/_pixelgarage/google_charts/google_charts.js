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
      // create column with corresponding row labels, if any or alternatively with row index
      cLabel = firstColIsStr ? dt.getValue(ri,0).toString() : ri.toString();
      newDT.addColumn ('number', cLabel);
      for (var ci=ciStart; ci < nColumn; ci++)
        newDT.setValue (ci-ciStart, ri-riStart+1, dt.getValue (ri,ci));
    }
    return newDT;
  };

  /**
   * Gets the second data table for a difference chart.
   * The second data table is extracted from the input dt. The data corresponds to the i-th data column
   * in the data table (label column is not counted)
   *
   * @param dt     DataTable
   *    The data table for the difference chart.
   * @param i   int
   *    The index of the column (without label column), that has to be extracted for the second data table
   */
  var getSecondDT = function(dt, i) {
    var secondDT = dt.clone(),
        cStart = dt.getColumnType(0) === 'string' ? 1 : 0;

    // remove all columns before the i-th column
    if (i > 0) {
      secondDT.removeColumns(cStart, i);
    }

    return secondDT;
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
              showDiffChart = settings['chart_show_diff'],
              hasHeaderRow = settings['chart_has_header_row'],
              handleQueryResponse = function(response) {
                if (response.isError()) {
                  alert('Error in query: ' + response.getMessage() + ' ' + response.getDetailedMessage());
                  return;
                }

                //
                // get data and switch column/row if requested
                var dataTable = response.getDataTable();

                if (doTranspose) {
                  // transpose the data table
                  dataTable = transposeDT(dataTable, hasHeaderRow);
                }
                else if (hasHeaderRow) {
                  // set column labels from header row
                  var nColumn = dataTable.getNumberOfColumns(),
                      cStart = dataTable.getColumnType(0) === 'string' ? 1 : 0;
                  for (var i=cStart; i < nColumn; i++) {
                    dataTable.setColumnLabel(i, dataTable.getValue(0,i).toString());
                  }
                  // remove header row (no data row)
                  dataTable.removeRow(0);
                }

                //
                // show difference chart, if requested, otherwise draw chart type
                if (showDiffChart) {
                  var chart = null,
                      firstColIsStr = dataTable.getColumnType(0) === 'string',
                      nCol = dataTable.getNumberOfColumns(),
                      elem = document.getElementById(field_id),
                      slider_id = field_id + '-slider';

                  // add slider element
                  $(elem).after('<div id="' + slider_id + '"><div id="' + slider_id + '-handle" class="ui-slider-handle"></div></div>');

                  // get chart
                  switch (settings['chart_type']) {
                    case 'BarChart':
                      chart = new google.visualization.BarChart(elem);
                      break;
                    case 'ColumnChart':
                      chart = new google.visualization.ColumnChart(elem);
                      break;
                    case 'PieChart':
                    default:
                      chart = new google.visualization.PieChart(elem);
                      break;
                    case 'ScatterChart':
                      chart = new google.visualization.ScatterChart(elem);
                      break;
                  }

                  // define diff chart and draw it
                  var drawDiffChart = function(t) {
                    // Draw the diff chart.
                    // get second data table
                    var secondTable = getSecondDT(dataTable, t),
                        diffData = chart.computeDiff(dataTable, secondTable);

                    settings.chart_options.title = firstColIsStr ? dataTable.getColumnLabel(t+1) : dataTable.getColumnLabel(t);
                    chart.draw(diffData, settings.chart_options);
                  };

                  // initialize slider element
                  var handle = $( "#custom-handle" );
                  handle.text(0);
                  $('#' + slider_id).slider({
                    min: 0,
                    max: firstColIsStr ? nCol-2 : nCol-1,
                    slide: function(event, ui) {
                      var t = ui.value;
                      handle.text(t);
                      drawDiffChart(t);
                    }
                  });

                  //
                  // draw initial diff chart
                  drawDiffChart(0);
                }
                else {
                  //
                  // get chart wrapper
                  var chartWrapper = new google.visualization.ChartWrapper({
                    'containerId': field_id,
                    'chartType': settings['chart_type'],
                    'dataTable': dataTable,
                    'options': settings['chart_options']
                  });

                  // draw chart
                  chartWrapper.draw();
                }

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

