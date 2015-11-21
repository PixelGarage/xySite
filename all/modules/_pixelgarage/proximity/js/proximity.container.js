/**
 * Contains all functions and behaviors to handle the content containers.
 *
 * Created by ralph on 20.11.15.
 */

(function($) {
  /**
   *  Modal dialog handling.
   *
   *  Opens the modal dialog once, if a full page request is performed. The flag show_modal is only set
   *  when a proximity item request is performed without AJAX.
   *  Additionally the hidden.modal event implementation guarantees, that all content is cleared (audio, video stopped)
   *  when the modal dialog is closed.
   */
  Drupal.behaviors.modalDialogHandling = {
    attach: function () {
      // Iterate through all proximity container instances
      $.each(Drupal.settings.proximity, function (container, settings) {

        var $container    = $('#' + container),
            $dialog       = $container.find('.modal'),
            transDuration = parseInt(settings.trans_duration);


        //
        // Open modal dialog on a full page load (show_modal flag is TRUE)
        // REMARK: behaviors are also called during each ajax request, so reset the flag to guarantee that
        // the modal dialog is only opened once
        if (settings.show_modal) {
          $dialog.fadeIn(transDuration).modal('show');
          settings.show_modal = false; // disable to open modal dialog on subsequent calls

        }

        // when modal dialog is closed, make sure that all content is cleared (videos, audios etc.)
        $dialog.once('modal-hidden', function () {
          $dialog.on('hidden.bs.modal', function () {
            // empty the modal body stopping all media etc.
            $(this).find('.modal-body').empty();
          });
        });

      }); // proximity container instances
    }
  };

})(jQuery);
