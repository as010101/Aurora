#!/usr/local/bin/perl -w
#
# Removes the Windows-generated ^M characters from the source files
# in all of the directories recursively.  
#
# Written by Ionut Aron (ia@cs.brown.edu)
#

use strict;

sub read_dir {
    my $dir = shift;
    
    if( opendir DIR, $dir ) {	
	my @files = grep { /^[^\.]/ } readdir(DIR);
	closedir DIR;
	
	map { 
	    if( -f "$dir/$_" ) {
		open F, "$dir/$_" or 
		    die "Cannot read file $dir/$_\n";
		my @data = <F>;
		close F;
		
		my $found = 0;
		foreach my $i ( 0..$#data ) {
		    if( $data[$i] =~ /$/ ) {
			$found = 1;
			$data[$i] =~ s/([^]*)/$1/;
		    }
		}
		if( $found ) {
		    print "Need to update file $dir/$_\n";
		    open F, ">$dir/$_" or die "Cannot write into $dir/$_\n";
		    print F @data;
		    close F;
		}		
	    } elsif ( -d "$dir/$_" ) {
		&read_dir("$dir/$_");
	    }
	} @files;
	
    } else {
	die "Can't read current directory $dir.\n";
    }
}

&read_dir(shift);













