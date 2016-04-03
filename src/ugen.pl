#!perl -w

use File::Find;
use Data::Dumper;

$num_args = $#ARGV + 1;
if ($num_args != 1)
{
  print "\nUsage: ugen.pl directory_name\n";
  exit;
}

$path = $ARGV[0];
print "Scanning... ".$path."\n";

my @files;

find(
  sub { (-f && /\.utm$/i) or return; 
           push @files, $File::Find::name; 
    }, $path);

foreach(@files)
{
  my $utmfile = $_;
  $utmfile =~ /(\S+)\/(\S+).utm/ || next;

  my $ufolder = $1;
  if ($ufolder eq ".") {
    $ufolder = "./";
  }
  my $uclassname = $2;

  print "found: ".$ufolder.", class: ".$uclassname."\n";
  generate_uclass($uclassname, $ufolder);
}

sub trim {
    (my $s = $_[0]) =~ s/^\s+|\s+$//g;
    return $s;        
}

sub generate_uclass {
  my $given_classname = shift;
  my $folder = shift;
  my $classname = $given_classname."_base";

  print "### Generating class: ".$classname."\n";

  my $classfile = $folder."/".$given_classname.".utm";

  open(F, $classfile) or die "Error opening $classfile for reading";
  
  my %parser;
  $parser{"guard"} = "UTM_".uc($classname)."_H";
  
  my $guard = "UTM_".uc($classname)."_H";
  my $template;

  my $pure_virtual = 0;
  
  while(<F>)
  {
	chomp;
        $currentline = trim($_);
	next if ($currentline =~ /^#/);
	next if ($currentline =~ /^\/\//);

	if ($currentline =~ /(.+)\/\//) {
	  $currentline = trim($1);
        };

        $_ = $currentline;
        next if ($_ eq "");

	$currentline =~ s/\$classname/$classname/g;
	
	$parser{'registry_on'} = 1 if ($_ =~ /registry:true/);
	$parser{'threadsafe_rw'} = "    mutable boost::mutex threadsafe_rw_guard;" if ($_ =~ /threadsafe_rw:true/);
	
	my ($key, $value) = split(":", $_, 2);
	$value =~ s/\s*$//g;  # chomp string with removing all trailing whitespaces	
	my @values = split(/\s+/, $value);

	$parser{'template'} = $value if ($key eq "template");
	$parser{'xmlroot'} = $value if ($key eq "xmlroot");

	if (($key eq "include")) {
	  my $tmp;
	  if (defined($parser{$key})) {
	    $tmp = $parser{$key};
	  }
          foreach (@values) {
	    next if ($_ eq '');
	    push (@$tmp, "#".$key." ".$_);
	  }
	  $parser{$key} = $tmp;
	}
	
	if (($key eq "typedef") || ($key eq "virtual") || ($key eq "define")) {
	  my $tmp;
	  if (defined($parser{$key})) {
	    $tmp = $parser{$key};
	  }
	  if ($key eq "define") {
            push (@$tmp, "#".$key." ".$value);
	  } else {
            push (@$tmp, $key." ".$value.";");
	  }
	  $parser{$key} = $tmp;
	  
	  if ($key eq "virtual") {
	    if ($tmp =~ /= \s0$/) {
	      # pure virtual function detected
	      $pure_virtual = 1;
	    }
	  }
	}

	$currentline =~ /member: ([:_<>\w\s\d]+)\s+\(([\S]+)\)\s+([_\w\d\[\]]+)\s+\"([\w]+)\"/ || next;

	my %member;
        $member{'typename'} = $1;
	$member{'defaultvalue'} = $2;
	$member{'membername'} = $3;
	$member{'xmltagname'} = $4;
	$member{'xmlcreate'} = "";
	$member{'xmlparse'} = "";

	if ($currentline =~ /member: ([:_<>\w\s\d]+)\s+\(([\S]+)\)\s+([_\w\d\[\]]+)\s+\"([\w]+)\"\s+(\S+)\s+(\S+)/) {
	  $member{'xmlcreate'} = $5;
	  $member{'xmlparse'} = $6;
	}

        my $is_atomic = 0;
        $is_atomic = 1 if ($member{'typename'} =~ /std::atomic/);

        my $nodefault = 0;
        if ($member{'typename'} =~ /:nodefault/) {
          $member{'typename'} =~ s/:nodefault//g;
          $nodefault = 1;
        };        

        my $is_array = 0;
        my $clearmembername = $member{'membername'};
        my $equal = "    if (!(".$member{'membername'}." == other->".$member{'membername'}.")) return false;";
        if (($member{'membername'} =~ /([_\w\d]+)\[/) || ($member{'defaultvalue'} =~ /memset/)) {
          $is_array = 1;
          $clearmembername = $1 if ($member{'membername'} =~ /([_\w\d]+)\[/);
          $equal =  "    if ((memcmp(&".$clearmembername.", &other->".$clearmembername.", sizeof(".$clearmembername."))!=0)) return false;";
        }
	
	my $tmp1 = $parser{'equal_operator'};
	push (@$tmp1, $equal);
	$parser{'equal_operator'} = $tmp1;

	my $tmp2 = $parser{'members'};
	push (@$tmp2, "    ".$member{'typename'}." ".$member{'membername'}.";");
	$parser{'members'} = $tmp2;
	
	my $tmp10 = $parser{'assignment_operator'};
	if ($is_array == 1) {
	  push (@$tmp10, "    memcpy(&".$clearmembername.", &rhs.".$clearmembername.", sizeof(".$clearmembername."));");
	} elsif ($is_atomic == 1) {
	  push (@$tmp10, "    ".$clearmembername." = rhs.".$clearmembername.".load();");
        } else {
	  push (@$tmp10, "    ".$clearmembername." = rhs.".$clearmembername.";");
	}
	$parser{'assignment_operator'} = $tmp10;

	my $tmp3 = $parser{'clear_function'};
        if ($is_array == 1) {
	  if (($member{'defaultvalue'} =~ /memset/)) {
	    push (@$tmp3, "    memset(&".$clearmembername.", 0, sizeof(".$clearmembername."));");
	  } else {
	    push (@$tmp3, "    memset(&".$clearmembername.", ".$member{'defaultvalue'}.", sizeof(".$clearmembername."));");
	  }
        } elsif ($member{'defaultvalue'} =~ /^\.\S+/) {
 	  push (@$tmp3, "    ".$member{'membername'}.$member{'defaultvalue'}.";");
	} else {
	  push (@$tmp3, "    ".$member{'membername'}." = ".$member{'defaultvalue'}.";");
	}
	$parser{'clear_function'} = $tmp3;

    my $prefix = "  ";
	my $tmp4 = $parser{'xml_create_function'};
	if ($member{'xmlcreate'} ne "") {
  	  push (@$tmp4, $prefix."  xml_append_node(\"".$member{'xmltagname'}."\", ".$member{'xmlcreate'}.");");
	} elsif ($is_atomic == 1) {
 	  push (@$tmp4, $prefix."  xml_append_node(\"".$member{'xmltagname'}."\", ".$member{'membername'}.".load(), orig.".$member{'membername'}.".load());");
	} elsif ($nodefault == 1) {
 	  push (@$tmp4, $prefix."  xml_append_node(\"".$member{'xmltagname'}."\", ".$member{'membername'}.");");
        } else {
 	  push (@$tmp4, $prefix."  xml_append_node(\"".$member{'xmltagname'}."\", ".$member{'membername'}.", orig.".$member{'membername'}.");");
	}
	$parser{'xml_create_function'} = $tmp4;

	my $tmp5 = $parser{'xml_catch_value_function'};
	if ($member{'xmlparse'} ne "") {
  	  push (@$tmp5, $prefix."  if (xml_check_value(keyname, \"".$member{'xmltagname'}."\", keyvalue, ".$member{'xmlparse'}.")) return;");
	} else {
  	  push (@$tmp5, $prefix."  if (xml_check_value(keyname, \"".$member{'xmltagname'}."\", keyvalue, ".$member{'membername'}.")) return;");
	}
	$parser{'xml_catch_value_function'} = $tmp5;
	
	$parser{'xml_root_operator'} = "    xml_append_root(".$parser{'xmlroot'}.");";

        if (defined ($parser{'registry_on'})) {
	  $parser{'has_registry_true'} = "    bool has_registry() { return true; };";
	  
	  my $tmp6 = $parser{'registry_save_function'};
	  push (@$tmp6, "    RegistrySave(h, \"".$member{'xmltagname'}."\", ".$member{'membername'}.");");
	  $parser{'registry_save_function'} = $tmp6;

	  my $tmp7 = $parser{'registry_load_function'};  
          push (@$tmp7, "    RegistryLoad(h, \"".$member{'xmltagname'}."\", ".$member{'membername'}.");");
	  $parser{'registry_load_function'} = $tmp7; 
        }
  }

  close(F);

  if (defined ($parser{'threadsafe_rw'})) {
    my $inc = $parser{'include'};
    push (@$inc, "#include <boost/thread/mutex.hpp>");
    $parser{'include'} = $inc;
    
    $parser{'member_threadsafe_rw'} = "    mutable boost::mutex guard_threadsafe_rw;";
    $parser{'method_threadsafe_rw'} = "    static void threadsafe_copyobj(bool lock_src, const ".$classname."& src, ".$classname."& dst);";
    $parser{'ctor_threadsafe_copyobj'} = "    if (make_threadsafe_copyobj) { threadsafe_copyobj(true, rhs, *this); return; };";
    
    my $thr;
    push (@$thr, "void ".$classname."::threadsafe_copyobj(bool lock_src, const ".$classname."& src, ".$classname."& dst)");
    push (@$thr, "{");
    push (@$thr, "    if (lock_src)");
    push (@$thr, "        boost::mutex::scoped_lock lock(src.guard_threadsafe_rw);");
    push (@$thr, "    else");
    push (@$thr, "        boost::mutex::scoped_lock lock(dst.guard_threadsafe_rw);");
    push (@$thr, "    ");
    push (@$thr, "    dst = src;");
    push (@$thr, "}");
    $parser{'implementation_threadsafe_rw_function'} = $thr;
  }
  
  if (!defined($template)) {
    $template = "utemplate_standard";
  }

  process_template($classname, $folder, '.h', \%parser);
  process_template($classname, $folder, '.cpp', \%parser);
}

sub read_template {
  my $templatefile = pop;
  my $classname = pop;
  my $classname_child = $classname;
  $classname_child =~ s/_base//g;

  my @t;
  open(TT, "$templatefile") or die "Error opening $templatefile for reading";
  while(<TT>)
  {
    $_ =~ s/\s*$//g;  # chomp string with removing all trailing whitespaces
    $_ =~ s/\$classname_child/$classname_child/g;
    $_ =~ s/\$classname/$classname/g;
    push(@t, $_);
  }
  close(TT);
  return @t;
}

sub process_template() {
  my $classname = shift;
  my $folder = shift;
  my $fileext = shift;
  my $p = shift;
  my %parser = %{$p};

  my $template = $parser{'template'};
  if (!defined($template)) {
    $template = "utemplate_standard";
  }

  my @templ = read_template($classname, $template.$fileext);
  my $filename = $folder."/".$classname.$fileext;
  
  print $filename."\n";
  open(H, ">$filename") or die "Error opening $filename for writing";
  
  foreach (@templ) {
    if ($_ =~ /\*\*(\w+)\*\*/) {
      my $tag = $1;
      my $val = $parser{$tag};
      if (defined($val)) {
        my $isa = is_array($val);
	if ($isa == 1) {
	  my @arr = @{$val};
	  foreach(@arr) {
	    my $s = $_;
	    print H $s."\n";
	  }
	} else {
	  (my $str = $_) =~ s/\*\*\w+\*\*/$val/g;
	  print H $str."\n";
	}
      } else {
	print H "\n";
      }
    }
    else
    {
      print H $_."\n";
    }
  }
  
  close(H);
}

sub is_array {
  my ($ref) = @_;
  # Firstly arrays need to be references, throw
  #  out non-references early.
  return 0 unless ref $ref;

  # Now try and eval a bit of code to treat the
  #  reference as an array.  If it complains
  #  in the 'Not an ARRAY reference' then we're
  #  sure it's not an array, otherwise it was.
  eval {
    my $a = @$ref;
  };
  if ($@=~/^Not an ARRAY reference/) {
    return 0;
  } elsif ($@) {
    die "Unexpected error in eval: $@\n";
  } else {
    return 1;
  }
}
