#!/usr/bin/perl
use strict;
use warnings;
use MIME::Base64;

# Read PEM file
my $file = shift or die "Usage: $0 <pem_file>\n";
my ($input_file_name) = $file =~ /^(.*?)\.[^.]*$/;
open my $fh, '<', $file or die "Cannot open $file: $!\n";
my $pem = do { local $/; <$fh> };
close $fh;

# Extract the base64 encoded part (remove header/footer)
$pem =~ s/^-----BEGIN.*?-----\s*//s;
$pem =~ s/-----END.*?-----\s*$//s;
$pem =~ s/\s+//g;

# Decode base64
my $binary = decode_base64($pem);

# Output as C array
my @bytes = unpack('C*', $binary);
my $length = scalar(@bytes);

print "/*This is an auto generated code please DO NOT modify */";
print "/* RSA key from $file */\n";
print "static const unsigned char $input_file_name\[] = {\n\t";

for (my $i = 0; $i < $length; $i++) {
    printf "0x%02x", $bytes[$i];
    print "," if $i < $length - 1;
    print "\n\t" if ($i + 1) % 16 == 0 && $i < $length - 1;
}

print "\n};\n";
print "unsigned $input_file_name\_size = ", $length, ";\n";
