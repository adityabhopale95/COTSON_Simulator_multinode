require 'test_helper'

class MetricsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:metrics)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_metric
    assert_difference('Metric.count') do
      post :create, :metric => { }
    end

    assert_redirected_to metric_path(assigns(:metric))
  end

  def test_should_show_metric
    get :show, :id => metrics(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => metrics(:one).id
    assert_response :success
  end

  def test_should_update_metric
    put :update, :id => metrics(:one).id, :metric => { }
    assert_redirected_to metric_path(assigns(:metric))
  end

  def test_should_destroy_metric
    assert_difference('Metric.count', -1) do
      delete :destroy, :id => metrics(:one).id
    end

    assert_redirected_to metrics_path
  end
end
