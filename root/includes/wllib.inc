<?php

// Nyelvi információ (a META elemnek megfelelően)
header("Content-language: hu");

/**
 * Konstansok a különböző szótárak azonosítására. Legalább máshol ne kelljen
 * ezeket a számokat bedrótozni. A Drupalnak különben szinte mindegyikre van
 * változója is.
 */
define("WL_V_FORUMS",     3); // forum_nav_vocabulary
define("WL_V_SERIES",     7); // series_nav_vocabulary
define("WL_V_JOBS",      11);
define("WL_V_TAGS",      12);

/**
 * Munka/állás ügyi term azonosítók
 */
define("JOB_OFFERED",   228);
define("JOB_LOOKEDFOR", 229);

/**
 * Többesszámos magyar tartalom típus nevek (pl. RSS csatornához)
 */
function wllib_plural_typename($typename) {
  switch($typename) {
    case 'Blog bejegyzés' :         return 'Blog bejegyzések';
    case 'Cikk' :                   return 'Cikkek';
    case 'Könyvajánló' :            return 'Könyvajánlók';
    case 'Blogmark' :               return 'Blogmarkok';
    case 'Szavazás' :               return 'Szavazások';
    case 'Fórum téma' :             return 'Fórum témák';
    default :                       return $typename;
  }
}

/**
 * Visszatér az automatikus URL átalakítás során használt megfelelők
 * tömbjével. A bemeneti és kimeneti URL átalakításnál is szükség van
 * rá.
 */
function wllib_get_autoaliases() {
  return array(
    "forum/"          => "forumok/",
    "archive"         => "archivum",
  );
}

/**
 * A felhasználókhoz tartozó alcímek leképezése.
 */
function wllib_user_autoaliases() {
  return array(
    ''                       => '',
    '/edit'                  => '/szerkesztes',
    '/edit/Személyes adatok' => '/szerkesztes/szemelyes',
    '/edit/hirlevel'         => '/szerkesztes/hirlevel',
    '/track'                 => '/kovetes',
    '/track/navigation'      => '/kovetes/navigacio',
    '/contact'               => '/kapcsolat',
  );
}

/**
 * Visszaadja, hogy a kifejezések mely szótárakban vannak, illetve,
 * hogy egy konkrét kifejezés mely szótárban van. Elsősorban URL
 * átírásokra jó, mert így deríthető ki például, hogy ha egy
 * bejegyzés a blog szótárába van besorolva, akkor milyen URL
 * aliasnak kell hozzárendelődnie.
 */
function wllib_get_taxo_vocabs($taxoid = NULL) {
  static $cache = 0;
  static $map = array();
  if (!$cache) {
    $result = db_query("SELECT tid, vid FROM {term_data}");
    while ($data = db_fetch_object($result)) {
      $map[$data->tid] = $data->vid;
    }
    $cache = 1;
  }
  if (isset($taxoid)) {
    return $map[$taxoid];
  }
  else {
    return $map;
  }
}

/**
 * Node szintű metainformáció tárolás a gyorsabb álnév képzéshez
 */
function wllib_node_meta($nid, $type = NULL, $alias = NULL) {
  static $node_meta_cache = array();

  // Set node meta information
  if (isset($type) && isset($alias)) {
    $node_meta_cache[$nid] = array($type, $alias);
  }
  // Get node meta information, if available
  elseif (isset($node_meta_cache[$nid])) {
    return $node_meta_cache[$nid];
  }
  // Grab from database otherwise
  else {
    $node = db_fetch_object(db_query(db_rewrite_sql('SELECT n.type, p.dst FROM {node} n LEFT JOIN {url_alias} p ON p.src = CONCAT("node/", n.nid) WHERE n.nid = %d'), $nid));
    return ($node_meta_cache[$nid] = array($node->type, $node->dst));
  }
  return FALSE;
}

/**
 * A Drupal beépített drupal_get_path_alias() teljes újraimplementálása, hogy
 * kevés újrahívást alkalmazzon, támogassa a saját URL álneveinket, és lehetőleg
 * kevés memóriát fogyasszon.
 */
