require 'test_helper'

class ExperimentEventsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:experiment_events)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_experiment_event
    assert_difference('ExperimentEvent.count') do
      post :create, :experiment_event => { }
    end

    assert_redirected_to experiment_event_path(assigns(:experiment_event))
  end

  def test_should_show_experiment_event
    get :show, :id => experiment_events(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => experiment_events(:one).id
    assert_response :success
  end

  def test_should_update_experiment_event
    put :update, :id => experiment_events(:one).id, :experiment_event => { }
    assert_redirected_to experiment_event_path(assigns(:experiment_event))
  end

  def test_should_destroy_experiment_event
    assert_difference('ExperimentEvent.count', -1) do
      delete :destroy, :id => experiment_events(:one).id
    end

    assert_redirected_to experiment_events_path
  end
end
