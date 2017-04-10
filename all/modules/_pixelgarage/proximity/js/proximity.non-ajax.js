/**
 * Handles an full area item click, if AJAX is disabled.
 *
 * Created on 11.12.15.
 */

(function ($) {
  /**
   * Guarantees full area clickable proximity items in case of disabled AJAX.
   */
  Drupal.behaviors.proximityItemFullSizeClick = {
    attach: function () {
      // Iterate through all proximity container instances
      $.each(Drupal.settings.proximity, function (container, settings) {

        var $container = $('#' + container),
            $items     = $container.find('.pe-item-ajax'),
            $linkedItems = $container.find('.pe-item-linked');

        $items.once('click', function () {
          $(this).on('click', function () {
            window.location = $(this).find("a:first").attr("href");
            return false;
          });
        });

        $linkedItems.once('click', function () {
          $(this).on('click', function () {
            var link = $(this).find(".pe-item-inner > a").attr("href");
            window.open(link, '_blank');
            return false;
          });
        });
      });
    }
  };

})(jQuery);


