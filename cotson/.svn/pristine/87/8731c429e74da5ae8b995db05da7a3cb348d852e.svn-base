class ExperimentMachinesController < ApplicationController
  # GET /experiment_machines
  # GET /experiment_machines.xml
  def index
    @experiment_machines = ExperimentMachines.find(:all)

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @experiment_machines }
    end
  end

  # GET /experiment_machines/1
  # GET /experiment_machines/1.xml
  def show
    @experiment_machines = ExperimentMachines.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @experiment_machines }
    end
  end

  # GET /experiment_machines/new
  # GET /experiment_machines/new.xml
  def new
    @experiment_machines = ExperimentMachines.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @experiment_machines }
    end
  end

  # GET /experiment_machines/1/edit
  def edit
    @experiment_machines = ExperimentMachines.find(params[:id])
  end

  # POST /experiment_machines
  # POST /experiment_machines.xml
  def create
    @experiment_machines = ExperimentMachines.new(params[:experiment_machines])

    respond_to do |format|
      if @experiment_machines.save
        flash[:notice] = 'ExperimentMachines was successfully created.'
        format.html { redirect_to(@experiment_machines) }
        format.xml  { render :xml => @experiment_machines, :status => :created, :location => @experiment_machines }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @experiment_machines.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /experiment_machines/1
  # PUT /experiment_machines/1.xml
  def update
    @experiment_machines = ExperimentMachines.find(params[:id])

    respond_to do |format|
      if @experiment_machines.update_attributes(params[:experiment_machines])
        flash[:notice] = 'ExperimentMachines was successfully updated.'
        format.html { redirect_to(@experiment_machines) }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @experiment_machines.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /experiment_machines/1
  # DELETE /experiment_machines/1.xml
  def destroy
    @experiment_machines = ExperimentMachines.find(params[:id])
    @experiment_machines.destroy

    respond_to do |format|
      format.html { redirect_to(experiment_machines_url) }
      format.xml  { head :ok }
    end
  end
end
