ActionController::Routing::Routes.draw do |map|
  map.resources :sandbox_models
  map.resources :sandboxes
  map.resources :commands
  map.resources :node_processes
  
  #External experiments database controlers
  map.resources :metrics
  map.resources :heartbeats
  map.resources :metric_names
  map.resources :parameters
  map.resources :experiments
  
  map.connect '/experiments/:experiment_id/graphics/:action', :controller => "graphics" 
  
  map.root :controller => "main"
  
  # The priority is based upon order of creation: first created -> highest priority.

  # Sample of regular route:
  #   map.connect 'products/:id', :controller => 'catalog', :action => 'view'
  # Keep in mind you can assign values other than :controller and :action

  # Sample of named route:
  #   map.purchase 'products/:id/purchase', :controller => 'catalog', :action => 'purchase'
  # This route can be invoked with purchase_url(:id => product.id)

  # Sample resource route (maps HTTP verbs to controller actions automatically):
  #   map.resources :products

  # Sample resource route with options:
  #   map.resources :products, :member => { :short => :get, :toggle => :post }, :collection => { :sold => :get }
  get_list = [:get_mediator_value, :kill, :mediator, 
              :start_go, :status, :get_vnc_webport, :consoleX, :stop]
  post_list = [:set_mediator_value, :copy_files, :list_files]

  map.connect 'get_load', :controller => "sandboxes", :action => "get_load"
  map.connect 'ping', :controller => "sandboxes", :action => "ping"

  get_list.each { |g| map.resources :sandboxes, :member => { g => :get } }
  post_list.each { |p| map.resources :sandboxes, :member => { p => :post } }
  
#  map.resources :sandboxes, :check_regression => { :check_regression => :get }
#  map.resources :sandboxes, :clean => { :clean => :post }
#  map.resources :sandboxes, :console => { :console => :post }
#  map.resources :sandboxes, :functional => { :functional => :post }
#  map.resources :sandboxes, :gdb => { :gdb => :post }
#  map.resources :sandboxes, :go => { :go => :post }
#  map.resources :sandboxes, :injector => { :injector => :get }
#  map.resources :sandboxes, :list_consoles => { :list_consoles => :get }
#  map.resources :sandboxes, :list_vncs => { :list_vncs => :get }
#  map.resources :sandboxes, :ngo => { :ngo => :post }
#  map.resources :sandboxes, :regression => { :regression => :post }
#  map.resources :sandboxes, :run => { :run => :post }
#  map.resources :sandboxes, :send_command => { :send_command => :post }
#  map.resources :sandboxes, :start => { :start => :post }
#  map.resources :sandboxes, :stop => { :stop => :post }
#  map.resources :sandboxes, :view => { :view => :post }
  
  # Sample resource route with sub-resources:
  #   map.resources :products, :has_many => [ :comments, :sales ], :has_one => :seller

  # Sample resource route within a namespace:
  #   map.namespace :admin do |admin|
  #     # Directs /admin/products/* to Admin::ProductsController (app/controllers/admin/products_controller.rb)
  #     admin.resources :products
  #   end

  # You can have the root of your site routed with map.root -- just remember to delete public/index.html.
  # map.root :controller => "welcome"

  # See how all your routes lay out with "rake routes"

  # Install the default routes as the lowest priority.
  map.connect ':controller/:action/:id'
  map.connect ':controller/:action/:id.:format'
end
