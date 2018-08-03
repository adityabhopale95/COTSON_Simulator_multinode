class AddActiveToMachine < ActiveRecord::Migration
  def self.up
	add_column :machines, :active, :boolean
  end

  def self.down
	remove_column :machines, :active
  end
end
