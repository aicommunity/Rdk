#-------------------------------------------------
#
# Project created by QtCreator 2012-11-11T15:17:55
#
#-------------------------------------------------

QT       -= gui

TARGET = curl.qt
TEMPLATE = lib
CONFIG += staticlib

DEFINES += LIBRDK_LIBRARY_EXPORT
DEFINES += RDK_UNICODE_RUN
DEFINES += RDK_QT
DEFINES += CURL_STATICLIB
INCLUDEPATH += $$PWD/../../include

unix {
    DESTDIR = $$PWD/../../../../../Bin/Platform/Unix/Lib.Qt
}

windows {
    DESTDIR = $$PWD/../../../../../Bin/Platform/Win/Lib.Qt

 LIBS +=   -lWldap32 -lWs2_32 -lCrypt32
}

HEADERS += \
    ../../lib/amigaos.h \
    ../../lib/arpa_telnet.h \
    ../../lib/asyn.h \
    ../../lib/axtls.h \
    ../../lib/bundles.h \
    ../../lib/config-amigaos.h \
    ../../lib/config-dos.h \
    ../../lib/config-mac.h \
    ../../lib/config-os400.h \
    ../../lib/config-riscos.h \
    ../../lib/config-symbian.h \
    ../../lib/config-tpf.h \
    ../../lib/config-vxworks.h \
    ../../lib/config-win32.h \
    ../../lib/config-win32ce.h \
    ../../lib/conncache.h \
    ../../lib/connect.h \
    ../../lib/content_encoding.h \
    ../../lib/cookie.h \
    ../../lib/curl_addrinfo.h \
    ../../lib/curl_base64.h \
    ../../lib/curl_darwinssl.h \
    ../../lib/curl_fnmatch.h \
    ../../lib/curl_gethostname.h \
    ../../lib/curl_gssapi.h \
    ../../lib/curl_hmac.h \
    ../../lib/curl_ldap.h \
    ../../lib/curl_md4.h \
    ../../lib/curl_md5.h \
    ../../lib/curl_memory.h \
    ../../lib/curl_memrchr.h \
    ../../lib/curl_multibyte.h \
    ../../lib/curl_ntlm.h \
    ../../lib/curl_ntlm_core.h \
    ../../lib/curl_ntlm_msgs.h \
    ../../lib/curl_ntlm_wb.h \
    ../../lib/curl_rand.h \
    ../../lib/curl_rtmp.h \
    ../../lib/curl_sasl.h \
    ../../lib/curl_schannel.h \
    ../../lib/curl_setup.h \
    ../../lib/curl_setup_once.h \
    ../../lib/curl_sspi.h \
    ../../lib/curl_threads.h \
    ../../lib/curlx.h \
    ../../lib/cyassl.h \
    ../../lib/dict.h \
    ../../lib/easyif.h \
    ../../lib/escape.h \
    ../../lib/file.h \
    ../../lib/fileinfo.h \
    ../../lib/formdata.h \
    ../../lib/ftp.h \
    ../../lib/ftplistparser.h \
    ../../lib/getinfo.h \
    ../../lib/gopher.h \
    ../../lib/gtls.h \
    ../../lib/hash.h \
    ../../lib/hostcheck.h \
    ../../lib/hostip.h \
    ../../lib/http.h \
    ../../lib/http_chunks.h \
    ../../lib/http_digest.h \
    ../../lib/http_negotiate.h \
    ../../lib/http_proxy.h \
    ../../lib/if2ip.h \
    ../../lib/imap.h \
    ../../lib/inet_ntop.h \
    ../../lib/inet_pton.h \
    ../../lib/krb4.h \
    ../../lib/llist.h \
    ../../lib/memdebug.h \
    ../../lib/multihandle.h \
    ../../lib/multiif.h \
    ../../lib/netrc.h \
    ../../lib/non-ascii.h \
    ../../lib/nonblock.h \
    ../../lib/nssg.h \
    ../../lib/parsedate.h \
    ../../lib/pingpong.h \
    ../../lib/pipeline.h \
    ../../lib/polarssl.h \
    ../../lib/polarssl_threadlock.h \
    ../../lib/pop3.h \
    ../../lib/progress.h \
    ../../lib/qssl.h \
    ../../lib/rawstr.h \
    ../../lib/rtsp.h \
    ../../lib/select.h \
    ../../lib/sendf.h \
    ../../lib/setup-os400.h \
    ../../lib/setup-vms.h \
    ../../lib/share.h \
    ../../lib/slist.h \
    ../../lib/smtp.h \
    ../../lib/sockaddr.h \
    ../../lib/socks.h \
    ../../lib/speedcheck.h \
    ../../lib/splay.h \
    ../../lib/ssh.h \
    ../../lib/sslgen.h \
    ../../lib/ssluse.h \
    ../../lib/strdup.h \
    ../../lib/strequal.h \
    ../../lib/strerror.h \
    ../../lib/strtok.h \
    ../../lib/strtoofft.h \
    ../../lib/telnet.h \
    ../../lib/tftp.h \
    ../../lib/timeval.h \
    ../../lib/transfer.h \
    ../../lib/url.h \
    ../../lib/urldata.h \
    ../../lib/warnless.h \
    ../../lib/wildcard.h \
    ../../include/curl/curl.h \
    ../../include/curl/curlbuild.h \
    ../../include/curl/curlbuild.h.in \
    ../../include/curl/curlrules.h \
    ../../include/curl/curlver.h \
    ../../include/curl/easy.h \
    ../../include/curl/mprintf.h \
    ../../include/curl/multi.h \
    ../../include/curl/stdcheaders.h \
    ../../include/curl/typecheck-gcc.h

