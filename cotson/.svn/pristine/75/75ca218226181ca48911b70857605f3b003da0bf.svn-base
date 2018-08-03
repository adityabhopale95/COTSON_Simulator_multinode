# Be sure to restart your server when you modify this file

# Uncomment below to force Rails into production mode when
# you don't control web/app server and can't set it the proper way
# ENV['RAILS_ENV'] ||= 'production'

# Specifies gem version of Rails to use when vendor/rails is not present
RAILS_GEM_VERSION = '2.1.0' unless defined? RAILS_GEM_VERSION

# Bootstrap the Rails environment, frameworks, and default configuration
require File.join(File.dirname(__FILE__), 'boot')

do_I_need_migration=false
Rails::Initializer.run do |config|
  # Settings in config/environments/* take precedence over those specified here.
  # Application configuration should go into files in config/initializers
  # -- all .rb files in that directory are automatically loaded.
  # See Rails::Configuration for more options.

  # Skip frameworks you're not going to use. To use Rails without a database
  # you must remove the Active Record framework.
  # config.frameworks -= [ :active_record, :active_resource, :action_mailer ]

  # Specify gems that this application depends on. 
  # They can then be installed with "rake gems:install" on new installations.
  # config.gem "bj"
  # config.gem "hpricot", :version => '0.6', :source => "http://code.whytheluckystiff.net"
  # config.gem "aws-s3", :lib => "aws/s3"

  # Only load the plugins named here, in the order given. By default, all plugins 
  # in vendor/plugins are loaded in alphabetical order.
  # :all can be used as a placeholder for all plugins not explicitly named
  # config.plugins = [ :exception_notification, :ssl_requirement, :all ]

  # Add additional load paths for your own custom dirs
  # config.load_paths += %W( #{RAILS_ROOT}/extras )

  # Force all environments to use the same logger level
  # (by default production uses :info, the others :debug)
  # config.log_level = :debug

  # Make Time.zone default to the specified zone, and make Active Record store time values
  # in the database in UTC, and return them converted to the specified local zone.
  # Run "rake -D time" for a list of tasks for finding time zone names. Uncomment to use default local time.
  config.time_zone = 'UTC'

  # Your secret key for verifying cookie session data integrity.
  # If you change this key, all old sessions will become invalid!
  # Make sure the secret is at least 30 characters and all random, 
  # no regular words or you'll be exposed to dictionary attacks.
  config.action_controller.session = {
    :session_key => '_web_session',
    :secret      => '84bff6e6898f5ab0e0761c2d2d1dea0765fc72ad8773c9cced5c5f8c1bb471a7c95648377b833b0f5b3796e6222b6cb7850db2c11e3c0a905acaa0dedcff3d67'
  }

  # Use the database for sessions instead of the cookie-based default,
  # which shouldn't be used to store highly confidential information
  # (create the session table with "rake db:sessions:create")
  # config.action_controller.session_store = :active_record_store
  require 'cotson'
  host=this_host 
  
  db="config/database_#{host}.yml"
  if !File.exist?(db) then
    puts "Creating configuration file: #{db}" 
    db_hash = YAML::load_file("config/webdb_template.yml")
    db_hash['development']['database'].gsub!(/\[\%.*\%\]/,"#{host}") if db_hash['development'] && db_hash['development']['database']
    db_hash['test']['database'].gsub!(/\[\%.*\%\]/,"#{host}") if db_hash['test'] && db_hash['test']['database']
    db_hash['production']['database'].gsub!(/\[\%.*\%\]/,"#{host}") if db_hash['production'] && db_hash['production']['database']
    
    exp_hash = YAML::load_file("config/experimentsdb.yml")
    exp_hash.each do |k,v|
      db_hash["experiments_#{k}"]=v
    end
    File.open(db,"w") do |f|
      f.puts YAML::dump(db_hash)
    end
    do_I_need_migration=true
  end
  
  puts "Using #{db} file..."
  config.database_configuration_file = db

  # Use SQL instead of Active Record's schema dumper when creating the test database.
  # This is necessary if your schema can't be completely dumped by the schema dumper,
  # like if you have constraints or database-specific column types
  # config.active_record.schema_format = :sql

  # Activate observers that should always be running
  # config.active_record.observers = :cacher, :garbage_collector
end


if do_I_need_migration then
  puts "Creating database (this may take a while)..."
  ActiveRecord::Migration.verbose = false
  ActiveRecord::Migrator.migrate('db/migrate/')
end

#check if it is possible to connect to the graphic data database
begin
  GraphicBase.connection
  ENV['graphic_db'] = 'true'
rescue Exception =>e
  puts e.class
  puts e.to_s
  ENV['graphic_db'] = nil
end

