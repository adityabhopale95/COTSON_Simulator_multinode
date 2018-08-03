$data_cache = {} 

class GraphicData
  @@preset_metrics = {
      :ipc  => {:name => "IPC Chart", :filter => "~'cpu.*\.instcount'", :frequency => 2200.0 },
      :idle => {:name => "Idle Chart", :filter => "~'cpu.*\.idlecount'"},
      :packets => { :name => "Packet Traffic", :filter => "~'nic.*\.pack_read'"} ,
      #TODO: Check what does this information mean
      :bytes => {:name => "Bytes", :filter => "~'nic.*\.bytes_read'"}
          }
          
  NANOS_IN_MILI = 1000000
  MHZ_IN_HZ = 1000000
  MILI_IN_SEC = 1000

  def initialize(eid)
    raise "Experiment id is required" if eid == nil
    @connection = ExperimentData.connection
    @experiment = ExperimentData.find(eid)
    raise "Experiment with id = #{eid} does not exist" if @experiment == nil
    @graphic_points = 100 #default
    @filter = nil #no metric set
    @factor = nil #no multiply factor for values
    @series_data = {} 
    @series_nanos = {} 
    @time = {}
    @last_heartbeat = @experiment.heartbeats.minimum("heartbeat_id") - 1
    @heartbeat_limit = nil
  end
  
  def GraphicData.preset_graphics
    res = {}
    @@preset_metrics.each { |k,v| res[k] = v[:name] }
    res
  end

  def set_preset_metric(pm, parms=nil)
    raise "Preset metric does not exist" if @@preset_metrics[pm] == nil

    @filter = @@preset_metrics[pm][:filter]
    @title = @@preset_metrics[pm][:name]
    @preset_metric = pm

    if pm == :ipc
      #Get frequency
      m = Metric.find(:first,
                      :joins => 'natural join metric_names',
                      :conditions => "name='simnow.cycles_per_usec' AND heartbeat_id IN (SELECT heartbeat_id FROM heartbeats WHERE experiment_id=#{@experiment.experiment_id} LIMIT 1) "
                      ) 
      frequency = nil
      frequency = m.value if m
      if parms != nil && parms[:frequency] != nil
        frequency=parms[:frequency]
      else
        if frequency != nil then
          puts "                            FREQUENCY: #{frequency}"
        else
          frequency = @@preset_metrics[pm][:frequency]
        end
      end
      @factor = Float(MILI_IN_SEC) / (Float(frequency) * MHZ_IN_HZ)
    end
    if parms != nil && parms[:factor] != nil
      @factor = parms[:factor]
    end
    puts "---FACTOR = #{@factor}"
  end

  def limit_points(limit)
    @graphic_points = limit    
  end
  
  def limit_heartbeat(limit=nil)
    @heartbeat_limit = limit
  end

  def retrieve_data
    puts "DATA CACHED (at retrieve):#{$data_cache.size}"
