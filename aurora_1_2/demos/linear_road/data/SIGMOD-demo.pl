#!/usr/bin/perl -w

use strict;

# Output format is:
# type, time, carid, speed, expressway, lane, direction, position.
# 5 segments, eastbound only, 1000 cars.
# 0, [0..], [1..1000], [0..100], 0, [0-3], 0, [0..(5280*5))]

# Create an array of arrayrefs, sort it later and print it.
my @datapoints;

foreach my $time (1..990) {
  # 50 regular cars driving 60 mph from end to end.
  my $carid;
  foreach $carid (1..50) {
    # Car that has id mod 30 = time mod 30 outputs
    if ($carid % 30 == $time % 30) {
      my $position = int(($time%330)*60*5280/3600);
      my $lane;
      if ($position < 2640 || $position > (5280*5)-2640) {
	$lane = 0;
      } else {
	$lane = int(rand(3)+1);
      }
      my @data = (0, $time, $carid, 60, 0, $lane, 0, $position);
      print STDERR "Output for carid $carid at time $time: ",
	join(",", @data), "\n";
      push @datapoints, \@data;
    }
  }

  # 300 cars going 10 miles per hour on segment 3 and 4.
  foreach $carid (101..400) {
    if ($carid % 30 == $time % 30) {
      my $position = int((2*5280) + (($time%330) * 10 * 5280 / 3600));
      my $lane;
      if ($position < (2.5*5280) || $position > (3.5*5280)) {
	$lane = 0;
      } else {
	$lane = int(rand(3)+1);
      }
      my @data = (0, $time, $carid, 10, 0, $lane, 0, $position);
      print STDERR "Output for carid $carid at time $time: ",
	join(",", @data), "\n";
      push @datapoints, \@data;
    }
  }
}

# Output.
foreach my $a (sort {$a->[1] <=> $b->[1]} @datapoints) {
  print join(",",@$a), "\n";
}
