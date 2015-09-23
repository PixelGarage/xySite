(function ($) {
  Drupal.behaviors.flexpaper2Viewer = {
    attach: function (context, settings) {
      //Get paths for published files
      var swfFiles = Drupal.settings.flexpaper2.swfFiles;
      var jsonFiles = Drupal.settings.flexpaper2.jsonFiles;
      var pdfFiles = Drupal.settings.flexpaper2.pdfFiles;
      var pngFiles = Drupal.settings.flexpaper2.pngFiles;
      var renderOrder = Drupal.settings.flexpaper2.renderOrder;

      var flexpaper2_init = function (element_id, index) {
        $('#' + element_id).FlexPaperViewer(
            { config: {
              SWFFile: swfFiles[index],
              JSONFile: jsonFiles[index],
              IMGFiles: pngFiles[index],
              PDFFile: pdfFiles[index],

              Scale: Drupal.settings.flexpaper2.scale,
              ZoomTransition: Drupal.settings.flexpaper2.zoomTransition,
              ZoomTime: Drupal.settings.flexpaper2.zoomTime,
              ZoomInterval: Drupal.settings.flexpaper2.zoomInterval,
              FitPageOnLoad: Drupal.settings.flexpaper2.fitPageOnLoad == 1,
              FitWidthOnLoad: Drupal.settings.flexpaper2.fitWidthOnLoad == 1,
              FullScreenAsMaxWindow: Drupal.settings.flexpaper2.fullScreenAsMaxWindow == 1,
              ProgressiveLoading: Drupal.settings.flexpaper2.progressiveLoading == 1,
              MinZoomSize: Drupal.settings.flexpaper2.minZoomSize,
              MaxZoomSize: Drupal.settings.flexpaper2.maxZoomSize,
              SearchMatchAll: Drupal.settings.flexpaper2.searchMatchAll == 1,
              InitViewMode: Drupal.settings.flexpaper2.initViewMode,
              RenderingOrder: renderOrder,

              jsDirectory: Drupal.settings.flexpaper2.jsDirectory,
              cssDirectory: Drupal.settings.flexpaper2.cssDirectory,
              localeDirectory: Drupal.settings.flexpaper2.localeDirectory,

              ViewModeToolsVisible: Drupal.settings.flexpaper2.viewModeToolsVisible == 1,
              ZoomToolsVisible: Drupal.settings.flexpaper2.zoomToolsVisible == 1,
              NavToolsVisible: Drupal.settings.flexpaper2.navToolsVisible == 1,
              CursorToolsVisible: Drupal.settings.flexpaper2.cursorToolsVisible == 1,
              SearchToolsVisible: Drupal.settings.flexpaper2.searchToolsVisible == 1,

              UIConfig: Drupal.settings.flexpaper2.uiConfig,
              localeChain: 'en_US',
              JSONDataType: 'json',
              key: Drupal.settings.flexpaper2.licenseKey
            }}
        );
      };

      var viewers = $('div.flexpaper_viewer');

      //Put ids for this elements, because flexpaper need it.
        viewers.each(function (index) {
        var elementId = 'documentViewer_' + index;
        $(this).attr('id', elementId);
        flexpaper2_init(elementId, index);
      });
    }
  }
})(jQuery);
