<?php
function wlfilter_tips_page() {
ob_start();
/*
Nem dokumentált bbkódok:
 - img opciók
 - táblázat extrák (rowspan és colspan opciók, [caption])
 - [color], [size], [font], [align], [float], [email], [hr]
 - [center], [left], [right]
 - [nodelink] elem (csak BC okok miatt van)
 - [q] elem (inline idézet)
*/
$isadmin = user_access('administer nodes');
?>
<div id="static-page" class="filter-format-help">

<p>
A Weblabor alapvetően BBCode formázású tartalom beküldést támogat,
melynek lehetőségeit az alábbiakban igyekeztünk összefoglalni. Megfelelő
képességű böngészőket használók számára egy billentyűzetről is jól irányítható
szerkesztőt biztosítunk, mely jelentősen megkönnyítheti a szövegbevitelt.
Ezt a szerkesztőt regisztrált felhasználóink kikapcsolhatják.
</p>

<div class="center"><table>
 <tr><th>BBCode</th><th>BBEditor</th><th>Hatás</th></tr>
 <tr><td>[b]...[/b]</td><td>Ctrl+b</td><td><a href="/bekuldes/tippek#egyszeru">Vastagítás</a><td></tr>
 <tr><td>[i]...[/i]</td><td>Ctrl+i</td><td><a href="/bekuldes/tippek#egyszeru">Dőlt szöveg</a><td></tr>
 <tr><td>[u]...[/u]</td><td>Ctrl+u</td><td><a href="/bekuldes/tippek#egyszeru">Aláhúzás</a><td></tr>
 <tr><td>[url=http://...]...[/url]</td><td>Ctrl+l</td><td><a href="/bekuldes/tippek#linkek">Link</a><td></tr>
 <tr><td>[code]...[/code]</td><td>Ctrl+o</td><td><a href="/bekuldes/tippek#kodok">Egyszerű kód</a><td></tr>
 <tr><td>[colorer=nyelvnév]...[/colorer]</td><td>Ctrl+p</td><td><a href="/bekuldes/tippek#kodok">Színes kód</a><td></tr>
 <tr><td>[quote]...[/quote]</td><td>Ctrl+q</td><td><a href="/bekuldes/tippek#idezetek">Idézet</a><td></tr>
 <tr><td>[list]...[*]...[/list]</td><td>Ctrl+*</td><td><a href="/bekuldes/tippek#listak">Lista</a><td></tr>
 <tr><td>[table]lásd lent[/table]</td><td>Ctrl+e</td><td><a href="/bekuldes/tippek#tablazatok">Táblázat</a><td></tr>
 <tr><td>[img]http://...[/img]</td><td>nincs</td><td><a href="/bekuldes/tippek#kepek">Kép</a><td></tr>
 <tr><td></td><td>Ctrl+f</td><td>Keresés a szövegmezőben<td></tr>
<?php if ($isadmin) { ?> <tr><td>[comment:center]...[/comment]</td><td>Ctrl+m</td><td>Megjegyzés<td></tr>
 <tr><td>[hX]...[/hX]</td><td>Ctrl+X</td><td>2-3-4-(5-6)-os szintű fejléc<td></tr>
 <tr><td>[attach:$aid:center]...[/attach]</td><td>nincs</td><td>Csatolmány<td></tr>
 <tr><td>[attach:$aid:$aid:center]...[/attach]</td><td>nincs</td><td>Bélyegképes csatolmány<td></tr>
 <tr><td>[anchor=idnev]</td><td>nincs</td><td>Adott nevű horgony<td></tr>
<?php } ?>
</table></div>

<dl>
 <dt><a id="szoveg"></a>Szövegbevitel</dt>
 <dd>
  A soremelések sortörésekként jelennek meg, a folyamatosan gépelt
  szöveg a böngészőkben automatikusan tördelődik. Hosszabb szövegek,
  fórum témák vagy blog bejegyzések beküldésénél használható a
  <code>&lt;!--break--&gt;</code> elválasztó, mely a bevezető egyedi
  megjelölésére alkalmas. A <a href="/szojegyzek">szójegyzékben</a>
  található rövidítések automatikusan linkekké alakulnak.
 </dd>
 <dt><a id="egyszeru"></a>Egyszerű formázás</dt>
 <dd>
  A <strong>[b]</strong>, <strong>[i]</strong>,
  <strong>[u]</strong> jelölők használhatóak vastag, dőlt illetve
  aláhúzott szövegek megjelenítésére. Hatásuk a <strong>[/b]</strong>,
  <strong>[/i]</strong> illetve <strong>[/u]</strong> lezáró párokig
  tart.
 </dd>
 <dt><a id="linkek"></a>Linkek elhelyezése</dt>
 <dd>
  Minden szövegbe írt egyértelműen felismerhető (<code>http://</code>
  előtaggal ellátott és/vagy <code>www.</code> kezdetű) webcím automatikus
  linkké alakul. Szebb linkek alakíthatóak ki az
  <strong>[url=http://example.com]link szövege[/url]</strong> forma
  használatával. Az email címek hasonlóképpen automatikusan linkekké
  alakulnak. A <a href="/cikkek/what">kapcsolódó cikkünkben</a> leírt
  JavaScript alapú módszert alkalmazzuk az email linkek spamvédelmére.
  Ez semmit nem garantál, de a gyakorlat azt mutatja, hogy hatásos.
  Minden link úgy kerül megjelenítésre, hogy a keresők indexelői nem
  követik, tehát kéretlen linkek beküldésének nincs értelme.
 </dd>
 <dt><a id="kodok"></a>Forráskódok formázása</dt>
 <dd>
  Fájlnevek, függvénynevek szövegbeli jelölésére használható a
  <strong>[code]mail()[/code]</strong> forma. A [code] jobban meg nem
  határozható nagyobb kódblokkok formázására is alkalmas, de a
  <strong>[colorer=nyelvnév]forráskód[/colorer]</strong> sokkal hatásosabb,
  ha ismert nyelvű kódról van szó. A nyelvnév lehet <code>html</code>,
  <code>javascript</code>, <code>xml</code>, <code>css</code>,
  <a href="http://colorer.sourceforge.net/lang-list.html">és így tovább</a>.
  A <code>cmd</code> típussal megadható parancssori részlet is.
 </dd>
 <dt><a id="idezetek"></a>Idézetek</dt>
 <dd>
  A <strong>[quote]idézet szövege[/quote]</strong> használható
  más webhelyekről vagy korábbi hozzászólásokból szöveg idézéséhez.
  Az idézetek mindig blokk elemként jelennek meg.
 </dd>
 <dt><a id="listak"></a>Listák</dt>
 <dd>
  Listák bevitelére a <strong>[list]</strong> és <strong>[/list]</strong>
  jelölők között van lehetőség, ahol a kezdőelem típust is megadhat
  <strong>[list=típus]</strong> formában, melyben a típus "i", "I", "1",
  "a" és "A" egyike a kívánt lista formázás szerint. A lista elemeit
  <strong>[*]</strong> jelzéssel lehet felvenni. A BBEditor a sortörések
  és * jelek által határolt listákat alakítja a BBCode szabályainak
  megfelelő formájúra.
 </dd>
 <dt><a id="tablazatok"></a>Táblázatok</dt>
 <dd>
  A táblázatok a <strong>[table]</strong> és <strong>[/table]</strong>
  elemek között adhatóak meg. A HTML-hez hasonló szerkezetben, ám
  eltérő nevekkel sorokat a <strong>[row]</strong> és <strong>[/row]</strong>,
  cellákat a <strong>[cell]</strong> és <strong>[/cell]</strong> elemekkel
  lehet megadni egymásba ágyazottan, a fejléc cellák a
  <strong>[hcell]</strong> illetve <strong>[/hcell]</strong> által határoltak.
  A BBEditor a szövegszerkesztőből vagy böngészőből másolt tabokkal
  elválasztott cellás formázását tudja táblázattá alakítani automatikusan.
 </dd>
 <dt><a id="kepek"></a>Képek</dt>
 <dd>
  Képek megjelenítésére szükség esetén a
  <strong>[img]http://example.com/image.gif[/img]</strong> formában van lehetőség.
 </dd>
</dl>

</div>
<?php return ob_get_clean(); } ?>
