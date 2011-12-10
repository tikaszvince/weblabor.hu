<?php
// $Id: settings.php,v 1.37 2006/12/30 15:28:33 dries Exp $
$db_url    = 'mysql://root:root@localhost/weblabor';
$db_prefix = '';
$update_free_access = FALSE;
// $base_url  = 'http://weblabor.hu';  // Optional. NO trailing slash!

ini_set('arg_separator.output',     '&amp;');
ini_set('magic_quotes_runtime',     0);
ini_set('magic_quotes_sybase',      0);
ini_set('session.cache_expire',     200000);
ini_set('session.cache_limiter',    'none');
ini_set('session.cookie_lifetime',  2000000);
ini_set('session.gc_maxlifetime',   200000);
ini_set('session.save_handler',     'user');
ini_set('session.use_only_cookies', 1);
ini_set('session.use_trans_sid',    0);
ini_set('url_rewriter.tags',        '');

$cookie_domain = 'weblabor.hu';
ini_set('session.cookie_domain', 'weblabor.hu');
ini_set('session.name',          'WEBLABOR');

define('STORAGE_FOLDER',  '/var/www/weblabor.hu/');
define('HIRLEVEL_FOLDER', STORAGE_FOLDER .'hirlevel/');
define('LEVLISTAK_PATH',  '/usr/local/mailman/archives/public/');
define('LEVLISTAK_URL',   'https://bors.hoszting.com/pipermail/');

include_once 'includes/wllib.inc';
