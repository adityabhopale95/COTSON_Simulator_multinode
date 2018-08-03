require 'test_helper'

class SimulationsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:simulations)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_simulation
    assert_difference('Simulation.count') do
      post :create, :simulation => { }
    end

    assert_redirected_to simulation_path(assigns(:simulation))
  end

  def test_should_show_simulation
    get :show, :id => simulations(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => simulations(:one).id
    assert_response :success
  end

  def test_should_update_simulation
    put :update, :id => simulations(:one).id, :simulation => { }
    assert_redirected_to simulation_path(assigns(:simulation))
  end

  def test_should_destroy_simulation
    assert_difference('Simulation.count', -1) do
      delete :destroy, :id => simulations(:one).id
    end

    assert_redirected_to simulations_path
  end
end
