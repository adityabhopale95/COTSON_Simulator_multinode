require 'test_helper'

class MetricNamesControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:metric_names)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_metric_name
    assert_difference('MetricName.count') do
      post :create, :metric_name => { }
    end

    assert_redirected_to metric_name_path(assigns(:metric_name))
  end

  def test_should_show_metric_name
    get :show, :id => metric_names(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => metric_names(:one).id
    assert_response :success
  end

  def test_should_update_metric_name
    put :update, :id => metric_names(:one).id, :metric_name => { }
    assert_redirected_to metric_name_path(assigns(:metric_name))
  end

  def test_should_destroy_metric_name
    assert_difference('MetricName.count', -1) do
      delete :destroy, :id => metric_names(:one).id
    end

    assert_redirected_to metric_names_path
  end
end
