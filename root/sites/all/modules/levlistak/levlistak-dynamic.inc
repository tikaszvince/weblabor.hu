<?php

/**
 * Levelezőlista index oldal.
 */
function levlistak_archive_index($listname) {
  
  // A lista neve már a menüben ellenőrizve volt.
  $file = file_get_contents(LEVLISTAK_PATH . $listname .'/index.html');
  
  // Ez nem az az index.html, amiből táblázatot tudnánk csinálni.
  if (!preg_match_all("!<td><a href=\"(\\d{4})-([a-zA-Z]+)\\.txt(\\.gz)?\">\\[.+ (\\d+ \\w+) \\]</a></td>!Uis", $file, $found, PREG_SET_ORDER)) {
    drupal_access_denied();
    return;
  }
  
  $output = '<div id="mailinglist-index">';
  $year = 0;
  $found[] = array(1 => 3000, 2 => 'January');
  $calendar = array();

  foreach ($found as $item) {
    if ($year != $item[1]) {
      if (count($calendar)) {
        // Van előző calendar, meg kell jeleníteni.
        $table = array();
        $month_num = 1;
        while ($month_num <= 12) {
          $table[floor(($month_num - 1) / 4)][$month_num % 4] = (isset($calendar[$month_num]) ? l(levlistak_translate_month($month_num, TRUE), $calendar[$month_num]) : '-'); 
          $month_num++;
        }
        $output .= theme('table', array(), $table);
        $calendar = array();
      }
      if ($item[1] != 3000) {
        // Boldog új évet. Még úgy ezer évig jó lesz.
        $year = $item[1];
        $output .= '<h2>'. $year .'</h2>';
      }
    }
    $month = levlistak_translate_month($item[2]);
    $calendar[(int) $month] = 'levlistak/'. $listname .'/'. $item[1] .'/'. levlistak_translate_month($item[2]);
  }
  $output .= '</div>';
  
  drupal_set_breadcrumb(array(l(t('Home'), NULL), l('Levezőlisták', 'levlistak')));
  drupal_set_title($listname .' archívum');
  return $output;
}

/**
 * Archívum oldal adott év/hó és típus szerint.
 */
function levlistak_archive_page($listname, $year, $month, $op = 'thread') {
  
  $datestr = $year .'-'. levlistak_translate_month($month);
  $monthstr = sprintf('%02d', $month);
    
  drupal_set_breadcrumb(array(
    l(t('Home'), NULL),
    l('Levelezőlisták', 'levlistak'),
    l(ucfirst($listname .' archívum'), 'levlistak/'. $listname),
    $year .' '. levlistak_translate_month($month, TRUE)
  ));
  drupal_set_title($listname .' archívum - '. $year .' '. levlistak_translate_month($month, TRUE));

  switch ($op) {
    case 'subject':
    case 'author':
    case 'date':
      // Sorrendezett subject/author/date archívum.
      $page = _levlistak_getfile($listname, $datestr, $op);
      
      // Nem megfelelő formátum.
      if (!preg_match_all('!<li><a href="(\\d+).html">(.+)</a><a NAME="(\\d+)">&nbsp;</a>\\s+<i>(.+)</i>!Uis', $page, $found, PREG_SET_ORDER)) {
        drupal_access_denied();
        return;
      }
      
      $rows = array();
      foreach ($found as $msg) {
        $msg[2] = _levlistak_decode_string($msg[2]);
        $msg[4] = _levlistak_decode_string($msg[4]);
        $rows[] = array(
          l(
            _levlistak_refine_title($msg[2], $listname),
            'levlistak/'. $listname .'/'. $year .'/'. $monthstr .'/'. $msg[1]
          ),
          _levlistak_iso_to_utf8($msg[4])
        );
      }
      return _levlistak_metainfo($page) . theme('table', array('Téma', 'Szerző'), $rows);
      break;
  
    case 'thread':
      // Szálak szerint sorredezett lista.
      $page = _levlistak_getfile($listname, $datestr, $op);
      preg_match('%<p>\\s+<ul>(\\s+<!--.+</ul>)\\s+<p>%Us', $page, $found);
      $threads = _levlistak_iso_to_utf8(preg_replace('%<!--[ 0-9-]+-->%', '', '<ul id="mlthread">'. $found[1]));
      while (TRUE) {
        $threads_conv = _levlistak_decode_string($threads);
        if ($threads_conv == $threads) {
          break;
        }
        else {
          $threads = $threads_conv;
        }
      }
      $threads = _levlistak_drop_listname($threads, $listname);
      $threads = preg_replace(
        '!<li><a HREF="([0-9]+).html">(.+)</a><a NAME="[0-9]+">&nbsp;</a>\\s+<i>(.+)</i>\\s+!Usie',
        "'<li><a href=\"/levlistak/$listname/$year/$monthstr/\\1\">' . trim('\\2') . '</a> <span class=\"mlauthor\">(' . trim('\\3') . ')</span></li>'",
        $threads
      );
      return _levlistak_metainfo($page) . $threads;
      break;
  }  
}

