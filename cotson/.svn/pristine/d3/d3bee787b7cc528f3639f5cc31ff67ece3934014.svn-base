class Heartbeat < GraphicBase
  set_table_name 'heartbeats'
  set_primary_key 'heartbeat_id'

  belongs_to :experiment_data, :foreign_key => 'experiment_id'
  has_many :metrics
 end
