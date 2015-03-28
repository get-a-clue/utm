#include "stdafx.h"
#include "mimetypes.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <stringtools.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

#define MIME_TYPE_FILE_DELIMITERS " \t"
#define MIME_TYPES_FILENAME "mime.types"

namespace utm {

mimetypes::mimetypes(void)
{
	init();
}

mimetypes::~mimetypes(void)
{
}

void mimetypes::init()
{
	mtypes.clear();
	mtypes.insert(std::make_pair("%", "application/x-trash"));
	mtypes.insert(std::make_pair("323", "text/h323"));
	mtypes.insert(std::make_pair("3gp", "video/3gpp"));
	mtypes.insert(std::make_pair("7z", "application/x-7z-compressed"));
	mtypes.insert(std::make_pair("abw", "application/x-abiword"));
	mtypes.insert(std::make_pair("ai", "application/postscript"));
	mtypes.insert(std::make_pair("aif", "audio/x-aiff"));
	mtypes.insert(std::make_pair("aifc", "audio/x-aiff"));
	mtypes.insert(std::make_pair("aiff", "audio/x-aiff"));
	mtypes.insert(std::make_pair("alc", "chemical/x-alchemy"));
	mtypes.insert(std::make_pair("amr", "audio/amr"));
	mtypes.insert(std::make_pair("anx", "application/annodex"));
	mtypes.insert(std::make_pair("apk", "application/vnd.android.package-archive"));
	mtypes.insert(std::make_pair("art", "image/x-jg"));
	mtypes.insert(std::make_pair("asc", "text/plain"));
	mtypes.insert(std::make_pair("asf", "video/x-ms-asf"));
	mtypes.insert(std::make_pair("asn", "chemical/x-ncbi-asn1"));
	mtypes.insert(std::make_pair("aso", "chemical/x-ncbi-asn1-binary"));
	mtypes.insert(std::make_pair("asx", "video/x-ms-asf"));
	mtypes.insert(std::make_pair("atom", "application/atom+xml"));
	mtypes.insert(std::make_pair("atomcat", "application/atomcat+xml"));
	mtypes.insert(std::make_pair("atomsrv", "application/atomserv+xml"));
	mtypes.insert(std::make_pair("au", "audio/basic"));
	mtypes.insert(std::make_pair("avi", "video/x-msvideo"));
	mtypes.insert(std::make_pair("awb", "audio/amr-wb"));
	mtypes.insert(std::make_pair("axa", "audio/annodex"));
	mtypes.insert(std::make_pair("axv", "video/annodex"));
	mtypes.insert(std::make_pair("b", "chemical/x-molconn-z"));
	mtypes.insert(std::make_pair("bak", "application/x-trash"));
	mtypes.insert(std::make_pair("bat", "application/x-msdos-program"));
	mtypes.insert(std::make_pair("bcpio", "application/x-bcpio"));
	mtypes.insert(std::make_pair("bib", "text/x-bibtex"));
	mtypes.insert(std::make_pair("bin", "application/octet-stream"));
	mtypes.insert(std::make_pair("bmp", "image/x-ms-bmp"));
	mtypes.insert(std::make_pair("boo", "text/x-boo"));
	mtypes.insert(std::make_pair("book", "application/x-maker"));
	mtypes.insert(std::make_pair("brf", "text/plain"));
	mtypes.insert(std::make_pair("bsd", "chemical/x-crossfire"));
	mtypes.insert(std::make_pair("c", "text/x-csrc"));
	mtypes.insert(std::make_pair("c++", "text/x-c++src"));
	mtypes.insert(std::make_pair("c3d", "chemical/x-chem3d"));
	mtypes.insert(std::make_pair("cab", "application/x-cab"));
	mtypes.insert(std::make_pair("cac", "chemical/x-cache"));
	mtypes.insert(std::make_pair("cache", "chemical/x-cache"));
	mtypes.insert(std::make_pair("cap", "application/cap"));
	mtypes.insert(std::make_pair("cascii", "chemical/x-cactvs-binary"));
	mtypes.insert(std::make_pair("cat", "application/vnd.ms-pki.seccat"));
	mtypes.insert(std::make_pair("cbin", "chemical/x-cactvs-binary"));
	mtypes.insert(std::make_pair("cbr", "application/x-cbr"));
	mtypes.insert(std::make_pair("cbz", "application/x-cbz"));
	mtypes.insert(std::make_pair("cc", "text/x-c++src"));
	mtypes.insert(std::make_pair("cda", "application/x-cdf"));
	mtypes.insert(std::make_pair("cdf", "application/x-cdf"));
	mtypes.insert(std::make_pair("cdr", "image/x-coreldraw"));
	mtypes.insert(std::make_pair("cdt", "image/x-coreldrawtemplate"));
	mtypes.insert(std::make_pair("cdx", "chemical/x-cdx"));
	mtypes.insert(std::make_pair("cdy", "application/vnd.cinderella"));
	mtypes.insert(std::make_pair("cef", "chemical/x-cxf"));
	mtypes.insert(std::make_pair("cer", "chemical/x-cerius"));
	mtypes.insert(std::make_pair("chm", "chemical/x-chemdraw"));
	mtypes.insert(std::make_pair("chrt", "application/x-kchart"));
	mtypes.insert(std::make_pair("cif", "chemical/x-cif"));
	mtypes.insert(std::make_pair("class", "application/java-vm"));
	mtypes.insert(std::make_pair("cls", "text/x-tex"));
	mtypes.insert(std::make_pair("cmdf", "chemical/x-cmdf"));
	mtypes.insert(std::make_pair("cml", "chemical/x-cml"));
	mtypes.insert(std::make_pair("cod", "application/vnd.rim.cod"));
	mtypes.insert(std::make_pair("com", "application/x-msdos-program"));
	mtypes.insert(std::make_pair("cpa", "chemical/x-compass"));
	mtypes.insert(std::make_pair("cpio", "application/x-cpio"));
	mtypes.insert(std::make_pair("cpp", "text/x-c++src"));
	mtypes.insert(std::make_pair("cpt", "application/mac-compactpro"));
	mtypes.insert(std::make_pair("cr2", "image/x-canon-cr2"));
	mtypes.insert(std::make_pair("crl", "application/x-pkcs7-crl"));
	mtypes.insert(std::make_pair("crt", "application/x-x509-ca-cert"));
	mtypes.insert(std::make_pair("crw", "image/x-canon-crw"));
	mtypes.insert(std::make_pair("csd", "audio/csound"));
	mtypes.insert(std::make_pair("csf", "chemical/x-cache-csf"));
	mtypes.insert(std::make_pair("csh", "application/x-csh"));
	mtypes.insert(std::make_pair("csm", "chemical/x-csml"));
	mtypes.insert(std::make_pair("csml", "chemical/x-csml"));
	mtypes.insert(std::make_pair("css", "text/css"));
	mtypes.insert(std::make_pair("csv", "text/csv"));
	mtypes.insert(std::make_pair("ctab", "chemical/x-cactvs-binary"));
	mtypes.insert(std::make_pair("ctx", "chemical/x-ctx"));
	mtypes.insert(std::make_pair("cu", "application/cu-seeme"));
	mtypes.insert(std::make_pair("cub", "chemical/x-gaussian-cube"));
	mtypes.insert(std::make_pair("cxf", "chemical/x-cxf"));
	mtypes.insert(std::make_pair("cxx", "text/x-c++src"));
	mtypes.insert(std::make_pair("d", "text/x-dsrc"));
	mtypes.insert(std::make_pair("dat", "application/x-ns-proxy-autoconfig"));
	mtypes.insert(std::make_pair("davmount", "application/davmount+xml"));
	mtypes.insert(std::make_pair("dcr", "application/x-director"));
	mtypes.insert(std::make_pair("deb", "application/x-debian-package"));
	mtypes.insert(std::make_pair("dif", "video/dv"));
	mtypes.insert(std::make_pair("diff", "text/x-diff"));
	mtypes.insert(std::make_pair("dir", "application/x-director"));
	mtypes.insert(std::make_pair("djv", "image/vnd.djvu"));
	mtypes.insert(std::make_pair("djvu", "image/vnd.djvu"));
	mtypes.insert(std::make_pair("dl", "video/dl"));
	mtypes.insert(std::make_pair("dll", "application/x-msdos-program"));
	mtypes.insert(std::make_pair("dmg", "application/x-apple-diskimage"));
	mtypes.insert(std::make_pair("dms", "application/x-dms"));
	mtypes.insert(std::make_pair("doc", "application/msword"));
	mtypes.insert(std::make_pair("docm", "application/vnd.ms-word.document.macroenabled.12"));
	mtypes.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	mtypes.insert(std::make_pair("dot", "application/msword"));
	mtypes.insert(std::make_pair("dotm", "application/vnd.ms-word.template.macroenabled.12"));
	mtypes.insert(std::make_pair("dotx", "application/vnd.openxmlformats-officedocument.wordprocessingml.template"));
	mtypes.insert(std::make_pair("dv", "video/dv"));
	mtypes.insert(std::make_pair("dvi", "application/x-dvi"));
	mtypes.insert(std::make_pair("dx", "chemical/x-jcamp-dx"));
	mtypes.insert(std::make_pair("dxr", "application/x-director"));
	mtypes.insert(std::make_pair("emb", "chemical/x-embl-dl-nucleotide"));
	mtypes.insert(std::make_pair("embl", "chemical/x-embl-dl-nucleotide"));
	mtypes.insert(std::make_pair("eml", "message/rfc822"));
	mtypes.insert(std::make_pair("ent", "chemical/x-ncbi-asn1-ascii"));
	mtypes.insert(std::make_pair("eps", "application/postscript"));
	mtypes.insert(std::make_pair("eps2", "application/postscript"));
	mtypes.insert(std::make_pair("eps3", "application/postscript"));
	mtypes.insert(std::make_pair("epsf", "application/postscript"));
	mtypes.insert(std::make_pair("epsi", "application/postscript"));
	mtypes.insert(std::make_pair("erf", "image/x-epson-erf"));
	mtypes.insert(std::make_pair("es", "application/ecmascript"));
	mtypes.insert(std::make_pair("etx", "text/x-setext"));
	mtypes.insert(std::make_pair("exe", "application/x-msdos-program"));
	mtypes.insert(std::make_pair("ez", "application/andrew-inset"));
	mtypes.insert(std::make_pair("fb", "application/x-maker"));
	mtypes.insert(std::make_pair("fbdoc", "application/x-maker"));
	mtypes.insert(std::make_pair("fch", "chemical/x-gaussian-checkpoint"));
	mtypes.insert(std::make_pair("fchk", "chemical/x-gaussian-checkpoint"));
	mtypes.insert(std::make_pair("fig", "application/x-xfig"));
	mtypes.insert(std::make_pair("flac", "audio/flac"));
	mtypes.insert(std::make_pair("fli", "video/fli"));
	mtypes.insert(std::make_pair("flv", "video/x-flv"));
	mtypes.insert(std::make_pair("fm", "application/x-maker"));
	mtypes.insert(std::make_pair("frame", "application/x-maker"));
	mtypes.insert(std::make_pair("frm", "application/x-maker"));
	mtypes.insert(std::make_pair("gal", "chemical/x-gaussian-log"));
	mtypes.insert(std::make_pair("gam", "chemical/x-gamess-input"));
	mtypes.insert(std::make_pair("gamin", "chemical/x-gamess-input"));
	mtypes.insert(std::make_pair("gan", "application/x-ganttproject"));
	mtypes.insert(std::make_pair("gau", "chemical/x-gaussian-input"));
	mtypes.insert(std::make_pair("gcd", "text/x-pcs-gcd"));
	mtypes.insert(std::make_pair("gcf", "application/x-graphing-calculator"));
	mtypes.insert(std::make_pair("gcg", "chemical/x-gcg8-sequence"));
	mtypes.insert(std::make_pair("gen", "chemical/x-genbank"));
	mtypes.insert(std::make_pair("gf", "application/x-tex-gf"));
	mtypes.insert(std::make_pair("gif", "image/gif"));
	mtypes.insert(std::make_pair("gjc", "chemical/x-gaussian-input"));
	mtypes.insert(std::make_pair("gjf", "chemical/x-gaussian-input"));
	mtypes.insert(std::make_pair("gl", "video/gl"));
	mtypes.insert(std::make_pair("gnumeric", "application/x-gnumeric"));
	mtypes.insert(std::make_pair("gpt", "chemical/x-mopac-graph"));
	mtypes.insert(std::make_pair("gsf", "application/x-font"));
	mtypes.insert(std::make_pair("gsm", "audio/x-gsm"));
	mtypes.insert(std::make_pair("gtar", "application/x-gtar"));
	mtypes.insert(std::make_pair("h", "text/x-chdr"));
	mtypes.insert(std::make_pair("h++", "text/x-c++hdr"));
	mtypes.insert(std::make_pair("hdf", "application/x-hdf"));
	mtypes.insert(std::make_pair("hh", "text/x-c++hdr"));
	mtypes.insert(std::make_pair("hin", "chemical/x-hin"));
	mtypes.insert(std::make_pair("hpp", "text/x-c++hdr"));
	mtypes.insert(std::make_pair("hqx", "application/mac-binhex40"));
	mtypes.insert(std::make_pair("hs", "text/x-haskell"));
	mtypes.insert(std::make_pair("hta", "application/hta"));
	mtypes.insert(std::make_pair("htc", "text/x-component"));
	mtypes.insert(std::make_pair("htm", "text/html"));
	mtypes.insert(std::make_pair("html", "text/html"));
	mtypes.insert(std::make_pair("hxx", "text/x-c++hdr"));
	mtypes.insert(std::make_pair("ica", "application/x-ica"));
	mtypes.insert(std::make_pair("ice", "x-conference/x-cooltalk"));
	mtypes.insert(std::make_pair("ico", "image/x-icon"));
	mtypes.insert(std::make_pair("ics", "text/calendar"));
	mtypes.insert(std::make_pair("icz", "text/calendar"));
	mtypes.insert(std::make_pair("ief", "image/ief"));
	mtypes.insert(std::make_pair("iges", "model/iges"));
	mtypes.insert(std::make_pair("igs", "model/iges"));
	mtypes.insert(std::make_pair("iii", "application/x-iphone"));
	mtypes.insert(std::make_pair("info", "application/x-info"));
	mtypes.insert(std::make_pair("inp", "chemical/x-gamess-input"));
	mtypes.insert(std::make_pair("ins", "application/x-internet-signup"));
	mtypes.insert(std::make_pair("iso", "application/x-iso9660-image"));
	mtypes.insert(std::make_pair("isp", "application/x-internet-signup"));
	mtypes.insert(std::make_pair("ist", "chemical/x-isostar"));
	mtypes.insert(std::make_pair("istr", "chemical/x-isostar"));
	mtypes.insert(std::make_pair("jad", "text/vnd.sun.j2me.app-descriptor"));
	mtypes.insert(std::make_pair("jam", "application/x-jam"));
	mtypes.insert(std::make_pair("jar", "application/java-archive"));
	mtypes.insert(std::make_pair("java", "text/x-java"));
	mtypes.insert(std::make_pair("jdx", "chemical/x-jcamp-dx"));
	mtypes.insert(std::make_pair("jmz", "application/x-jmol"));
	mtypes.insert(std::make_pair("jng", "image/x-jng"));
	mtypes.insert(std::make_pair("jnlp", "application/x-java-jnlp-file"));
	mtypes.insert(std::make_pair("jpe", "image/jpeg"));
	mtypes.insert(std::make_pair("jpeg", "image/jpeg"));
	mtypes.insert(std::make_pair("jpg", "image/jpeg"));
	mtypes.insert(std::make_pair("js", "application/javascript"));
	mtypes.insert(std::make_pair("json", "application/json"));
	mtypes.insert(std::make_pair("kar", "audio/midi"));
	mtypes.insert(std::make_pair("key", "application/pgp-keys"));
	mtypes.insert(std::make_pair("kil", "application/x-killustrator"));
	mtypes.insert(std::make_pair("kin", "chemical/x-kinemage"));
	mtypes.insert(std::make_pair("kml", "application/vnd.google-earth.kml+xml"));
	mtypes.insert(std::make_pair("kmz", "application/vnd.google-earth.kmz"));
	mtypes.insert(std::make_pair("kpr", "application/x-kpresenter"));
	mtypes.insert(std::make_pair("kpt", "application/x-kpresenter"));
	mtypes.insert(std::make_pair("ksp", "application/x-kspread"));
	mtypes.insert(std::make_pair("kwd", "application/x-kword"));
	mtypes.insert(std::make_pair("kwt", "application/x-kword"));
	mtypes.insert(std::make_pair("latex", "application/x-latex"));
	mtypes.insert(std::make_pair("lha", "application/x-lha"));
	mtypes.insert(std::make_pair("lhs", "text/x-literate-haskell"));
	mtypes.insert(std::make_pair("lin", "application/bbolin"));
	mtypes.insert(std::make_pair("lsf", "video/x-la-asf"));
	mtypes.insert(std::make_pair("lsx", "video/x-la-asf"));
	mtypes.insert(std::make_pair("ltx", "text/x-tex"));
	mtypes.insert(std::make_pair("lyx", "application/x-lyx"));
	mtypes.insert(std::make_pair("lzh", "application/x-lzh"));
	mtypes.insert(std::make_pair("lzx", "application/x-lzx"));
	mtypes.insert(std::make_pair("m3g", "application/m3g"));
	mtypes.insert(std::make_pair("m3u", "audio/mpegurl"));
	mtypes.insert(std::make_pair("m3u8", "application/x-mpegurl"));
	mtypes.insert(std::make_pair("m4a", "audio/mpeg"));
	mtypes.insert(std::make_pair("maker", "application/x-maker"));
	mtypes.insert(std::make_pair("man", "application/x-troff-man"));
	mtypes.insert(std::make_pair("manifest", "text/cache-manifest"));
	mtypes.insert(std::make_pair("mcif", "chemical/x-mmcif"));
	mtypes.insert(std::make_pair("mcm", "chemical/x-macmolecule"));
	mtypes.insert(std::make_pair("mdb", "application/msaccess"));
	mtypes.insert(std::make_pair("me", "application/x-troff-me"));
	mtypes.insert(std::make_pair("mesh", "model/mesh"));
	mtypes.insert(std::make_pair("mid", "audio/midi"));
	mtypes.insert(std::make_pair("midi", "audio/midi"));
	mtypes.insert(std::make_pair("mif", "application/x-mif"));
	mtypes.insert(std::make_pair("mkv", "video/x-matroska"));
	mtypes.insert(std::make_pair("mm", "application/x-freemind"));
	mtypes.insert(std::make_pair("mmd", "chemical/x-macromodel-input"));
	mtypes.insert(std::make_pair("mmf", "application/vnd.smaf"));
	mtypes.insert(std::make_pair("mml", "text/mathml"));
	mtypes.insert(std::make_pair("mmod", "chemical/x-macromodel-input"));
	mtypes.insert(std::make_pair("mng", "video/x-mng"));
	mtypes.insert(std::make_pair("moc", "text/x-moc"));
	mtypes.insert(std::make_pair("mol", "chemical/x-mdl-molfile"));
	mtypes.insert(std::make_pair("mol2", "chemical/x-mol2"));
	mtypes.insert(std::make_pair("moo", "chemical/x-mopac-out"));
	mtypes.insert(std::make_pair("mop", "chemical/x-mopac-input"));
	mtypes.insert(std::make_pair("mopcrt", "chemical/x-mopac-input"));
	mtypes.insert(std::make_pair("mov", "video/quicktime"));
	mtypes.insert(std::make_pair("movie", "video/x-sgi-movie"));
	mtypes.insert(std::make_pair("mp2", "audio/mpeg"));
	mtypes.insert(std::make_pair("mp3", "audio/mpeg"));
	mtypes.insert(std::make_pair("mp4", "video/mp4"));
	mtypes.insert(std::make_pair("mpc", "chemical/x-mopac-input"));
	mtypes.insert(std::make_pair("mpe", "video/mpeg"));
	mtypes.insert(std::make_pair("mpeg", "video/mpeg"));
	mtypes.insert(std::make_pair("mpega", "audio/mpeg"));
	mtypes.insert(std::make_pair("mpg", "video/mpeg"));
	mtypes.insert(std::make_pair("mpga", "audio/mpeg"));
	mtypes.insert(std::make_pair("mph", "application/x-comsol"));
	mtypes.insert(std::make_pair("mpv", "video/x-matroska"));
	mtypes.insert(std::make_pair("ms", "application/x-troff-ms"));
	mtypes.insert(std::make_pair("msh", "model/mesh"));
	mtypes.insert(std::make_pair("msi", "application/x-msi"));
	mtypes.insert(std::make_pair("mvb", "chemical/x-mopac-vib"));
	mtypes.insert(std::make_pair("mxf", "application/mxf"));
	mtypes.insert(std::make_pair("mxu", "video/vnd.mpegurl"));
	mtypes.insert(std::make_pair("nb", "application/mathematica"));
	mtypes.insert(std::make_pair("nbp", "application/mathematica"));
	mtypes.insert(std::make_pair("nc", "application/x-netcdf"));
	mtypes.insert(std::make_pair("nef", "image/x-nikon-nef"));
	mtypes.insert(std::make_pair("nwc", "application/x-nwc"));
	mtypes.insert(std::make_pair("o", "application/x-object"));
	mtypes.insert(std::make_pair("oda", "application/oda"));
	mtypes.insert(std::make_pair("odb", "application/vnd.oasis.opendocument.database"));
	mtypes.insert(std::make_pair("odc", "application/vnd.oasis.opendocument.chart"));
	mtypes.insert(std::make_pair("odf", "application/vnd.oasis.opendocument.formula"));
	mtypes.insert(std::make_pair("odg", "application/vnd.oasis.opendocument.graphics"));
	mtypes.insert(std::make_pair("odi", "application/vnd.oasis.opendocument.image"));
	mtypes.insert(std::make_pair("odm", "application/vnd.oasis.opendocument.text-master"));
	mtypes.insert(std::make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
	mtypes.insert(std::make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
	mtypes.insert(std::make_pair("odt", "application/vnd.oasis.opendocument.text"));
	mtypes.insert(std::make_pair("oga", "audio/ogg"));
	mtypes.insert(std::make_pair("ogg", "audio/ogg"));
	mtypes.insert(std::make_pair("ogv", "video/ogg"));
	mtypes.insert(std::make_pair("ogx", "application/ogg"));
	mtypes.insert(std::make_pair("old", "application/x-trash"));
	mtypes.insert(std::make_pair("one", "application/onenote"));
	mtypes.insert(std::make_pair("onepkg", "application/onenote"));
	mtypes.insert(std::make_pair("onetmp", "application/onenote"));
	mtypes.insert(std::make_pair("onetoc2", "application/onenote"));
	mtypes.insert(std::make_pair("orc", "audio/csound"));
	mtypes.insert(std::make_pair("orf", "image/x-olympus-orf"));
	mtypes.insert(std::make_pair("otg", "application/vnd.oasis.opendocument.graphics-template"));
	mtypes.insert(std::make_pair("oth", "application/vnd.oasis.opendocument.text-web"));
	mtypes.insert(std::make_pair("otp", "application/vnd.oasis.opendocument.presentation-template"));
	mtypes.insert(std::make_pair("ots", "application/vnd.oasis.opendocument.spreadsheet-template"));
	mtypes.insert(std::make_pair("ott", "application/vnd.oasis.opendocument.text-template"));
	mtypes.insert(std::make_pair("oza", "application/x-oz-application"));
	mtypes.insert(std::make_pair("p", "text/x-pascal"));
	mtypes.insert(std::make_pair("p7r", "application/x-pkcs7-certreqresp"));
	mtypes.insert(std::make_pair("pac", "application/x-ns-proxy-autoconfig"));
	mtypes.insert(std::make_pair("pas", "text/x-pascal"));
	mtypes.insert(std::make_pair("pat", "image/x-coreldrawpattern"));
	mtypes.insert(std::make_pair("patch", "text/x-diff"));
	mtypes.insert(std::make_pair("pbm", "image/x-portable-bitmap"));
	mtypes.insert(std::make_pair("pcap", "application/cap"));
	mtypes.insert(std::make_pair("pcf", "application/x-font"));
	mtypes.insert(std::make_pair("pcf.z", "application/x-font"));
	mtypes.insert(std::make_pair("pcx", "image/pcx"));
	mtypes.insert(std::make_pair("pdb", "chemical/x-pdb"));
	mtypes.insert(std::make_pair("pdf", "application/pdf"));
	mtypes.insert(std::make_pair("pfa", "application/x-font"));
	mtypes.insert(std::make_pair("pfb", "application/x-font"));
	mtypes.insert(std::make_pair("pgm", "image/x-portable-graymap"));
	mtypes.insert(std::make_pair("pgn", "application/x-chess-pgn"));
	mtypes.insert(std::make_pair("pgp", "application/pgp-signature"));
	mtypes.insert(std::make_pair("php", "application/x-httpd-php"));
	mtypes.insert(std::make_pair("php3", "application/x-httpd-php3"));
	mtypes.insert(std::make_pair("php3p", "application/x-httpd-php3-preprocessed"));
	mtypes.insert(std::make_pair("php4", "application/x-httpd-php4"));
	mtypes.insert(std::make_pair("php5", "application/x-httpd-php5"));
	mtypes.insert(std::make_pair("phps", "application/x-httpd-php-source"));
	mtypes.insert(std::make_pair("pht", "application/x-httpd-php"));
	mtypes.insert(std::make_pair("phtml", "application/x-httpd-php"));
	mtypes.insert(std::make_pair("pk", "application/x-tex-pk"));
	mtypes.insert(std::make_pair("pl", "text/x-perl"));
	mtypes.insert(std::make_pair("pls", "audio/x-scpls"));
	mtypes.insert(std::make_pair("pm", "text/x-perl"));
	mtypes.insert(std::make_pair("png", "image/png"));
	mtypes.insert(std::make_pair("pnm", "image/x-portable-anymap"));
	mtypes.insert(std::make_pair("pot", "text/plain"));
	mtypes.insert(std::make_pair("potm", "application/vnd.ms-powerpoint.template.macroenabled.12"));
	mtypes.insert(std::make_pair("potx", "application/vnd.openxmlformats-officedocument.presentationml.template"));
	mtypes.insert(std::make_pair("ppam", "application/vnd.ms-powerpoint.addin.macroenabled.12"));
	mtypes.insert(std::make_pair("ppm", "image/x-portable-pixmap"));
	mtypes.insert(std::make_pair("pps", "application/vnd.ms-powerpoint"));
	mtypes.insert(std::make_pair("ppsm", "application/vnd.ms-powerpoint.slideshow.macroenabled.12"));
	mtypes.insert(std::make_pair("ppsx", "application/vnd.openxmlformats-officedocument.presentationml.slideshow"));
	mtypes.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
	mtypes.insert(std::make_pair("pptm", "application/vnd.ms-powerpoint.presentation.macroenabled.12"));
	mtypes.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	mtypes.insert(std::make_pair("prf", "application/pics-rules"));
	mtypes.insert(std::make_pair("prt", "chemical/x-ncbi-asn1-ascii"));
	mtypes.insert(std::make_pair("ps", "application/postscript"));
	mtypes.insert(std::make_pair("psd", "image/x-photoshop"));
	mtypes.insert(std::make_pair("py", "text/x-python"));
	mtypes.insert(std::make_pair("pyc", "application/x-python-code"));
	mtypes.insert(std::make_pair("pyo", "application/x-python-code"));
	mtypes.insert(std::make_pair("qgs", "application/x-qgis"));
	mtypes.insert(std::make_pair("qt", "video/quicktime"));
	mtypes.insert(std::make_pair("qtl", "application/x-quicktimeplayer"));
	mtypes.insert(std::make_pair("ra", "audio/x-pn-realaudio"));
	mtypes.insert(std::make_pair("ram", "audio/x-pn-realaudio"));
	mtypes.insert(std::make_pair("rar", "application/rar"));
	mtypes.insert(std::make_pair("ras", "image/x-cmu-raster"));
	mtypes.insert(std::make_pair("rb", "application/x-ruby"));
	mtypes.insert(std::make_pair("rd", "chemical/x-mdl-rdfile"));
	mtypes.insert(std::make_pair("rdf", "application/rdf+xml"));
	mtypes.insert(std::make_pair("rdp", "application/x-rdp"));
	mtypes.insert(std::make_pair("rgb", "image/x-rgb"));
	mtypes.insert(std::make_pair("rhtml", "application/x-httpd-eruby"));
	mtypes.insert(std::make_pair("rm", "audio/x-pn-realaudio"));
	mtypes.insert(std::make_pair("roff", "application/x-troff"));
	mtypes.insert(std::make_pair("ros", "chemical/x-rosdal"));
	mtypes.insert(std::make_pair("rpm", "application/x-redhat-package-manager"));
	mtypes.insert(std::make_pair("rss", "application/rss+xml"));
	mtypes.insert(std::make_pair("rtf", "application/rtf"));
	mtypes.insert(std::make_pair("rtx", "text/richtext"));
	mtypes.insert(std::make_pair("rxn", "chemical/x-mdl-rxnfile"));
	mtypes.insert(std::make_pair("scala", "text/x-scala"));
	mtypes.insert(std::make_pair("sce", "application/x-scilab"));
	mtypes.insert(std::make_pair("sci", "application/x-scilab"));
	mtypes.insert(std::make_pair("sco", "audio/csound"));
	mtypes.insert(std::make_pair("scr", "application/x-silverlight"));
	mtypes.insert(std::make_pair("sct", "text/scriptlet"));
	mtypes.insert(std::make_pair("sd", "chemical/x-mdl-sdfile"));
	mtypes.insert(std::make_pair("sd2", "audio/x-sd2"));
	mtypes.insert(std::make_pair("sda", "application/vnd.stardivision.draw"));
	mtypes.insert(std::make_pair("sdc", "application/vnd.stardivision.calc"));
	mtypes.insert(std::make_pair("sdd", "application/vnd.stardivision.impress"));
	mtypes.insert(std::make_pair("sdf", "application/vnd.stardivision.math"));
	mtypes.insert(std::make_pair("sds", "application/vnd.stardivision.chart"));
	mtypes.insert(std::make_pair("sdw", "application/vnd.stardivision.writer"));
	mtypes.insert(std::make_pair("ser", "application/java-serialized-object"));
	mtypes.insert(std::make_pair("sfv", "text/x-sfv"));
	mtypes.insert(std::make_pair("sgf", "application/x-go-sgf"));
	mtypes.insert(std::make_pair("sgl", "application/vnd.stardivision.writer-global"));
	mtypes.insert(std::make_pair("sh", "application/x-sh"));
	mtypes.insert(std::make_pair("shar", "application/x-shar"));
	mtypes.insert(std::make_pair("shp", "application/x-qgis"));
	mtypes.insert(std::make_pair("shtml", "text/html"));
	mtypes.insert(std::make_pair("shx", "application/x-qgis"));
	mtypes.insert(std::make_pair("sid", "audio/prs.sid"));
	mtypes.insert(std::make_pair("sik", "application/x-trash"));
	mtypes.insert(std::make_pair("silo", "model/mesh"));
	mtypes.insert(std::make_pair("sis", "application/vnd.symbian.install"));
	mtypes.insert(std::make_pair("sisx", "x-epoc/x-sisx-app"));
	mtypes.insert(std::make_pair("sit", "application/x-stuffit"));
	mtypes.insert(std::make_pair("sitx", "application/x-stuffit"));
	mtypes.insert(std::make_pair("skd", "application/x-koan"));
	mtypes.insert(std::make_pair("skm", "application/x-koan"));
	mtypes.insert(std::make_pair("skp", "application/x-koan"));
	mtypes.insert(std::make_pair("skt", "application/x-koan"));
	mtypes.insert(std::make_pair("sldm", "application/vnd.ms-powerpoint.slide.macroenabled.12"));
	mtypes.insert(std::make_pair("sldx", "application/vnd.openxmlformats-officedocument.presentationml.slide"));
	mtypes.insert(std::make_pair("smi", "application/smil"));
	mtypes.insert(std::make_pair("smil", "application/smil"));
	mtypes.insert(std::make_pair("snd", "audio/basic"));
	mtypes.insert(std::make_pair("spc", "chemical/x-galactic-spc"));
	mtypes.insert(std::make_pair("spl", "application/futuresplash"));
	mtypes.insert(std::make_pair("spx", "audio/ogg"));
	mtypes.insert(std::make_pair("sql", "application/x-sql"));
	mtypes.insert(std::make_pair("src", "application/x-wais-source"));
	mtypes.insert(std::make_pair("stc", "application/vnd.sun.xml.calc.template"));
	mtypes.insert(std::make_pair("std", "application/vnd.sun.xml.draw.template"));
	mtypes.insert(std::make_pair("sti", "application/vnd.sun.xml.impress.template"));
	mtypes.insert(std::make_pair("stl", "application/sla"));
	mtypes.insert(std::make_pair("stw", "application/vnd.sun.xml.writer.template"));
	mtypes.insert(std::make_pair("sty", "text/x-tex"));
	mtypes.insert(std::make_pair("sv4cpio", "application/x-sv4cpio"));
	mtypes.insert(std::make_pair("sv4crc", "application/x-sv4crc"));
	mtypes.insert(std::make_pair("svg", "image/svg+xml"));
	mtypes.insert(std::make_pair("svgz", "image/svg+xml"));
	mtypes.insert(std::make_pair("sw", "chemical/x-swissprot"));
	mtypes.insert(std::make_pair("swf", "application/x-shockwave-flash"));
	mtypes.insert(std::make_pair("swfl", "application/x-shockwave-flash"));
	mtypes.insert(std::make_pair("sxc", "application/vnd.sun.xml.calc"));
	mtypes.insert(std::make_pair("sxd", "application/vnd.sun.xml.draw"));
	mtypes.insert(std::make_pair("sxg", "application/vnd.sun.xml.writer.global"));
	mtypes.insert(std::make_pair("sxi", "application/vnd.sun.xml.impress"));
	mtypes.insert(std::make_pair("sxm", "application/vnd.sun.xml.math"));
	mtypes.insert(std::make_pair("sxw", "application/vnd.sun.xml.writer"));
	mtypes.insert(std::make_pair("t", "application/x-troff"));
	mtypes.insert(std::make_pair("tar", "application/x-tar"));
	mtypes.insert(std::make_pair("taz", "application/x-gtar-compressed"));
	mtypes.insert(std::make_pair("tcl", "application/x-tcl"));
	mtypes.insert(std::make_pair("tex", "text/x-tex"));
	mtypes.insert(std::make_pair("texi", "application/x-texinfo"));
	mtypes.insert(std::make_pair("texinfo", "application/x-texinfo"));
	mtypes.insert(std::make_pair("text", "text/plain"));
	mtypes.insert(std::make_pair("tgf", "chemical/x-mdl-tgf"));
	mtypes.insert(std::make_pair("tgz", "application/x-gtar-compressed"));
	mtypes.insert(std::make_pair("thmx", "application/vnd.ms-officetheme"));
	mtypes.insert(std::make_pair("tif", "image/tiff"));
	mtypes.insert(std::make_pair("tiff", "image/tiff"));
	mtypes.insert(std::make_pair("tk", "text/x-tcl"));
	mtypes.insert(std::make_pair("tm", "text/texmacs"));
	mtypes.insert(std::make_pair("torrent", "application/x-bittorrent"));
	mtypes.insert(std::make_pair("tr", "application/x-troff"));
	mtypes.insert(std::make_pair("ts", "video/mp2t"));
	mtypes.insert(std::make_pair("tsp", "application/dsptype"));
	mtypes.insert(std::make_pair("tsv", "text/tab-separated-values"));
	mtypes.insert(std::make_pair("txt", "text/plain"));
	mtypes.insert(std::make_pair("udeb", "application/x-debian-package"));
	mtypes.insert(std::make_pair("uls", "text/iuls"));
	mtypes.insert(std::make_pair("ustar", "application/x-ustar"));
	mtypes.insert(std::make_pair("val", "chemical/x-ncbi-asn1-binary"));
	mtypes.insert(std::make_pair("vcd", "application/x-cdlink"));
	mtypes.insert(std::make_pair("vcf", "text/x-vcard"));
	mtypes.insert(std::make_pair("vcs", "text/x-vcalendar"));
	mtypes.insert(std::make_pair("vmd", "chemical/x-vmd"));
	mtypes.insert(std::make_pair("vms", "chemical/x-vamas-iso14976"));
	mtypes.insert(std::make_pair("vrm", "x-world/x-vrml"));
	mtypes.insert(std::make_pair("vrml", "model/vrml"));
	mtypes.insert(std::make_pair("vsd", "application/vnd.visio"));
	mtypes.insert(std::make_pair("wad", "application/x-doom"));
	mtypes.insert(std::make_pair("wav", "audio/x-wav"));
	mtypes.insert(std::make_pair("wax", "audio/x-ms-wax"));
	mtypes.insert(std::make_pair("wbmp", "image/vnd.wap.wbmp"));
	mtypes.insert(std::make_pair("wbxml", "application/vnd.wap.wbxml"));
	mtypes.insert(std::make_pair("webm", "video/webm"));
	mtypes.insert(std::make_pair("wk", "application/x-123"));
	mtypes.insert(std::make_pair("wm", "video/x-ms-wm"));
	mtypes.insert(std::make_pair("wma", "audio/x-ms-wma"));
	mtypes.insert(std::make_pair("wmd", "application/x-ms-wmd"));
	mtypes.insert(std::make_pair("wml", "text/vnd.wap.wml"));
	mtypes.insert(std::make_pair("wmlc", "application/vnd.wap.wmlc"));
	mtypes.insert(std::make_pair("wmls", "text/vnd.wap.wmlscript"));
	mtypes.insert(std::make_pair("wmlsc", "application/vnd.wap.wmlscriptc"));
	mtypes.insert(std::make_pair("wmv", "video/x-ms-wmv"));
	mtypes.insert(std::make_pair("wmx", "video/x-ms-wmx"));
	mtypes.insert(std::make_pair("wmz", "application/x-ms-wmz"));
	mtypes.insert(std::make_pair("wp5", "application/vnd.wordperfect5.1"));
	mtypes.insert(std::make_pair("wpd", "application/vnd.wordperfect"));
	mtypes.insert(std::make_pair("wrl", "model/vrml"));
	mtypes.insert(std::make_pair("wsc", "text/scriptlet"));
	mtypes.insert(std::make_pair("wvx", "video/x-ms-wvx"));
	mtypes.insert(std::make_pair("wz", "application/x-wingz"));
	mtypes.insert(std::make_pair("x3d", "model/x3d+xml"));
	mtypes.insert(std::make_pair("x3db", "model/x3d+binary"));
	mtypes.insert(std::make_pair("x3dv", "model/x3d+vrml"));
	mtypes.insert(std::make_pair("xbm", "image/x-xbitmap"));
	mtypes.insert(std::make_pair("xcf", "application/x-xcf"));
	mtypes.insert(std::make_pair("xht", "application/xhtml+xml"));
	mtypes.insert(std::make_pair("xhtml", "application/xhtml+xml"));
	mtypes.insert(std::make_pair("xlam", "application/vnd.ms-excel.addin.macroenabled.12"));
	mtypes.insert(std::make_pair("xlb", "application/vnd.ms-excel"));
	mtypes.insert(std::make_pair("xls", "application/vnd.ms-excel"));
	mtypes.insert(std::make_pair("xlsb", "application/vnd.ms-excel.sheet.binary.macroenabled.12"));
	mtypes.insert(std::make_pair("xlsm", "application/vnd.ms-excel.sheet.macroenabled.12"));
	mtypes.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	mtypes.insert(std::make_pair("xlt", "application/vnd.ms-excel"));
	mtypes.insert(std::make_pair("xltm", "application/vnd.ms-excel.template.macroenabled.12"));
	mtypes.insert(std::make_pair("xltx", "application/vnd.openxmlformats-officedocument.spreadsheetml.template"));
	mtypes.insert(std::make_pair("xml", "application/xml"));
	mtypes.insert(std::make_pair("xpi", "application/x-xpinstall"));
	mtypes.insert(std::make_pair("xpm", "image/x-xpixmap"));
	mtypes.insert(std::make_pair("xsd", "application/xml"));
	mtypes.insert(std::make_pair("xsl", "application/xml"));
	mtypes.insert(std::make_pair("xspf", "application/xspf+xml"));
	mtypes.insert(std::make_pair("xtel", "chemical/x-xtel"));
	mtypes.insert(std::make_pair("xul", "application/vnd.mozilla.xul+xml"));
	mtypes.insert(std::make_pair("xwd", "image/x-xwindowdump"));
	mtypes.insert(std::make_pair("xyz", "chemical/x-xyz"));
	mtypes.insert(std::make_pair("zip", "application/zip"));
	mtypes.insert(std::make_pair("zmt", "chemical/x-mopac-input"));
	mtypes.insert(std::make_pair("~", "application/x-trash"));
}

void mimetypes::readfromfile(const std::string& configpath)
{
	mtypes.clear();
	boost::filesystem::path path(configpath);
	path.remove_filename();
	std::string filename = path.string() + "/" + MIME_TYPES_FILENAME;

	std::ifstream infile(filename.c_str(), std::ios_base::in);
	if (infile)
	{
		std::string line;
		while (getline(infile, line, '\n'))
		{
			if ((line.empty()) || (line.at(0) == '#'))
				continue;

			std::transform(line.begin(), line.end(), line.begin(), ::tolower);

			std::vector<std::string> tokens;
			stringtools::split(tokens, line, '\t', split::no_empties);
			if (tokens.size() == 2)
			{
				std::vector<std::string> exts;
				stringtools::split(exts, tokens[1], ' ', split::no_empties);
				for (auto iter = exts.begin(); iter != exts.end(); ++iter)
				{
					mtypes.insert(std::make_pair(*iter, tokens[0]));
				}
			}
		}
		infile.close();
	}
	else
	{
		std::ostringstream oss;
		oss << "Unable to open file with mime-types \"" << filename << "\"";
		std::string what = oss.str();

		throw std::exception(what.c_str());
	}
/*
	filename += ".cpp";
	std::ofstream ofile(filename.c_str());
	if (ofile)
	{
		for (auto iter = mtypes.begin(); iter != mtypes.end(); ++iter)
		{
			ofile << "      mtypes.insert(std::make_pair(\"" << iter->first << "\", \"" << iter->second << "\"));" << std::endl;
		}
		ofile.close();
	}
*/
}

const char* mimetypes::get_by_extension(const std::string& extension) const
{
	mimetypes_map::const_iterator iter = mtypes.find(extension);

	if (iter == mtypes.end())
		return NULL;

	return iter->second.c_str();
}

const char* mimetypes::get_by_path(const std::string& path) const
{
	size_t pos_ext = path.rfind(".");

	if (pos_ext != std::string::npos)
		return get_by_extension(path.substr(pos_ext + 1));

	return get_by_extension(path);
}

}