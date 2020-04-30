#!/usr/bin/perl -w

use strict;

if (@ARGV != 2) {
    print "Usage: multdata.pl <multiplier> <caridoffset>\n";
    exit(1);
}

my $multiplier = $ARGV[0];
my $caridoffset = $ARGV[1];

while (<STDIN>) {
  my @f = split ',', $_;
  for (my $ii = 0; $ii < $multiplier; $ii++) {
    # increase the carid, which is this third field.
    $f[2] += $ii * $caridoffset;
    # set thee highway,  which is the fifth field.
    if ($f[4] == 0) {
	$f[4] = $ii;
    }
    print join(',', @f);
  }
}