SOURCES += \
    ../../lib/amigaos.c \
    ../../lib/asyn-ares.c \
    ../../lib/asyn-thread.c \
    ../../lib/axtls.c \
    ../../lib/base64.c \
    ../../lib/bundles.c \
    ../../lib/conncache.c \
    ../../lib/connect.c \
    ../../lib/content_encoding.c \
    ../../lib/cookie.c \
    ../../lib/curl_addrinfo.c \
    ../../lib/curl_darwinssl.c \
    ../../lib/curl_fnmatch.c \
    ../../lib/curl_gethostname.c \
    ../../lib/curl_gssapi.c \
    ../../lib/curl_memrchr.c \
    ../../lib/curl_multibyte.c \
    ../../lib/curl_ntlm.c \
    ../../lib/curl_ntlm_core.c \
    ../../lib/curl_ntlm_msgs.c \
    ../../lib/curl_ntlm_wb.c \
    ../../lib/curl_rand.c \
    ../../lib/curl_rtmp.c \
    ../../lib/curl_sasl.c \
    ../../lib/curl_schannel.c \
    ../../lib/curl_sspi.c \
    ../../lib/curl_threads.c \
    ../../lib/cyassl.c \
    ../../lib/dict.c \
    ../../lib/easy.c \
    ../../lib/escape.c \
    ../../lib/file.c \
    ../../lib/fileinfo.c \
    ../../lib/formdata.c \
    ../../lib/ftp.c \
    ../../lib/ftplistparser.c \
    ../../lib/getenv.c \
    ../../lib/getinfo.c \
    ../../lib/gopher.c \
    ../../lib/gtls.c \
    ../../lib/hash.c \
    ../../lib/hmac.c \
    ../../lib/hostasyn.c \
    ../../lib/hostcheck.c \
    ../../lib/hostip.c \
    ../../lib/hostip4.c \
    ../../lib/hostip6.c \
    ../../lib/hostsyn.c \
    ../../lib/http.c \
    ../../lib/http_chunks.c \
    ../../lib/http_digest.c \
    ../../lib/http_negotiate.c \
    ../../lib/http_negotiate_sspi.c \
    ../../lib/http_proxy.c \
    ../../lib/idn_win32.c \
    ../../lib/if2ip.c \
    ../../lib/imap.c \
    ../../lib/inet_ntop.c \
    ../../lib/inet_pton.c \
    ../../lib/krb4.c \
    ../../lib/krb5.c \
    ../../lib/ldap.c \
    ../../lib/llist.c \
    ../../lib/md4.c \
    ../../lib/md5.c \
    ../../lib/memdebug.c \
    ../../lib/mprintf.c \
    ../../lib/multi.c \
    ../../lib/netrc.c \
    ../../lib/non-ascii.c \
    ../../lib/nonblock.c \
    ../../lib/nss.c \
    ../../lib/nwlib.c \
    ../../lib/nwos.c \
    ../../lib/openldap.c \
    ../../lib/parsedate.c \
    ../../lib/pingpong.c \
    ../../lib/pipeline.c \
    ../../lib/polarssl.c \
    ../../lib/polarssl_threadlock.c \
    ../../lib/pop3.c \
    ../../lib/progress.c \
    ../../lib/qssl.c \
    ../../lib/rawstr.c \
    ../../lib/rtsp.c \
    ../../lib/security.c \
    ../../lib/select.c \
    ../../lib/sendf.c \
    ../../lib/share.c \
    ../../lib/slist.c \
    ../../lib/smtp.c \
    ../../lib/socks.c \
    ../../lib/socks_gssapi.c \
    ../../lib/socks_sspi.c \
    ../../lib/speedcheck.c \
    ../../lib/splay.c \
    ../../lib/ssh.c \
    ../../lib/sslgen.c \
    ../../lib/ssluse.c \
    ../../lib/strdup.c \
    ../../lib/strequal.c \
    ../../lib/strerror.c \
    ../../lib/strtok.c \
    ../../lib/strtoofft.c \
    ../../lib/telnet.c \
    ../../lib/tftp.c \
    ../../lib/timeval.c \
    ../../lib/transfer.c \
    ../../lib/url.c \
    ../../lib/version.c \
    ../../lib/warnless.c \
    ../../lib/wildcard.c

DISTFILES +=
