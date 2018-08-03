class ExperimentData < GraphicBase 
  set_table_name 'experiments'
  set_primary_key 'experiment_id'
                              
  has_one :experiment                            
  has_many :parameters, :foreign_key => 'experiment_id'
  has_many :heartbeats, :foreign_key => 'experiment_id'
end
