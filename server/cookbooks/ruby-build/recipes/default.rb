package "libssl-dev" do
  action :install
end

package "gcc" do
  action :install
end

package "libreadline5-dev" do
  action :install
end

package "zlib1g-dev" do
  action :install
end

bash "ruby-build" do
    user "ubuntu"
    code <<-EOH
      git clone https://github.com/sstephenson/ruby-build.git ~/ruby-build
      cd $HOME/ruby-build
      sudo ./install.sh
      cd $HOME
      rm -rf $HOME/ruby-build
    EOH
end
