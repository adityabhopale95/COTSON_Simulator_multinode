# This file is auto-generated from the current state of the database. Instead of editing this file, 
# please use the migrations feature of Active Record to incrementally modify your database, and
# then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your database schema. If you need
# to create the application database on another system, you should be using db:schema:load, not running
# all the migrations from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended to check this file into your version control system.

ActiveRecord::Schema.define(:version => 20090522125132) do

  create_table "experiment_events", :force => true do |t|
    t.string   "description"
    t.integer  "experiment_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "experiment_machines", :force => true do |t|
    t.integer  "experiment_id"
    t.integer  "machine"
    t.integer  "sandbox_id"
    t.boolean  "mediator"
    t.string   "server"
    t.integer  "vnc_port"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "console_port"
  end

  create_table "experiments", :force => true do |t|
    t.integer  "simulation_id"
    t.datetime "started_at"
    t.datetime "ended_at"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "experiment_data_id"
  end

  create_table "machines", :force => true do |t|
    t.string   "host"
    t.integer  "port"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.boolean  "active"
  end

  create_table "simulations", :force => true do |t|
    t.string   "description"
    t.text     "lua"
    t.text     "command"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end
