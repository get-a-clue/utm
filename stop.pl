#!perl -w

my $timefile = "track.time";

my $n = time;

open(F, "$timefile") or die "Error opening $timefile for reading";
$prevtime = <F>;
close(F);

my $diff = $n - $prevtime;
print "Elapsed time: $diff\n";


