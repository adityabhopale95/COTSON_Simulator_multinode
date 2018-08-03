#!/usr/bin/perl 
use strict;
my %DIR;
my $r="test.unit";
## collect sources
foreach my $fp (<*/*.cpp>) {
    my ($d,$f) = split('/',$fp);
	$DIR{$f}=$d;
}
## search for lines like '// #include "cache_timing_l2.cpp"'
foreach my $f (<$r/*.cpp>) {
    open FH,"<$f";
	my $o = $f; $o =~ s/\.cpp/.o/;
	my $x = $f; $x =~ s/\.cpp//;
	print "$x: $o";
	while(<FH>) {
        chomp; 
	    if (/\#include[\s]+\"(.*)\.cpp\"/) {
			my $s = "$1.cpp";
			my $o = "$1.o";
			print " $DIR{$s}/$o" if $DIR{$s};
		}
	}
    print "\n";
	close FH;
}

