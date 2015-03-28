#!perl -w

my $utm_header = "utm\\utm.h";
my $utm_header_bak = "utm\\utm.h.bak";

open FILE_IN, "<", $utm_header or die $!;
open FILE_OUT, ">", $utm_header_bak or die $!;
while (<FILE_IN>) {
  if ($_ =~ m/#define\sUTM_VERSION\s\"(\d{1,2})\.(\d{1,2})\.(\d{1,5})\"/) {
     my $build = $3 + 1;
     print FILE_OUT "#define UTM_VERSION \"$1.$2.$build\"\n";
  } else {
     print FILE_OUT $_;
  }
}
close(FILE_IN);
close(FILE_OUT);

rename $utm_header_bak, $utm_header;
