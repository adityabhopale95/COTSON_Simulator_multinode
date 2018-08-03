class CreateExperiments < ActiveRecord::Migration
  def self.up
    create_table :experiments do |t|
      t.integer :simulation_id
      t.datetime :started_at
      t.datetime :ended_at

      t.timestamps
    end
  end

  def self.down
    drop_table :experiments
  end
end
