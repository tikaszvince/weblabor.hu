<?php

/**
 * Szűrő megvalósítás.
 */
function _wlfilter_process($body) {
  $replaces = array(
    "#\n*\[colorer=([a-z0-9\-]+)\](.*?)\[/colorer\]\n*#msie" =>
    "_wlfilter_highlight('\\2','\\1')",

    "#\n*\[h([2-6])\](.*?)\[/h\\1\]\n*#si" =>
    '<h\\1>\\2</h\\1>',

    "#\[anchor=(\\w+)\]#si" =>
    '<a id="\\1"></a>',

    "#\[comment:(left|right|center)\](.*?)\[/comment\]#si" =>
    '<div class="bb-comment bb-comment-\\1"><div class="bb-content">\\2</div></div>',

    "#\[nodelink=(\\d+)\](.*?)\[/nodelink]?#sie" =>
    "l('\\2', 'node/\\1')",

    "#\[q\](.*?)\[/q\]#si" =>
    '<q>\\1</q>'
  );
  $body = preg_replace(array_keys($replaces), array_values($replaces), $body);

  // Replaces for table cell tags
  $cell_tags = array(
    // Normal cells
    "#\[cell\](.*?)\[/cell\]#si" =>
    '<td>\\1</td>',

    "#\[cell:h(\\d+)\](.*?)\[/cell\]#si" =>
    '<td colspan="\\1">\\2</td>',

    "#\[cell:v(\\d+)\](.*?)\[/cell\]#si" =>
    '<td rowspan="\\1">\\2</td>',

    "#\[cell:h(\\d+):v(\\d+)\](.*?)\[/cell\]#si" =>
    '<td colspan="\\1" rowspan="\\2">\\3</td>',

    // Header cells
    "#\[hcell\](.*?)\[/hcell\]#si" =>
    '<th>\\1</th>',

    "#\[hcell:h(\\d+)\](.*?)\[/hcell\]#si" =>
    '<th colspan="\\1">\\2</th>',

    "#\[hcell:v(\\d+)\](.*?)\[/hcell\]#si" =>
    '<th rowspan="\\1">\\2</th>',

    "#\[hcell:h(\\d+):v(\\d+)\](.*?)\[/hcell\]#si" =>
    '<th colspan="\\1" rowspan="\\2">\\3</th>',
  );

  // Get all tables, and only allow table related tags in them
  if (preg_match_all("#\[table\](.*?)\[/table\]#si", $body, $tables, PREG_SET_ORDER)) {
    foreach ($tables as $table) {
      $table_replace = '';

      // Find rows in this table (only rows are allowed)
      if (preg_match_all("#\[row\](.*?)\[/row\]#si", $table[1], $rows, PREG_SET_ORDER)) {
        foreach ($rows as $num => $row) {
          // Add all rows to the replacement of this table with their cells
          $table_replace .= '<tr class="'. (($num % 2 == 1) ? 'dark' : 'light') .'">'. trim(preg_replace(array_keys($cell_tags), array_values($cell_tags), $row[1])) .'</tr>';
        }
      }

      // Get caption if specified on top of table
      $caption_tag = '';
      if (preg_match("#^\\s*\[caption\](.*?)\[/caption\]#", $table[1], $caption)) {
        $caption_tag = '<caption>'. trim($caption[1]) .'</caption>';
      }

      // Replace actual table with HTML converted code
      $body = str_replace($table[0], '<div class="center"><table>'. $caption_tag . $table_replace .'</table></div>', $body);
    }
  }

  // remove too long URL text, and put it into the title attribute
  //  if the link's text contains a word that is more than 40 characters
  //  long, then compress the whole text (it's not the best, should just
  //  compress the "bad" word, but try to do it in regex...)
  $body = preg_replace("#(<a\s+[^>]+)>(?=[^<]*[^< ]{40})([^<]{20})([^<]*)([^<]{20})(</a>)#", "\\1 title=\"\\2\\3\\4\">\\2...\\4\\5", $body);

  // attach nofollow to links
  if (_wlfilter_set_nofollow()) {
    $body = preg_replace("#<a\s+(.*?</a>)#", "<a rel=\"nofollow\" \\1", $body);
  }

  _wlfilter_bbcode_filter($body);
  
  // WHAT szűrés és sortörés átalakítás.
  $body = '<div class="whateffect">'. $body .'</div>';
  $body = preg_replace('/([^<.,)?!\\\\\'" \\n\\t]+)@([^<.,)?!\\\\\'" \\n\\t]+\\.)/', "\${1}##kukac##\${2}", $body);
  $body = str_replace(array("\r\n", "\r"), "\n", $body);
  $body = str_replace('&lt;!--intro--&gt;', '', $body);
  return str_replace('<antibr /><br />', '', nl2br($body));
}

