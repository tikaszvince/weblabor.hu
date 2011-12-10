<?php

/**
 * Oldal megjelenítés hírlevél információkkal.
 */
function hirlevel_page() {
  global $user;
  
  $note = $napi = $heti = '';
  if ($user->uid == 0) {
    $note = '<p><strong>Csak belépett felhasználóink tudnak fel- vagy leiratkozni hírleveleinkre.</strong></p>';
  }
  else {
    $napi = ($user->wl_hirlevel_napi ? 'fel vagy iratkozva' : 'nem vagy feliratkozva');
    $napi = "<p>
 Jelenleg <strong>{$napi}</strong> a napi hírlevélre. Állapotodat megváltoztathatod
 <a href=\"/tagok/". $user->uid ."/szerkesztes/hirlevel\">felhasználód adatmódosító oldalán</a>.
</p>";
    
    $heti = ($user->wl_hirlevel_heti ? 'fel vagy iratkozva' : 'nem vagy feliratkozva');
    $heti = "<p>
 Jelenleg <strong>{$heti}</strong> a heti hírlevélre. Állapotodat megváltoztathatod
 <a href=\"/tagok/". $user->uid ."/szerkesztes/hirlevel\">felhasználód adatmódosító oldalán</a>.
</p>";
  }
  $output = <<< HIRLEVEL_PAGE
<p>
 Webhelyünk kétféle hírlevéllel áll regisztrált felhasználói rendelkezésére. A
 napi hírlevelet elsősorban azoknak ajánljuk, akik gyakran szeretnének oldalunk
 válogatott tartalmából szemezgetni, a heti hírlevél leginkább azoknak lehet
 hasznos, akiknek kevés ideje jut a Weblabor újdonságainak megismerésére, de a
 hasznos tippekről, cikkekről nem szeretnének lemaradni.
</p>
{$note}

<div class="attach-file attach-right"><img src="/sites/all/themes/wl/images/hirlevel.png" alt="" /><p>Napi hírlevél pillanatkép</p></div>
<h2>Napi hírlevél</h2>
<p>
 A feliratkozottak minden nap hajnalban kapják, így reggel már a levelesládájukban
 várakozik az előző nap automatikus összefoglalója. Legfontosabb tartalmai a blog bejegyzések,
 de hivatkozik a legutóbb megjelent cikkekre és az előző nap blogmarkjait is
 felsorolja. Szöveges formátumú, ezért nem sok helyet foglal, reklámot nem tartalmaz.
 Kedvcsinálónak érdemes lehet megtekinteni a <a href="/hirlevel/napi/friss">legutóbbi
 napi hírlevelünket</a>.
</p>
{$napi}

<h2>Heti hírlevél</h2>
<p>
 A heti hírlevelet igénylők minden hétfő hajnalban kapják meg az elmúlt hétre
 vonatkozó automatikus összefoglalót. Legfontosabb tartalmi eleme a héten megjelent
 cikk vagy cikkek, illetve az emellett publikált blog bejegyzések. Listázza a héten felvett
 blogmarkokat is, amely dinamikus természeténél fogva komoly mennyiséget adhat ki.
 Szöveges formátumú, ezért nem sok helyet foglal, reklámot nem tartalmaz.
 Kedvcsinálónak érdemes lehet megtekinteni a <a href="/hirlevel/heti/friss">legutóbbi
 heti hírlevelünket</a>.
</p>
{$heti}

<h2>Javaslatod lenne?</h2>
<p>
 Amennyiben valami nem tűnik megfelelőnek, vagy jobb ötleted lenne, szokás szerint
 a hírlevelekkel kapcsolatban is szívesen fogadjuk az építő kritikát. Lásd az
 <a href="/weblabor/impresszum">impresszumot</a> a lehetséges kapcsolatfelvételi
 módokért.
</p>

HIRLEVEL_PAGE;
  return $output;
}

/**
 * Friss napi vagy heti hírlevél megjelenítés.
 */
function hirlevel_friss($type = 'napi') {
  if (in_array($type, array('napi', 'heti'))) {
    header("Content-Type: text/plain; charset=iso-8859-2");
    $filepath = HIRLEVEL_FOLDER .'wl-hirlevel_legfrissebb.'. $type .'.txt';
    if (file_exists($filepath) && is_readable($filepath)) {
      readfile($filepath);
      exit;
    }
    else {
      print 'A legutobbi '. $type .' hirlevel nem erheto el.';
      exit;
    }
  }
  else {
    drupal_not_found();
    exit;
  }
}