/**
 * Archívum oldal adott év/hó és típus szerint.
 */
function levlistak_archive_message($listname, $year, $month, $id) {
  
  $datestr = $year .'-'. levlistak_translate_month($month);
  $monthstr = sprintf('%02d', $month);
  $file = _levlistak_getfile($listname, $datestr, $id);
  
  // Ez nem az üzenet fájlja.
  if (!preg_match('/<!--beginarticle-->(.+)<!--endarticle-->/s', $file, $found)) {
    drupal_access_denied();
    return;
  }
  
  drupal_set_breadcrumb(array(
    l(t('Home'), NULL),
    l('Levelezőlisták', 'levlistak'),
    l(ucfirst($listname .' archívum'), 'levlistak/'. $listname),
    $year .' '. levlistak_translate_month($month, TRUE)
  ));

  // Navigációs elemek kiemelése. 
  $nav = array();
  if (preg_match('!<li>El.z. .zenet: <a HREF="(.+).html">(.+)</a></li>!Usi', $file, $navi)) {
    $nav[] = t('previous') .': '. l(
      _levlistak_refine_title($navi[2], $listname),
      'levlistak/'. $listname .'/'. $year .'/'. $monthstr .'/'. $navi[1]
    );
  }
  if (preg_match('!<li>K.vetkez. .zenet: <a HREF="(.+).html">(.+)</a></li>!Usi', $file, $navi)) {
    $nav[] = t('next') .': '. l(
      _levlistak_refine_title($navi[2], $listname),
      'levlistak/'. $listname .'/'. $year .'/'. $monthstr .'/'. $navi[1]
    );
  }

  // Meta információk összeállítása.
  preg_match('!<h1>(.+)</h1>\\s+<b>(.+)</b>.+TITLE=".+">(.+)</a><br>\\s+<i>(.+)</i>!Usi', $file, $meta);
  drupal_set_title(_levlistak_refine_title($meta[1]));
  $meta[3] = preg_replace('/([^<.,)?!\\\\\'" \\n\\t]+) at ([^<.,)?!\\\\\'" \\n\\t]+\\.)/', "\${1}##kukac##\${2}", $meta[3]);
  $output = "\n".'<dl class="important">'."\n".'<dt>Beküldte:</dt><dd>'.
    _levlistak_iso_to_utf8(trim($meta[2])) .
    ' [<span class="whateffect"><a href="mailto:'. _levlistak_iso_to_utf8(trim($meta[3])) .'">'.
    _levlistak_iso_to_utf8(trim($meta[3])) .'</a></span>]</dd>'.
    "\n".'<dt>Időpont:</dt><dd>'. _levlistak_convert_date(_levlistak_iso_to_utf8($meta[4])) .'</dd>'.
    "\n".'<dt>Szöveg:</dt><dd>';
  
  // A törzs előkerítése, belső linkek kattinthatóvá átalakításával.
  $found[1] = preg_replace('!<a HREF="(https://www.weblabor.hu/mailman/listinfo/[^\"]+)">(.*?)</a>!ie', '"\\1"!="\\2"?"\\2":"<a href=\"\\1\">\\2</a>"', $found[1]);
  $found[1] = preg_replace('!<(/?)PRE>!', '<$1pre>', $found[1]);
  $found[1] = preg_replace('!<(/?)I>!', '<$1i>', $found[1]);
  $output .= _levlistak_get_body($found[1]);
  
  $output .= '</dd>'."\n";
  if (count($nav)) {
    $output .= '<dt>Navigáció</dt><dd>'. theme('item_list', $nav) .'</dd></dl>';
  }
  return $output;
}

/**
 * Letöltés link egy levelezőlista adott hónapi archívumára.
 */ 
function levlistak_archive_download($listname, $year, $month) {
  $datestr = $year .'-'. levlistak_translate_month($month);
  drupal_goto(LEVLISTAK_URL . $listname .'/'. $datestr .'.txt.gz');
}

/**
 * Betölt egy levlista fájlt.
 */
function _levlistak_getfile($listname, $datestr, $viewname) {
  return @file_get_contents(LEVLISTAK_PATH . $listname .'/'. $datestr .'/'. $viewname .'.html');
}

/**
 * UTF-8 kódolásra alakít és a levlista nevét törli a karaktersorozatból.
 */
