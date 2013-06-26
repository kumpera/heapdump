#!/usr/bin/env ruby

require 'rubygems'
require 'json'

f1 = JSON.parse( IO.read(ARGV[0]))
f2 = JSON.parse( IO.read(ARGV[1]))

puts f1.count
puts f2.count

h1 = {}
f1.each {|k| h1[k["name"]] = k}

h2 = {}
f2.each {|k| h2[k["name"]] = k}


res = h2.collect do |k,v| 
  h1v = h1 [k]
  if h1v.nil?
    [ v['size'], "New type #{k} size #{v['size']} count #{v['count']}"]
  else
    [ v['size'] - h1v['size'], "Type #{k} size diff #{v['size'] - h1v['size']} count #{v['count'] - h1v['count']}" ]
  end
end

res.sort_by {|k| k[0] }.each {|k| puts k[1] }