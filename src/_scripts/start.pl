#!perl -w

my $timefile = "track.time";

my $n = time;

open(F, ">$timefile") or die "Error opening $timefile for writing";
print F $n;
close(F);

