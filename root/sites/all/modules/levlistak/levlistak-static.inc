<?php

function levlistak_links($listname) {
  return array(
    'list_mailman' => array(
      'title' => 'Feliratkozás, leiratkozás',
      'href' => 'https://weblabor.hu/mailman/listinfo/'. $listname,
    ),
    'list_archive' => array(
      'title' => 'Archívum',
      'href' => 'levlistak/'. $listname,
    ),
    'list_send' => array(
      'title' => 'Levél küldés',
      'href' => 'mailto:'. $listname .'@weblabor.hu',
    ),
  );
}

function levlistak_index() {
  $ml_links = array();
  foreach (levlistak_list_names() as $mlname) {
    $ml_links[$mlname] = theme('links', levlistak_links($mlname));
  }
  $index_text = <<<INDEX_END
<p>
 A weblabor szerverén az alábbi nyilvános levelezőlistákat üzemeltetjük. Minden
 levelezőlista általános szabálya, hogy csak feliratkozott tagok írhatnak rá. Az
 üzenetek postázásakor a <a href="/levlistak/illemszabaly">listák illemszabályát</a>
 kérjük figyelembe venni.
</p>

<dl class="maillists">
 <dt>wl-phplista</dt>
 <dd>
  A Weblabor legforgalmasabb levelezőlistája. Témája a PHP nyelv és érintőlegesen a szorosan
  kapcsolódó webfejlesztési ismeretek. <strong>Nem témája</strong> például a HTML, a JavaScript,
  a CSS és az SQL nyelv használata, erre más levelezőlisták léteznek.
  <div class="maillist-links">{$ml_links["wl-phplista"]}</div>
 </dd>
 
 <dt>wl-levlista</dt>
 <dd>
  Visszafogott forgalmú levelezőlista. Témája minden olyan webfejlesztéssel kapcsolatos témakör,
  melyhez nem kapcsolódik más levelezőlista, így például a HTML, CSS, JavaScript ismeretek,
  webszerverek beállítása és egyéb kapcsolódó témakörök, technológiák.
  <div class="maillist-links">{$ml_links["wl-levlista"]}</div>
 </dd>
 
 <dt>wl-csharplista</dt>
 <dd>
  Témája a C# nyelv és az összes ehhez kapcsolódó témakör, így eshet szó a Visual Basic nyelvről
  is és az egész MS .NET témához kapcsolódó technológiákról is.
  <div class="maillist-links">{$ml_links["wl-csharplista"]}</div>
 </dd>

 <dt>wl-designlista</dt>
 <dd>
  Weboldalak és webes alkalmazások felületének tervezése a témája,
  beleértve a grafikus kialakítást, a felhasználói felületi elemek
  megtervezését.
  <div class="maillist-links">{$ml_links["wl-designlista"]}</div>
 </dd>

 <dt>wl-flashlista</dt>
 <dd>
  Témája a Flash és az ActionScript programozás és az ezekhez kapcsolódó
  témakörök, főleg a programozás oldaláról.
  <div class="maillist-links">{$ml_links["wl-flashlista"]}</div>
 </dd>

 <dt>wl-prmglista</dt>
 <dd>
  Projekt-menedzsment lista, azaz a különböző projektek kezeléséhez
  használható eszközök, módszerek, módszertanok eszmecseréinek helye.
  <div class="maillist-links">{$ml_links["wl-prmglista"]}</div>
 </dd>

 <dt>wl-offlista</dt>
 <dd>
  Témája bármilyen szakmai kérdés, amely más levelezőlisták kereteibe nem fér be. Ha az
  egyik listán <strong>offtopic szakmai téma</strong> merül fel, akkor minél előbb
  erre a listára illik a társalgást átterelni.
  <div class="maillist-links">{$ml_links["wl-offlista"]}</div>
 </dd>

 <dt>wl-flamelista</dt>
 <dd>
  Témája bármi. Abszolút moderálatlan. Csak azoknak, akik bírják.
  Nyomokban mogyorót is tartalmazhat!
  <div class="maillist-links">{$ml_links["wl-flamelista"]}</div>
 </dd>
</dl>

<h2>Ajánlott, máshol elérhető magyar levelezőlisták</h2>

<dl class="maillists">
 <dt>Java lista</dt>
 <dd>
  A <a href="http://javasite.bme.hu/javalist">javalist</a> a
  Jáva Felhasználók Társasága által üzemeltetett, rendkívül nagy
  múltra visszatekintő Java levelezési lista.
 </dd>
 
 <dt>Perl lista</dt>
 <dd>
  <a href="http://www.perl.org.hu/mailman/listinfo/perl">A magyar perl
  levelezőlista</a> a Perl.org.hu szerveren üzemel.
 </dd>
 
 <dt>Python (és Zope) lista</dt>
 <dd>
  Az fsn.hu üzemelteti a <a href="http://lists.fsn.hu/sympa/info/python">magyar
  python levelezőlistát</a>.
 </dd>

 <dt>Ruby on Rails (RoR) lista</dt>
 <dd>
  A magyar Ruby on Rails honlapon érhető el a
  <a href="https://lists.rubyonrails.hu/mailman/listinfo/ror">magyar
  RoR levelezőlista</a>.
 </dd>

 <dt>SQL lista</dt>
 <dd>
  Az <a href="http://list.dev.hu/cgi-bin/mailman/listinfo/sq-l">SQ-L</a> néven
  működő lista éppen megfelelő adatbázis kérdések megvitatására.
 </dd>
</dl>
INDEX_END;
  return $index_text;
}

