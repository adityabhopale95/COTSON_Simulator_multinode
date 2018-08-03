class CreateExperimentMachines < ActiveRecord::Migration
  def self.up
    create_table :experiment_machines do |t|
      t.integer :experiment_id
      t.integer :machine
      t.integer :sandbox_id
      t.boolean :mediator
      t.string :server
      t.integer :vnc_port

      t.timestamps
    end
  end

  def self.down
    drop_table :experiment_machines
  end
end