/**
 * JavaScript highlighter-nek megfelelő stuktúrát alakít ki.
 */
function _wlfilter_highlight($content = NULL, $type = NULL) {
  static $languages = array();
  
  // Return list of types used on the page.
  if (!isset($content)) {
    return $languages;
  }
  
  // Collect types for theme.
  $type = preg_replace('/[^a-z]/', '', strtolower($type));
  $content = str_replace("\n", "<antibr />\n", $content);
  $languages[] = $type;

  /* A <pre> tag CSS osztalyat a SyntaxHiglighter altal megkivant formaban
  adjuk meg.  A nyelv utan kotelezoen a ":code" kell, hogy alljon, igy talalja
  meg az SH.  Eredetileg a name="code" alapjan kereste ki a hasznalando
  elemeket. */
  return '<div class="sourcecode sourcecode-'. $type .'"><pre class="'. $type .':code"><code class="language-'. $type .'">'. $content .'</code></pre></div>';
}

/**
 * Csatolmány megjelenítés.
 */
function _wlfilter_nodeapi(&$node, $isfeed, $teaser) {
  
  // Files lista másolat, hogy törölhessünk belőle.
  $files = $node->files;
  if (!is_array($files)) {
    $files = array();
  }
  
  // TODO: rss item and alter hook bug exposed
  if ($isfeed) {
    $text = &$node->content['body']['#value'];
  }
  elseif ($teaser) {
    $text = &$node->teaser;
  }
  else {
    $text = &$node->body;
  }
  
  // Ikon hozzáadása, ha van, (az alt a node címe).
  foreach ($files as $fid => $file) {
    if ($file->list == 1) {
      $text = '<img src="'. file_create_url($file->filename) .'" class="attach-icon" alt="'. check_plain($node->title) .'" />'. $text;
      unset($files[$fid]);
    }
  }
  
  // Csatolmányok thumbnail és eredeti ID-vel.
  if (preg_match_all("!\\[attach:(\\d+):(\\d+):(left|right|center)\\]!", $text, $found)) {
    foreach ($found[1] as $i => $fid) {
      if (isset($files[$fid]) && isset($files[$found[2][$i]])) {
        _wlfilter_attach_replace(
          $text,
          $found[0][$i],
          wlfilter_attach_thumb_theme($files[$fid], $files[$found[2][$i]], $found[3][$i]),
          $isfeed
        );
        unset($files[$fid]);
        unset($files[$found[2][$i]]);
      }
    }
  }
  
  // Csatolmányok ID szerint.
  if (preg_match_all("!\\[attach:(\\d+):(left|right|center)\\]!", $text, $found)) {
    foreach ($found[1] as $i => $fid) {
      if (isset($files[$fid])) {
        _wlfilter_attach_replace(
          $text,
          $found[0][$i],
          wlfilter_attach_file_theme($files[$fid], $found[2][$i]),
          $isfeed
        );
        unset($files[$fid]);
      }
    }
  }
  
  // Maradék csatolmányok listázása.
  if (preg_match("!\\[attach:listall:(left|right|center)\\]!", $text, $found)) {
    _wlfilter_attach_replace(
      $text,
      $found[0],
      wlfilter_attach_listall_theme($files, $found[1]),
      $isfeed
    );
  }
  
  // Maradék képek galériaként.
  if (preg_match("!\\[attach:album\\]!", $text, $found)) {
    _wlfilter_attach_replace(
      $text,
      $found[0],
      wlfilter_attach_album_theme($files),
      $isfeed
    );
  }
  
  // A sorozatok blokkjának helyére tegyük a blokkot.
  if (preg_match("!\\[series-info:(left|right|center)\\]!", $text, $match)) {
    // A sorozat, amibe a node beletartozik.
    $terms = array_keys(taxonomy_node_get_terms_by_vocabulary($node, WL_V_SERIES));
    if (count($terms) == 1) {
      $nodelist = _wlfilter_series_nodelist(array_shift($terms));
      if (count($nodelist) > 1) {
        $nodelinks = array();
        foreach ($nodelist as $node_in_series) {
          if ($node_in_series->nid == $node->nid) {
            $nodelinks[] = $node_in_series->title;
          }
          else {
            $nodelinks[] = l($node_in_series->title, 'node/'. $node_in_series->nid);
          }
        }
        $text = str_replace($match[0], wlfilter_series_box_theme($nodelinks, $match[1]), $text);
        return;
      }
    }
    // A sorozat doboz hivatkozást mindenképpen törölni kell, ha nem
    // vagyunk sorozatban, akkor is.
    $text = str_replace($match[0], '', $text);
  }
}

