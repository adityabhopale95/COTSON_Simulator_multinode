class RemoveUnwantedModels < ActiveRecord::Migration
	def self.up
		drop_table :node_processes
		drop_table :commands
		drop_table :simulations
	end

	def self.down
		create_table "commands", :force => true do |t|
			t.string   "command"
			t.string   "result"
			t.boolean  "error"
			t.datetime "created_at"
			t.datetime "updated_at"
		end

		create_table "node_processes", :force => true do |t|
			t.boolean  "running"
			t.datetime "created_at"
			t.datetime "updated_at"
			t.string   "sandbox"
			t.boolean  "mediator"
			t.string   "configuration"
			t.integer  "nodeid"
			t.string   "mediator_data"
			t.text     "results"
			t.boolean  "finished"
		end

		create_table "simulations", :force => true do |t|
			t.string   "configuration"
			t.integer  "nodes"
			t.datetime "created_at"
			t.datetime "updated_at"
		end
	end
end
