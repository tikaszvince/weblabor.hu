<?php

/**
 * @file
 *   Weblabor smink előfedolgozó és segéd függvényei.
 */

/**
 * Változók a teljes oldal összeállításához.
 */
function wl_preprocess_page($variables, $hook) {
  global $user;

  // Blogmark popup, más megjelenéssel.
  if (($_GET['q'] == 'blogmarkok/bekuldes' && (isset($_GET['u']) || isset($_GET['embed']))) || ($_GET['q'] == 'blogmarkok/koszonjuk')) {
    $variables['stripped_page'] = TRUE;
  }

  // JS syntax highlighter
  $highlighter = array(
    'shCore.js', 'shBrushCss.js', 'shBrushJScript.js', 'shBrushPhp.js', 'shBrushSql.js',
    'shBrushXml.js', 'shBrushApache.js'
  );
  foreach ($highlighter as $jsfile) {
    $variables['scripts'] .= "\n". ' <script type="text/javascript" src="/sites/all/themes/wl/highlight/' . $jsfile . '"></script>';
  }
  $variables['styles'] .= "\n". ' <link rel="stylesheet" type="text/css" href="/sites/all/themes/wl/highlight/SyntaxHighlighter.css"></link>';
  $variables['closure'] .= '<script type="text/javascript">
  dp.SyntaxHighlighter.ClipboardSwf = \'/sites/all/themes/wl/highlight/clipboard.swf\';
  dp.SyntaxHighlighter.HighlightAll(\'code\', true, false);
 </script>';

  // Cím korrekt beállítása.
  $raw_title = strip_tags($variables['title']);
  $variables['head_title'] = ($raw_title ? $raw_title .' &middot; Weblabor' : 'Weblabor &middot; A fejlesztői forrás');

  // Saját fejléc elemek felvétele.
  $variables['head'] .= '
 <meta name="author" content="Weblabor" />
 <meta http-equiv="Content-Language" content="hu" />
 <meta name="robots" content="index, follow" />
 <meta name="revisit-after" content="7 days" />
 <meta name="verify-v1" content="xVGI+oagDma6DwB0tjQrwPASRzHtb4h+9V61Y/TaeUA=" />
 <link rel="start" title="Weblabor - A fejlesztői forrás" href="/" />
 <link rel="search" title="Weblabor keresés" href="/kereses" />
 <link rel="search" type="application/opensearchdescription+xml" title="Weblabor keresés" href="http://weblabor.hu/misc/search/opensearch.xml" />
 <link rel="license" title="Weblabor licenc" href="http://creativecommons.org/licenses/by-nc-sa/2.0/" />
 <!--
 <rdf:RDF xmlns="http://web.resource.org/cc/"
     xmlns:dc="http://purl.org/dc/elements/1.1/"
     xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
  <Work rdf:about="">
   <dc:type rdf:resource="http://purl.org/dc/dcmitype/Text" />
   <license rdf:resource="http://creativecommons.org/licenses/by-nc-sa/2.0/" />
  </Work>
  <License rdf:about="http://creativecommons.org/licenses/by-nc-sa/2.0/">
   <permits rdf:resource="http://web.resource.org/cc/Reproduction" />
   <permits rdf:resource="http://web.resource.org/cc/Distribution" />
   <requires rdf:resource="http://web.resource.org/cc/Notice" />
   <requires rdf:resource="http://web.resource.org/cc/Attribution" />
   <prohibits rdf:resource="http://web.resource.org/cc/CommercialUse" />
   <permits rdf:resource="http://web.resource.org/cc/DerivativeWorks" />
   <requires rdf:resource="http://web.resource.org/cc/ShareAlike" />
  </License>
 </rdf:RDF>
 -->
';

  // Kulcsszavak képzése a címből, a túl rövid szavaktól és számoktól eltekintve.
  $keywords_array = preg_split("/[\s,;\.\-]+/", $raw_title .' programozás, informatika, web, fejlesztés');
  foreach ($keywords_array as $i => $keyword) {
    if (strlen($keyword) <= 2 or preg_match("/^[IVX]+$/", $keyword)) {
      unset($keywords_array[$i]);
    }
  }
  $variables['head'] .= "\n".'<meta name="keywords" content="'. join(', ', $keywords_array) .'" />';

  // Description elem csak a honlapra. A többi oldalra a keresők generáljanak.
  if ($_GET['q'] == variable_get('site_frontpage', 'node')) {
    $variables['head'] .= "\n".'<meta name="description" content="Weblabor: cikkek, blog, fórumok, levelezőlisták, linkek" />';
  }

  // A felhasználók kikapcsolhatják a bbeditort.
  if (!empty($user->bbeditordisable)) {
    $variables['body_classes'] .= ' no-rich-controls';
  }

  // Belépett felhasználók alapvető infója és belépés link külön panelen.
  $variables['userpanel'] = ($user->uid != 0) ? 'Belépve '. l($user->name, 'user/'. $user->uid) .' néven. '. l('Kilépés', 'logout') : l('Belépés', 'user/login', array('query'=> array('destination' => $_GET['q']))) .' vagy '. l('regisztráció', 'user/register');

  // Fő navigáció.
  $nav = array(
    'blog'       => array('Blog', 'Rövidebb írásaink'),
    'blogmarkok' => array('Blogmarkok', 'Más weboldalakat ajánlunk'),
    'cikkek'     => array('Cikkek', 'Hosszabb lélegzetvételű írásaink'),
    'forumok'    => array('Fórumok', 'A közösség kibontakozása'),
    'levlistak'  => array('Levlisták', 'Levezőlista archívumok, feliratkozás'),
    'munka'      => array('Munka és állás', 'Webes állás és munkahirdetések'),
    'konyvek'    => array('Könyvek', 'Általunk olvasott könyvek'),
    'koveto'     => array('Friss', 'Minden friss tartalom')
  );
  $page_type = 'page-normal';
  $output = '<ul><li>';
  foreach ($nav as $path => $item) {
    $attributes = array('title' => $item[1]);
    // $_SERVER['REQUEST_URI'] ugyan user provided, ezért nem biztonságos
    // információ forrás, de itt nem kell XSS miatt aggódni, ezért ezt
    // használjuk (jobbat nem tudunk).
    if (preg_match('!^/'. $path .'($|/.|\\?)!', $_SERVER['REQUEST_URI'], $found_in_uri)) {
      $attributes['class'] = 'active';
      if (!empty($found_in_uri[1])) {
        $page_type = 'page-wide';
      }
    }
    $nav[$path] = l($item[0], $path, array('attributes' => $attributes));
  }
  $output .= join('</li><li>', array_values($nav));
  $output .= '</li></ul>';
  $variables['page_navigation'] = $output;
  $variables['body_classes'] .= ' '. $page_type;

  if ($page_type == 'page-wide') {
    // PHP-ből mergelünk, elkerülendő a CSS trükközést.
    $variables['right'] .= $variables['left'];
    unset($variables['left']);
    // A tagadelic blokk menjen a végére.
    if (preg_match(',(div id="block-tagadelic-12".*<!--/block-tagadelic-12--),s', $variables['right'], $tags_block)) {
      $variables['right'] = str_replace('<'. $tags_block[1] .'>', '', $variables['right']);
      $variables['right'] .= '<'. $tags_block[1] .'>';
    }
    /*if (preg_match(',(<div id="block-blogmark-0".*<!--/block-blogmark-0-->),s', $variables['right'], $tags_block)) {
      str_replace($tags_block[1], '', $variables['right']);
      $variables['right'] .= $tags_block[1];
    }*/

  }

  // @todo: itt viszont XSS veszélyes kívülről megadott adatokat használni(?)
  $request_uri = 'http://weblabor.hu'. urlencode(preg_replace('/&/', '&amp;', $_SERVER['REQUEST_URI']));
  $variables['footer'] = isset($variables['footer']) ? $variables['footer'] : ''.'
 <div id="licence-issn">
  (CC) Weblabor, 1999–2011<br />
  HU ISSN 1785-9573
 </div>
 <div id="footercontent">
  <div id="hosting-logo"><a href="http://phphost.hu/" title="Tárhely: PHPHost"><img src="/sites/all/themes/wl/images/phphost.png" alt="PHPHost logó" /></a></div>
  <a href="/weblabor/impresszum">Impresszum</a> &middot;
  <a href="/weblabor/joginyilatkozat">Jogi nyilatkozat</a> &middot;
  <a href="/weblabor/adatvedelem">Adatvédelem</a> &middot;
  <a href="/weblabor/mediaajanlat">Médiaajánlat</a><br />
  <a href="/weblabor/rolunk">Rólunk</a> &middot;
  <a href="/weblabor/terkep">Webhely térkép</a> &middot;
  <a href="/weblabor/segitseg">Gyakran Ismételt Kérdések</a>
 </div>
</div>
<div class="only-in-print print-footer">
 Ennek az oldalnak az eredeti elérhetősége: <strong>'. $request_uri .'</strong><br />
 A Weblabor (HU ISSN 1785-9573) lapjain megjelenő tartalmak a Creative Commons
 Attribution-NonCommercial-ShareAlike 2.0 licenc alatt érhetőek el, és
 használhatóak fel. A licenc a
 <strong>http://creativecommons.org/licenses/by-nc-sa/2.0/</strong> címen olvasható.
';

  // Google analytics, admin oldalakon nem.
  if (!preg_match("!^(admin|aa)!", $_GET['q'])) {
    $variables['closure'] .= '
 <script src="http://www.google-analytics.com/urchin.js" type="text/javascript"></script>
 <script type="text/javascript"> _uacct = "UA-81958-1"; urchinTracker();</script>';
  }

  // Gyors tartalom cím szerkeszthetőség (admin.js-sel és activeadmin.module-lal).
  $variables['page_title_attrib'] = 'class="title" id="page-title"';
  if (user_access('access administration pages')) {
    $variables['scripts'] .= '<script type="text/javascript" src="/sites/all/themes/wl/admin.js"></script>';
    if (!empty($variables['title']) && preg_match('!^node/(\\d+)($|/\\d)!', $_GET['q'], $found)) {
      // Node címeknél felvesszük a node id-t az id-be, hogy tudjuk menteni.
      $variables['page_title_attrib'] = 'class="title node-title-editable" id="node-title-'. $found[1] .'"';
    }
  }

  // Címlapon ne legyen breadcrumb.
  if (variable_get('site_frontpage', 'node') == $_GET['q']) {
    unset($variables['breadcrumb']);
  }
}

/**
 * Node megjelenítése.
 *
 * Nem éri meg a node.tpl.php-vel foglalkozni, mert alig van benne valami,
 * és nekünk sokkal több logika kell itt.
 */
function wl_node($node, $teaser = FALSE, $page = FALSE) {
  $output  = '<div class="node clear-block '.
             ((!$node->status) ? 'node-unpublished ' : '') .
             ($node->sticky ? 'sticky ' : '') . $node->type ."\">\n";

  $output .= theme('mark', node_mark($node->nid, $node->created));

  if (!$page) {
    // Node cím megjelenítése, ha nem teljes oldalon vagyunk.
    $output .= " <h2 class=\"title\">". ($teaser ? l($node->title, 'node/'. $node->nid, array('attributes' => array('id' => 'node-title-'. $node->nid, 'class' => 'node-title-editable'))) : check_plain($node->title)) ."</h2>\n";
  }

  // Statisztika számláló rövidebben.
  $visitcount = '';
  if (isset($node->links['statistics_counter']) && preg_match('!^(\\d+) !', $node->links['statistics_counter']['title'], $found)) {
    $visitcount = l($found[1] .' olvasás', 'node/'. $node->nid .'/track', array('attributes' => array('class' => 'statistics_counter', 'title' => 'A legutóbbi látogatások naplója.')));
  }
  unset($node->links['statistics_counter']);
  // A blogmark kattintás számálót rakjuk mellé.
  if (isset($node->links['blogmark_counter'])) {
    $visitcount .= ', '. $node->links['blogmark_counter']['title'] .' látogatás';
  }
  unset($node->links['blogmark_counter']);
  $visitcount = ($visitcount ? ' &middot; <span class="visitcount">('. $visitcount .')</span>' : '');

  // Meta információk a felhasználó képével és a megjelenés idejével.
  $output .= '<div class="meta">'. theme('username', $node, TRUE) .' &middot; <span title="A megjelenés ideje: '. check_plain(wl_formatted_date($node->created, 'complete')) .'">'. wl_formatted_date($node->created) .'</span>'. $visitcount ."</div>\n";

  // Kezdődik a tartalom.
  $output .= " <div class=\"content\">\n";
  $qed = '&nbsp;<abbr title="Vége a bejegyzésnek." class="qed">■</abbr>';
  if ($teaser && $node->teaser) {
    $output .= $node->teaser;
    if (!$node->readmore) {
      $output .= $qed;
    }
  }
  else {
    // A csatolmányokat így nem jelenítjük meg.
    $output .= preg_replace('!<table id="attachments"(.*)</table>!Us', '', $node->body);
    $output .= $qed;
  }
  $output .= " </div>\n";

  // További olvasás link eltűntetése.
  if (isset($node->links['node_read_more'])) {
    unset($node->links['node_read_more']);
  }

  // Hozzászólás számok rövidítése.
  if (isset($node->links['comment_comments']) && preg_match('!^(\\d+) !', $node->links['comment_comments']['title'], $found)) {
    $node->links['comment_comments']['title'] = $found[1];
    $node->links['comment_comments']['attributes']['title'] = $found[1] .' hozzászólás. Ugrás az elsőre.';
  }
  if (isset($node->links['comment_new_comments']) && preg_match('!^(\\d+) !', $node->links['comment_new_comments']['title'], $found)) {
    $node->links['comment_new_comments']['attributes']['class'] = 'comments-new';
    $node->links['comment_comments'] = array(
      'title' => l($node->links['comment_comments']['title'], $node->links['comment_comments']['href'], array('attributes' => $node->links['comment_comments']['attributes'])) .', '. l($found[1] .' új', $node->links['comment_new_comments']['href'], array('attributes' => $node->links['comment_new_comments']['attributes'], 'fragment' => "new")),
      'html' => TRUE
    );
    unset($node->links['comment_comments']['href']);
    unset($node->links['comment_comments']['attributes']);
    unset($node->links['comment_new_comments']);
  }
  if (isset($node->links['comment_forbidden'])) {
    // Tiltott hozzászólás esetén rejtsük el a nag szöveget.
    unset($node->links['comment_forbidden']);
  }

  // A letöltések linkjét ne jelenítsük meg.
  unset($node->links['upload_attachments']);

  // Címkék!
  $tags = array();
  if (isset($node->taxonomy)) {
    foreach ($node->taxonomy as $term) {
      if ($term->vid != WL_V_SERIES) {
        // A sorozatokat kihagyjuk a megjelenítésben.
        $tags['taxonomy_term_'. $term->tid] = array(
          'title' => $term->name,
          'href' => taxonomy_term_path($term),
          'attributes' => array('rel' => 'tag', 'title' => strip_tags($term->description))
        );
      }
    }
  }
  if (count($tags)) {
    $output .= '<div class="tags">'. theme('links', $tags, array('class' => 'links inline')) .'</div>';
  }

  if (count($node->links)) {
    // A többi link (a fenti formázások utáni állapotban).
    $output .= '<div class="links">'. theme('links', $node->links, array('class' => 'links inline')) ."</div>\n";
  }

  if ($page && $node->type == 'article') {
    // Cikk oldalakon a felhasználó bioja is megjelenik.
    $output .= theme('userbio_display', $node);
  }

  // Ennyi volt, vége a node-nak.
  $output .= "</div>\n";

  // Morzsákat típus szerint kell beállítani.
  if ($page && $node->type != 'forum') {
    $breadcrumb = array(l(t('Home'), NULL));
    switch ($node->type) {
      case 'blogpost':
        $breadcrumb[] = l('Blog', 'blog');
        break;
      case 'blogmark':
        $breadcrumb[] = l('Blogmarkok', 'blogmarkok');
        break;
      case 'article':
        $breadcrumb[] = l('Cikkek', 'cikkek');
        break;
      case 'job':
        $term = array_shift(array_values($tags));
        $breadcrumb[] = l('Munka és állás', 'munka');
        $breadcrumb[] = l($term['title'], $term['href'], array('attributes' => $term['attributes']));
        break;
      case 'bookreview':
        $breadcrumb[] = l('Könyvek', 'konyvek');
        break;
    }
    drupal_set_breadcrumb($breadcrumb);
  }

  return $output;
}

function wl_comment($comment, $node, $links = array()) {
  static $ccounter = 0;

  // Init comment counter
  if ($ccounter == 0) {
    wl_comment_counter($comment->nid);
  }

  $links['forum_topic'] = array(
    'title' => 'új téma',
    'href' => 'forumok/bekuldes',
  );

  $ccounter++;
  $output  = '<div class="comment clear-block'. ($comment->status == COMMENT_NOT_PUBLISHED ? ' comment-unpublished' : '') . (($ccounter % 2 == 0) ? ' even' : ' odd') .'">';
  $output .= "<div class=\"commentnum\">". wl_comment_counter(NULL, $comment->cid) ."</div>". theme('mark', $comment->new, TRUE);
  $output .= '<h3 class="subject">'. l($comment->subject, $_GET['q'], array('fragment' => 'comment-'. $comment->cid)) ." </h3>\n";
  $output .= '<div class="meta">'. theme('username', $comment, TRUE) .' &middot; <span title="A megjelenés ideje: '. check_plain(wl_formatted_date($comment->timestamp, 'complete')) .'">'. wl_formatted_date($comment->timestamp) ."</span></div>\n";
  $output .= '<div class="content">'. $comment->comment .'</div>';
  $output .= '<div class="links">'. theme('links', $links, array('class' => 'links inline')) .'</div>';
  $output .= '</div>';
  return $output;
}

// RSS kép HTML kódját generálja.
function wl_rss_image($info, $type = 'n', $withid = FALSE) {
  $ttype = ($type == 'n' ? 'Tartalom' : 'Hozzászólások');
  $image = ($type == 'n' ? '' : 'c');
  $id    = ($withid ? " id=\"tool-rss-$type\"" : '');
  $class = ($id ? "" : " class=\"feed-icon\"");
  $title = (is_array($info) ? $info[1] : "RSS - $ttype");
  $href  = (is_array($info) ? $info[0] : $info);
  return "<a href=\"{$href}\" title=\"{$title}\"{$class}{$id}><img src=\"/sites/all/themes/wl/images/rss{$image}.png\" alt=\"RSS - {$ttype}\" /></a>";
}


// Type marker for tracker
function wl_type_marker($nodetype, $justfilename = FALSE) {
  $nodeicons = array('blogmark', 'job', 'blogpost', 'article', 'forum', 'poll', 'bookreview');
  if (in_array($nodetype, $nodeicons)) {
    if ($justfilename) {
      return $nodeicons[$nodetype];
    }
    $typename = node_get_types('name', $nodetype);
    $image = '<img src="/sites/all/themes/wl/images/node-'. $nodetype .'.png" alt="'. $typename .'" title="'. $typename .'" />';
    return '<div class="typemarker" title="'. $typename .'">'. $image .'</div>';
  }
}

function wl_breadcrumb($list = array()) {
  // Some job taxonomy page viewed
  if (in_array($_GET['q'], array('taxonomy/term/'. JOB_OFFERED, 'taxonomy/term/'. JOB_LOOKEDFOR))) {
    $list = array(
      l(t('Home'), NULL),
      l('Munka és állás', 'munka'),
    );
  }
  // Some tag page viewed (we don't use taxonomy pages for other purposes)
  elseif (preg_match('!^taxonomy/term/(\\d+)$!', $_GET['q'])) {
    $list = array(
      l(t('Home'), NULL),
      l('Címkék', 'cimkek'),
    );
  }
  if (!empty($list)) {
    return '<div class="breadcrumb">'. implode(' » ', $list) .'</div>';
  }
}

/**
 * Profile előfeldolgozó.
 */
function wl_preprocess_user_profile(&$variables) {
  if (userbio_canhave($variables['account']) && isset($variables['account']->userbio) && strlen($variables['account']->userbio)) {
    $variables['user_profile'] = theme('userbio_display', $variables['account'], FALSE, TRUE) . $variables['user_profile'];
  }
}

function wl_formatted_date($ts, $type = 'short') {
  switch ($type) {
    case 'complete':
      return str_replace("május.", "máj.", format_date($ts, 'custom', 'Y. M. j. (D), H.i'));
    case 'short':
      if (date("Yz") == date("Yz", $ts)) {
        return format_date($ts, 'custom', 'H.i');
      }
      elseif (date("YW") == date("YW", $ts)) {
        return format_date($ts, 'custom', 'l, H.i');
      }
      elseif (date("Y") == date("Y", $ts) || (date("Y") == (date("Y", $ts) + 1) && date("n") < 6 && date("n", $ts) > 5)) {
        return format_date($ts, 'custom', 'M. j. (D), H.i');
      }
      else {
        return str_replace("május.", "máj.", format_date($ts, 'custom', 'Y. M. j. (D), H.i'));
      }
      break;
    case 'day':
      if (date("Yz") == date("Yz", $ts)) {
        return 'ma';
      }
      elseif (date("Yz") == date("Yz", $ts - 60*60*24)) {
        return 'holnap';
      }
      elseif (floor($ts/60*60*24) - floor(time()/60*60*24) < 7) {
        return format_date($ts, 'custom', 'D');
      }
      else {
        return str_replace("május.", "máj.", format_date($ts, 'custom', 'M. j. (l)'));
      }
      break;
  }
}

// Avatar
function wl_username($object, $withavatar = FALSE) {
  if ($object->uid && $object->name) {
    // Shorten the name when it is too long or it will break many tables.
    /*if (drupal_strlen($object->name) > 20) {
      $name = drupal_substr($object->name, 0, 15) .'...';
    }
    else {*/
      $name = $object->name;
    //}

    if (user_access('access user profiles')) {
      $attributes = array('title' => t('View user profile.'));
      if ($withavatar) {
        $avatar = WL_DEFAULT_AVATAR;
        if (isset($object->wl_avatar)) {
          $avatar = $object->wl_avatar;
        }
        elseif (isset($object->data)) {
          $data = unserialize($object->data);
          $avatar = (!empty($data['wl_avatar']) ? $data['wl_avatar'] : $avatar);
        }
        $attributes += array('class' => 'wlavatar', 'style' => 'background-image: url(/misc/avatar/'. sprintf("%02d", $avatar) .'.gif)');
      }
      $output = l($name, 'user/'. $object->uid, array('attributes' => $attributes));
    }
    else {
      $output = check_plain($name);
    }
  }
  else if ($object->name) {
    // Sometimes modules display content composed by people who are
    // not registered members of the site (e.g. mailing list or news
    // aggregator modules). This clause enables modules to display
    // the true author of the content.
    if ($object->homepage) {
      $output = l($object->name, $object->homepage);
    }
    else {
      $output = check_plain($object->name);
    }

    $output .= ' ('. t('not verified') .')';
  }
  else {
    $output = variable_get('anonymous', t('Anonymous'));
  }

  return $output;
}

function wl_avatar_image($uid) {
  static $avatars = array();
  if (!count($avatars)) {
    $result = db_query("SELECT uid, data FROM {users} WHERE data != 'N;'");
    while ($row = db_fetch_array($result)) {
      $ud = unserialize($row['data']);
      if (isset($ud['wl_avatar'])) {
        $avatars[$row['uid']] = $ud['wl_avatar'];
      }
    }
  }
  if ($uid == 0) {
    return 0;
  }
  else {
    return (isset($avatars[$uid]) ? $avatars[$uid] : WL_DEFAULT_AVATAR);
  }
}

function wl_comment_counter($nid = NULL, $cid = NULL) {
  static $comments = NULL;
  if (!isset($comments) && isset($nid)) {
    $comments = array();
    $counter = 1;
    $result = db_query("SELECT cid FROM {comments} WHERE nid = %d ORDER BY timestamp ASC", $nid);
    while ($comment = db_fetch_object($result)) {
      $comments[$comment->cid] = $counter++;
    }
  }
  elseif (!isset($nid) && isset($comments) && $cid) {
    return $comments[$cid];
  }
}

/**
 * Újdonságok jelzése.
 */
function wl_mark($type = MARK_NEW, $comment = FALSE) {
  global $user;
  if ($user->uid) {
    if ($type == MARK_NEW) {
      return ' <span class="marker marker-new'. ($comment ? ' marker-comment' : '') .'">*</span>';
    }
    else if ($type == MARK_UPDATED) {
      return ' <span class="marker marker-updated'. ($comment ? ' marker-comment' : '') .'">^</span>';
    }
  }
}

/**
 * Ugyanaz, mint theme_archive_separator, de rd, th, stb. nélkül.
 */
function wl_archive_separator($date_created, $separators) {
  $date_sep = '';
  if ($separators['year'] && $separators['month'] && $separators['day']) {
    $date_sep = format_date($date_created, 'custom', 'F j, Y');
  }
  else if ($separators['month'] && $separators['day']) {
    $date_sep = format_date($date_created, 'custom', 'F j');
  }
  else if ($separators['day']) {
    $date_sep = format_date($date_created, 'custom', 'F j');
  }
  return '<h3>'. $date_sep .'</h3>';
}

function wl_theme($existing, $type, $theme, $path) {
  return array(
    'rss_image' => array(
      'arguments' => array('info' => NULL, 'type' => NULL, 'withid' => NULL),
    ),
  );
}
