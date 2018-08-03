class AddConsolePortToExperimentMachines < ActiveRecord::Migration
  def self.up
    add_column :experiment_machines, :console_port, :integer
  end

  def self.down
    remove_column :experiment_machines, :console_port
  end
end
