#!/usr/bin/perl

#
# Circular String Matching
# Copyright (C) 2015 Xenofon Kyriakidis, Solon Pissis and Ahmad Retha
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

use strict;
use warnings;
use POSIX;
use Time::HiRes qw(time);
use Text::Levenshtein qw(distance);

#application
my $exePath = "./../../";
my $exe = "csm";

#percent dissimilarity between read and target sequence
my @diss = (10); #, 20, 30, 40);

#pattern lengths
my @plens = (16); #, 32, 64, 128, 256, 512);

#initial mismatch limit
my $k = 1;

#sequences to be matched and the rotated pSeq
my $tSeq = "";
my $pSeq = "";
my $rSeq = "";

for my $pc (@diss) {

    #open file and read lines of file into t and p seq
    my $inFile = "./data_ed_${pc}pc.fasta";
    open (IF, "<", $inFile) or die("Could not open file: $inFile");
    my $line = 1;
    while (<IF>) {
        chomp;
        if ($line == 2) {
            $tSeq = $_;
        } elsif ($line == 4) {
            $pSeq = $_;
        }
        $line++;
    }
    close(IF);

    #Run tSeq and pSeq through Levenshtein to get original Edit Distance score
    my $originalEditDistance = distance($tSeq, $pSeq);
    
    #randomly rotate $pSeq
    my $tSeqLen = length($tSeq);
    my $pSeqLen = length($pSeq);
    my $min = 10;
    my $max = $pSeqLen - $min;
    my $randRotPos = $min + int(rand($max - $min));
    $rSeq = substr($pSeq, $randRotPos) . substr($pSeq, 0, $randRotPos);

    #holds a hash with pattern lengths for keys and the edit distance scores as values
    my %editDistanceScores = ();

    #open file to save output to
    my $outFile = "./output_${pc}.txt";
    open(OF, ">", $outFile) or die("Could not open file: $outFile");

    #try out the different pattern lengths to find a match
    for my $m (@plens) {
        #calculate how many errors to allow based on pattern length and percentage mutation
        $k = ceil($m / $pc);

        #only read m chars to create the pattern to search with
        my $pattern = substr($rSeq, 0, $m);

        #run command and time it
        my $cmd = "$exePath$exe -t $tSeq -p $pattern -k $k";
        my $start = time();
        my $cmdOutput = `$cmd`;
        chop($cmdOutput);
        my $finish = (time() - $start) * 1000;

        #read string from output into vars
        my @lines = split(/\r?\n/, $cmdOutput);
        my $lastLine = $lines[$#lines];
        my $rotatedBackRSeq = "";
        if ($lastLine !~ m/^position i/)
        {
            $lastLine =~ m/^(\d+)\s+(\d+)\s+(\d+)/;
            my $ii = $1;
            my $jj = $2;
            my $dd = $3;

            #rotate $rSeq sequence back to resemble original pSeq (if csm did it correctly)            
            my $pos = 0;
            if ($ii >= $jj) {
                $pos = $ii - $jj;
            } else {
                my $a = $jj - $ii;
                my $b = $tSeqLen - $ii - 1;
                my $c = ( $a <= $b ) ? $a : $b;
                $pos = $pSeqLen - $c;
            }
            $rotatedBackRSeq = substr(substr($rSeq, $pos) . substr($rSeq, 0, $pos), 0, $pSeqLen);
        }
        else
        {
            #match not found!!!!
            $rotatedBackRSeq = "<<<Match not found!>>>";
        }

        #Run tSeq and rSeq through Levenshtein to get original Edit Distance score
        $editDistanceScores{$m} = distance($tSeq, $rotatedBackRSeq);

        print OF "Dissimilarity: $pc, k: $k, Pattern Length: $m, Pattern = $pattern\n";
        print OF "T: $tSeq\nP: $pSeq\nR: $rSeq\nB: $rotatedBackRSeq\n";
        print OF "Last line: $lastLine\n";
        print OF "Edit Distance between tSeq and rSeq: $editDistanceScores{$m}\n";
        print OF "Completed search in $finish ms\n\n";
    }
    
    for my $key (keys(%editDistanceScores)) {
        my $score = $originalEditDistance - $editDistanceScores{$key};
        print OF "Pattern length $key: $originalEditDistance - $editDistanceScores{$key} = $score\n";
    }

    close(OF);
}

1;
