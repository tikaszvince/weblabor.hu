function whatCorrector() {
  var replaces = 0;
  $('.whateffect:contains("##kukac##")').each(function(i) {
    while ((pos = this.innerHTML.indexOf('##kukac##')) != -1 && replaces < 10000) {
      chr = this.innerHTML.substring(pos+9, pos+10);
      this.innerHTML = this.innerHTML.replace(
        '##kukac##',
        ('<.,)?!\'" '.indexOf(chr) != -1) ? '<span>##</span>kukac<span>##</span>' : String.fromCharCode(64)
      );
      replaces++;
    }
  });
};

function linkModifiers() {
  $('a.feed-icon').click(
    function() {
      document.location='http://weblabor.hu/rss/miez?href=' + this.href.replace('http://weblabor.hu/', '');
      return false;
    }
  );
  $("a[@href]").not("[@href^=/]").not("[@href^=#]").not("[@href^=http://libikoka.weblabor.hu]").addClass('link-external');
};

$(document).ready(whatCorrector);
$(document).ready(linkModifiers);
