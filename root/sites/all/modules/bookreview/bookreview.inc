<?php

function bookreview_page() {
  drupal_add_feed(url("nodesbytype/feed/bookreview", array('absolute' => TRUE)), 'RSS - Könyvek');

  $header = array(
    '&nbsp;',
    array('data' => 'Ajánló', 'field' => 'n.created', 'sort' => 'desc'),
    array('data' => 'Cím', 'field' => 'n.title'),
    array('data' => 'Kiadás', 'field' => 'b.copyright'),
    array('data' => 'ISBN', 'field' => 'b.isbn')
  );

  $sql = db_rewrite_sql("SELECT b.nid, n.vid, n.title, n.created, b.publisher, b.isbn, b.copyright, b.pages, b.rating FROM {bookreview} b INNER JOIN {node} n ON b.nid = n.nid WHERE n.type = 'bookreview' AND n.status = 1");
  $sql .= tablesort_sql($header);
  $result = pager_query($sql, 20);

  while ($node = db_fetch_object($result)) {
    $output = '';
    $count = 0;
    $a = db_query('SELECT author, weight FROM {bookreview_authors} WHERE nid = %d ORDER BY weight', $node->nid);
    $authors = '';
    while ($author = db_fetch_object($a)) {
      $count++;
      if ($authors) {
        $authors .= ", $author->author";
      }
      else {
        $authors = $author->author;
      }
    }
    if ($authors) {
      $output = t('<strong>%tag:</strong> %authors', array('%tag' => format_plural($count, 'Szerző', 'Szerzők'), '%authors' => $authors)) .'<br />';
    }
    if ($node->copyright) {
      $output .= t('<strong>Kiadás éve:</strong> %copyright', array('%copyright' => $node->copyright)) .'<br />';
    }
    if ($node->publisher) {
      $output .= t('<strong>Kiadó:</strong> %publisher', array('%publisher' => $node->publisher)) .'<br />';
    }
    if ($node->isbn) {
      $output .= t('<strong>ISBN:</strong> %isbn', array('%isbn' => $node->isbn)) .'<br />';
    }
    if ($node->pages) {
      $output .= t('<strong>Oldalak száma:</strong> %pages', array('%pages' => $node->pages)) .'<br />';
    }
    if ($node->rating) {
      $output .= t('<strong>Értékelés:</strong> %rating', array('%rating' => $node->rating)) .'<br />';
    }
    
    $cover = '';
    $files = upload_load($node);
    if (is_array($files) && count($files)) {
      $cover = _bookreview_cover($files);
    }
    
    $rows[] = array(
      $cover,
      array(
        'data' => l($node->title, 'node/'. $node->nid, array('attributes' => array('title' => t('View this posting.')))) .'<br /><br />'. $output,
        'colspan' => '4'
      )
    );
  }

  if (!$rows) {
    $rows[] = array(array('data' => 'Nincs elérhető könyvajánló', 'colspan' => '5'));
  }

  $output = '<div class="bookreview-list"><p>Könyvajánlóink az egyes olvasók szubjektív véleményét tükrözik, így bárki beküldhet saját ajánlót. Az alábbi lista alapértelmezésben az ajánló megjelenési ideje szerint fordított időrendben jelenik meg, a táblázat fejléc linkjeivel változtatható a sorrend.</p>';
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, 20, 0);
  $output .= '</div>';

  return $output;
}

function _bookreview_nodeapi(&$node, $op, $a3 = NULL, $a4 = NULL) {
  switch ($op) {
    case 'delete':
      db_query('DELETE FROM {bookreview} WHERE nid = %d', $node->nid);
      db_query('DELETE FROM {bookreview_authors} WHERE nid = %d', $node->nid);
      db_query('DELETE FROM {bookreview_links} WHERE nid = %d', $node->nid);
      break;

    case 'insert':
      db_query("INSERT INTO {bookreview} (nid, publisher, copyright, isbn, pages, rating) VALUES (%d, '%s', %d, '%s', %d, %d)", $node->nid, $node->publisher, $node->copyright, $node->isbn, $node->pages, $node->rating);
      _bookreview_authors_and_links($node);
      break;

    case 'load':
      $bookreview = db_fetch_array(db_query("SELECT * FROM {bookreview} WHERE nid = %d", $node->nid));
      $authors = db_query("SELECT author, weight FROM {bookreview_authors} WHERE nid = %d ORDER BY weight", $node->nid);
      while ($author = db_fetch_object($authors)) {
        $bookreview['authors'][$author->weight] = $author->author;
      }
      $bookreview['numauthors'] = count($bookreview['authors']);
    
      $booklinks = db_query("SELECT booklink, description, weight FROM {bookreview_links} WHERE nid = %d ORDER BY weight", $node->nid);
      while ($booklink = db_fetch_object($booklinks)) {
        $bookreview['booklinks'][$booklink->weight] = array($booklink->booklink, $booklink->description);
      }
      $bookreview['numlinks'] = count($bookreview['booklinks']);
      return $bookreview;
      break;

    case 'update':
      db_query("UPDATE {bookreview} SET publisher = '%s', copyright = %d, isbn = '%s', pages = %d, rating = %d WHERE nid = %d", $node->publisher, $node->copyright, $node->isbn, $node->pages, $node->rating, $node->nid);
      db_query('DELETE FROM {bookreview_authors} WHERE nid = %d', $node->nid);
      db_query('DELETE FROM {bookreview_links} WHERE nid = %d', $node->nid);
      _bookreview_authors_and_links($node);
      break;
    
    case 'view':
      _bookreview_view($node, $a3, $a4);
  }
}

