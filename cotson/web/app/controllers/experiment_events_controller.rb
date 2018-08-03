class ExperimentEventsController < ApplicationController
  # GET /experiment_events
  # GET /experiment_events.xml
  def index
    @experiment_events = ExperimentEvent.find(:all)

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @experiment_events }
    end
  end

  # GET /experiment_events/1
  # GET /experiment_events/1.xml
  def show
    @experiment_event = ExperimentEvent.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @experiment_event }
    end
  end

  # GET /experiment_events/new
  # GET /experiment_events/new.xml
  def new
    @experiment_event = ExperimentEvent.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @experiment_event }
    end
  end

  # GET /experiment_events/1/edit
  def edit
    @experiment_event = ExperimentEvent.find(params[:id])
  end

  # POST /experiment_events
  # POST /experiment_events.xml
  def create
    @experiment_event = ExperimentEvent.new(params[:experiment_event])

    respond_to do |format|
      if @experiment_event.save
        flash[:notice] = 'ExperimentEvent was successfully created.'
        format.html { redirect_to(@experiment_event) }
        format.xml  { render :xml => @experiment_event, :status => :created, :location => @experiment_event }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @experiment_event.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /experiment_events/1
  # PUT /experiment_events/1.xml
  def update
    @experiment_event = ExperimentEvent.find(params[:id])

    respond_to do |format|
      if @experiment_event.update_attributes(params[:experiment_event])
        flash[:notice] = 'ExperimentEvent was successfully updated.'
        format.html { redirect_to(@experiment_event) }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @experiment_event.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /experiment_events/1
  # DELETE /experiment_events/1.xml
  def destroy
    @experiment_event = ExperimentEvent.find(params[:id])
    @experiment_event.destroy

    respond_to do |format|
      format.html { redirect_to(experiment_events_url) }
      format.xml  { head :ok }
    end
  end
end
