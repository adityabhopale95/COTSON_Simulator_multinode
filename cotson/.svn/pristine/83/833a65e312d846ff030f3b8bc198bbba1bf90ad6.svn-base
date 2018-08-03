require 'test_helper'

class HeartbeatsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:heartbeats)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_heartbeat
    assert_difference('Heartbeat.count') do
      post :create, :heartbeat => { }
    end

    assert_redirected_to heartbeat_path(assigns(:heartbeat))
  end

  def test_should_show_heartbeat
    get :show, :id => heartbeats(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => heartbeats(:one).id
    assert_response :success
  end

  def test_should_update_heartbeat
    put :update, :id => heartbeats(:one).id, :heartbeat => { }
    assert_redirected_to heartbeat_path(assigns(:heartbeat))
  end

  def test_should_destroy_heartbeat
    assert_difference('Heartbeat.count', -1) do
      delete :destroy, :id => heartbeats(:one).id
    end

    assert_redirected_to heartbeats_path
  end
end