/**
 * Csatolmány kitöltő helyettesítés, ha feed, akkor törlés.
 */
function _wlfilter_attach_replace(&$content, $source, $dest, $isfeed) {
  $content = str_replace($source, ($isfeed ? '' : $dest), $content);
}

/**
 * Csatolt fájl megjelenítése.
 */
function wlfilter_attach_file_theme($file, $align) {
  if (is_object($file)) {
    $title = check_plain($file->description);
    if (preg_match('!^image/!', $file->filemime)) {
      $img = file_create_url($file->filename);
      return '<div class="attach-file attach-'. $align .'"><img src="'. $img .'" alt="" title="'. $title .'" /><p>'. $title .'</p></div>';
    }
    else {
      $link = '<a href="'. file_create_url($file->filename) .'">'. $title .'</a> ('. format_size($file->filesize) .')';
      return '<div class="attach-list attach-'. $align .'"><p>'. $link .'</p></div>';
    }
  }
}

/**
 * Csatolt fájl megjelenítése bélyegképpel.
 */
function wlfilter_attach_thumb_theme($thumb, $file, $align) {
  if (is_object($thumb) && is_object($file)) {
    $thumbfile = file_create_url($thumb->filename);
    $thumbtitle = check_plain($file->description);
    $origfile = file_create_url($file->filename);
    $subtext =  '<a href="'. $origfile .'">'. $thumbtitle .'</a> ('. format_size($file->filesize) .')';
    return '<div class="attach-file attach-'. $align .'"><a href="'. $origfile .'"><img src="'. $thumbfile .'" alt="" title="'. $thumbtitle .'" /></a><p>'. $subtext .'</p></div>';
  }
}

/**
 * Fájl lista megjelenítése.
 */
function wlfilter_attach_listall_theme($files, $align) {
  if (count($files) == 0) {
    $list = 'Egy csatolmány sincs még ehhez a tartalomhoz kapcsolva.';
  }
  else {
    $links = array();
    foreach ($files as $file) {
      $links[] = '<a href="'. file_create_url($file->filename) .'">'. $file->description .'</a> ('. format_size($file->filesize) .')';
    }
    $list = theme('item_list', $links, 'Csatolmányok');
  }
  return '<div class="attach-list attach-list-'. $align .'">'. $list .'</div>';
}

/**
 * Galéria megjelenítése.
 */
function wlfilter_attach_album_theme($files) {
  if (count($files) == 0) {
    $list = 'Egy csatolmány sincs még ehhez a tartalomhoz kapcsolva.';
  }
  else {
    $images = array();
    foreach ($files as $file) {
      if (preg_match('!^image/!', $file->filemime)) {
        $title = check_plain($file->description);
        $list .= '<div><img src="'. file_create_url($file->filename) .'" alt="" title="'. $title .'"/><p>'. $title .'</p></div>';
      }
    }
  }
  return '<div class="attach-album">'. $list .'<span>&#160;</span></div>';
}

/**
 * Sorozat doboz megjelenítése.
 */
function wlfilter_series_box_theme($nodelinks, $align) {
  return '<div class="series-box series-'. $align .'">'. theme('item_list', $nodelinks, 'A sorozatban megjelent') .'</div>'; 
}

/**
 * Egy sorozatba tartozó tartalmak listája.
 */
function _wlfilter_series_nodelist($tid) {
  $nodelist = array();
  if ($result = db_query('SELECT DISTINCT n.nid, n.title FROM {term_node} t INNER JOIN {node} n ON t.nid = n.nid WHERE n.status = 1 AND t.tid = %d ORDER BY n.created', $tid)) {
    while ($node = db_fetch_object($result)) {
      $nodelist[] = $node;
    }
  }
  return $nodelist;
}

