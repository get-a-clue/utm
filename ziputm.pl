#!perl -w

my $utm_header = "include\\utm.h";
my $utm_version = "-";

open FILE_IN, "<", $utm_header or die $!;
while (<FILE_IN>) {
  if ($_ =~ m/#define\sUTM_VERSION\s\"(\d{1,2}\.\d{1,2}\.\d{1,5})\"/) {
     $utm_version = $1;
  }
}
close(FILE_IN);

my $cmd = "zip -r artefacts/utm-$utm_version.zip include/ lib/";
open(PS, "$cmd |") || die "Failed: $!\n";
while ( <PS> )
{
   print $_;
}
close(PS);