function _bookreview_authors_and_links(&$node) {
  foreach ($node->authors as $weight => $name) {
    if (!empty($name)) {
      db_query("INSERT INTO {bookreview_authors} (nid, author, weight) VALUES (%d, '%s', %d)", $node->nid, $name, $weight);
    }
  }
  foreach ($node->booklinks as $weight => $data) {
    if (!empty($data[0])) {
      db_query("INSERT INTO {bookreview_links} (nid, booklink, description, weight) VALUES (%d, '%s', '%s', %d)", $node->nid, $data[0], $data[1], $weight);
    }
  }
}

/**
 * Könvyborító megjelenítés.
 */
function _bookreview_cover($files) {
  $file = array_shift($files);
  return '<div class="attach-file attach-right"><img src="'.
         file_create_url($file->filename) .'" alt="" title="'.
         check_plain($file->description) .'" /></div>';
}

/**
 * hook_nodeapi() view.
 */
function _bookreview_view(&$node, $teaser = FALSE, $page = FALSE) {
  $cover = NULL;
  if (is_array($node->files) && count($node->files)) {
    $cover = _bookreview_cover($node->files);
  }
  
  if ($teaser) {
    /*if ($cover) {
      $node->content[] = array('#value' => $cover, '#weight' => -200);
    }*/
    return;
  }
  
  $header .= "<div class=\"header clear-block\">\n";
  $header .= $cover;
  if ($node->authors[0]) {
    foreach ($node->authors as $author) {
      if ($authors) {
        $authors .= ", $author";
      }
      else {
        $authors = $author;
      }
    }
    $header .= '<div class="wrap"><h3 class="author">'. format_plural(count($node->authors), 'Szerző', 'Szerzők') .':</h3><p>'. check_plain($authors) ."</p></div>\n";
  }
  if ($node->publisher) {
    $header .= '<div class="wrap"><h3 class="publisher">Kiadó:</h3><p>'. check_plain($node->publisher) ."</p></div>\n";
  }
  if ($node->copyright) {
    $header .= '<div class="wrap"><h3 class="copyright">Kiadás éve:</h3><p>'. check_plain($node->copyright) ."</p></div>\n";
  }
  if ($node->isbn) {
    $header .= '<div class="wrap"><h3 class="isbn">ISBN:</h3><p>'. check_plain($node->isbn) ."</p></div>\n";
  }
  if ($node->pages) {
    $header .= '<div class="wrap"><h3 class="pages">Oldalak száma:</h3><p>'. check_plain($node->pages) ."</p></div>\n";
  }
  if ($node->rating) {
    $header .= '<div class="wrap"><h3 class="rating">Értékelés:</h3><p>'. check_plain($node->rating) ."</p></div>\n";
  }
  
  $footer = '';
  if (!empty($node->booklinks)) {
    $footer .= "<h3>Linkek</h3><ul>\n";
    foreach ($node->booklinks as $weight => $data) {
      $booklink = '';
      $linkdescription = !empty($data[1]) ? $data[1] : $data[0];
      if (ereg('^http://|^https://|ftp://', $data[0])) {
        // off site link
        $booklink = "<a href=\"$data[0]\">". check_plain($linkdescription) .'</a>';
      }
      elseif (!empty($data[0])) {
        // on site link
        $booklink = l($linkdescription, $data[0]);
      }
      if (!empty($booklink)) {
        $footer .= '<li>'. $booklink ."</li>\n";
      }
    }
    $footer .= "  </ul>\n";
  }
  $footer .= "</div>\n"; // end of header
  $node->content[] = array('#value' => $header . $footer, '#weight' => -100);
}

/**
 * Könyv mezőket tesz az űrlapba.
 */
