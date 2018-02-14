
require './ttrl/releaser.rb'

base = File.dirname( File.expand_path( __FILE__ ) )
project_name = "ttl"

## -- source -----
STDERR.puts( "== source ====================" )

releaser = TTRL::Releaser.new( project_name, base )
releaser.strftime_template = '%y%m%d_%H%M%S'

Dir.glob( "*" ) {|one|
  if File.directory?( one )
    
  else
    releaser.add_file( one )
  end
}


[
"test_template",
"sample",
"ttrl",
].each {|dir| releaser.add_dir( dir ) }


releaser.add_pre_copy {|file_utils, to_base|
  [
  "lib",
  "lib\\x86",
  "lib\\x64",
  ].each {|one| file_utils.mkdir( "#{to_base}/#{one}" ) }
}


releaser.release_dir = "../backups/#{project_name}/backup"
releaser.additional_string = ""
releaser.make_zip
