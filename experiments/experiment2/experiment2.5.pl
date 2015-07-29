#! /usr/bin/perl
use strict;
use warnings;
use POSIX;
use Time::HiRes qw(time);
use Text::Levenshtein qw(distance);

my $exePath = "./../../";
my $exe = "csm";

my @percentages = (10, 20, 30, 40);

my @patternLengths = (16, 32, 64, 128, 256, 512); #

for my $pc (@percentages)
{
    #read seqs into x and y
    open(FH, "<", "./data_ed_${pc}pc.fasta") or die($!);
    my @lines = <FH>;
    close(FH);
    my $x = $lines[1]; chop($x);
    my $y = $lines[3]; chop($y);
    my $m = length($y);
    my $distanceXY = distance($x, $y);

    #randomly rotate y into r
    my $min = 10;
    my $max = $m - $min;
    my $randRotPos = $min + int(rand($max - $min));
    my $r = substr($y, $randRotPos) . substr($y, 0, $randRotPos);
    my $distanceXR = distance($x, $r);

    for my $pLen (@patternLengths)
    {
        my $k = 1 + ceil($pc/100 * $pLen);
        my $p = substr($r, 0, $pLen);
        my $cmd = "$exePath$exe -t $x -p $p -k $k";
        my $start = time();
        my $cmdOutput = `$cmd`;
        my $finish = (time() - $start) * 1000;
        my @cmdLines = split(/\r?\n/, $cmdOutput);
        my $lastLine = $cmdLines[$#cmdLines];

        #print "PC:$pc, PL:$pLen, LL:$lastLine\n";
        #print "X:$x\nR:$r\nP:$p\n";

        if ($lastLine =~ m/^(\d+)\s+(\d+)\s+(\d+)/)
        {
            my ($ii, $jj, $dd) = ($1, $2, $3);
            my $pos = $ii + 1 - $pLen;
            my $z = substr($x, $pos) . substr($x, 0, $pos); #rotate x to match r
            my $distanceRZ = distance($r, $z);

            print "PC:$pc\nPL:$pLen\nLL:$lastLine\nX:$x\nR:$r\nZ:$z\ndistanceXY:$distanceXY\ndistanceXR:$distanceXR\ndistanceRZ:$distanceRZ\n";
        } else {
	        print "Not found!!!\nX:$x\nP:$p\n";
        }
        last;
    }
    last;
}

1;