function _wlfilter_bbcode_filter(&$body) {
  
  $preg = array(    
    // Font, text and alignment
    '#\[color=(.*?)(?::\w+)?\](.*?)\[/color(?::\w+)?\]#si'   => '<span style="color:\\1">\\2</span>',
    '#\[size=(.*?)(?::\w+)?\](.*?)\[/size(?::\w+)?\]#si'     => '<span style="font-size:\\1">\\2</span>',
    '#\[font=(.*?)(?::\w+)?\](.*?)\[/font(?::\w+)?\]#si'     => '<span style="font-family:\\1">\\2</span>',
    '#\[align=(.*?)(?::\w+)?\](.*?)\[/align(?::\w+)?\]#si'   => '<div style="text-align:\\1">\\2</div>',
    '#\[float=(.*?)(?::\w+)?\](.*?)\[/float(?::\w+)?\]#si'   => '<div style="float:\\1">\\2</div>',
    '#\[b(?::\w+)?\](.*?)\[/b(?::\w+)?\]#si'                 => '<span style="font-weight:bold">\\1</span>',
    '#\[i(?::\w+)?\](.*?)\[/i(?::\w+)?\]#si'                 => '<span style="font-style:italic">\\1</span>',
    '#\[u(?::\w+)?\](.*?)\[/u(?::\w+)?\]#si'                 => '<span style="text-decoration:underline">\\1</span>',
    '#\[center(?::\w+)?\](.*?)\[/center(?::\w+)?\]#si'       => '<div style="text-align:center">\\1</div>',
    '#\[left(?::\w+)?\](.*?)\[/left(?::\w+)?\]#si'           => '<div style="text-align:left">\\1</div>',
    '#\[right(?::\w+)?\](.*?)\[/right(?::\w+)?\]#si'         => '<div style="text-align:right">\\1</div>',
    
    // Links without a protocol, with a protocol and with good looking text
    '#\[url(?::\w+)?\]www\.(.*?)\[/url(?::\w+)?\]#si'        => '<a href="http://www.\\1" class="bb-url">\\1</a>',
    '#\[url(?::\w+)?\](.*?)\[/url(?::\w+)?\]#si'             => '<a href="\\1" class="bb-url">\\1</a>',
    '#\[url=(.*?)(?::\w+)?\](.*?)\[/url(?::\w+)?\]#si'       => '<a href="\\1" class="bb-url">\\2</a>',
    
    // Images without or with client-side sizing
    '#\[img(?::\w+)?\](.*?)\[/img(?::\w+)?\]#si'             => '<img src="\\1" class="bb-image" alt="" />',
    '#\[img=(.*?)x(.*?)(?::\w+)?\](.*?)\[/img(?::\w+)?\]#si' => '<img width="\\1" height="\\2" src="\\3" class="bb-image" alt="" />',
    
    // Quoting with or without specifying the source
    '#\[quote(?::\w+)?\](?:[\r\n])*(.*?)\[/quote(?::\w+)?\]#si'         => '<div class="bb-quote"><div class="bb-quote-body">\\1</div></div>',
    '#\[quote=(?:&quot;|"|\')?(.*?)["\']?(?:&quot;|"|\')?\](?:[\r\n])*(.*?)\[/quote(?::\w+)?\]#si'   => '<div class="bb-quote"><div class="bb-quote-body">\\2</div></div>',
    
    // Different kinds of lists and list items
    '#\[\*(?::\w+)?\](.+)(?=\[\*(?::\w+)?\]|\[/list)#Usi'    => '<li>\\1</li>',
    '#\[list(?::\w+)?\](.*?)\[/list(?::\w+)?\]#si'           => _wlfilter_bbcode_list_tag(),
    '#\[list(?::\w+)?\](.*?)\[/list:u(?::\w+)?\]#s'          => _wlfilter_bbcode_list_tag(),
    '#\[list(?::\w+)?\](.*?)\[/list:o(?::\w+)?\]#s'          => _wlfilter_bbcode_list_tag('decimal'),
    '#\[list=1(?::\w+)?\](.*?)\[/list(?::\w+)?\]#si'         => _wlfilter_bbcode_list_tag('decimal'),
    '#\[list=i(?::\w+)?\](.*?)\[/list(?::\w+)?\]#s'          => _wlfilter_bbcode_list_tag('lower-roman'),
    '#\[list=I(?::\w+)?\](.*?)\[/list(?::\w+)?\]#s'          => _wlfilter_bbcode_list_tag('upper-roman'),
    '#\[list=a(?::\w+)?\](.*?)\[/list(?::\w+)?\]#s'          => _wlfilter_bbcode_list_tag('lower-alpha'),
    '#\[list=A(?::\w+)?\](.*?)\[/list(?::\w+)?\]#s'          => _wlfilter_bbcode_list_tag('upper-alpha'),
    
    // Clean up the list output a bit
    '#<ol(.*?)>(?:.*?)<li(.*?)>#si'         => '<ol\\1><li\\2>',
    '#<ul(.*?)>(?:.*?)<li(.*?)>#si'         => '<ul\\1><li\\2>'
  );
  $body = preg_replace(array_keys($preg), array_values($preg), $body);

  // We cannot evaluate the variable in callback function because there is no way to pass the $format variable
  $body = preg_replace(
    array('#\[email(?::\w+)?\](.*?)\[/email(?::\w+)?\]#si','#\[email=(.*?)(?::\w+)?\](.*?)\[/email(?::\w+)?\]#si'),
    array('<a href="mailto:\\1" class="bb-email">\\1</a>', '<a href="mailto:\\1" class="bb-email">\\2</a>'),
    $body
  );

  // Horizontal delimiter
  $body = str_replace("[hr]", '<hr class="bb-hr" />', $body);

  // Find all [code] tags and check if they contain a newline. In case we find a newline,
  // that [code] should be rendered as a block, otherwise it will still be inline
  if (preg_match_all('#\[code(?::\w+)?\](.*?)\[/code(?::\w+)?\]#si', $body, $code_tags, PREG_SET_ORDER)) {
    foreach ($code_tags as $code_tag) {
      if (strpos($code_tag[1], "\n") !== FALSE) {
        $code_tag[1] = str_replace(array("\n ", "  ", "\n"), array("<br />&nbsp;", " &nbsp;", "<br />"), trim($code_tag[1]));
        $code_block = '<div class="bb-code-block">%s</div>';
      } else {
        $code_block = '<code class="bb-code">%s</code>';
      }
      $body = str_replace($code_tag[0], sprintf($code_block, $code_tag[1]), $body);
    }
  }
  
  /**
   * - Goes through the given string, and replaces xxxx://yyyy with an HTML <a> tag linking
   *  to that URL
   * - Goes through the given string, and replaces www.xxxx.yyyy[zzzz] with an HTML <a> tag linking
   *  to http://www.xxxx.yyyy[/zzzz]
   * - Goes through the given string, and replaces xxxx@yyyy with an HTML mailto: tag linking
   *    to that email address
   * - Only matches these 2 patterns either after a space, or at the beginning of a line
   *
   * Notes: the email one might get annoying - it's easy to make it more restrictive, though.. maybe
   * have it require something like xxxx@yyyy.zzzz or such. We'll see.
   */

  // pad it with a space so we can match things at the start of the 1st line.
  $ret = ' ' . $body;

  // matches an "xxxx://yyyy" URL at the start of a line, or after a space.
  // xxxx can only be alpha characters.
  // yyyy is anything up to the first space, newline, comma, double quote or <
  $ret = preg_replace("#([\t\r\n ])([a-z0-9]+?){1}://([\w\-]+\.([\w\-]+\.)*[\w]+(:[0-9]+)?(/[^ \"\n\r\t<]*)?)#i", '\1<a href="\2://\3">\2://\3</a>', $ret);

  // matches a "www|ftp.xxxx.yyyy[/zzzz]" kinda lazy URL thing
  // Must contain at least 2 dots. xxxx contains either alphanum, or "-"
  // zzzz is optional.. will contain everything up to the first space, newline,
  // comma, double quote or <.
  $ret = preg_replace("#([\t\r\n ])(www|ftp)\.(([\w\-]+\.)*[\w]+(:[0-9]+)?(/[^ \"\n\r\t<]*)?)#i", '\1<a href="http://\2.\3">\2.\3</a>', $ret);

  // matches an email@domain type address at the start of a line, or after a space.
  // Note: Only the followed chars are valid; alphanums, "-", "_" and or ".".
  $ret = preg_replace("#([\n ])([a-z0-9\-_.]+?)@([\w\-]+\.([\w\-\.]+\.)*[\w]+)#i", "\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>", $ret);

  // Remove our padding..
  $ret = substr($ret, 1);

  return $ret;
}

function _wlfilter_bbcode_list_tag($type = NULL) {
  if (isset($type)) {
    return '<ol class="bb-list" style="list-style-type:' . $type . ';">\\1</ol>';
  }
  else {
    return '<ul class="bb-list">\\1</ul>';
  }
}