function _levlistak_refine_title($title, $listname = '') {
  return _levlistak_drop_listname(_levlistak_iso_to_utf8(_levlistak_decode_string(trim($title))), $listname);
}

/**
 * Törli a karaktersorozatból a megadott lista list-idját.
 */
function _levlistak_drop_listname($string, $listname = '') {
  if ($listname == 'wl-flamelista') {
    $string = str_replace('[wl-#!@$*lista]', '', $string);
  }
  elseif ($listname == 'wl-csharplista') {
    $string = str_replace('[wl-c#lista]', '', $string);
  }
  elseif ($listname) {
    $string = str_replace('['. $listname .']', '', $string);
  }
  return trim($string);
}

/**
 * ISO és base64 kódolt karatersorozatokat kódol vissza.
 */
function _levlistak_decode_string($text) {
  $text = str_replace("\n\t", '', $text);
  if (preg_match('!^(.*)=\?[^\?]*\?Q\?(.*)\?=(.*)$!Usi', $text, $regs)) {
    return trim($regs[1] . str_replace('_', ' ', _levlistak_decode_iso($regs[2])) . $regs[3]);
  }
  if (preg_match('!^(.*)=\?[^\?]*\?B\?(.*)\?=(.*)$!Usi', $text, $regs)) {
    return trim($regs[1] . base64_decode($regs[2]) . $regs[3]);
  }
  /*if (preg_match('!(.*)"(.*)"(.*)!', $result, $regs)) {
    return trim($regs[1] . $regs[2] . $regs[3]);
  }*/
  return trim($text);
}

/**
 * ISO kódolt karaktersorozatot kódol vissza.
 */
function _levlistak_decode_iso($text) {
  $i = 0;
  $result = '';
  
  while ($i < strlen($text)) {
    $c = $text[$i];
    if ($c != '=') {
      $result .= $c;
    }
    else {
      if ($text[$i + 1] == "\n") {
        $i++;
      }
      else {
        $hex = substr($text, $i + 1, 2);
        if (eregi('[0-9a-f]{2}', $hex)) {
          $int = hexdec($hex);
          $i += 2;
          $result .= chr($int);
        }
        else {
          $result .= $c;
        }
      }
    }
    $i++;
  }
  return $result;
}

/**
 * ISO és base64 kódolt karatersorozatokat kódol vissza.
 */
function _levlistak_iso_to_utf8($string) {
  return str_replace(
    array("õ", "Õ", "û", "Û"),
    array("ő", "Ő", "ű", "Ű"),
    utf8_encode($string)
  );
}

/**
 * Standard formára alakítja a dátumot.
 */
function _levlistak_convert_date($date) {
  $hunmonths = array(1 => "Jan", "Feb", "Már", "Ápr", "Május", "Jún", "Júl", "Aug", "Sze", "Okt", "Nov", "Dec");
  
  // 2003. Júl. 29., K, 13:35:22 CEST
  // 2003. Okt.  1., Sze, 08:39:43 CET
  if (preg_match('!(\\d{4})\\. (.+)\\. {1,2}(\\d+)\\., (.+), (\\d{2}):(\\d{2}):(\\d{2}) CES?T!', $date, $parts)) {
    return format_date(
      mktime($parts[5], $parts[6], $parts[7], array_search($parts[2], $hunmonths), $parts[3], $parts[1]),
      'large'
    );
  }
  else {
    return 'Nem ismert';
  }
}

/**
 * Megjeleníthatő formára alakítja a törzset.
 */
function _levlistak_get_body($text) {
  return str_replace(
    array('<pre>',  '</pre>',  "\n ",          "\n",     '  '),
    array('<code>', '</code>', '<br />&nbsp;', '<br />', ' &nbsp;'),
    _levlistak_iso_to_utf8($text)
  );
}

/**
 * Listák oldalaihoz tartozó metainformáció átalakítása.
 */
function _levlistak_metainfo($page) {
  preg_match(
    '!<b>Kezd. id.pont:</b> <i>(.+)</i><br>.+<b>Utols. id.pont:</b> <i>(.+)</i><br>\\s+<b>.zenetek:</b> (\\d+)<p>!Us',
    $page,
    $meta
  );
  return '<dl class="important">'.
         "\n".'<dt>Első levél</dt><dd>'. _levlistak_convert_date(_levlistak_iso_to_utf8($meta[1])) .'</dd>'.
         "\n".'<dt>Utolsó levél</dt><dd>'. _levlistak_convert_date(_levlistak_iso_to_utf8($meta[2])) .'</dd>'.
         "\n".'<dt>Levelek száma</dt><dd>'. $meta[3] .'</dd></dl>'."\n";
}
