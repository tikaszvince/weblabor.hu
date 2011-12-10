<?php

// TODO: type markerek!
function _pages_sitemap() {
  $rss = array(
    'home'      => '',
    'edited'    => '/szerkesztett',
    'articles'  => '/cikkek',
    'blog'      => '/blog',
    'books'     => '/konyvek',
    'blogmarks' => '/blogmarkok',
    'forums'    => '/forumok',
    'polls'     => '/szavazasok',
    'job'       => '/munka',
    'jobo'      => '/munka/kinal',
    'jobs'      => '/munka/keres',
  );
  foreach ($rss as $key => $path) {
    $rss[$key] = theme('rss_image', "/rss{$path}", 'n') . ((strpos($key, 'job') !== 0) ? ' '. theme('rss_image', "/rss/hozzaszolas{$path}", 'c') : '');
  }
  
  $content = <<<TERKEP
<div id="static-page">
<p>
 Az alábbiakban a Weblabor webhely szerkezeti térképét látható. Azon kívül,
 hogy a tartalmak típusok szerinti rendszerben is elérhetőek, mindent
 felölelő címkézést használunk, így a címkék mentén a tartalmak más-más
 szelete érhető el.
</p>

<ul class="menu" id ="sitemap">
 <li class="expanded">
  <a href="/" class="main">Címlap</a> minden tartalom: {$rss['home']} vagy csak a szerkesztettek: {$rss['edited']}
  <ul>
 
   <li class="leaf"><a href="/bekuldes" class="main">Tartalom beküldés</a></li>
   <li class="leaf"><a href="/cikkek" class="main">Cikkek</a> {$rss['articles']}</li>
   <li class="leaf"><a href="/blog" class="main">Blog</a> {$rss['blog']}</li>
   <li class="leaf"><a href="/blogmarkok" class="main">Blogmarkok</a> {$rss['blogmarks']}</li>
   <li class="leaf"><a href="/konyvek" class="main">Könyvek</a> {$rss['books']}</li>
   <li class="expanded">
    <a href="/munka" class="main">Munka/állás</a> {$rss['job']}
    <ul>
     <li class="leaf"><a href="/munka/kinal">Kínál</a> {$rss['jobo']}</li>
     <li class="leaf"><a href="/munka/keres">Keres</a> {$rss['jobs']}</li>
    </ul>
   </li>
   <li class="expanded">
    <a href="/forumok" class="main">Fórumok</a> {$rss['forums']}
    <ul>
     <li class="leaf"><a href="/forumok/idoszerint">Fórumok időrendben</a></li>
    </ul>
   </li>
   <li class="leaf"><a href="/levlistak" class="main">Levelezőlisták</a></li>
   <li class="leaf"><a href="/szavazasok" class="main">Szavazások</a> {$rss['polls']}</li>
   <li class="leaf"><a href="/hirlevel" class="main">Hírlevél</a></li>
   <li class="expanded">
    <strong>Navigáció</strong>
    <ul>
     <li class="leaf"><a href="/cimkek">Címkék</a></li>
     <li class="leaf"><a href="/koveto">Friss tartalom</a></li>
     <li class="leaf"><a href="/archivum">Archívum</a></li>
    </ul>
   </li>
   <li class="expanded">
    <strong>Weblabor</strong>
    <ul>
     <li class="leaf"><a href="/weblabor/impresszum">Impresszum</a></li>
     <li class="leaf"><a href="/weblabor/rolunk">Bővebben rólunk</a></li>
     <li class="leaf"><a href="/weblabor/rolunk#csoportok">Felhasználói csoportok</a></li>
     <li class="leaf"><a href="/weblabor/joginyilatkozat">Jogi nyilatkozat</a></li>
     <li class="leaf"><a href="/weblabor/adatvedelem">Adatvédelem</a></li>
     <li class="leaf"><a href="/weblabor/mediaajanlat">Médiaajánlat</a></li>
     <li class="leaf"><a href="/weblabor/segitseg">Segítség</a></li>
    </ul>
   </li>
  </ul>
 </li>
</ul>
</div>
TERKEP;
  return $content;
}

function _pages_feed_all($arg1 = NULL, $arg2 = NULL) {
  if (isset($arg1)) {
    switch ($arg1) {
      case 'miez':
        return _pages_feed_info();
        break;
      case 'szerkesztett':
        return _pages_edited_feed();
        break;
      case 'hozzaszolas':
        if ($arg2 == 'szerkesztett') {
          return _pages_edited_comment_feed();
        }
        break;
    }
  }
  elseif (!isset($arg2)) {
    // Teljes feed ha sem arg1 sem arg2 nincs.
    $result = db_query_range('SELECT nid FROM {node} WHERE status = 1 ORDER BY created DESC', 0, 15);
    $nids = array();
    while ($node = db_fetch_object($result)) {
      $nids[] = $node->nid;
    }
    return node_feed($nids);
  }
  return drupal_not_found();
}

