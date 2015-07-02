#! /usr/bin/perl
use strict;
use warnings;
use Time::HiRes qw(time);

my $exePath = "./../../";
my $exe = "csm";

#2000 chars
my $text = <<'SEQ';
tcttttcttatcgttgacatttaaactctggggcaggtcctcgcgtagaa
cccggttgtcagatctgctacttccccccgagaagcggctttgagaagtg
tgggaaccagcgccaccagactcacctgacaccccaccctcggctcacag
gtaaccgctttgattctagccagactcaccccaagagtagcggcgcccct
ctccgacgtgcaacctgtgtgttcaggtcatagaaaccctccccgagtgg
ccccggccttgatttttttctttttctttttaaaggaagcttacaaagat
ggtagaggagttgctgcggatgggacgggggtgagggggaggggatgtct
ttgcgaagcatgcttctttgtaaaagtaacaaaatgttgtggtctcaaga
gaggacttcccgtctggccctgagctgggggtgatgtggggggtgggagg
gtggtggctgtggccgcagtctaaagaaatcgctgagatcctaagaaaag
caggcggtgggcatgcagtttgcaacacgccttcccccgatgcaattagc
gacattcatgcttggagaagccaccaagcctcgcgccgaattcatggagc
accccgcggaaggtcgggcgacttgcgacaaaagtttccccaaggttgtg
tagatgtgtgttccatatcgaagtcaaagtcttggggttcgtttcccccg
aaaggagcacaggacgaaagaatgcggccctccagagctgggcagcgcgc
gcgaaggccccagcgtgtgatttgagcttccttcggaagacctaataatt
agcgattctcactgagctagaatgcgggctcgggttaccgggaacgggtt
cctagcagcggcggagctctttgcctcggcgggaagcatgtgggctccag
gggaacccggttgctgagtgccctggagagagatacccagccatgctccg
gctccaggaaggccagaacttggccgaagtgggtcaggaaagtgattacc
cctaacttaaactaaaaggtgttttcgccaggttggaaaatctctcactc
aagccctaagcttaccaggagccgctcaggctgcctgagtcggggcaccg
gagcaacccaccggaaatttgaggccatgtagcaggcctggcctggatct
cgcccactcccacgcgtgctcctgacgccctctctctagacccgctggag
gcacttgtccagatcgtttggctctcaagcacctgtgacaggtgcccagg
agtctaactctgagcagagtcccctctgagcgcctcacatagaaaggagg
ccacgggcaaagtggccaccagtctgagcaccttacctgaacgggggtcc
tctcctccgagagttccttattccagagttgagccgtgagcagtttgccc
gcttcgcagtaggaagtggaccttctcctctagtcataaatcaaaccagc
catcctcgggcctcctccctcattagagatgtttattggagattgtgttt
attcggctgtcacggcgagaaaacgcggtgacataattacctctgaccag
agtcctcgccctgcgcccagggtgagccagggacttcctctgctgtggtc
ttactctgctgccctccttgatcagagggcaaatcggattggtgtgtcta
ctttttcctccctcagacttcgtatcatttgaattattggagaacttagt
attcaaatttccgccagtgcccagacttggaaaagaaaaccaaagagaat
ccagcttccagggagagagggagagagaagtaagggagagagagagggaa
agggagagagagagagagagagagagagagagagagagagagagagagag
agtaaatcttgcatgagaactttctagtgcactggcagagatgcttggcc
agacctagaaggacgagtgcctgtatttgtggtattattttttcttttgc
ttccctttttcattccttttcttttcgggaggaatctggctgcagagcct
SEQ

$text =~ s/\r?\n//g;
my $n = length($text);

my $pattern = "aaaaaaaaaa";
my $m = length($pattern); #10
my $mStart = $m;

my $k = 1;

my $repeats = 8;

my $i = 0;

open(FH, ">", "runningTimes.csv") or die($!);

#make title row
print FH "type,";
while ($i < $repeats) {
    if ($i != 0) {
	$m *= 2;
    }
    print FH "$m,";
    $i++;
}
print FH "\n";

################
# Experiment 1 #
#  No matches  #
################

$i = 0;
print FH "no-match,";
while ($i < $repeats) {
    if ($i != 0) {
	$pattern .= $pattern;
	$m *= 2;
    }
    my $start = time();
    system("$exePath$exe -t $text -p $pattern -k $k");
    my $finish = time() - $start;
    print FH "$finish,";
    $i++;
}
print FH "\n";

################
# Experiment 2 #
# Exact match  #
################

$m = $mStart;
$pattern = substr($text, -$m);
$i = 0;
print FH "match,";
while ($i < $repeats) {
    if ($i != 0) {
	$m *= 2;
	$pattern = substr($text, -$m);
    }

    my $start = time();
    system("$exePath$exe -t $text -p $pattern -k $k");
    my $finish = time() - $start;
    print FH "$finish,";

    $i++;
}

close(FH);

1;
