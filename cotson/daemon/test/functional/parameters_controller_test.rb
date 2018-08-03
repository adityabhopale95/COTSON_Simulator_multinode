require 'test_helper'

class ParametersControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:parameters)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_parameter
    assert_difference('Parameter.count') do
      post :create, :parameter => { }
    end

    assert_redirected_to parameter_path(assigns(:parameter))
  end

  def test_should_show_parameter
    get :show, :id => parameters(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => parameters(:one).id
    assert_response :success
  end

  def test_should_update_parameter
    put :update, :id => parameters(:one).id, :parameter => { }
    assert_redirected_to parameter_path(assigns(:parameter))
  end

  def test_should_destroy_parameter
    assert_difference('Parameter.count', -1) do
      delete :destroy, :id => parameters(:one).id
    end

    assert_redirected_to parameters_path
  end
end
