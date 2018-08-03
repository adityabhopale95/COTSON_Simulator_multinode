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

# $Id: node_processes_controller_test.rb 6977 2009-12-09 16:18:35Z frb $

require File.dirname(__FILE__) + '/../test_helper'

class NodeProcessesControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:node_processes)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_node_process
    assert_difference('NodeProcess.count') do
      post :create, :node_process => { }
    end

    assert_redirected_to node_process_path(assigns(:node_process))
  end

  def test_should_show_node_process
    get :show, :id => node_processes(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => node_processes(:one).id
    assert_response :success
  end

  def test_should_update_node_process
    put :update, :id => node_processes(:one).id, :node_process => { }
    assert_redirected_to node_process_path(assigns(:node_process))
  end

  def test_should_destroy_node_process
    assert_difference('NodeProcess.count', -1) do
      delete :destroy, :id => node_processes(:one).id
    end

    assert_redirected_to node_processes_path
  end
end
