#! /usr/bin/perl
use strict;
use warnings;

open(FI, "<", "hs_alt_CHM1_1.1_chr22.fa") or die($!);
open(FO, ">", "chr22.txt") or die($!);
my $i = 0;
while(<FI>){
    chomp;
    s/N//g; 
    if ($i != 0){
        print FO $_;
    }
    $i++;
}
close(FO);
close(FI);

1;

