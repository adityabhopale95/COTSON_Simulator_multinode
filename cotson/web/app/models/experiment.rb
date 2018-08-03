class Experiment < ActiveRecord::Base
  belongs_to :simulation
  has_many :experiment_events
  has_many :experiment_machines
  
  belongs_to :experiment_data, :foreign_key => 'experiment_data_id'
  
  def before_create
    self.started_at = Time.now
  end
  
  def after_create
    #Start experiment
    exec("script/runner cotson_process.rb #{self.id}") if fork == nil
  end
  
  def log(event)
    self.experiment_events.new(:description =>"#{event}").save
  end
  
  def files
    Dir.glob("./db/experiments/#{self.id}/*")
  end
  
  def add_graphic
    ed=ExperimentData.new
    ed.description = "#{self.simulation.description} - #{Time.now}"
    ed.save
    self.experiment_data = ed
    self.save
  end
end
