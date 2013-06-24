#!/usr/bin/env ruby
# Upload .FIT files to Garmin Connect
#
# Based on http://github.com/livioso/GarminFitFilesUploader

require 'io/console'
require 'mechanize'
require 'pp'

SIGNIN_URL='https://connect.garmin.com/signin'
UPLOAD_URL='http://connect.garmin.com/api/upload/widget/manualUpload.faces?uploadServiceVersion=1.1'

if ARGV.length < 1
  puts "Usage: #$0 <.FIT file> ..."
  exit 1
end

# Prompt user for credentials
print "Garmin username: "
username = STDIN.gets.chomp

print "Garmin password: "
password = STDIN.noecho(&:gets).chomp
puts

agent = Mechanize.new { |a| a.follow_meta_refresh = true }

# Sign into Garmin Connect
agent.get(SIGNIN_URL) do |signin|
  home = signin.form_with(:name => 'login') do |form|
    form['login:loginUsernameField'] = username
    form['login:password'] = password
  end.submit

  ARGV.each do |fit_file|
    puts "Processing #{fit_file} ..."

    if not File.exist? fit_file
      STDERR.puts "Cannot find #{fit_file}, skipping."
    end

    agent.transact do
      # Upload a single fit file
      agent.get(UPLOAD_URL) do |upload|
        res = upload.form_with(:id => 'uploadForm') do |form|
          # they do some "fancy" javascript stuff to ensure the uploaded file
          # is valid (not too big, correct file format etc.) but in the end
          # they just POST it to to the URL below :-)
          form.action = '/proxy/upload-service-1.1/json/upload/.fit'
          form.file_uploads.first.file_name = fit_file
        end.submit

        pp res
        pp res.code
        pp res.response
      end
    end
  end
end
