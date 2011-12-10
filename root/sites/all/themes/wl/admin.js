var editTitleElem = null;
var followTimeOut = null;

function bindDblClickEdit() {
  $('.node-title-editable').click(function (event) { editTitleClick(this, event); });
}

function editTitleClick(elem, e) {
  // Single click: if link, set timeout to follow link
  if (e.detail == 1) {
    if ($(elem).attr('href')) {
      followTimeOut = setTimeout(function() { window.location=$(elem).attr('href') }, 400);
    }
  }
  // Double click: clear timeout, display editor
  else if (e.detail == 2) {
    clearTimeout(followTimeOut);
    editTitleElem = $(elem);
    $(elem).hide();
    // Escape HTML for inclusion in form
    var text = $(elem).text().replace(/&/g, '&amp;').replace(/"/g, '&quot;').replace(/</, '&lt;').replace(/>/, '&gt;');
    $(elem).after(
      '<form onsubmit="return submitTitle();" id="editTitleForm">'+
      '<input type="text" name="title" value="' + text +
      '" style="width: 100%;" id="editTitleText" class="form-text" '+
      'onblur="editEnd()"></form>'
    );
    // TODO: this seems to be lame, find out how can we focus better
    setTimeout(function() { $('#editTitleText')[0].focus(); }, 150);
  }
  e.preventDefault();
}

function submitTitle() {
  // POST title with AJAX
  $.post(
    '/aa/nodetitle/edit',
    {
      node: $(editTitleElem).attr('id'),
      title: $('#editTitleText').attr('value')
    },
    function(data) { submittedTitle(data); }
  );

  // Inform user that something is happening
  $('#editTitleText').toggleClass('form-inplace-sending');
  $('#editTitleText').attr('disabled', 'disabled');
  $('#editTitleText').attr('value', '');

  return false;
}

function submittedTitle(data) {
  // Actualize title, show title again instead of form
  $(editTitleElem).empty().append(data);
  editEnd();
}

function editEnd() {
  $(editTitleElem).show();
  $('#editTitleForm').remove();
}

$(document).ready(bindDblClickEdit);
