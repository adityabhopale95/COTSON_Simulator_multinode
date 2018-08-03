#!/usr/bin/ruby -w
# (C) Copyright 2009 Hewlett-Packard Development Company, L.P.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#

# $Id: daemon_service.rb 6977 2009-12-09 16:18:35Z frb $

$: << File.expand_path(File.dirname(__FILE__) + 
	'/../daemon/vendor/rails/activesupport/lib/')
require 'active_support'
require 'net/http'

class DaemonServiceCall
	private 
	def process_response(r)
		if !r.kind_of? Net::HTTPSuccess then
      raise CotsonError.new(:DaemonProcessResponse, :response_type=>"#{r.class}",:host=>@host, :port=>@port, :body=>r.body )  
		end
		result = Hash.from_xml(r.body)
		@result=result['hash']
		if !@result || @result['errorcode'] != 0 then
      msg= @result['formated_message'] ? @result['formated_message'].gsub("#ESC#","\e") : @result['message']
      raise CotsonError.new(:DaemonProcessResponse, :errorcode=>@result['errorcode'], :message=>msg , :body=>r.body, :host=>@host, :port=>@port)
		end
	end

	def parms_to_query(parms={})
		q=parms.map do |k,v| 
			v!=nil ?
			"%s=%s" % [URI.escape(k.to_s), URI.escape(v.to_s)] :
			""
		end.join('&')
		q != "" ? "?#{q}" : ""
	end

	def get(path,parms={})
    begin 
  		query=parms_to_query(parms)
  		req = Net::HTTP::Get.new("#{path}#{query}")
  		r = Net::HTTP.start(@host, @port) {|http| http.request(req) }
  		process_response(r)
    rescue Exception => e
      raise CotsonError.new(:DaemonCall, :exception=>e, :type=>:get, :host=>@host, :port=>@port, :message=>"#{e}")
    end
    
		@result	
        end

	def post(path,parms={})
    begin 
		req = Net::HTTP::Post.new(path)
		req.set_form_data(parms)
		r = Net::HTTP.start(@host, @port) {|http| http.request(req) }
		process_response(r)
    rescue Exception => e
      raise CotsonError.new(:DaemonCall, :exception=>e, :type=>:post, :host=>@host, :port=>@port, :message=>"#{e}")
    end

    @result	
        end

	def delete(path)
    begin 
		req = Net::HTTP::Delete.new(path)
		r = Net::HTTP.start(@host, @port) {|http| http.request(req) }
		process_response(r)
    rescue Exception => e
      raise CotsonError.new(:DaemonCall, :exception=>e, :type=>:delete, :host=>@host, :port=>@port, :message=>"#{e}")
    end

    @result	
        end

	public
	def initialize(server,id=nil)
		url = URI.parse("http://#{server}")
		@host=url.host
		@port=url.port
		@id=id
		@result = nil
	end

	def create_mediator(configuration, exec_dir)
		@id=post("/sandboxes.xml",
			'sandbox[mediator]'=>1, 
			'sandbox[exec_dir]'=>exec_dir,
			'sandbox[configuration]'=>configuration)['data']
		return @id
	end

	def create_node(node_id, configuration, exec_dir)
		@id=post("/sandboxes.xml",
			'sandbox[mediator]'=>0, 
			'sandbox[exec_dir]'=>exec_dir,
			'sandbox[configuration]'=>configuration,
			'sandbox[node_id]'=>node_id)['data']
		return @id
	end

	def index
		get("/sandboxes.xml")['data']
	end

	def show
		get("/sandboxes/#{@id}.xml")['data']
	end
  
	def clean
		begin
			delete("/sandboxes/#{@id}.xml")
		rescue # clean should not fail at all
			debug2("clean failed to remove sandbox #{@id}")
		end
	end

	def mediator
		get("/sandboxes/#{@id}/mediator.xml")
	end

  def consoleX
    get("/sandboxes/#{@id}/consoleX.xml")
  end

  def start_go
		get("/sandboxes/#{@id}/start_go.xml")
  end

  def stop
    get("/sandboxes/#{@id}/stop.xml")
  end

	def kill
		get("/sandboxes/#{@id}/kill.xml")
	end

	def status 
		get("/sandboxes/#{@id}/status.xml")['data']
	end

  def get_load 
    get("/get_load")['data']
  end

  def ping
    get("/ping")['data']
  end

  def get_mediator_value(key=nil)
		get("/sandboxes/#{@id}/get_mediator_value.xml", 'key' => key)['data']
  end

  def get_vnc_webport
    get("/sandboxes/#{@id}/get_vnc_webport.xml")['data']
  end

	def set_mediator_value(key, value)
		post("/sandboxes/#{@id}/set_mediator_value.xml",
			'key' => key, 'value' => value)
	end
  
	def list_files(opts)
		post("/sandboxes/#{@id}/list_files.xml", opts)['data']
	end

	def copy_files (opts)
		post("/sandboxes/#{@id}/copy_files.xml", opts)
        end

  def id
    return @id
  end
  
  def to_s
    "#{@host}:#{@port}<#{@id}>"
  end
end
