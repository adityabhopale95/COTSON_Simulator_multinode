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

# $Id: commands_controller_test.rb 6977 2009-12-09 16:18:35Z frb $

require File.dirname(__FILE__) + '/../test_helper'

class CommandsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:commands)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_command
    assert_difference('Command.count') do
      post :create, :command => { }
    end

    assert_redirected_to command_path(assigns(:command))
  end

  def test_should_show_command
    get :show, :id => commands(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => commands(:one).id
    assert_response :success
  end

  def test_should_update_command
    put :update, :id => commands(:one).id, :command => { }
    assert_redirected_to command_path(assigns(:command))
  end

  def test_should_destroy_command
    assert_difference('Command.count', -1) do
      delete :destroy, :id => commands(:one).id
    end

    assert_redirected_to commands_path
  end
end
