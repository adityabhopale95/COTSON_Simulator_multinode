# Be sure to restart your server when you modify this file

# Uncomment below to force Rails into production mode when
# you don't control web/app server and can't set it the proper way
# ENV['RAILS_ENV'] ||= 'production'

# Specifies gem version of Rails to use when vendor/rails is not present
RAILS_GEM_VERSION = '2.0.2' unless defined? RAILS_GEM_VERSION

# Bootstrap the Rails environment, frameworks, and default configuration
require File.join(File.dirname(__FILE__), 'boot')

do_I_need_migration=false
Rails::Initializer.run do |config|
  # Settings in config/environments/* take precedence over those specified here.
  # Application configuration should go into files in config/initializers
  # -- all .rb files in that directory are automatically loaded.
  # See Rails::Configuration for more options.

  # Skip frameworks you're not going to use (only works if using vendor/rails).
  # To use Rails without a database, you must remove the Active Record framework
  # config.frameworks -= [ :active_record, :active_resource, :action_mailer ]

  # Only load the plugins named here, in the order given. By default, all plugins 
  # in vendor/plugins are loaded in alphabetical order.
  # :all can be used as a placeholder for all plugins not explicitly named
  # config.plugins = [ :exception_notification, :ssl_requirement, :all ]

  # Force all environments to use the same logger level
  # (by default production uses :info, the others :debug)
  # config.log_level = :debug

  # Your secret key for verifying cookie session data integrity.
  # If you change this key, all old sessions will become invalid!
  # Make sure the secret is at least 30 characters and all random, 
  # no regular words or you'll be exposed to dictionary attacks.
  config.action_controller.session = {
    :session_key => '_daemon_session',
    :secret      => 'e4a79aae33497c2585f688ac8002e0a8f016072675680f58ab27c9d97868bb9cf4627207c3b3fa637c6d317cf9e2d74e9caeed2c48332368229cd58921305904'
  }

  # Use the database for sessions instead of the cookie-based default,
  # which shouldn't be used to store highly confidential information
  # (create the session table with 'rake db:sessions:create')
  # config.action_controller.session_store = :active_record_store

  require 'cotson'
  host=this_host 
  
  db="config/database_#{host}.yml"
  if !File.exist?(db) then
    puts "Creating configuration file: #{db}" 
    db_hash = YAML::load_file("config/daemondb_template.yml")
    db_hash['development']['database'].gsub!(/\[\%.*\%\]/,"#{host}") if db_hash['development'] && db_hash['development']['database']
    db_hash['test']['database'].gsub!(/\[\%.*\%\]/,"#{host}") if db_hash['test'] && db_hash['test']['database']
    db_hash['production']['database'].gsub!(/\[\%.*\%\]/,"#{host}") if db_hash['production'] && db_hash['production']['database']
    
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

  # Make Active Record use UTC-base instead of local time
  # config.active_record.default_timezone = :utc
end


if do_I_need_migration then
	puts "Creating database (this may take a while)..."
	ActiveRecord::Migration.verbose = false
	ActiveRecord::Migrator.migrate('db/migrate/')
end