function drupal_get_path_alias($path) {
  static $node2type     = NULL;
  static $taxo2vocab    = NULL;
  static $map           = NULL;
  static $autoalias     = NULL;
  static $userautoalias = NULL;
  static $nodeurltypes  = NULL;

  // Lokális gyorstár feltöltése, ha nincs még
  if (is_null($map)) {

    // src -> dst összerendelés
    $map = array();
    $result = db_query('SELECT * FROM {url_alias} WHERE LEFT(src, 5) != "node/" OR LEFT(src, 8) = "node/add"');
    while ($data = db_fetch_object($result)) {
      $map[$data->src] = $data->dst;
    }

    // Egyirányban használt aliasok (befelé másra képződik le)
    $map['poll']             = 'szavazasok';
    $map['nodesbytype/poll'] = 'szavazasok';
    $map['nodesbytype/job']  = 'munka';
    $map['nodesbytype/blogmark']  = 'blogmarkok';
    $map['nodesbytype/blogpost']  = 'blog';
    $map['nodesbytype/article']  = 'cikkek';
    $map['nodesbytype/book']  = 'konyvek';
    $map['nodesbytype/forum']  = 'forumok';
    $map['node/add']         = 'bekuldes';
    $map['node/feed']        = 'rss';
    $map['tagadelic']        = 'cimkek';
    $map['taxonomy/term/'. JOB_OFFERED] = 'munka/kinal';
    $map['taxonomy/term/'. JOB_LOOKEDFOR] = 'munka/keres';

    // Az összerendelésekhez szükséges tömbök
    $taxo2vocab    = wllib_get_taxo_vocabs();
    $autoalias     = wllib_get_autoaliases();
    $userautoalias = wllib_user_autoaliases();

    $nodeurltypes = array(
      "forum"    => "forumok/temak",
      "blogmark" => "blogmarkok",
      "poll"     => "szavazasok",
      "job"      => "munka",
    );
  }

  // Az elérés benne van a path mapben, nem kell számítani
  if (isset($map[$path])) {
    return $map[$path];
  }

  // Ilyen URL akkor jön, amikor node beküldés oldalon vagyunk, és az alias
  // mezőben valamilyen alapértelmezett aliast meg kell jeleníteni
  if ($path == 'node/') {
    switch($_GET['q']) {
      case 'node/add/blogpost'   : return "blog/" . date("Ymd", time()) . "/"; break;
      case 'node/addd/article'   : return "cikkek/"; break;
      case 'node/add/bookreview' : return "konyvek/"; break;
      default: return ""; break;
    }
  }

  // Egy konkrét node, aminek még nincs gyorstárazott álneve
  elseif (preg_match("!^node/(\\d+)$!", $path, $nodeid)) {
    $meta = wllib_node_meta($nodeid[1]);
    if (is_array($meta) && count($meta) == 2) {
      if (strlen($meta[1])) {
        return ($map[$path] = $meta[1]);
      }
      elseif ($nodeurltypes[$meta[0]]) {
        return ($map[$path] = $nodeurltypes[$meta[0]] . '/'.  $nodeid[1]);
      }
      else {
        return ($map[$path] = "olvasas/$nodeid[1]");
      }
    }
    else {
      return ($map[$path] = "olvasas/$nodeid[1]");
    }
  }

  // Felhasználók oldalai
  elseif (preg_match("!^user/(\\d+)(.*)$!", $path, $match)) {
    if (isset($userautoalias[$match[2]])) {
      return ($map[$path] = 'tagok/' . $match[1] . $userautoalias[$match[2]]);
    }
  }

  // Valamilyen taxonómia oldal
  elseif (preg_match("!^taxonomy/term/(\\d+)(/0)?(/feed)?$!", $path, $taxodata)) {
    list(, $taxoid, , $ptype) = $taxodata;
    if (isset($taxo2vocab[$taxoid])) {
      $prefix = ($ptype == '/feed' ? 'rss/' : '');
      switch($taxo2vocab[$taxoid]) {
        case WL_V_BLOG:
        case WL_V_ARTICLES:
        case WL_V_JOBS:
          return ("{$prefix}" .
            (isset($map["taxonomy/term/$taxoid"]) ?
                   $map["taxonomy/term/$taxoid"] :
                        "taxonomy/term/$taxoid"
            )
          );
        case WL_V_FORUMS:
          return ("{$prefix}" .
            (isset($map["forum/$taxoid"]) ?
                   $map["forum/$taxoid"] :
                        "forumok/$taxoid"
            )
          );
        case WL_V_POLLS     : return "{$prefix}szavazasok";
        case WL_V_BOOKS     : return "{$prefix}konyvek";
        case WL_V_BLOGMARKS : return "{$prefix}blogmarkok";
        case WL_V_TAGS      : return "{$prefix}cimkek/$taxoid";
      }
    }
  }

  // Válasz egy nodera (a node aliasa után írjuk, ami még kell)
  elseif (preg_match("!^comment/reply/(\\d+)(/\\d+)?$!", $path, $nodeid)) {
    return drupal_get_path_alias("node/$nodeid[1]") ."/hozzaszolas$nodeid[2]";
  }

  // Egy nodehoz tartozó hozzászólás feed címe
  elseif (preg_match("!^crss/node/(\\d+)$!", $path, $nodeid)) {
    return 'rss/hozzaszolas/'. drupal_get_path_alias("node/$nodeid[1]");
  }

  // Egy típushoz tartozó hozzászólás feed címe
  elseif (preg_match("!^crss/nodetype/([a-z-]+)$!", $path, $typename)) {
    return 'rss/hozzaszolas/'. drupal_get_path_alias("nodesbytype/$typename[1]");
  }

  // Egy termhez tartozó hozzászólás feed címe
  elseif (preg_match("!^crss/term/(\\d+)$!", $path, $tid)) {
    return 'rss/hozzaszolas/'. drupal_get_path_alias("taxonomy/term/$tid[1]");
  }

  // Új hozzászólás egy fórumban
  //elseif (preg_match("!^forum/(\\d+)/new$!", $path, $match)) {
  //  return "forumok/$match[1]/uj";
  //}

  // Csak a szavazásoknál előforduló URL típus (eredmenyek megtekintése)
  elseif (preg_match("!^node/(\\d+)/results$!", $path, $match)) {
    return "szavazasok/$match[1]/eredmenyek";
  }

  // Beküldő URL a fórum témákban
  elseif (preg_match("!^forum/(\\d+)/bekuldes$!", $path, $match)) {
    return drupal_get_path_alias("forum/$match[1]") .'/bekuldes';
  }

  // Automatikus fordítás különböző prefixekre, ha más nincs
  elseif (preg_match("!^(" . join("|", array_keys($autoalias)) . ")(.*)$!", $path, $match)) {
    return $autoalias[$match[1]] . $match[2];
  }

  // Nincs más, csak visszaadjuk az eredetit
  else {
      return $path;
  }
}

