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

# $Id: sandboxes_controller.rb 6977 2009-12-09 16:18:35Z frb $

require 'cotson'
require 'computer_info'

class SandboxesController < ApplicationController
	skip_before_filter :verify_authenticity_token

	private
	def render_ok(data,errorcode=0)
		render :xml => { :errorcode => errorcode,
			:message => 'Ok',
			:data =>	data }
	end

	def render_error(output,message)
    if message.kind_of?(CotsonError) then
      formated_message=message.to_s.gsub(/\e/,"#ESC#")
      message=message.to_s.gsub(/\e[^m]+m/,"") #Take out escape sequences
      return    render :xml => { :errorcode => 1 ,
        :message => message,
        :output => output,
        :cotson_error => true,
        :formated_message =>formated_message}
    end
		render :xml => { :errorcode => 1 ,
			:message => message,
			:output => output}
	end

	def get_sandbox
		@sandbox_model = SandboxModel.find(params[:id])
		sandbox_location = @sandbox_model.sandbox
		sandbox=Sandbox.new(nil,nil,@sandbox_model.sandbox)
		sandbox.read_config
		sandbox
	end

	public
	# GET /sandboxes.xml
	def index
		render_ok(SandboxModel.find(:all))
	end

	# GET /sandboxes/1.xml
	def show
		begin 
			render_ok(SandboxModel.find(params[:id]))
		rescue StandardError => e
			render_error("","error executing 'show': #{e}")
		end
	end

	# POST /sandboxes.xml
	def create
		begin
			@sandbox_model = SandboxModel.new(params[:sandbox])

			my_args = []
			my_args << "exec_dir='#{@sandbox_model.exec_dir}'"
			@sandbox_model.configuration.split(/\s+/).each { |x| my_args << x }

			@lua_defs=LuaDefinitions.new :command_line => my_args, 
				:exec_dir => @sandbox_model.exec_dir
			@opts=Options.new(@lua_defs)
			@opts.load :cluster_nodes

			cluster_nodes=@opts[:cluster_nodes].to_i

			if @sandbox_model.mediator == true
 				@opts.load :mediator_file, :mediator_link,
					:subscribe_result, :clean_sandbox
			else
				@opts=Options.new(@lua_defs, :NODE => @sandbox_model.node_id, 
					:TOTAL => cluster_nodes)
				@lua_defs.instance_defaults :NODE => @sandbox_model.node_id, 
					:TOTAL => cluster_nodes
				@opts.load :data_dir, :display,
					:bsd, :bsd_link,
					:hdd, :hdd_link,
					:vncviewer, :xterm, :screen, :Xvnc, :Xvnc_args, :Xvncwm, 
					:simnow_dir, :simnow_dir_link,
					:abaeterno_so, :abaeterno_so_link,
					:user_script, :clean_sandbox,
					:subscribe_result
			end
			sandbox=Sandbox.new(@lua_defs,@opts)
			if @sandbox_model.mediator == true
				sandbox.install :mediator, :abaeterno_in
			else
				sandbox.install :abaeterno_in, :simnow_pre_cluster, 
					:simnow_dir, :abaeterno_so, :bsd, :hdd, 
					:user_script, :cluster_sh
			end

			@sandbox_model.sandbox = sandbox.to_str

			if @sandbox_model.save
				render_ok(@sandbox_model.id)
			else
				sandbox.clean
				render_error('',"error saving model: #{@sandbox_model.errors}")
			end

		rescue StandardError => e
			render_error('',e)
		end
	end

	# DELETE /sandboxes/1.xml
	def destroy
		begin 
			@sandbox_model.destroy
			get_sandbox.clean
			render_ok('')
		rescue StandardError => e
			render_error('',e)
		end
  end

  def ping
    render_ok('Ok')
  end

  #GET /sandboxes/1/get_load.xml
  def get_load
    begin 
      resp={}
      resp[:Info]=ComputerInfo.info
      resp[:Release]=ComputerInfo.release
      resp[:MemTotal]=ComputerInfo.memory[:MemTotal]
      resp[:MemFree]=ComputerInfo.memory[:MemFree]
      resp[:Processors]=ComputerInfo.processors
      resp[:Simulations]=ComputerInfo.ps('simnow').size
      render_ok(resp)
    rescue StandardError => e
      render_error("",e)
    end
  end

  #GET /sandboxes/1/get_mediator_value
	def get_mediator_value
		begin 
      o=get_sandbox.get_mediator_value(params[:key])
			render_ok(o)
		rescue StandardError => e
			render_error(o,e)
		end
	end

  def get_vnc_webport
    begin 
      o=get_sandbox.get_vnc_webport
      render_ok(o)
    rescue StandardError => e
      render_error(o,e)
    end
  end

  #GET /sandboxes/1/mediator.xml
	def mediator
		begin 
      get_sandbox.mediator
			render_ok('')
		rescue StandardError => e
			render_error('',e)
		end
	end

	#GET /sandboxes/1/kill.xml
	def kill
		begin 
      i,o=get_sandbox.kill
			render_ok(o,i == 255 ? i : 0)
		rescue StandardError => e
			render_error(o,e)
		end
	end

	#POST /sandboxes/1/set_mediator_value
	def set_mediator_value
		begin 
      get_sandbox.set_mediator_value(params[:key], params[:value])
			render_ok('')
		rescue StandardError => e
			render_error('',e)
		end
	end

	#POST /sandboxes/1/list_files
	def list_files
		begin 
      get_sandbox.list_files(params)
			render_ok('')
		rescue StandardError => e
			render_error('',e)
		end
	end

	#POST /sandboxes/1/copy_files
	def copy_files
		begin 
			get_sandbox.copy_files(params)
			render_ok('')
		rescue StandardError => e
			render_error('',e)
		end
	end

	#GET /sandboxes/1/start_go.xml
	def start_go
		begin 
			get_sandbox.start_go
			render_ok('')
		rescue StandardError => e
			render_error('',e)
		end
	end

  def stop
    begin 
      get_sandbox.stop
      render_ok('')
    rescue StandardError => e
      render_error('',e)
    end
  end

	#GET /sandboxes/1/status.xml
	def status
		begin 
			o=get_sandbox.status
      render_ok(o)
		rescue StandardError => e
			render_error(o,e)
		end
  end

  def consoleX
    begin
      o=get_sandbox.consoleX(:quiet => true)
      render_ok(o)
    rescue StandardError => e
      render_error(0,e)
    end
  end
end
