#ifndef THIRD_PARTY_CPP
#define THIRD_PARTY_CPP

//#ifndef _UNICODE
//#define RDK_UNICODE_RUN
//#define _UNICODE
//#endif

#ifdef RDK_UNICODE_RUN
#define XML_UNICODE_RUN
#endif

#define _USE_XMLPARSER_DLL
#define _CRT_SECURE_NO_WARNINGS
#include "xmlParser/xmlParser.cpp"
#undef _CRT_SECURE_NO_WARNINGS

//#ifdef RDK_UNICODE_RUN
//#undef _UNICODE
//#undef RDK_UNICODE_RUN
//#endif

// Add curl
#ifndef RDK_CURL
#define RDK_CURL

#ifdef __BORLANDC__
//#pragma link "libcurl.bcb.lib"
#pragma link "Ws2_32.lib"
#endif

#ifdef _MSC_VER
//#pragma comment( lib, "curl/Bin/Platform/Win/libcurl.vs.lib" )
#pragma comment( lib, "Ws2_32.lib" )
#endif

#include "jpeg/JpegIntegration.cpp"


		 /*
#define CURL_STATICLIB
#define BUILDING_LIBCURL
#define HTTP_ONLY


#include "curl/lib/amigaos.c"
#include "curl/lib/asyn-ares.c"
#include "curl/lib/asyn-thread.c"
#include "curl/lib/axtls.c"
#include "curl/lib/base64.c"
#include "curl/lib/bundles.c"
#include "curl/lib/conncache.c"
#include "curl/lib/connect.c"
#include "curl/lib/content_encoding.c"
#include "curl/lib/cookie.c"
#include "curl/lib/curl_addrinfo.c"
#include "curl/lib/curl_darwinssl.c"
#include "curl/lib/curl_fnmatch.c"
#include "curl/lib/curl_gethostname.c"
#include "curl/lib/curl_gssapi.c"
#include "curl/lib/curl_memrchr.c"
#include "curl/lib/curl_multibyte.c"
#include "curl/lib/curl_ntlm.c"
#include "curl/lib/curl_ntlm_core.c"
#include "curl/lib/curl_ntlm_msgs.c"
#include "curl/lib/curl_ntlm_wb.c"
#include "curl/lib/curl_rand.c"
#include "curl/lib/curl_rtmp.c"
#include "curl/lib/curl_sasl.c"
#include "curl/lib/curl_schannel.c"
#include "curl/lib/curl_sspi.c"
#include "curl/lib/curl_threads.c"
#include "curl/lib/cyassl.c"
#include "curl/lib/dict.c"
#include "curl/lib/easy.c"
#include "curl/lib/escape.c"
#include "curl/lib/file.c"
#include "curl/lib/fileinfo.c"
#include "curl/lib/formdata.c"
#include "curl/lib/ftp.c"
#include "curl/lib/ftplistparser.c"
#include "curl/lib/getenv.c"
#include "curl/lib/getinfo.c"
#include "curl/lib/gopher.c"
#include "curl/lib/gtls.c"
#include "curl/lib/hash.c"
#include "curl/lib/hmac.c"
#include "curl/lib/hostasyn.c"
#include "curl/lib/hostcheck.c"
#include "curl/lib/hostip.c"
#include "curl/lib/hostip4.c"
#include "curl/lib/hostip6.c"
#include "curl/lib/hostsyn.c"
#include "curl/lib/http.c"
#include "curl/lib/http_chunks.c"
#include "curl/lib/http_digest.c"
#include "curl/lib/http_negotiate.c"
#include "curl/lib/http_negotiate_sspi.c"
#include "curl/lib/http_proxy.c"
#include "curl/lib/idn_win32.c"
#include "curl/lib/if2ip.c"
#include "curl/lib/imap.c"
#include "curl/lib/inet_ntop.c"
#include "curl/lib/inet_pton.c"
#include "curl/lib/krb4.c"
#include "curl/lib/krb5.c"
#include "curl/lib/ldap.c"
#include "curl/lib/llist.c"
#include "curl/lib/md4.c"
#include "curl/lib/md5.c"
#include "curl/lib/memdebug.c"
#include "curl/lib/mprintf.c"
#include "curl/lib/multi.c"
#include "curl/lib/netrc.c"
#include "curl/lib/non-ascii.c"
#include "curl/lib/nonblock.c"
#include "curl/lib/nss.c"
#include "curl/lib/nwlib.c"
#include "curl/lib/nwos.c"
#include "curl/lib/openldap.c"
#include "curl/lib/parsedate.c"
#include "curl/lib/pingpong.c"
#include "curl/lib/pipeline.c"
#include "curl/lib/polarssl.c"
#include "curl/lib/polarssl_threadlock.c"
#include "curl/lib/pop3.c"
#include "curl/lib/progress.c"
#include "curl/lib/qssl.c"
#include "curl/lib/rawstr.c"
#include "curl/lib/rtsp.c"
#include "curl/lib/security.c"
#include "curl/lib/select.c"
#include "curl/lib/sendf.c"
#include "curl/lib/share.c"
#include "curl/lib/slist.c"
#include "curl/lib/smtp.c"
#include "curl/lib/socks.c"
#include "curl/lib/socks_gssapi.c"
#include "curl/lib/socks_sspi.c"
#include "curl/lib/speedcheck.c"
#include "curl/lib/splay.c"
#include "curl/lib/ssh.c"
#include "curl/lib/sslgen.c"
#include "curl/lib/ssluse.c"
#include "curl/lib/strdup.c"
#include "curl/lib/strequal.c"
#include "curl/lib/strerror.c"
#include "curl/lib/strtok.c"
#include "curl/lib/strtoofft.c"
#include "curl/lib/telnet.c"
#include "curl/lib/tftp.c"
#include "curl/lib/timeval.c"
#include "curl/lib/transfer.c"
#include "curl/lib/url.c"
#include "curl/lib/version.c"
#include "curl/lib/warnless.c"
#include "curl/lib/wildcard.c"
           */
#endif


#endif