#    if recover_from_cache != nil
#      return 0
#    end
    #Recover data from cache
    puts "Recovering from cache ..."
    recover_from_cache
    #Complement with new data
    puts "Complement with new data ..."
    max_heartbeat = @heartbeat_limit == nil ? @experiment.heartbeats.maximum("heartbeat_id") : @heartbeat_limit
    puts "(last beat = #{@last_heartbeat} - max beat = #{max_heartbeat})"
    if max_heartbeat == @last_heartbeat
      #No new heartbeat => data is up to date
      puts "no new heartbeat"
      return 0
    end
    
    puts "retrieve_data start  ... "
    raise "No metric filter/selection" if @filter == nil
    @time[:retrieve] = Time.now
    
    conditions = "name #{@filter}"
    data = Metric.find_by_sql("SELECT metrics.metric_id,machine_id,nanos,name,value
                                FROM heartbeats NATURAL JOIN metrics NATURAL JOIN metric_names
                                WHERE experiment_id=#{@experiment.experiment_id} AND 
                                      heartbeats.heartbeat_id > #{@last_heartbeat} and heartbeats.heartbeat_id <= #{max_heartbeat} AND
                                      metric_id IN ( SELECT metric_id 
                                                     FROM metric_names natural join metrics
                                                     WHERE #{conditions} AND 
                                                           metrics.heartbeat_id > #{@last_heartbeat} and metrics.heartbeat_id <= #{max_heartbeat} AND
                                                           heartbeat_id IN ( SELECT heartbeat_id 
                                                                             FROM heartbeats 
                                                                             WHERE experiment_id = #{@experiment.experiment_id}  
                                                                           ) 
                                                   ) 
                                ORDER BY machine_id,name,nanos;" )
    @last_heartbeat = max_heartbeat
    @time[:retrieve] = Time.now - @time[:retrieve]
    puts "retrieve_data duration: #{@time[:retrieve]} sec -- size #{data.size} rows"
    push_series_data(data)
    return data.size
  end
  
  def graphic_data_xml
    raise "No data exists to graphic, check if data was retrieved" if @series_data.size == 0
    
    tdata = graphic_transform_data
    return xml_for_amchart(tdata)
  end

  
  private 
  
  def push_series_data(data)
    return @series_data.size if data.size == 0
    puts "push_series_data start..."
    @time[:push_data] = Time.now 
    
    last_metric_id = data.first.metric_id
    last_machine_id = data.first.machine_id
    serie_name = "#{data.first.machine_id}.#{data.first.name}"
    serie_data = []
    
    data.each do |row|
      if row.metric_id != last_metric_id || row.machine_id != last_machine_id then

        
        if @series_data[serie_name] != nil
          puts "last before"
          sz = @series_data[serie_name].size
          @series_data[serie_name][sz-10..sz-1].each {|r| puts "#{r.nanos}"}
          puts "end last before"
          puts "first 10 adding"
          serie_data[0..10].each {|r| puts "#{r.nanos}"}
          puts "first 10 ending"
        end
        
        @series_data[serie_name] = [] if @series_data[serie_name] == nil
        @series_data[serie_name] += serie_data
        serie_name = "#{row.machine_id}.#{row.name}"
        last_metric_id = row.metric_id
        last_machine_id = row.machine_id
        serie_data = []
      end
      serie_data << row
    end
    @series_data[serie_name] = [] if @series_data[serie_name] == nil
    @series_data[serie_name] += serie_data

    total = 0
    @series_data.each do |serie_name, serie_data|
      @series_nanos[serie_name] = { :min => Float(serie_data.first.nanos), :max => Float(serie_data.last.nanos)}
      total += serie_data.size
    end
    puts "Total records #{total}"
    
    @time[:push_data] = Time.now - @time[:push_data]
    puts "push_series_data duration: #{@time[:push_data]} sec"
    
    series_datacheck
    
    puts "DATA CACHED:#{$data_cache.size}"
    
    cache_data 
    puts "Series: #{@series_data.size}"
    return @series_data.size
  end
  
  def series_datacheck
    puts "data_check start..."
    @time[:check] = Time.now 
    @series_data.each do |serie_name, data|
#      puts "Serie: #{serie_name} has #{data.size} rows"
      maid=data[0].machine_id
      name=data[0].name
      prev_nano = data[0].nanos
      data.each do |row|
        raise "ERROR MACHINE ID OR NAME" if maid != row.machine_id || name != row.name
        raise "ERROR NANOS row=#{row.nanos} prev= #{prev_nano} [#{row.nanos}<#{prev_nano}] " if Float(row.nanos) < Float(prev_nano)
        prev_nano = row.nanos
      end
    end
    @time[:check] = Time.now - @time[:check]
    puts "series_datacheck duration: #{@time[:check]} sec"
  end
  
  def graphic_transform_data
    puts "graphic_transform_data start..."
    @time[:transform] = Time.now 
    
    puts "FIRST"
    min_nanos = @series_nanos[@series_nanos.keys[0]][:min]
    max_nanos = @series_nanos[@series_nanos.keys[0]][:max]
    @series_nanos.each do |k, v|
      #By now I put the interval between the maximum min nanos and the minimum max nanos
      #where I know there are points
      #TODO: Put interval from min min to max max 
      min_nanos = v[:min] if v[:min] > min_nanos
      max_nanos = v[:max] if v[:max] < max_nanos
    end
    
    transformed = {}
    transformed[:x_axis] = []
    
    delta = (max_nanos - min_nanos)/@graphic_points
    @graphic_points.times { |i| transformed[:x_axis] << (min_nanos + i * delta + delta / 2) / NANOS_IN_MILI }
    
    #TODO: Get each point before a mark and after a mark and ponder the value to put on it
    
    transformed[:series] = {}
    c=0
    @series_data.each do |serie_name,data|
      last_row = nil
      last_nanos = nil
      data_points = []
      index = 0
      data.each do |row|
        if last_row == nil
          last_row = row
          last_nanos = Float(row.nanos) / NANOS_IN_MILI
          next
        end
        nanos = Float(row.nanos) / NANOS_IN_MILI
#        puts "   --- ROW: nanos=#{nanos} -- value=#{row.value}"
        if nanos > transformed[:x_axis][index]
          while index < @graphic_points && nanos > transformed[:x_axis][index] do
            #Between last_value and row
            value = (row.value - last_row.value) / (nanos - last_nanos)
            value = value * @factor if @factor
            data_points << value
            index += 1
          end
          if index == @graphic_points
            break
          end
        end
        last_row = row
        last_nanos = nanos
      end
      transformed[:series][serie_name] = data_points
    end
    
    @time[:transform] = Time.now - @time[:transform]
    puts "graphic_transform_data duration: #{@time[:transform]} sec"
    transformed
  end
  
  
  def xml_for_amchart(chart)
    puts "xml_for_amchart start..."
    @time[:xml] = Time.now 
    xml = Builder::XmlMarkup.new
    
    x_axis = chart[:x_axis]
    data = chart[:series]

    xml.instruct! :xml, :version=>"1.0", :encoding=>"UTF-8"
    xml.chart do
      # xml.message "You can broadcast any message to chart from data XML file", :bg_color => "#FFFFFF", :text_color => "#000000"
      xml.series do
        x_axis.each_with_index do |value, index|
          xml.value  "%.3f" % value, :xid => index
        end
      end
      
      xml.graphs do
        data.each do |serie, values|
          xml.graph :gid => "#{serie}", :title => "#{serie[0..15]}"  do
            values.each_with_index do |nanos,i|
                xml.value "%.4f" % nanos, :xid => i #, :description => "#{serie[0..15]} [#{nanos} ms]"
            end
          end
        end
      end
    end
    @time[:xml] = Time.now - @time[:xml]
    puts "xml_for_amchart duration: #{@time[:xml]} sec"
    return xml.target!  
  end
  
  def cache_data
    $data_cache[@experiment.experiment_id] = {} if $data_cache[@experiment.experiment_id] == nil
    $data_cache[@experiment.experiment_id][@preset_metric] = { :last_beat => @last_heartbeat, :data => @series_data}
  end
  
  def recover_from_cache
    return nil if $data_cache[@experiment.experiment_id] == nil
    return nil if $data_cache[@experiment.experiment_id][@preset_metric] == nil

    puts "recover from cache started"
    @time[:recover] = Time.now
    
    @series_data = $data_cache[@experiment.experiment_id][@preset_metric][:data]
    @last_heartbeat = $data_cache[@experiment.experiment_id][@preset_metric][:last_beat]
    
    total = 0
    @series_data.each do |serie_name, serie_data|
      @series_nanos[serie_name] = { :min => Float(serie_data.first.nanos), :max => Float(serie_data.last.nanos)}
      total += serie_data.size
    end
    @time[:recover] = Time.now - @time[:recover]
    puts "xml_for_amchart duration: #{@time[:xml]} sec [#{total} rows]"
    return total
  end
end
