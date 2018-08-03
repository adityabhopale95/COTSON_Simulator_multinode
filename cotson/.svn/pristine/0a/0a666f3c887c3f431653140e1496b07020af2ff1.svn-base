class CreateExperimentEvents < ActiveRecord::Migration
  def self.up
    create_table :experiment_events do |t|
      t.string :description
      t.integer :experiment_id

      t.timestamps
    end
  end

  def self.down
    drop_table :experiment_events
  end
end
