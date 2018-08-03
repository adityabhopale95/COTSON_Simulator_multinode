class CreateSimulations < ActiveRecord::Migration
  def self.up
    create_table :simulations do |t|
      t.string :description
      t.text :lua
      t.text :command

      t.timestamps
    end
  end

  def self.down
    drop_table :simulations
  end
end
