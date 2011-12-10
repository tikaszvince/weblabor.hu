// Attach later then ui.datepicker.js, not using behaviors.
$(document).ready(function() {
  d = new Date();
  $('#edit-publish-on').datepicker({dateFormat: 'yy-mm-dd 12:00:00 +0' + (d.getTimezoneOffset() / (-60)) + '00'});
});
