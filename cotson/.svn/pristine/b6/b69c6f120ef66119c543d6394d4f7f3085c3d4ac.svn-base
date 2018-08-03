class Metric < GraphicBase
  set_table_name 'metrics'
  set_primary_key 'metric_id'

  belongs_to :heartbeat
  belongs_to :metric_name , :foreign_key => 'metric_id'
end
