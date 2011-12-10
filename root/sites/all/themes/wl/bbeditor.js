/*----------------------------------------------------------------------------
 * 
 *  BBEditor 1.5 (requires jQuery!)
 *  Copyright (c) 2006-2007 by András Bártházi (http://barthazi.hu) &
 *                             Publishing Kft  (http://publishing.hu)
 *                             Weblabor.hu     (http://weblabor.hu)
 *                             Gábor Hojtsy    (http://hojtsy.hu)
 *
 *  License: Attribution-ShareAlike 2.0
 *  (http://creativecommons.org/licenses/by-sa/2.0/)
 * 
/*--------------------------------------------------------------------------*/

// HELPER FUNCTIONS ============================================================

function findPosX(obj) { // only in Mozilla
  var curleft = 0;
  if (obj.offsetParent) {
    while (obj.offsetParent) {
      curleft += obj.offsetLeft
      obj = obj.offsetParent;
    }
  } else if (obj.x) {
    curleft += obj.x;
  }
  return curleft;
}

function findPosY(obj) { // only in Mozilla
  var curtop = 0;
  if (obj.offsetParent) {
    while (obj.offsetParent) {
      curtop += obj.offsetTop
      obj = obj.offsetParent;
    }
  } else if (obj.y) {
    curtop += obj.y;
  }
  return curtop;
}

function insertAtCursor(t, before, after, code ) { // only in Mozilla
  var startPos = t.selectionStart;
  var endPos = t.selectionEnd;
  var text = t.value.substring(startPos, endPos);
  if (code) {
    text = code(text);
  }
  var scrollPos = t.scrollTop;
  t.value = t.value.substring(0, startPos) + before + text + after + t.value.substring(endPos, t.value.length);
  t.focus();
  t.selectionStart = startPos;
  t.selectionEnd = startPos + before.length + text.length + after.length;
  t.scrollTop = scrollPos;
}

// TEXTARE CONTROL =============================================================

var controls_textarea = null;
var controls_timeout = null;

function setupControls() {
  // Disable editor if user wishes so
  if ($('body').is('.no-rich-controls')) {
    return;
  }

  // Only enable editor for capable browsers
  var elems = $('textarea');
  if (!document.defaultView ||
      !document.getElementsByTagName ||
      !document.createElement ||
      !document.createTextNode ||
      !elems ||
      elems.length == 0 ||
      !typeof(elems[0].selectionStart)=='number' ||
      !document.body) {
      return;
  }

  // Add textareaControl HTML attributes
  $(
    '<div id="textareaControl" style="display: none; position: absolute; text-align: center; padding: 0 2px;">' +
    '<button onclick="controlsGeneralTag(controls_textarea, \'b\');" title="Félkövér" style="width: 30px; font-size: 10px;"><b>b</b></button>' +
    '<button onclick="controlsGeneralTag(controls_textarea, \'i\');" title="Dőlt" style="width: 30px; font-size: 10px;"><i>i</i></button>' +
    //'<button onclick="controlsGeneralTag(controls_textarea, \'u\');" title="Aláhúzott" style="width: 30px; font-size: 10px;"><u>U</u></button>' +
    '<button onclick="controlsGeneralTag(controls_textarea, \'url\',\'=\',controlsAskForLink);" title="Hivatkozás" style="width: 30px; font-size: 10px;"><u style="color: #00f;">lnk</u></button>' +
    '<button onclick="controlsGeneralTag(controls_textarea, \'quote\');" title="Idézet" style="width: 30px; font-size: 10px;"><b>"</b></button>' +
    '<button onclick="controlsGeneralTag(controls_textarea, \'colorer\',\'=\',controlsAskForColorerType);" title="Színezett kód" style="width: 30px; font-size: 10px;"><b>{}</b></button>' +
    '</div>'
  ).appendTo('body');
    
  elems.keypress(function(event) { return (controlsKeyHandler(this, event)); });
  elems.blur(function() { controls_timeout = setTimeout('detachControl()', 1000);});
  elems.focus(function() { attachControl(this); });
}

function attachControl(t) {
  clearTimeout(controls_timeout);
  var left = findPosX(t);
  var top = findPosY(t);
  var width = parseInt(document.defaultView.getComputedStyle(t, "").getPropertyValue("width"));
  //var height = parseInt(document.defaultView.getComputedStyle(t, "").getPropertyValue("height"));
  
  $('#textareaControl').
    css('display', 'block').css('top', top + 'px').css('padding-top', '2px').
    css('height', '210px').css('left', (left+width) + 'px').
    css('width', '40px');
  
  // Only show fade effect if we newly attach the control
  if (controls_textarea == null) {
    $('#textareaControl').fadeIn('slow');
  }
  
  // Change control (we might quickly change between two textareas)
  controls_textarea = t;
}

function detachControl() {
  controls_textarea = null;
  $('#textareaControl').fadeOut('normal');
}

