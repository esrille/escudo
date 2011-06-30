#!/usr/bin/perl
use strict;
use warnings;

# Scrape IDL definitions from the HTML 5 Draft Recommendation
#
# usage: idl_scraper.pl http://www.whatwg.org/specs/web-apps/current-work/

use URI;
use Web::Scraper;

my $res = scraper {
    process 'pre.idl', 'idl[]' => 'TEXT';
    process 'code.idl-code', 'idl[]' => 'TEXT';
    result 'idl';
}->scrape(URI->new(shift));

while (my $def = shift(@$res)) {
    # Obsolete features are described as "[XXX] interface"
    if (0 <= index($def, "[XXX] interface")) {
        next;
    }
    # Ignore snippets
    # if (index($def, "interface") == -1) {
    #     next;
    # }
    # Ignore examples
    if (0 <= index($def, "interface Example") && 0 <= index($def, "// this is an IDL definition")) {
        next;
    }
    print $def;
    print "\n\n";
}