function _bookreview_form_alter(&$form, $form_state, $form_id) {
  
  $node = &$form['#node'];
  $post = &$form_state['values'];
  
  if ($post['numauthors']) {
    // Előnézet.
    if (!empty($post['bookreview_more_authors'])) {
      // Több szerző kell.
      if ($post['numauthors'] % 2) {
        $node->numauthors = $post['numauthors'] + 1;
      }
      else {
        $node->numauthors = $post['numauthors'] + 2;
      }
    }
    else {
      // Nem kell több szerző.
      $node->numauthors = $post['numauthors'];
    }
  }
  elseif (empty($node->numauthors)) {
    // Új node, egy szerző mező lesz.
    $node->numauthors = 1;
  }

  if ($post['numlinks']) {
    // Előnézet.
    if ($post['bookreview_more_links'] == 1) {
      // Több link kell.
      $node->numlinks = $post['numlinks'] + 1;
    }
    else {
      // Nem kell több link.
      $node->numlinks = $post['numlinks'];
    }
  }
  elseif (empty($node->numlinks)) {
    // Új node, egy link mező lesz.
    $node->numlinks = 1;
  }

  $form['numauthors'] = array(
    '#type' => 'value',
    '#value' => $node->numauthors,
  );
  $form['numlinks'] = array(
    '#type' => 'value',
    '#value' => $node->numlinks,
  );

  $form['book_information'] = array(
    '#type' => 'fieldset',
    '#title' => 'A könyv adatai',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['book_information']['authors_wrap'] = array(
    '#type' => 'fieldset',
    '#title' => 'Szerző vagy szerzők',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['book_information']['authors_wrap']['authors_begin'] = array(
      '#type' => 'markup',
      '#value' => '<table>',
  );
  $form['book_information']['authors_wrap']['authors'] = array(
    '#tree' => TRUE,
  );
  for ($i = 0; $i < $node->numauthors; $i++) {
    $form['book_information']['authors_wrap']['authors'][$i] = array(
      '#type' => 'textfield',
      '#default_value' => isset($node->authors[$i]) ? $node->authors[$i] : '',
      '#size' => 60,
      '#maxlength' => 255,
      '#description' => '',
      '#prefix' => ($i%2) ? '<td>' : '<tr><td>',
      '#suffix' => ($i%2) ? '</td></tr>' : '</td>',
    );
  }
  $form['book_information']['authors_wrap']['authors_end'] = array(
      '#type' => 'markup',
      '#value' => ($node->numauthors + 1 % 2) ? '</table>' : '<td></td></tr></table>',
  );
  $form['book_information']['authors_wrap']['bookreview_more_authors'] = array(
    '#type' => 'checkbox',
    '#title' => 'További szerzők hozzáadása',
    '#description' => 'Itt lehet kérni további szerző mezőket, melyek az <strong>előnézetre kattintás után</strong> jelennek majd meg.',
  );

  $form['book_information']['rating'] = array(
    '#type' => 'select',
    '#title' => 'Minősítés',
    '#default_value' => isset($node->rating) ? $node->rating : 5,
    '#options' => (array('<'. t('none') .'>') + drupal_map_assoc(range(1, 10))),
    '#description' => 'A könyv minősítése 1-10-ig terjedő skálán.',
  );

  $form['book_information']['publisher'] = array(
    '#type' => 'textfield',
    '#title' => 'Kiadó',
    '#default_value' => isset($node->publisher) ? $node->publisher : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => 'A könyv kiadójának neve.',
  );

  $form['book_information']['copyright'] = array(
    '#type' => 'textfield',
    '#title' => 'Kiadás éve',
    '#default_value' => isset($node->copyright) ? $node->copyright : '',
    '#maxlength' => 255,
    '#description' => 'A könyv kiadásának éve, például 2007.',
  );

  $form['book_information']['isbn'] = array(
    '#type' => 'textfield',
    '#title' => 'ISBN',
    '#default_value' => isset($node->isbn) ? $node->isbn : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => 'ISBN szám',
  );

  $form['book_information']['pages'] = array(
    '#type' => 'textfield',
    '#title' => 'Oldalak száma',
    '#default_value' => isset($node->pages) ? $node->pages : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => 'A könyv oldalainak száma.',
  );

  $form['book_information']['offsite_links'] = array(
    '#type' => 'fieldset',
    '#title' => 'Linkek',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['book_information']['offsite_links']['booklinks_begin'] = array(
      '#type' => 'markup',
      '#value' => '<table><tr><th>Webcím</th><th>Leírás</th></tr>',
  );
  $form['book_information']['offsite_links']['booklinks'] = array(
    '#tree' => TRUE
  );
  for ($i = 0; $i < $node->numlinks; $i++) {
    $form['book_information']['offsite_links']['booklinks'][$i][0] = array(
      '#type' => 'textfield',
      '#title' => '',
      '#default_value' => isset($node->booklinks[$i][0]) ? $node->booklinks[$i][0] : '',
      '#size' => 45,
      '#maxlength' => 255,
      '#description' => '',
      '#prefix' => '<tr><td>',
      '#suffix' => '</td>',
    );
    $form['book_information']['offsite_links']['booklinks'][$i][1] = array(
      '#type' => 'textfield',
      '#title' => '',
      '#default_value' => isset($node->booklinks[$i][1]) ? $node->booklinks[$i][1] : '',
      '#size' => 45,
      '#maxlength' => 255,
      '#description' => '',
      '#prefix' => '<td>',
      '#suffix' => '</td></tr>',
    );
  }
  $form['book_information']['offsite_links']['booklinks_end'] = array(
      '#type' => 'markup',
      '#value' => '</table>',
  );
  $form['book_information']['offsite_links']['bookreview_more_links'] = array(
    '#type' => 'checkbox',
    '#title' => 'További linkek hozzáadása',
    '#description' => 'Itt lehet kérni további link mezőket, melyek az <strong>előnézetre kattintás után</strong> jelennek majd meg.',
  );
}
