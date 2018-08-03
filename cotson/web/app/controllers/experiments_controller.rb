require 'graphic_data'

class ExperimentsController < ApplicationController
  # GET /experiments
  # GET /experiments.xml
  def index
    @experiments = Experiment.find(:all, :order => 'started_at desc')

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @experiments }
    end
  end

  # GET /experiments/1
  # GET /experiments/1.xml
  def show
    @experiment = Experiment.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @experiment }
    end
  end

  # GET /experiments/new
  # GET /experiments/new.xml
  def new
    @experiment = Experiment.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @experiment }
    end
  end

  # GET /experiments/1/edit
  def edit
    @experiment = Experiment.find(params[:id])
  end

  # POST /experiments
  # POST /experiments.xml
  def create
    @experiment = Experiment.new(params[:experiment])
    @experiment.add_graphic if params[:graphic][:value]=='1'

    respond_to do |format|
      if @experiment.save
        flash[:notice] = 'Experiment was successfully created.'
        format.html { redirect_to(:controller => "experiments", :id=>@experiment.id, :action => "status") }
        format.xml  { render :xml => @experiment, :status => :created, :location => @experiment }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @experiment.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /experiments/1
  # PUT /experiments/1.xml
  def update
    @experiment = Experiment.find(params[:id])

    respond_to do |format|
      if @experiment.update_attributes(params[:experiment])
        flash[:notice] = 'Experiment was successfully updated.'
        format.html { redirect_to(@experiment) }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @experiment.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /experiments/1
  # DELETE /experiments/1.xml
  def destroy
    @experiment = Experiment.find(params[:id])
    @experiment.destroy

    respond_to do |format|
      format.html { redirect_to(experiments_url) }
      format.xml  { head :ok }
    end
  end
  
  def get_file
    send_file(RAILS_ROOT+"/"+params[:file])
  end

  def view_file
    @experiment = Experiment.find(params[:id])
    @file = "#{RAILS_ROOT}/#{params[:file]}"
    @file_name = File.basename(@file)
    @data = []
    File.open(@file).each_line {|l| @data << l}
  end
  
  def status
    @experiment = Experiment.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @experiment }
    end
  end
  
  def screen
    @experiment = Experiment.find(params[:id])
    @experiment_machine = @experiment.experiment_machines.find(params[:experiment_machine_id])
    @screen = "http://#{@experiment_machine.screen_server}"
  end
  
  def console
    @experiment = Experiment.find(params[:id])
    @experiment_machine = @experiment.experiment_machines.find(params[:experiment_machine_id])
    if @experiment_machine != nil
      @screen = "http://#{@experiment_machine.console_server}"
    else
      @screen = nil
    end
  end

  
  def graphic
    @experiment = Experiment.find(params[:id])
    @experiment_data = @experiment.experiment_data
    @presets = GraphicData.preset_graphics

    respond_to do |format|
      if @experiment_data != nil
        format.html # show.html.erb
        format.xml  { render :xml => @experiment }
      else
        flash[:notice] = 'No graphics exist for this experiment'
        format.html { redirect_to(:controller => "experiments", :id=>@experiment.id, :action => "show") }
      end
    end
  end
  
  def graphic_data
    puts  "Cotson data parameters"
    experiment=Experiment.find(params[:id])
    experiment_data=experiment.experiment_data
    puts params
    gd = GraphicData.new(experiment_data.id)
    gd.set_preset_metric(params[:graphic].to_sym)
    puts "   POINTS SET TO #{params[:points]}"
    if params[:points]
      gd.limit_points(params[:points].to_i)
    else
      gd.limit_points(100)
    end
    
    gd.retrieve_data
    
    render :xml => gd.graphic_data_xml
  end
end
