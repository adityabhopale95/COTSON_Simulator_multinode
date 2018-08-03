class MainController < ApplicationController
  def index
    @machines = Machine.find(:all)
    @running = Experiment.find(:all, :conditions => {:ended_at => nil}, :order => "started_at desc" )
    @experiments = Experiment.find(:all, :conditions => "ended_at is not null", :order => "ended_at desc" )
  end
end
