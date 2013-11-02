require 'rubygems'
require 'hoe'
require 'rake/extensiontask'


Hoe.spec 'netbase' do
  developer('Pannous GmbH', 'info@pannous.net')
  self.readme_file   = 'README.rdoc'
  self.history_file  = 'CHANGELOG.rdoc'
  self.extra_rdoc_files  = FileList['*.rdoc']
  self.extra_dev_deps << ['rake-compiler', '>= 0']
  self.spec_extras = { :extensions => ["ext/netbase/extconf.rb"] }

  Rake::ExtensionTask.new('netbase', spec) do |ext|
    ext.lib_dir = File.join('lib', 'netbase')
  end
end

Rake::Task[:test].prerequisites << :compile