// Keypress events for more than what the buttons can do
function controlsKeyHandler(t, e) {
  // Only care about ctrl-paired keys
  if (!e.ctrlKey) {
    return;
  }
  switch(e.which) {
    case  98: controlsGeneralTag(t, 'b'); break; // ctrl-b
    case 105: controlsGeneralTag(t, 'i'); break; // ctrl-i
    case 117: controlsGeneralTag(t, 'u'); break; // ctrl-u
    case 113: controlsGeneralTag(t, 'quote'); break; // ctrl-q
    case 111: controlsGeneralTag(t, 'code'); break; // ctrl-o (ctrl-c is taken)
    case  50: controlsGeneralTag(t, 'h2'); break; // ctrl-2
    case  51: controlsGeneralTag(t, 'h3'); break; // ctrl-3
    case  52: controlsGeneralTag(t, 'h4'); break; // ctrl-4
    case 109: controlsGeneralTag(t, 'comment', ':center'); break; // ctrl-m
    case 108: controlsGeneralTag(t, 'url', '=', controlsAskForLink); break; // ctrl-l
    case 112: controlsGeneralTag(t, 'colorer', '=', controlsAskForColorerType); break; // ctrl-p
    case  42: controlsList(t); break; // ctrl-*
    case 101: controlsTable(t); break; // ctrl-e
    case 102: searchTextArea(t); break; // ctrl-f

    default: return; // return to allow handling of event
  }
  // Prevent default event handling for all events already handled
  e.preventDefault();
}

function controlsAskForLink() {
  // Prevent controls from reappearing because of our blur
  clearTimeout(controls_timeout);
  return (window.prompt('Hova mutasson a link?', 'http://'));
}

function controlsAskForColorerType() {
  // Prevent controls from reappearing because of our blur
  clearTimeout(controls_timeout);
  return (window.prompt('Forráskód nyelve (php, css, html, javascript, sql stb.)?',''));
}

// Add list markup based on asterisks and newlines
function controlsList(t) {
  insertAtCursor(t, '', '',
    function(text) {
      var ret = text + '\n';
      // Remove tags if already present
      if (ret.match(/\s*\[list\]/)) {
        ret = ret.replace(/^[\r\n\s]*\[list\][\r\n\s]*|[\r\n\s]*\[\/list\][\r\n\s]*$/g, '');
        ret = ret.replace(/^\s*\[\*\]\s*/gm,' * ');
      // Add list tags
      } else {
        ret = ret.replace(/\n(?=\n)/gm, '');
        ret = ret.replace(/^\n$/g, '');
        ret = ret.replace(/^\s*\*\s+/gm, '[*]');
        ret = '[list]\n' + ret + '[/list]';
      }
      return(ret);
    }
  );
}

// Add table markup based on tabs and newlines
function controlsTable(t) {
  insertAtCursor(t,'','',
    function(text) {
      var ret = text + '\n';
      // Remove table tags if present
      if (ret.match(/\s*\[table/)) {
        ret = ret.replace(/[\s\r\n]*\[table.*?\][\s\r\n]*|[\s\r\n]*\[\/table\][\s\r\n]*/g, '');
        ret = ret.replace(/[\s\r]*\[\/cell\][\s\r]*\[cell\][\s\r]*/g, '\t');
        ret = ret.replace(/[\s\r]*\[row\][\s\r]*|[\s\r]*\[\/row\][\s\r]*/g, '\n');
        ret = ret.replace(/\s*\[cell\][\s\r]*|[\s\r]*\[\/cell\]\s*/g, '\n');
        ret = ret.replace(/\n(?=\n)/gm, '');
        ret = ret.replace(/^\n$|^\n|\n$/g, '');
      // Add table tags
      } else {
        ret = ret.replace(/\n(?=\n)/gm, '');
        ret = ret.replace(/^\n$/g, '');
        ret = ret.replace(/^(?!$)/gm, '\[row\]\[cell\]');
        ret = ret.replace(/\t/gm, '\[/cell\]\[cell\]');
        ret = ret.replace(/\n/gm, '\[/cell\]\[/row\]\n');
        ret = ret.replace(/\[row\]/gm, '\[row\]\n');
        ret = ret.replace(/\[cell\]/gm, '  \[cell\]');
        ret = ret.replace(/\[\/cell\]/gm, '\[/cell\]\n');
        ret = '\[table\]\n' + ret + '\[/table\]\n';
      }
      return(ret);
    }
  );
}

// Inject a general tag into the textarea with possible opening
// extras and a function to get tag parameters from
function controlsGeneralTag(t, tag, open, fn) {
  var param = '';
  insertAtCursor(t,'','',
    function(text) {
      var ret = text;
      var rx_open = new RegExp("\\["+tag+"(?:[:=][^\\]]+)?\\]","m");
      var rx_close = new RegExp("\\[/"+tag+"\\]","m");
      // Remove tag if found
      if (ret.match(rx_open)) {
        ret = ret.replace(rx_open,'');
        ret = ret.replace(rx_close,'');
      } else {
        // Get tag param from function
        if (fn) {
          param = fn();
        }
        if (!fn || param) {
	  ret = '[' + tag + (open ? open : '') + param + ']' + ret + '[/' + tag + ']';
        }
      }
      return(ret);
    }
  );
}

// Search some user specified string starting from the current
// cursor position. Highlight the found strings.
function searchTextArea(t) {
  var startPos = t.selectionStart;
  var selection = t.value.substring(t.selectionStart, t.selectionEnd);
  if (t.selectionEnd > startPos) {
    startPos++;
  }

  var string = window.prompt('Keresendő kifejezés?' , selection);
  if (string == '') {
    t.focus();
    return false;
  }

  var pos = t.value.indexOf(string, startPos)
  if (pos >= 0) {
    t.selectionStart = pos;
    t.selectionEnd = pos + string.length;
  } else {
    alert('Nincs találat!');
  }
}

// Setup global control to use later
$(document).ready(setupControls);
