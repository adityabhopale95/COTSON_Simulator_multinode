require 'test_helper'

class ExperimentMachinesControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:experiment_machines)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_experiment_machines
    assert_difference('ExperimentMachines.count') do
      post :create, :experiment_machines => { }
    end

    assert_redirected_to experiment_machines_path(assigns(:experiment_machines))
  end

  def test_should_show_experiment_machines
    get :show, :id => experiment_machines(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => experiment_machines(:one).id
    assert_response :success
  end

  def test_should_update_experiment_machines
    put :update, :id => experiment_machines(:one).id, :experiment_machines => { }
    assert_redirected_to experiment_machines_path(assigns(:experiment_machines))
  end

  def test_should_destroy_experiment_machines
    assert_difference('ExperimentMachines.count', -1) do
      delete :destroy, :id => experiment_machines(:one).id
    end

    assert_redirected_to experiment_machines_path
  end
end
