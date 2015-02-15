require 'sinatra'
require 'json'

USERNAME    = 'jamesbond'
PASSWORD    = 'goldfinger'
USER_AGENT  = '007'
FLAG        = 'MCA-FCA7DE76'
TROLL       = 'http://is.gd/uMcZgS'

helpers do

  def protected!
    unless authorized?
      response['WWW-Authenticate'] = %(Basic realm="Restricted Area")
      throw(:halt, [401, "Not authorized\n"])
    end
  end

  def authorized?
    @auth ||=  Rack::Auth::Basic::Request.new(request.env)
    @auth.provided? && 
    @auth.basic? && 
    @auth.credentials && 
    @auth.credentials == [USERNAME, PASSWORD]
  end

end


get '/' do
  haml :index
end

post '/agents/login.json' do
  protected!
  content_type :json
  
  json = {}

  begin
    JSON.parse(request.env["rack.input"].read)
    if request.user_agent == USER_AGENT
      status 200
      json = { flag: FLAG, award: TROLL }.to_json
    else
      status 403  
      json = { error: "Invalid User Agent" }
    end
  rescue
    status 406
    json = { error: 'Invalid POST body' }
  end 
  
  json.to_json

end