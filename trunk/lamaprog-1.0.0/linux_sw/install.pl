#!/usr/bin/perl -w
#LamaProg instalation program v 0.1
#
#Jan Parkman   (parkmaj@users.sourceforge.net)

$bindir = "/usr/bin";
$localedir = "/usr/share/locale/cs_CZ/LC_MESSAGES";
$device = "/dev/ttyS0";
 
print "LamaProg instalation program\n";


print "Select your bin dir [$bindir]";
$radek = <>;
chomp($radek);
if($radek ne "")
{
	$bindir=$radek
}
`cp prog $bindir`;

print "\nWould you install localization? [n]";
$radek = <>;
chomp($radek);
if($radek eq "y") 
{
	print "\nSelect your localization dir [$localedir]";
	$radek = <>;
	chomp($radek);
	if($radek ne "")
	{
		$localedir=$radek
	}
	`cp prog.mo $localedir`;
}

print "\nSelect communication device [$device]";
$radek = <>;
chomp($radek);
if($radek ne "")
{
	$device=$radek
}
`chmod 666 $device`;



print "\nEnd...\n\n";

