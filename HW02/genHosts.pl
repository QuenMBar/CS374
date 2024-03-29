#!/usr/bin/perl 

# Last updated 2018-09-28 cwieri39 for Maroon/Gold labs

use strict;
use Net::Ping;
use List::Util 'shuffle';

my @hosts = qw/gold00
gold01
gold02
gold03
gold04
gold05
gold06
gold07
gold08
gold09
gold10
gold11
gold12
gold13
gold14
gold15
gold16
gold17
gold18
gold19
gold20
gold21
gold22
gold23
gold24
gold25
gold26
gold27
gold28
gold29
gold30
gold31
gold32
gold33
gold34
gold35
maroon00
maroon01
maroon02
maroon03
maroon04
maroon05
maroon06
maroon07
maroon08
maroon09
maroon10
maroon11
maroon12
maroon13
maroon14
maroon15
maroon16
maroon17
maroon18
maroon19
maroon20
maroon21
maroon22
maroon23
maroon24
maroon25
maroon26
maroon27
maroon28
maroon29
maroon30
maroon31
maroon32
maroon33
maroon34
maroon35
syslab01
syslab02
syslab03
syslab04
syslab05
syslab06
syslab07
syslab08
syslab09
syslab10
syslab11
syslab12
syslab13
syslab14
syslab15
syslab16
syslab17
syslab18
syslab19
syslab20
syslab21
syslab22
syslab23
syslab24/;

my $p = Net::Ping->new("tcp",5);
$p->{port_num} = getservbyname("ssh", "tcp");
$p->service_check(1);
my $numchildren;

pipe(FIN,FOUT);

foreach my $host (@hosts) 
{
    $numchildren++;
    if ($numchildren > 100) {
            while(wait() != -1 && $numchildren > 50) { $numchildren--; }
    }
    unless(fork())
    {
        close(FIN);
        if ($p->ping($host))
        {
            print FOUT "$host\n";
        }
        exit 0;
    }
}

while (wait() != -1) { $numchildren--; }

close(FOUT);
my @linux_hosts;
while (<FIN>)
{
        push(@linux_hosts,$_);
}
close(FIN);

my @shuffled = shuffle(@linux_hosts);

foreach my $host(@shuffled) { chomp $host; print $host . "\n"; }
