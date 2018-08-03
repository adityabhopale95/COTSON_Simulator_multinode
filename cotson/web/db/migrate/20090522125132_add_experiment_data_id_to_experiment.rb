class AddExperimentDataIdToExperiment < ActiveRecord::Migration
  def self.up
    add_column :experiments, :experiment_data_id, :integer
  end

  def self.down
    remove_column :experiments, :experiment_data_id
  end
end
