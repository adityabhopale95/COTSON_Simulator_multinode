class AddExecDirToSandboxModels < ActiveRecord::Migration
  def self.up
    add_column :sandbox_models, :exec_dir, :string
  end

  def self.down
    remove_column :sandbox_models, :exec_dir
  end
end
