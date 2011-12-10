dp.sh.Brushes.Apache = function()
{
  var keywords = 'Include PHPINIDir Listen ServerName NameVirtualHost ServerAdmin DocumentRoot ErrorLog CustomLog SSLEngine ServerSignature ProxyPass ' +
  'LoadModule FastCgiIpcDir FastCgiWrapper SuExecUserGroup ScriptAlias Options SetHandler SuexecUserGroup Order AddType Action';
  // From http://httpd.apache.org/docs/1.3/mod/directives.html
  /*'AcceptFilter AcceptMutex AccessConfig AccessFileName Action AddAlt AddAltByEncoding AddAltByType AddCharset ' +
  'AddDefaultCharset AddDescription AddEncoding AddHandler AddIcon AddIconByEncoding AddIconByType AddLanguage AddModule ' +
  'AddModuleInfo AddType AgentLog Alias AliasMatch Allow AllowCONNECT AllowOverride Anonymous Anonymous_Authoritative ' +
  'Anonymous_LogEmail Anonymous_MustGiveEmail Anonymous_NoUserID Anonymous_VerifyEmail AuthAuthoritative AuthDBAuthoritative' +
  'AuthDBGroupFile AuthDBMAuthoritative AuthDBMGroupFile AuthDBMGroupFile AuthDBUserFile AuthDBMUserFile AuthDigestFile ' +
  'AuthGroupFile AuthName AuthType AuthUserFile BindAddress BrowserMatch BrowserMatchNoCase BS2000Account CacheDefaultExpire ' +
  'CacheDirLength CacheDirLevels CacheForceCompletion CacheGcInterval CacheLastModifiedFactor CacheMaxExpire CacheNegotiatedDocs ' +
  'CacheRoot CacheSize CGICommandArgs CheckSpelling ClearModuleList ContentDigest CookieDomain CookieExpires CookieFormat ' +
  'CookieLog CookiePrefix CookieStyle CookieTracking CoreDumpDirectory CustomLog DefaultIcon DefaultLanguage DefaultType ' +
  'Deny DirectoryIndex DocumentRoot EBCDICConvert EBCDICConvertByType EBCDICKludge EnableExceptionHook ' +
  'ErrorDocument ErrorHeader ErrorLog Example ExpiresActive ExpiresByType ExpiresDefault ExtendedStatus FancyIndexing FileETag ' +
  'ForceType ForensicLog Group Header HeaderName HostnameLookups IdentityCheck ' +
  'ImapBase ImapDefault ImapMenu Include IndexIgnore IndexOptions IndexOrderDefault ISAPIReadAheadBuffer ISAPILogNotSupported ' +
  'ISAPIAppendLogToErrors ISAPIAppendLogToQuery KeepAlive KeepAliveTimeout LanguagePriority  LimitInternalRecursion ' +
  'LimitRequestBody LimitRequestFields LimitRequestFieldsize LimitRequestLine Listen ListenBacklog LoadFile LoadModule ' +
  'LockFile LogFormat LogLevel MaxClients MaxKeepAliveRequests MaxRequestsPerChild MaxSpareServers MetaDir ' +
  'MetaFiles MetaSuffix MimeMagicFile MinSpareServers MMapFile NameVirtualHost NoCache Options Order PassEnv PidFile Port ' +
  'ProtocolReqCheck ProxyBlock ProxyDomain ProxyPass ProxyPassReverse ProxyReceiveBufferSize ProxyRemote ProxyRequests ' +
  'ProxyVia ReadmeName Redirect RedirectMatch RedirectPermanent RedirectTemp RefererIgnore RefererLog RemoveEncoding ' +
  'RemoveHandler RemoveType Require ResourceConfig RewriteBase RewriteCond RewriteEngine RewriteLock RewriteLog ' +
  'RewriteLogLevel RewriteMap RewriteOptions RewriteRule RLimitCPU RLimitMEM RLimitNPROC Satisfy ScoreBoardFile Script ' +
  'ScriptAlias ScriptAliasMatch ScriptInterpreterSource ScriptLog ScriptLogBuffer ScriptLogLength SendBufferSize ' +
  'ServerAdmin ServerAlias ServerName ServerPath ServerRoot ServerSignature ServerTokens ServerType SetEnv SetEnvIf ' +
  'SetEnvIfNoCase SetHandler ShmemUIDisUser StartServers ThreadsPerChild TimeOut TraceEnable TransferLog TypesConfig ' +
  'UnsetEnv UseCanonicalName User UserDir VirtualDocumentRoot VirtualDocumentRootIP  VirtualScriptAlias ' +
  'VirtualScriptAliasIP XBitHack' +
	
  // Used on weblabor.hu
  'FastCgiIpcDir FastCgiWrapper SuExecUserGroup SuexecUserGroup';*/

	this.regexList = [
		{ regex: dp.sh.RegexLib.SingleLinePerlComments,				css: 'comment' },
		{ regex: dp.sh.RegexLib.DoubleQuotedString,				css: 'string' },
		{ regex: dp.sh.RegexLib.SingleQuotedString,				css: 'string' },
		{ regex: new RegExp(this.GetKeywords(keywords), 'gm'),			css: 'keyword' }
	];
	this.CssClass = 'dp-apache';
}
dp.sh.Brushes.Apache.prototype	= new dp.sh.Highlighter();
dp.sh.Brushes.Apache.Aliases	= ['apache', 'httpd'];
