#!/usr/local/bin/ruby
require "cgi"
require 'net/http'

RELAYPORT	= 9999
RELAYHOST	= "localhost"

#Instantiate an html 4 CGI object
cgi = CGI.new("html4Tr")

#return page formated by type.
page_type = CGI::escapeHTML(cgi['type']) #html, sour, gedcom, repo, search, TA, TD, TDN, rel,index,indexfS,surname

#index based on search by.
firstname_target  = CGI::escapeHTML(cgi['Firstname']) #find entries with these firstnames
surname_target  = CGI::escapeHTML(cgi['Surname']) #find entries with this surname
match_target = CGI::escapeHTML(cgi['Match']) #Match against any part of name.

depth = CGI::escapeHTML(cgi['depth']) #for trees, TA, TD, TDN, rel
depth = "" if depth == nil 

target = CGI::escapeHTML(cgi['target'])
record = CGI::escapeHTML(cgi['record']) #alternate for target
target = record if target == nil || target == ""

second_target = CGI::escapeHTML(cgi['relative'])
record2 = CGI::escapeHTML(cgi['record2'])
second_target = record2 if second_target == nil || second_target == ""

case page_type
  when "html"; @query = "/html/#{target}"
  when "sour"; @query = "/sour/#{target}"
  when "repo"; @query = "/repo/#{target}"
  when "ged"; @query = "/ged/#{target}"
  when "search"; 
    if surname_target != nil && surname_target != ""
      @query = "/search?Surname=#{surname_target}"
    elsif firstname_target != nil && firstname_target != ""
      @query = "/search?Firstname=#{firstname_target}"
    elsif match_target != nil && match_target != ""
      @query = "/search?Match=#{match_target}"
    else
      @query = "/index/index_A"
    end
  when "TA"; @query = "/TA#{depth}/#{target}"
  when "TD"; @query = "/TD#{depth}/#{target}"
  when "TDN"; @query = "/TDN#{depth}/#{target}"
  when "rel"; @query = "/rel#{depth}/#{target}/#{second_target}"
  when "index"; @query = "/index/#{target}"
  when "surname"; @query = "/surname/#{target}" #Index of Those with just a surname (different format)
  when "indexf"; @query = "/indexf/#{target}"
  when "surnamef"; @query = "/surnamef/#{target}" #Index of Those with just a firstname (different format)
  else  @query = "/index/index_A"
end
  
begin
  Net::HTTP.start(RELAYHOST, RELAYPORT) do |http| 
    @response = http.get(@query) 
    #puts response.body 
  end
end

cgi.header("type"=>"text/html")
cgi.out do
  @response.body
  #cgi.html do
    #cgi.head{ cgi.title{"Test Response"} + "<META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\n" }  +
    #cgi.body { "<b>#{@query}<p>" }
  #end
end


