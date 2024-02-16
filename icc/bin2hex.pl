#/*****Note- This Script is coping the effect of xxd -i command**********/
#!/usr/bin/perl
use strict;
use warnings;

if (@ARGV != 2) {
    die "Usage: $0 input_binary_file output_text_file\n";
}

my ($input_file, $output_file) = @ARGV;
my ($input_file_name) = $input_file =~ /^(.*?)\.[^.]*$/;

open my $input_fh, '<:raw', $input_file or die "Cannot open input file '$input_file': $!\n";
open my $output_fh, '>', $output_file or die "Cannot open output file '$output_file': $!\n";

print $output_fh "static const unsigned char $input_file_name\[] = {\n\t";

my $n = 0;
while (read $input_fh, my $buffer, 1) {
    my $hex_byte = unpack 'H2', $buffer;
    if ($n > 0 ) {
	print $output_fh ",";
    }
    if ($n > 0 && ($n % 16) == 0) {
        print $output_fh "\n\t";
    }
    print $output_fh "0x", $hex_byte;
    $n = $n + 1;
}
print $output_fh "\n};\n";
print $output_fh "unsigned $input_file_name\_size = ", $n, ";\n";

close $input_fh;
close $output_fh;