function levlistak_illem() {
  drupal_set_title("Levelezőlisták illemszabálya (v1.05 - 2003. október 18)");
  $illem = <<<ILLEM_END
<div class="whateffect">
<p>
 A Weblabor szerveren <a href="/levlistak">több publikus levelezőlista is
 működik</a>. Ezeknek a levelezőlistáknak az illemtanát foglalja össze az
 alábbi írás. Az itt szereplő szabályok a listák több éves működése során
 alakultak ki, a Weblabor levelezőlisták tagjai által betartandó szabályokat
 jelentik. Amennyiben nem tudod elfogadni az itt leírtakat, vagy
 nem értesz egyet valamely ponttal, kérlek írj a
 <a href="mailto:info##kukac##weblabor.hu">info##kukac##weblabor.hu</a>
 email címre. A Weblabor levelezőliták tagjaitól elvárjuk, hogy a listákkal
 kapcsolatos kérdésekben az ebben a dokumentumban leírtak szerint cselekedjenek.
</p>

<h2>Feliratkozás es leiratkozás a Weblabor listákról</h2>

<p>
 A <a href="http://www.weblabor.hu/mailman/listinfo/">Mailman listakezelő
 felületen</a> lehet feliratkozni és leiratkozni a levlistákra, levlistákról.
 A feliratkozáskor egy jelszót kell megadni, amivel a továbbiakban a beállítások
 megváltoztathatóak, illetve a leiratkozás megtörténhet. A jelszó ne
 legyen máshol is használt jelszó, mivel havonta emlékeztetőként
 kiküldésre kerül! Elfelejtett jelszó esetén ettől függetlenül is lehet kérni
 a jelszó elküldését a feliratkozáskor használt email címre a Mailman felületen.
</p>
  
<p>
 A listák normál és napi egyszeri módban is elérhetőek. Normál mód esetén
 minden üzenetet rögtön megkapsz, napi egyszeri mód esetén többet kapsz
 meg egyszerre egy levélben. Ha gyakran megtelik a levélládád, vagy
 csak egyszerűen nem szeretnél mindent azonnal olvasni, jó választás
 lehet a második módszer. Ezt a módot később is megváltoztathatod
 jelszavad es email címed használatával.
</p>
  
<p>  
 Nem csak weben, hanem emailben is jelentkezhetsz a listákra, ilyenkor
 egy véletlenszerű jelszót kapsz. Az email cím, amire egy <tt>subscribe</tt>
 témamegjelölésű levelet kell küldeni: <tt>*****-request##kukac##weblabor.hu</tt>, 
 ahol a <tt>*****</tt> helyére a levelezőlista nevét kell beírni (például
 <tt>wl-levlista-request##kukac##weblabor.hu</tt>).
</p>
  
<p>
 Feliratkozás után bejelentkező levelet kapsz a listakezelő programtól.
 Azt <strong>feltétlenül őrizd meg</strong> későbbi használatra.
 A leiratkozás hasonló formákban történhet, erről már a kapott levélben
 részletesebb információkat találsz.
</p>
  
<p>
 Ha hosszabb ideig nem tudod olvasni a listát, mindenképpen érdemes
 bekapcsolni a <em>nomail</em> lehetőséget, amivel a lista tagja maradsz,
 de semmilyen üzenetet nem kapsz. Ha hosszabb ideig visszapattannak a
 levelek a címedről (betelt levelláda, megszűnt email cím, technikaliag
 elérhetetlen cím), automatikusan nomail-re állítódik az állapotod.
 Ilyenkor neked kell visszaállítanod a Mailman felületen.
</p>
  
<p>
 Amennyiben más vagy több email címmel szeretnél feliratkozni
 a listára, mint amivel olvasod, akkor fel kell iratkoznod az összes
 lehetséges email címeddel, és egy kivételével nomail-re kell
 állítanod a feliratkozásokat (pontosabban annyi kivételével,
 ahány helyre meg szeretnéd kapni a lista forgalmát).
</p>
  
<h2>Levelek küldése a Weblabor levelezőlistákra</h2>

<p>  
 A listára küldött levelek formátuma mindenképpen sima szöveges
 legyen (tehát <strong>nem HTML formátumú</strong>). Ne írj semmi
 olyasmit egy levélben, amit nem küldenél el levelezőlapon, azaz vállald
 a felelősséget azért, amit írsz! Próbáld mondanivalód kultúrált módon
 megfogalmazni! Tartsd észben, hogy minden amit írsz, egy archívumba
 kerül, így később rádkeresve az interneten ezeket is meg fogja a
 kereső találni.
</p>

<h3>Kérdések küldése</h3>

<p>
 Ha hatékonyan szeretnél információkhoz jutni, az itt olvasható,
 levelezőlistáinkra jellemző tanácsok mellett mindenképpen olvasd
 el az <a href="#egyeb">Egyéb kérédseknél</a> hivatkozott
 <em>Hogyan kérdezzünk okosan</em> című dokumentumot is.
</p>

<p>
 A levelezőlisták témájának megfelelő leveleket küldj a listákra! A
 szorosan kapcsolódó területek kérdései rendben lehetnek, más téma
 esetén próbálj egy másik levelezőlistát keresni! Érdemes pár napig
 csak figyelemmel kísérni a listát, amikor feliratkozol, hogy be
 tudj kapcsolódni a 'vérkeringésbe'.
</p>
  
<p>
 Mielőtt kérdést küldenél egy listára, próbálj meg utánanézni a
 levelezőlista archívumában illetve az interneten a témának!
 Ez utóbbihoz tudjuk javasolni a <a href="http://www.google.com/">Google
 keresőt</a>, ami, ha "helyesen teszed fel a kérdést", általában
 megtalálja neked, amit keresel.
</p>

<p>
 Amennyiben nem vagy tisztában a szálkövetéses levelezés fogalmával,
 mindenképpen olvasd el <a href="#szalak">a dokumentum végén található
 összefoglalót</a> a témában, és tartsd magad az ott leírtakhoz!
</p>

<p>
 Ne küldj el több listára is egy levelet! Ugyanúgy ne küldd el
 egy listára többször ugyanazt a levelet. Lehetséges, hogy a
 hálózat forgalma miatt a levelek kézbesítése kicsit késik. Ha
 listahibára gyanakszol, a
 <a href="mailto:info##kukac##weblabor.hu">info##kukac##weblabor.hu</a>
 címre írj!
</p>

<p>
 Ne sértődj meg, ha nem kapsz azonnal választ a kérdésedre.
 Különösen ne küldd el rövid időn belül másodszor a kérdést.
 Sokszor azért nem válaszolnak a levelekre, mert HTML levelet
 küldesz, vagy túl egyertelmű, archívumból vagy az interneten
 pillanatok alatt megszerezhető ismeretet tudnának csak
 megismételni a tagok.
</p>

<p>
 Próbáld meg minél pontosabban feltenni kérdésed. Általános
 kérdéseket ne tegyél fel (hogyan írjak XXX nyelven programot?),
 mert ezekre általában nem lehet korrekt választ adni. Ha valami
 nem megy, másolj be a listára egy pár soros példát az általad
 sejtett hibát tartalmazó sorokkal, vagy pedig ha ez nem lehetséges,
 más módon (de ne a listán!) tedd elérhetővé a listatagok számára
 a programot, kódrészletet.
</p>

<p>
 Vedd figyelembe, hogy a levelezőlistán különböző érdeklődésű,
 vallású, politikai nézetű, más és más technológiák és módszerek
 iránt elkötelezett emberek vannak jelen. Tartsd tiszteletben
 az ő véleményüket is, arról, hogy valakinek a zöld a kedvenc
 színe, másnak pedig a kék, nem érdemes vitatkozni.
</p>

<p>
 A levelezőlistákon <strong>szigorúan tilos</strong> vallásról,
 politikáról és kapcsolódó témakörökről beszélni, a listákon
 szigorúan csak az adott lista témájában szabad megszólalni!
</p>

<p>
 A levél témamegjelölése tükrözze a levél tartalmát! A téma
 nélküli leveleket lehet, hogy a szelektív olvasók meg sem
 nézik, vagy pedig nem szívesen válaszolnak rá.
</p>

<p>
 A kérdések írásakor próbáld meg röviden összefoglalni, hogy
</p>
<ul>     
 <li>mi a probléma</li>
 <li>
  milyen környezetben kellene megoldani a problémát
  [operációs rendszer, a használt nyelv verziója,
  egyéb fontos verzió és környezeti leírások]
 </li>
 <li><strong>milyen megoldásokat próbáltál már eddig</strong></li>
</ul>

<p>
 Ha automatikus aláírást használsz, fogd azt rövidre! Négy sornál
 semmiképpen ne legyen hosszabb. Hamar megismernek a listán, és
 csak felesleges forgalmat okozol.
</p>

<p>
 A listára küldött leveket már nem lehet visszavonni (mint pl.
 a HIX Webmesternél). Ezért csak olyan levelet küldj a listára,
 amiben biztos vagy, hogy oda szántad!
</p>

<p>
 Ne állíts be automata választ (pl. vakáción vagyok) a levlistáról
 érkező levelekre! Lánclevek, spam küldése a listára kifejezetten
 tiltott!
</p>

<p>
 Ne küldj a listára csatolt állományokat! A naponta kiküldött
 változat olvasói nem tudnak semmit kezdeni a csatolmányoddal,
 és a lista nagy részét valószínűleg csak terheled vele. Egy
 URL megadása bőven elég.
</p>

<h3>Válaszok küldése</h3>
  
<p>    
 A listára csak a lista témájában feltett kérdésekre illik
 válaszolni! Nem a lista témájához tartozó kérdésre magánban
 válaszolhatsz. Lánclevekre es indulatos 'flame' levelekre semmilyen
 formában sem szabad választ küldeni a listákra.
</p>

<p>
 Amikor a listára küldesz választ, a következőket gondold vegig:
</p>

<dl>
 <dt>Biztos, hogy a válasz a listára tartozik?</dt>
 <dd>
  Ha nem, magánba küldd az érintetteknek! A lista nagyrészét
  nem érdeklik a magánjellegű viták vagy információcserék.
  Ha véletlenül mégis magánlevelet küldesz a listára, kérj
  bocsánatot a listától és a szándékolt címzettől. Ha simán
  csak válszolsz egy levelezőlistán érkezett levélre és magánlevélnek
  szánod, figyelj arra, hogy automatikusan a lista címe kerül
  kitöltésre címzettként!
 </dd>
 <dt>Van a válasznak lényegi tartalma?</dt>
 <dd>
  Ne küldj 'thx', 'köszi', 'egyetértek', és hasonló, semmitmondó
  leveleket a listára. Ha fontosnak tartod ilyenirányú érzésed
  kifejezését, magánba küldd!
 </dd>
 <dt>Nem írta ugyanezt le előtted valaki?</dt>
 <dd>
  Mielőtt válaszolsz a levelekre, mindenképpen olvasd el a
  témában érkezett összes választ, és csak akkor írj te is
  választ, ha újat tudsz mondani!
 </dd>
 <dt>Megfelelő a levél témamegjelölése?</dt>
 <dd>
  Ha nem, írd át értelemszerűen, és írj a végére egy "volt:
  régitéma" részt, ahol a régitéma a régebbi témamegjelölés.
  Így segítesz a szelektív olvasóknak, hogy tényleg csak az
  őket érdeklő kérdéseket tudják megnézni.
 </dd>
 <dt>Mennyit kell beidézni az erdetiből?</dt>
 <dd>
  Azokat a részeket idézd be az eredeti levélből, amikre
  válaszolsz. Senki sem szereti feleslegesen többször elolvasni
  ugyanazt. A beidézett részek alá írd a választ, hogy könnyen
  követhető legyen, mire reagálsz. Semmiképpen se hagyj bent
  a válaszban régebbi aláírást, lista láblécet, vagy egyéb
  'digitális szemetet'.
 </dd>
</dl>
    
<p>
 Ha felteszel egy kérdést a listán, és magánban további
 jó tippeket kapsz, ami érdekelheti esetleg a listát,
 küldd be a listára is a tippet a forrás megjelölésével.
 Egy összefoglaló levelet is szívesen szoktak venni a lista
 tagjai.
</p>

<h2>Szankciók</h2>

<p>
 Amennyiben valamely listatag sorozatosan nem tartja be az itt leírt
 irányelveket, a lista adminisztrátora egy előzetes figyelmeztetés
 után korlátozhatja vagy megszüntetheti listatagságát. A lista
 adminisztrátorának feladata a legjobb tudása szerint úgy alakítani
 a levelezést, hogy ez ne történjen meg.
</p>

<a name="egyeb"></a>
<h2>Egyéb kérdések</h2>

<p>
 Egyéb itt nem kifejetett kérdésekben a
 <a href="http://www.mek.iif.hu/porta/szint/muszaki/szamtech/wan/kultura/rfc1855h.hun">Netikett</a>
 az irányadó. Eme dokumentum készítesekor is szabadon felhasználtuk az
 ott vázolt irányelveket. Az okos kérdezéshez nagyon tanulságos olvasmány
 Eric Steven Raymond <a href="http://www.catb.org/~esr/faqs/smart-questions.html">How
 To Ask Questions The Smart Way</a> című írása, amely Berta Krisztiánnak köszönhetően
 magyar nyelven is elérhető
 <a href="http://www.no.info.hu/~kryss/gnu/esr/smart-questions_hu.html">Hogyan kérdezzünk
 okosan</a> címmel.
</p>
  
<p>
 Ha valamilyen kétséged/kérdésed van, küldd azt a
 <a href="mailto:info##kukac##weblabor.hu">info##kukac##weblabor.hu</a>
 email címre. Bármilyen kiegészítést, ötletet szívesen fogadunk ezzel
 a dokumentummal kapcsolatban.
</p>

<h2>Mellékletek</h2>

<a name="szalak"></a>
<h3>Szálkezelés</h3>

<p>
 A weblabor levelezőlistáinak célcsoportja a webes szakmában dolgozók
 köre. Sokan vannak, akik úgy próbálnak lépést tartani a technológia
 fejlődésével, úgy próbálnak megoldásokat találni problémáikra mások
 segítségével, hogy több levelezőlista tagjai lesznek.
</p>
  
<p>
 Több levelezőlista forgalmával azonban nehéz lépést tartani, ezért a
 listákon folyó beszelgetések közül azokat kell kiválogatni, amik
 érdekesek lehetnek az olvasó számára. Mindent elolvasni egyszerűen
 lehetetlen.
</p>
  
<p>
 Szerencsére a témák kiválasztásában kiváló segítséget nyújt a modern
 levelezőprogramok "szálkövetés" (threading) támogatása. Ez azt jelenti,
 hogy egy faszerkezetbe gyűjtik az olvasó számára a beküldött kérdést,
 és az arra küldött válaszokat. Így ha egy kérdés teljesen kívül esik
 egy olvasó érdeklődési körén (például MySQL kérdés, ha az olvasót csak
 a PostgreSQL érdekli), akkor az egész szálat olvasottnak tekintheti,
 és a többi szál leveleivel folytathatja.
</p>
  
<p>
 A szálkövetést a legtöbb levelezőprogram támogatja. Működése arra épül,
 hogy levélíráskor a küldő program a gépnevet és egy egyedi kódot
 tartalmazó azonosítót helyez a levél fejlécébe. Amikor erre válaszol
 valaki, az azonosító hivatkozásként megjelenik a válaszlevél fejlécében
 (ami már egy másik azonosítót kap). Így lehet követni, hogy melyik
 levélre írta a szerző a választ. Fontos megjegyezni hogy általában azok
 a programok is kezelik a szálakra vonatkozó levél fejléceket, amelyek
 a szálak felhasználói felületen történő megjelenítésére nem képesek!
</p>
  
<p>
  Íme egy illusztráció: a wl-phplista egyik száljának megjelenítése
  Mozillában:
</p>
<p style="text-align: center;">
 <img src="/misc/threading.gif" alt="Egy szál a wl-phplista levlistáról" />
</p>

  
<p>
 Egyetlen probléma adódhat ezen szolgáltatással, mégpedig az, hogy
 néhányan nem veszik tudomásul, hogy <strong>a szálak sok olvasó
 számára fontosak</strong>. A fenti képen például látszik, hogy egy
 teljesen új "Cgi hivas" témájú kérdést küldött be egy Zsolt nevű
 listatag.
</p>
  
<p>
 Időnként szükség van egy szálban a témamegjelölés megváltoztatására,
 de itt nem ez történt. Zsolt nem új levelet írt a levelezőlistára, hanem
 választ indított CoL levelére, átírva a témát, és kitörölve az előző
 levél tartalmát. A levél írásához használt "válasz" művelet miatt a
 levelezőprogramja úgy értelmezte, hogy ő Col levelére szeretne reagálni,
 így elküldte Col levelének az azonosítóját a levél fejlécében. Emiatt
 jelent meg a szál nézetet támogató Mozillában a levél ebben a formában.
</p>
  
<p>
 Ez az új levél írási mód talán kényelmesnek tűnik, hiszen nem kell
 megjegyezni a levlista címét, vagy előkeresni a címtárból, de a szálak
 követését használó levelezők számára ez maga a rémálom. Így követhetetlenné
 válik, hogy mi új kérdés, és mi válasz. Akik ezt a módszert választják
 a kérdésük megírására, általában a szálkövető olvasók figyelmét
 teljesen elvesztik, hiszen a száltól teljes mértékben eltérő kérdést
 indítanak el.
</p>
  
<p>
 Érdemes a szálakat is támogató levelezőprogramok ezen szolgaltatását
 kipróbálni, mert hamar termeszetessé válik, és egy idő után már nem
 lehet nélküle élni. Javasoljuk, hogy ha eddig nem használtad ezt a
 levélolvasást jelentősen megkönnyítő szolgáltatást, mindenképpen
 tegyél egy próbát.
</p>
  
<p>
 Ha nem is szereted meg, egy dologra mindig emlékezz: <strong>új kérdés
 küldésekor új levelet írj, ne már beküldött levélre válaszolj</strong>!
 A listatagoktól elvárjuk a fentiek megértését, és betartását!
 Köszönjük.
</p>

<h3>Szálkezelés beállítása Outlook esetén</h3>

<p>
 A szálkezelés problémája fokozottan jelentkezik az Outlook* klienst
 használó tagoknál. Nekik szeretnénk segíteni, hogy be tudják állítani
 a szálkezelés felhasználói felületen történő megjelenítését. A menüpont
 megnevezésének sajnos semmi köze a szálakhoz (threading), itt másképpen
 hívják. Nézzük hogyan állítható be a szál nézet Outlook Expressben.
</p>
  
<ul>  
 <li>Angol verzió: View &raquo; Current View &raquo; Group Messages by Conversation.</li>
 <li>Magyar verzió: Nézet &raquo; Aktuális nézet &raquo; Üzenetek csoportosítása témakörök szerint.</li>
</ul>

<p>
 Még egyszer fontosnak tartjuk megjegyezni, hogy ezen beállítástól függetlenül
 az Outlook Express ugyanúgy nyilváltartja a szálakat, a válaszoláskor kezeli
 azokat, csupán alapértelmezésben nem így jeleníti meg a leveleket a felhasználó
 számára.
</p>
</div>
ILLEM_END;
  return $illem;
}
