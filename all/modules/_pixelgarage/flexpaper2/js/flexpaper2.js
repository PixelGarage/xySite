(function ($) {
  Drupal.behaviors.flexpaper2Viewer = {
    attach: function (context, settings) {
      //Get paths for published files
      var swfFiles       = Drupal.settings.flexpaper2.swfFiles;
      var jsonFiles      = Drupal.settings.flexpaper2.jsonFiles;
      var pdfFiles       = Drupal.settings.flexpaper2.pdfFiles;
      var pngFiles       = Drupal.settings.flexpaper2.pngFiles;
      var thumbFiles     = Drupal.settings.flexpaper2.thumbFiles;
      var renderingOrder = Drupal.settings.flexpaper2.RenderingOrder;

      var flexpaper2_init = function (element_id, index) {
        $('#' + element_id).FlexPaperViewer(
          {
            config: {
              SWFFile: swfFiles[index],
              JSONFile: jsonFiles[index],
              IMGFiles: pngFiles[index],
              PDFFile: pdfFiles[index],
              ThumbIMGFiles: thumbFiles[index],

              Scale: Drupal.settings.flexpaper2.Scale,
              ZoomTransition: Drupal.settings.flexpaper2.ZoomTransition,
              ZoomTime: Drupal.settings.flexpaper2.ZoomTime,
              ZoomInterval: Drupal.settings.flexpaper2.ZoomInterval,
              FitPageOnLoad: Drupal.settings.flexpaper2.FitPageOnLoad == 1,
              FitWidthOnLoad: Drupal.settings.flexpaper2.FitWidthOnLoad == 1,
              FullScreenAsMaxWindow: Drupal.settings.flexpaper2.FullScreenAsMaxWindow == 1,
              ProgressiveLoading: Drupal.settings.flexpaper2.ProgressiveLoading == 1,
              MinZoomSize: Drupal.settings.flexpaper2.MinZoomSize,
              MaxZoomSize: Drupal.settings.flexpaper2.MaxZoomSize,
              SearchMatchAll: Drupal.settings.flexpaper2.SearchMatchAll == 1,
              InitViewMode: Drupal.settings.flexpaper2.InitViewMode,
              RenderingOrder: renderingOrder,

              jsDirectory: Drupal.settings.flexpaper2.jsDirectory,
              cssDirectory: Drupal.settings.flexpaper2.cssDirectory,
              localeDirectory: Drupal.settings.flexpaper2.localeDirectory,

              ViewModeToolsVisible: Drupal.settings.flexpaper2.ViewModeToolsVisible == 1,
              ZoomToolsVisible: Drupal.settings.flexpaper2.ZoomToolsVisible == 1,
              NavToolsVisible: Drupal.settings.flexpaper2.NavToolsVisible == 1,
              CursorToolsVisible: Drupal.settings.flexpaper2.CursorToolsVisible == 1,
              SearchToolsVisible: Drupal.settings.flexpaper2.SearchToolsVisible == 1,

              UIConfig: Drupal.settings.flexpaper2.UIConfig,
              localeChain: 'en_US',
              JSONDataType: 'json',
              key: Drupal.settings.flexpaper2.licenseKey
            }
          }
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
