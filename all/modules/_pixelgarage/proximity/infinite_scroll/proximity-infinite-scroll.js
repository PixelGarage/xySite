(function ($) {
  "use strict";

  var $window = $(window);

  // The threshold for how far to the bottom you should reach before reloading.
  var scroll_threshold = 200;
  var vis_index = 0;

  /**
   * Insert a views infinite scroll view into the document after AJAX.
   *
   * @param {object} $new_view The new view coming from the server.
   */
  $.fn.infiniteScrollInsertView = function ($new_view) {
    var $existing_view = this;
    var $new_content = $new_view.find('.pe-container .pe-item'),
        $new_pager = $new_view.find('.pager--infinite-scroll'),
        $container = $existing_view.find('.pe-container'),
        $old_pager = $existing_view.find('.pager--infinite-scroll');

    // append new items to isotope container and layout them
    $container.append($new_content).isotope('appended', $new_content);

    // replace old pager with new one (only new pager has actual page link)
    $old_pager.replaceWith($new_pager);

    // call attachBehaviors, because we do not replace the old content with the new one
    Drupal.attachBehaviors($container, Drupal.settings);
  };

  /**
   * Handle the automatic paging based on the scroll amount.
   */
  Drupal.behaviors.proximity_infinite_scroll_automatic = {
    attach : function(context, settings) {

      var settings = settings.proximity_infinite_scroll;
      var loadingImg = '<div class="proximity_infinite_scroll-ajax-loader"><img src="' + settings.img_path + '" alt="loading..."/></div>';

      $('.pager--infinite-scroll.pager--infinite-scroll-auto', context).once().each(function() {
        var $pager = $(this);
        $pager.find('.pager__item').hide();
        if ($pager.find('.pager__item a').length) {
          $pager.append(loadingImg);
        }
        $window.bind('scroll.views_infinite_scroll_' + vis_index, function() {
          if (window.innerHeight + window.pageYOffset > $pager.offset().top - scroll_threshold) {
            $pager.find('.pager__item a').click();
            $window.unbind('scroll.views_infinite_scroll_' + vis_index);
          }
        });
        vis_index++;
      });

    }
  };

})(jQuery);
