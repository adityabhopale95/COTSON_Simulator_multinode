# Methods added to this helper will be available to all templates in the application.
module ApplicationHelper
  def start_corner
    '<div id="rightcolumn"><div class="innertube"><div class="buttonwrapper">'
  end
  def end_corner
    '</div></div></div>'
  end
  
  def button_link_to(title, options, html_options = nil)
    title="<span>#{title}</span>"    
    
    if html_options == nil
      html_options = {:class => "ovalbutton"}
    else
      html_options[:class] = "ovalbutton"
    end
    
    link_to title, options, html_options
  end
  
  
  def start_content
    '<div id="contentwrapper"><div id="contentcolumn"><div class="innertube">'
  end
  
  def end_content
    '</div></div></div>'
  end
end
