require File.dirname(__FILE__) + '/../test_helper'

class SandboxesControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:sandboxes)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_sandbox
    assert_difference('Sandbox.count') do
      post :create, :sandbox => { }
    end

    assert_redirected_to sandbox_path(assigns(:sandbox))
  end

  def test_should_show_sandbox
    get :show, :id => sandboxes(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => sandboxes(:one).id
    assert_response :success
  end

  def test_should_update_sandbox
    put :update, :id => sandboxes(:one).id, :sandbox => { }
    assert_redirected_to sandbox_path(assigns(:sandbox))
  end

  def test_should_destroy_sandbox
    assert_difference('Sandbox.count', -1) do
      delete :destroy, :id => sandboxes(:one).id
    end

    assert_redirected_to sandboxes_path
  end
end
