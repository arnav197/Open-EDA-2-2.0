#!/usr/bin/perl

print "Welcome to the converter script!\n";
print "\n";
print "This converts a given list of combinational '.bench' files into\n";
print "psuedo-combinational files.\n";
print "\n";
print "All files must be named '.bench'.\n";
print "\n";
print "The new files will be named '.pseudo.bench'.\n";
print "\n";

foreach my $file (@ARGV) {
	open(FH_read, '<', $file) or die "OOPS! I couldn't open the file '$file'!: $!\n\n";
	$file_write = $file;
	if(!($file_write =~ s/\.bench$/\.pseudo\.bench/)) {die "Problem: $file is not a '.bench' file (at least in name).\n"; }
	open(FH_write, '>', $file_write) or die "OOPS! I couldn't open the file '$file'!: $!\n\n";
	print "Converting...$file...";

	while(<FH_read>){
		if($_ =~ m/(\w+)\s*=\s*DFF\((\w+)\)\s*$/){
			print FH_write "OUTPUT($2)\n";
			print FH_write "INPUT($1)\n";
		} else {
			print FH_write $_;
		}
	}	

	close(FH_read);
	close(FH_write);
	print "DONE!\n";
}

print "\n\nConverting done! Enjoy your new benchmark circuits.\n\n";
