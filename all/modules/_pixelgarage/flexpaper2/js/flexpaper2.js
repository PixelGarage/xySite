(function ($) {

  /**
   * Initializes all flexpaper viewers.
   */
  Drupal.behaviors.flexpaper2Viewer = {
    attach: function (context, settings) {
      // initialize Flexpaper viewer
      var flexpaper2_init = function (element_id, index, settings) {
        var elementSettings = settings[index];

        $('#' + element_id).FlexPaperViewer(
          {
            config: {
              SWFFile: elementSettings.swfFiles,
              JSONFile: elementSettings.jsonFiles,
              IMGFiles: elementSettings.imgFiles,
              PDFFile: elementSettings.pdfFiles,
              ThumbIMGFiles: elementSettings.thumbFiles,

              Scale: parseFloat(elementSettings.Scale),
              ZoomTransition: elementSettings.ZoomTransition,
              ZoomTime: parseFloat(elementSettings.ZoomTime),
              ZoomInterval: parseFloat(elementSettings.ZoomInterval),
              FitPageOnLoad: elementSettings.FitPageOnLoad == 1,
              FitWidthOnLoad: elementSettings.FitWidthOnLoad == 1,
              FullScreenAsMaxWindow: elementSettings.FullScreenAsMaxWindow == 1,
              ProgressiveLoading: elementSettings.ProgressiveLoading == 1,
              MinZoomSize: parseFloat(elementSettings.MinZoomSize),
              MaxZoomSize: parseFloat(elementSettings.MaxZoomSize),
              SearchMatchAll: elementSettings.SearchMatchAll == 1,
              InitViewMode: elementSettings.InitViewMode,
              RenderingOrder: elementSettings.RenderingOrder,

              ViewModeToolsVisible: elementSettings.ViewModeToolsVisible == 1,
              ZoomToolsVisible: elementSettings.ZoomToolsVisible == 1,
              NavToolsVisible: elementSettings.NavToolsVisible == 1,
              CursorToolsVisible: elementSettings.CursorToolsVisible == 1,
              SearchToolsVisible: elementSettings.SearchToolsVisible == 1,
              UIConfig: elementSettings.UIConfig,

              jsDirectory: settings.jsDirectory,
              cssDirectory: settings.cssDirectory,
              localeDirectory: settings.localeDirectory,

              StartAtPage : 1,
              MixedMode : true,
              EnableWebGL : true,
              AutoAdjustPrintSize : true,
              PrintPaperAsBitmap : false,
              AutoDetectLinks :true,
              PublicationTitle : '',

              localeChain: 'en_US',
              WMode : 'transparent',
              JSONDataType: 'json',
              key: settings.licenseKey
            }
          }
        );
      };

      // iterate over all flexpaper fields
      $.each(Drupal.settings.flexpaper2, function (fieldName, settings) {
        var fieldClass  = '.field-name-' + fieldName,
            $viewers     = $(fieldClass + ' .flexpaper_viewer');

        //Put ids on all viewers per field, because flexpaper needs it.
        $viewers.each(function (index) {
          var elementId = fieldName + '-documentViewer-' + index;
          $(this).attr('id', elementId);
          flexpaper2_init(elementId, index, settings);
        });
      });

    }
  }
})(jQuery);
