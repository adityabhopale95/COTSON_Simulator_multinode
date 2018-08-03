require File.dirname(__FILE__) + '/../test_helper'

class SandboxModelsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:sandbox_models)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_sandbox_model
    assert_difference('SandboxModel.count') do
      post :create, :sandbox_model => { }
    end

    assert_redirected_to sandbox_model_path(assigns(:sandbox_model))
  end

  def test_should_show_sandbox_model
    get :show, :id => sandbox_models(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => sandbox_models(:one).id
    assert_response :success
  end

  def test_should_update_sandbox_model
    put :update, :id => sandbox_models(:one).id, :sandbox_model => { }
    assert_redirected_to sandbox_model_path(assigns(:sandbox_model))
  end

  def test_should_destroy_sandbox_model
    assert_difference('SandboxModel.count', -1) do
      delete :destroy, :id => sandbox_models(:one).id
    end

    assert_redirected_to sandbox_models_path
  end
end