// Szerkesztett tartalmak csatornája
function _pages_edited_feed() {
  $SQL = "SELECT DISTINCT(n.nid), n.type, n.created, n.changed, n.uid, n.sticky, u.name FROM {node} n, {term_node} tn, {term_data} td, {users} u WHERE n.nid = tn.nid AND tn.tid = td.tid AND n.uid = u.uid AND n.status = '1' AND n.type NOT IN('forum', 'job') ORDER BY n.sticky DESC, n.created DESC";
    
  $result = db_query_range($SQL, 0, 15);
  $nids = array();
  while ($node = db_fetch_object($result)) {
    $nids[] = $node->nid;
  }
  $channel = array(
    "link"        => url('', array('absolute' => TRUE)),
    "title"       => "Weblabor - Szerkesztett tartalmak",
    "description" => "A Weblabor szerkesztett tartalmainak újdonságai, azaz a fórumok és hirdetések kívételével minden megjelent újdonság csatornája."
  );
  node_feed($nids, $channel);
}

// Szerkesztett tartalmakhoz érkezett hozzászólások
function _pages_edited_comment_feed() {
  $items = commentrss_format_items('n.nid', '', 'type NOT IN ("forum", "job") AND');
  $channel = array(
    "link"        => url('', array('absolute' => TRUE)),
    'title'       => 'Weblabor - Szerkesztett tartalmak - Hozzászólások',
    "description" => "A Weblabor szerkesztett tartalmaira érkezett hozzászólások, azaz a fórumok hozzászólásai kívételével minden megjelent vélemény csatornája."
  );
  commentrss_format_feed($items, $channel);
}

// Szerkesztett tartalmakhoz érkezett hozzászólások
function _pages_comment_feed_bytype($type = NULL) {
  $typename = node_get_types('name', $type);
  $items = commentrss_format_items('n.nid', '', 'type = "%s" AND', $type);
  $channel = array(
    'link'        => url('nodesbytype/'. $type, array('absolute' => TRUE)),
    'title'       => 'Weblabor - '. wllib_plural_typename($typename) .' - Hozzászólások',
    //'description' => '...'
  );
  commentrss_format_feed($items, $channel);
}

// RSS magyarázó oldal
function _pages_feed_info() {
  global $base_url;
  $content = '<div id="static-page">';
  $request_uri = $base_url . $_SERVER['REQUEST_URI'];
  if (!empty($_GET['href']) && preg_match('!^'. preg_quote($base_url, '!') .'/(rss[a-z0-9/]*)$!', $_GET['href'], $found)) {
    $content .= "<p class=\"messages\">Ha nem szorulsz további magyarázatra, íme a forrás:<br /><a href=\"/{$found[1]}\">". $base_url ."/{$found[1]}</a>.</p>";
  }
  return $content . file_get_contents('sites/all/modules/pages/feed_info.html');
}

// Tartalom listázás node típus szerint
function _pages_nodes_bytype($type, $isfeed = FALSE) {
  
  // Biztonság a típus megadásához
  if (!preg_match("!^[a-z-]+$!", $type)) {
    drupal_not_found();
    return;
  }
  
  $query = "SELECT DISTINCT(n.nid), n.sticky, n.created FROM {node} n WHERE n.type = '%s' AND n.status = 1 ORDER BY n.sticky DESC, n.created DESC";
  $query_count = "SELECT COUNT(*) FROM {node} n WHERE n.type = '%s' AND n.status = 1 ORDER BY n.sticky DESC, n.created DESC";
  $typename = node_get_types('name', $type);
  
  if ($isfeed) {
    $result = db_query_range($query, $type, 0, 15);
    $nids = array();
    while ($node = db_fetch_object($result)) {
      $nids[] = $node->nid;
    }
    $channel = array(
      "link"        => url('nodesbytype/'. $type, array('absolute' => TRUE)),
      "title"       => 'Weblabor - '. wllib_plural_typename($typename),
      // "description" => '...'
    );
    node_feed($nids, $channel);
  }
  else {
    drupal_add_feed(url("nodesbytype/feed/$type", array('absolute' => TRUE)), 'RSS - '. wllib_plural_typename($typename));
    
    if (db_result(db_query($query_count, $type))) {
      $result = pager_query($query, variable_get('default_nodes_main', 10), 0, $query_count, $type);
      $output = '';
      while ($node = db_fetch_object($result)) {
        $output .= node_view(node_load($node->nid), TRUE);
      }
      $output .= theme("pager", NULL, variable_get('default_nodes_main', 10));
      drupal_set_title(wllib_plural_typename($typename));
      return $output;
    }
    else {
      drupal_not_found();
      return;
    }
  }
}
