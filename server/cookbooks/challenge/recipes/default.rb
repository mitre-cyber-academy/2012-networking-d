bash "initd" do
  user "root"
  code <<-EOH
    
    # move initd
    mv /home/ubuntu/chef-solo/cookbooks/challenge/aux/init_d_script /etc/init.d/challenge
    chmod 755 /etc/init.d/challenge
    
    # register it with boot sequence
    update-rc.d challenge defaults 98 02
    
  EOH
end

bash "challenge" do
  user "ubuntu"
  code <<-EOH
    
    # all challenge files
    mkdir $HOME/challenge
    mv /home/ubuntu/chef-solo/cookbooks/challenge/aux/* $HOME/challenge
    mv /home/ubuntu/chef-solo/cookbooks/challenge/aux/.rbenv-version $HOME/challenge
    
    # setup ruby
    PATH="/home/ubuntu/.rbenv/bin:$PATH"
    eval "$(rbenv init -)"
    cat $HOME/challenge/.rbenv-version | xargs rbenv install
    rbenv rehash
    cat $HOME/challenge/.rbenv-version | xargs rbenv global
    gem install bundler --no-rdoc --no-ri
    rbenv rehash
    
    # setup challenge
    cd $HOME/challenge
    bundle install --deployment --without development test
    rbenv rehash
    
    #start the service
    sudo /etc/init.d/challenge start
    
  EOH
end