/**
 * Bemenetkor egyszer kell előállítani egy normal path értéket,
 * az a dolgunk, hogy ezt megkeressük.
 */
function drupal_get_normal_path($path) {

  global $user;
  $autoalias     = wllib_get_autoaliases();
  $userautoalias = wllib_user_autoaliases();

  // Kíséreljük meg adatbázisból elővenni
  $result = db_query("SELECT src FROM {url_alias} WHERE dst = '%s'", $path);
  if ($data = db_fetch_object($result)) {
    return $data->src;
  }

  // Egyébként számítani kell (ami lehet, hogy visszahívja ezt a függvényt)
  else {

    // Spéci forumok/ pathok, amit nem szabad automatikusan forum/*-ra fordítani.
    if (in_array($path, array('forumok/bekuldes', 'forumok/idoszerint'))) {
      return $path;
    }

    // Hirlevélből klikkeltek ránk (legalábbis valószínű :)
    if (preg_match('!;(nhl|hhl)$!', $path, $found)) {
      if (empty($_SERVER["HTTP_REFERER"])) {
        $_SERVER["HTTP_REFERER"] = 'COMPUTED: ' . $found[1];
      }
      return drupal_get_normal_path(str_replace(";$found[1]", '', $path));
    }

    // Backward compatibility
    if (preg_match('!^(display|send|print)article(\\d+)(-nested|-mode=thread)?.html$!', $path, $nid)) {
      wllib_status(301);
      drupal_goto("node/$nid[2]");
    }
    elseif (preg_match('!^(rssfeed|backend).php$!', $path)) {
      wllib_status(301);
      drupal_goto('rss');
    }
    elseif ($path == 'rss/hozzaszolasok') {
      wllib_status(301);
      drupal_goto('rss/hozzaszolas');
    }
    elseif (preg_match('!^Topic(\\d+)(allstories)?.html$!', $path, $tid) ||
            preg_match('!^(weblabor/rovat|taxonomy/page/or)/(\\d+)$!', $path, $tid) ) {
      wllib_status(301);
      drupal_goto("taxonomy/term/$tid[1]");
    }
    elseif ($path == 'wl-phplista/etikett' || $path == 'illik.html') {
      wllib_status(301);
      drupal_goto('levlistak/illemszabaly');
    }
    elseif ($path == 'wl-phplista' || $path == 'wl-levlista') {
      wllib_status(301);
      drupal_goto('levlistak');
    }
    elseif (preg_match('!/modules.php\\?op=modload.+name=News.+sid=(\\d+)!', $_SERVER['REQUEST_URI'], $nid) || preg_match('!^node/view/(\\d+)$!', $path, $nid)) {
      wllib_status(301);
      drupal_goto("node/$nid[1]");
    }
    elseif (preg_match('!^(modules.php|XForum|Web_Links|Sections|Messages|Downloads|Topics|Submit_News|News|Search|FAQ|NS-Polls|Top_List|leiras|plus|szolg|modules/My_eGallery|hixwebm)!', $path)) {
      wllib_status(404);
      return 'weblabor/404';
    }
    elseif (preg_match('!^(php|php-doc-chm|php/newchm\\.(php|html))!', $path)) {
      wllib_status(301);
      header("Location: http://php.net/docs-echm");
      exit;
    }
    elseif (preg_match('!^(taxonomy/feed/or|rss/weblabor/rovat)/([0-9]+)$!', $path, $feedid)) {
      wllib_status(301);
      drupal_goto("taxonomy/term/$feedid[2]/0/feed");
    }
    elseif (preg_match('!^tagok/adatok/([0-9]+)$!', $path, $uid)) {
      wllib_status(301);
      drupal_goto("user/$uid[1]");
    }


    // Fórum áthelyezések
    elseif (strpos($path, "forumok/wfsz/") === 0) {
      wllib_status(301);
      drupal_goto('forumok/wfszwl/wfsz');
    }
    elseif ($path == "forumok/weblabor/weblaborrol") {
      wllib_status(301);
      drupal_goto('forumok/wfszwl/weblabor');
    }
    // Két fórum összevonása (a 178-as témacsoport már nem létezik)
    elseif (in_array($path, array("forumok/php/tartalomkezelok", "forumok/178"))) {
      wllib_status(301);
      drupal_goto('forumok/web/tartalomkezelok');
    }
    elseif ($path == "forumok/web/dhtmlflash") {
      wllib_status(301);
      drupal_goto('forumok/web/xhtmlajax');
    }
    // Korábbi rossz fórum témacsoport webcím kiírás
    elseif (preg_match('!^forumok/([0-9]+)$!', $path, $forumid) &&
            drupal_get_path_alias("forum/$forumid[1]") != "forumok/$forumid[1]") {
      wllib_status(301);
      drupal_goto("forum/$forumid[1]");
    }
    // Blogmarkok kitörése a linkek közül
    elseif (strpos($path, "linkek/blogmarkok/bekuldes/weben") === 0 || strpos($path, "linkek/blogmarkok/bekuldes") === 0) {
      wllib_status(301);
      drupal_goto('blogmarkok/bekuldes');
    }
    elseif (strpos($path, "linkek/blogmarkok") === 0) {
      wllib_status(301);
      drupal_goto('blogmarkok');
    }
    elseif (strpos($path, "rss/hozzaszolasok/linkek/blogmarkok") === 0) {
      wllib_status(301);
      drupal_goto('rss/hozzaszolas/blogmarkok');
    }
    elseif (strpos($path, "rss/linkek/blogmarkok") === 0) {
      wllib_status(301);
      drupal_goto('rss/blogmarkok');
    }
    elseif (preg_match('!^linkek/latogatas/([0-9]+)$!', $path, $nid)) {
      wllib_status(301);
      drupal_goto("blogmarkok/latogatas/$nid[1]");
    }
    elseif (preg_match('!^linkek/([0-9]+)$!', $path, $nid)) {
      wllib_status(301);
      drupal_goto("blogmarkok/$nid[1]");
    }

    // Hírek -> blog
    elseif (preg_match('!^hirek/rovatok(.*)$!', $path, $cat)) {
      wllib_status(301);
      drupal_goto("blog/rovatok$cat[1]");
    }
    elseif (strpos($path, "hirek/bekuldes") === 0) {
      wllib_status(301);
      drupal_goto('blog/bekuldes');
    }
    elseif (preg_match('!^hirek/2(.*)$!', $path, $cat)) {
      wllib_status(301);
      drupal_goto("blog/2$cat[1]");
    }
    elseif ($path == "hirek") {
      wllib_status(301);
      drupal_goto('blog');
    }
    elseif (preg_match('!^rss/hirek(.*)$!', $path, $rssinfo)) {
      wllib_status(301);
      drupal_goto('rss/blog' . $rssinfo[1]);
    }
    elseif ($path == "rss/hozzaszolas/hirek") {
      wllib_status(301);
      drupal_goto('rss/hozzaszolas/blog');
    }

    // Elirt RSS feedek
    elseif (preg_match('!^rss/hozzaszolasok(.+)$!', $path, $found)) {
      wllib_status(301);
      drupal_goto("rss/hozzaszolas$found[1]");
    }

    // Munka forum => Munka/allas
    elseif ($path == "forumok/vegyes/munkaallas") {
      wllib_status(301);
      drupal_goto('munka');
    }
    elseif ($path == "rss/forumok/vegyes/munkaallas") {
      wllib_status(301);
      drupal_goto('rss/munka');
    }

    // Ezt külön le kell kezelni, mert a kifelé aliasolás egyirányú,
    // itt pedig több aliashoz is ugyanaz tartozik.
    if ($path == 'rss/rss.xml') {
      return 'rss';
    }

    $path = preg_replace("!^(olvasas|blogmarkok|forumok/temak|szavazasok|munka)/(\\d+)$!", "node/\\2", $path);
    $path = preg_replace("!^szavazasok/(\\d+)/eredmenyek$!", "node/\\1/results", $path);
    $path = preg_replace("!^cimkek/(\\d+)$!", "taxonomy/term/\\1", $path);
    $path = preg_replace("!^rss/cimkek/(\\d+)$!", "taxonomy/term/\\1/0/feed", $path);
    $path = preg_replace("!^rss/forumok/(\\d+)$!",                "taxonomy/term/\\1/0/feed", $path);

    $path = preg_replace(
      "!^(olvasas|blogmarkok|forumok/temak|szavazasok)/(\\d+)/hozzaszolas(/\\d+)?$!",
      "comment/reply/\\2\\3",
      $path
    );

    if (preg_match("!^rss/((cikkek|blog)/rovatok/\\w+)$!", $path, $comps) &&
        ($normalpath = drupal_get_normal_path($comps[1]))) {
      return $normalpath . "/0/feed";
    }
    if (preg_match("!^rss/(munka/\\w+)$!", $path, $comps)) {
      return 'taxonomy/term/'. ($comps[1] == 'munka/kinal' ? JOB_OFFERED : JOB_LOOKEDFOR) .'/0/feed';
    }
    if (preg_match("!^rss/hozzaszolas/(.+)$!", $path, $comps)) {
      if ($comps[1] == 'szerkesztett') {
        return $path;
      }
      else {
        $normpath = drupal_get_normal_path($comps[1]);
        if (preg_match("!^node/(\\d+)$!", $normpath, $num)) {
          return "crss/node/$num[1]";
        }
        elseif (preg_match("!^nodesbytype/([a-z-]+)$!", $normpath, $typename)) {
          return "crss/nodetype/$typename[1]";
        }
        elseif (preg_match("!^(taxonomy/term|forum)/(\\d+)$!", $normpath, $num)) {
          return "crss/term/$num[2]";
        }
      }
    }
    elseif (preg_match("!^blog/(\\d{8})/([a-z0-9_-]+)/hozzaszolas(/\\d+)?$!", $path, $match)) {
      if (preg_match("!node/(\\d+)!", drupal_get_normal_path("blog/$match[1]/$match[2]"), $matchi)) {
        return "comment/reply/$matchi[1]$match[3]";
      }
    } elseif (preg_match("!^((konyvek|cikkek)/[a-z0-9_-]+)/hozzaszolas(/\\d+)?$!", $path, $match)) {
      if (preg_match("!node/(\\d+)!", drupal_get_normal_path($match[1]), $matchi)) {
        return "comment/reply/$matchi[1]$match[3]";
      }
    }
    //elseif (preg_match("!^forumok/(\\d+)/uj$!", $path, $match)) {
    //  return "forum/$match[1]/new";
    //}
    elseif (preg_match("!^forumok/(.+)/bekuldes$!", $path, $match)) {
      return drupal_get_normal_path('forumok/'. $match[1]) .'/bekuldes';
    } elseif (preg_match("!^(" . join("|", array_values($autoalias)) . ")(.*)$!", $path, $match)) {
      return array_search($match[1], $autoalias) . $match[2];
    } elseif (preg_match('!^rss/(forumok/(.+))$!', $path, $match) &&
              preg_match('!^forum/(\\d+)$!', drupal_get_normal_path($match[1]), $match)) {
      return 'taxonomy/term/' . $match[1] . '/0/feed';
    } elseif (preg_match("!^tagok/(\\d+)(.*)$!", $path, $match) && in_array($match[2], array_values($userautoalias))) {
      return 'user/' . $match[1] . array_search($match[2], $userautoalias);
    } else {
      return $path;
    }
  }
}

function wllib_status($code) {
  $status = array(
    301 => 'Moved Permanently',
    404 => 'Not Found',
    410 => 'Gone'
  );
  header("HTTP/1.0 $code {$status[$code]}");
  header("Status: $code {$status[$code]}", TRUE, $code);
}
