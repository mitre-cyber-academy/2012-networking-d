bash "rbenv" do
    user "ubuntu"
    code <<-EOH
  	  git clone https://github.com/sstephenson/rbenv.git ~/.rbenv
      echo 'export PATH="$HOME/.rbenv/bin:$PATH"' >> ~/.bashrc
      echo 'eval "$(rbenv init -)"' >> ~/.bashrc
      source ~/.bashrc
    EOH
end
