/*
	* Copyright (c) Rohde & Schwarz
	*
	* Licensed under the Apache License, Version 2.0 (the "License");
	* you may not use this file except in compliance with the License.
	* You may obtain a copy of the License at
	*
	*     http://www.apache.org/licenses/LICENSE-2.0
	*
	* Unless required by applicable law or agreed to in writing, software
	* distributed under the License is distributed on an "AS IS" BASIS,
	* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	* See the License for the specific language governing permissions and
	* limitations under the License.
*/

#include "constants.h"

#include <sstream>

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      const std::string Constants::XsltFileName = "open_IqTar_xml_file_in_web_browser.xslt";

      const std::string Constants::XmlRootNode = "RS_IQ_TAR_FileFormat";

      const std::string Constants::XmlApplicationName = "Name";

      const std::string Constants::XmlComment = "Comment";

      const std::string Constants::XmlDateTime = "DateTime";

      const std::string Constants::XmlDataFileName = "DataFilename";

      const std::string Constants::XmlFormat = "Format";

      const std::string Constants::XmlDataType = "DataType";

      const std::string Constants::XmlChannels = "NumberOfChannels";

      const std::string Constants::XmlSamples = "Samples";

      const std::string Constants::XmlClock = "Clock";

      const std::string Constants::XmlScalingFactor = "ScalingFactor";

      const std::string Constants::XmlUserData = "UserData";

      const std::string Constants::XmlRohdeSchwarz = "RohdeSchwarz";

      const std::string Constants::XmlCenterFrequency = "CenterFrequency";

      const std::string Constants::XmlChannelName = "ChannelName";

      const std::string Constants::XmlChannelNames = "ChannelNames";

      const std::string Constants::XmlMandatoryUserData = "DataImportExport_MandatoryData";

      const std::string Constants::XmlOptionalUserData = "DataImportExport_OptionalData";

      const std::string Constants::EndUserData = "DataImportExport_EndHeaderSection";

      const char Constants::SeparatorColon = ',';

      const char Constants::SeparatorSemiColon = ';';

      const char Constants::SeparatorDot = '.';

      const std::string Constants::CreationIdentifier = "saved by Rohde & Schwarz MOSAIK DataImportExport";

      std::string Constants::getXslt()
      {
        std::stringstream ss; 

		ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
		ss << "<!--" << endl;
		ss << "============================================================================" << endl;
		ss << "Copyright 2017-09-28 Rohde & Schwarz GmbH & Co. KG" << endl;
		ss << "" << endl;
		ss << "Licensed under the Apache License, Version 2.0 (the \"License\");" << endl;
		ss << "you may not use this file except in compliance with the License." << endl;
		ss << "You may obtain a copy of the License at" << endl;
		ss << "" << endl;
		ss << "  http://www.apache.org/licenses/LICENSE-2.0" << endl;
		ss << "" << endl;
		ss << "Unless required by applicable law or agreed to in writing, software" << endl;
		ss << "distributed under the License is distributed on an \"AS IS\" BASIS," << endl;
		ss << "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied." << endl;
		ss << "See the License for the specific language governing permissions and" << endl;
		ss << "limitations under the License." << endl;
		ss << "============================================================================" << endl;
		ss << "-->" << endl;
		ss << "" << endl;
		ss << "" << endl;
		ss << "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">" << endl;
		ss << "<xsl:output method=\"html\" doctype-system=\"about:legacy-compat\" indent=\"yes\"/>" << endl;
		ss << "" << endl;
		ss << "" << endl;
		ss << "<xsl:template match=\"/RS_IQ_TAR_FileFormat\">" << endl;
		ss << "" << endl;
		ss << "    <html>" << endl;
		ss << "      <head>" << endl;
		ss << "        <style>" << endl;
		ss << "body{font-family:Arial,sans-serif;background-color:white}#xmldata{display:none}table{empty-cells:show;table-layout:auto;margin-top:20px;margin-bottom:0}" << endl;
		ss << "th{background-color:#aeb5bb;border-style:none;padding:5px;font-size:large;text-align:left;vertical-align:top;font-weight:bold}" << endl;
		ss << "tbody tr:nth-child(even){background-color:#fff}tbody tr:nth-child(odd){background-color:#eff0f1}tr td:nth-child(1){font-weight:bold}" << endl;
		ss << "div.perDiv{font-size:small;font-weight:normal}tr td{border-style:solid;border-width:1px;border-color:#aeb5bb;padding:3px;vertical-align:top}" << endl;
		ss << "div.error{background-color:orangered;padding:2px}footer{font-size:smaller;border-top-style:solid;border-top-width:10px;border-top-color:#bfbfbf;padding-top:3px;margin-top:30px;padding-left:1px}" << endl;
		ss << "a{color:#008cda;text-decoration:none}" << endl;
		ss << "        </style>" << endl;
		ss << "" << endl;
		ss << "        <!-- Source code (c) by Rohde & Schwarz -->" << endl;
		ss << "        <!--<script src=\"RsIqTar.js\" ></script>-->" << endl;
		ss << "        <script>" << endl;
		ss << "        <xsl:text disable-output-escaping=\"yes\">" << endl;
		ss << "          <![CDATA[" << endl;
		ss << "function Iqtar(){var t=document.getElementsByTagName(\"RS_IQ_TAR_FILEFORMAT\")[0];this.fileFormatVersion=t.getAttribute(\"fileFormatVersion\");" << endl;
		ss << "this.Name=this.getChildValue(t,\"Name\");this.Comment=this.getChildValue(t,\"Comment\");this.DateTime=this.getChildValue(t,\"DateTime\");" << endl;
		ss << "this.Samples=parseInt(this.getChildValue(t,\"Samples\"));this.Clock=parseFloat(this.getChildValue(t,\"Clock\"));this.ClockUnit=this.getChildAttributeValue(t,\"Clock\",\"unit\");" << endl;
		ss << "this.Format=this.getChildValue(t,\"Format\");this.DataType=this.getChildValue(t,\"DataType\");this.ScalingFactor=parseFloat(this.getChildValue(t,\"ScalingFactor\"));" << endl;
		ss << "this.ScalingFactorUnit=this.getChildAttributeValue(t,\"ScalingFactor\",\"unit\");this.NumberOfChannels=parseInt(this.getChildValue(t,\"NumberOfChannels\"));" << endl;
		ss << "this.DataFilename=this.getChildValue(t,\"DataFilename\");this.UserData={RohdeSchwarz:{DataImportExport_MandatoryData:{},DataImportExport_OptionalData:{}}};" << endl;
		ss << "var e=t.getElementsByTagName(\"UserData\");if(e.length){var a=e[0].getElementsByTagName(\"RohdeSchwarz\");if(a.length){var i=a[0].getElementsByTagName(\"DataImportExport_MandatoryData\");" << endl;
		ss << "if(i.length)for(var r=i[0].children,n=0;n<r.length;n++)switch(r[n].tagName){case\"CHANNELNAMES\":var l=r[n].children;" << endl;
		ss << "this.UserData.RohdeSchwarz.DataImportExport_MandatoryData.ChannelNames=[];for(var h=0;h<l.length;h++)this.UserData.RohdeSchwarz.DataImportExport_MandatoryData.ChannelNames.push(l[h].innerText);" << endl;
		ss << "break;case\"CENTERFREQUENCY\":var s=parseFloat(r[n].innerText),o=\"\"+r[n].getAttribute(\"unit\"),d=\"\"+s+\" \"+o;\"HZ\"==o.toUpperCase()&&(d=GetFreqWithUnit(s)" << endl;
		ss << ");this.UserData.RohdeSchwarz.DataImportExport_MandatoryData.CenterFrequency=d;break;default:this.UserData.RohdeSchwarz.DataImportExport_MandatoryData[\"\"+r[n].tagName]=\"\"+r[n].innerText" << endl;
		ss << "}var m=a[0].getElementsByTagName(\"DataImportExport_OptionalData\");if(m.length)for(var c=m[0].getElementsByTagName(\"Key\"),v=0;" << endl;
		ss << "v<c.length;v++){var p=\"\"+c[v].getAttribute(\"name\"),f=\"\"+c[v].innerText;if(-1!==p.indexOf(\"[Hz]\")){p=p.substring(0,p.indexOf(\"[Hz]\"));" << endl;
		ss << "f=GetFreqWithUnit(parseFloat(f))}else if(-1!==p.indexOf(\"[dB]\")){p=p.substring(0,p.indexOf(\"[dB]\"));f+=\" dB\"}else if(-1!==p.indexOf(\"[dBm]\")){p=p.substring(0,p.indexOf(\"[dBm]\"));" << endl;
		ss << "f+=\" dBm\"}this.UserData.RohdeSchwarz.DataImportExport_OptionalData[p]=f}}}this.PreviewData={};this.PreviewData.Channel=[];" << endl;
		ss << "var g=t.getElementsByTagName(\"PreviewData\");if(g.length){var u=this.getChildrenByTagName(g[0],\"ArrayOfChannel\");if(u.length){var C=this.getChildrenByTagName(u[0],\"Channel\");" << endl;
		ss << "if(C.length)for(var y=0;y<C.length;y++){var D={};D.Name=this.getChildValue(C[y],\"Name\");D.Comment=this.getChildValue(C[y],\"Comment\");" << endl;
		ss << "D.PowerVsTime={};var b=this.getChildrenByTagName(C[y],\"PowerVsTime\");if(b.length){D.PowerVsTime.Min=this.getTrace(b[0],\"Min\");" << endl;
		ss << "D.PowerVsTime.Max=this.getTrace(b[0],\"Max\")}D.Spectrum={};var w=this.getChildrenByTagName(C[y],\"Spectrum\");if(b.length){D.Spectrum.Min=this.getTrace(w[0],\"Min\");" << endl;
		ss << "D.Spectrum.Max=this.getTrace(w[0],\"Max\")}D.IQ={};var T=this.getChildrenByTagName(C[y],\"IQ\");if(T.length){var N=T[0].getElementsByTagName(\"Histogram\");" << endl;
		ss << "if(N.length){D.IQ.width=parseInt(N[0].getAttribute(\"width\"));D.IQ.height=parseInt(N[0].getAttribute(\"height\"));D.IQ.histo=N[0].innerText" << endl;
		ss << "}}this.PreviewData.Channel.push(D)}}}}Iqtar.prototype.getChildrenByTagName=function(t,e){for(var a=[],i=t.children,r=0;r<i.length;r++)i[r].tagName==e.toUpperCase()&&a.push(i[r]);" << endl;
		ss << "return a};Iqtar.prototype.getChildValue=function(t,e){var a=\"\",i=t.getElementsByTagName(e);i.length&&(a=i[0].innerText);return a};" << endl;
		ss << "Iqtar.prototype.getChildAttributeValue=function(t,e,a){var i=\"\",r=t.getElementsByTagName(e);r.length&&(i=r[0].getAttribute(a));return i};" << endl;
		ss << "Iqtar.prototype.getTrace=function(t,e){var a=[],i=t.getElementsByTagName(e);if(i.length){var r=i[0].getElementsByTagName(\"ArrayOfFloat\");" << endl;
		ss << "if(r.length)for(var n=r[0].children,l=0;l<n.length;l++)a.push(parseFloat(n[l].innerText))}return a};Iqtar.prototype.toHtml=function(){document.title=decodeURI(\"\"+window.location.pathname.split(\"/\").pop());" << endl;
		ss << "var t=\"\";t+=\"<h1>\"+document.title+\" (of iq-tar file)</h1>\";t+='<table class=\"Top\" >';t+='<thead><tr><th colspan=\"2\" >Description</th></tr></thead>';t+=\"<tbody>\";if(this.Name){t+=\"<tr>\";" << endl;
		ss << "t+=\"<td>Saved by</td>\";t+=\"<td>\"+this.Name+\"</td>\";t+=\"</tr>\"}if(this.Comment){t+=\"<tr>\";t+=\"<td>Comment</td>\";t+=\"<td>\"+this.Comment+\"</td>\";t+=\"</tr>\"}if(this.DateTime){t+=\"<tr>\";" << endl;
		ss << "t+=\"<td>Date &amp; Time</td>\";t+=\"<td>\"+this.DateTime.replace(\"T\",\"&nbsp;&nbsp;\")+\"</td>\";t+=\"</tr>\"}t+=\"<tr>\";t+=\"<td>Sample rate</td>\";var e=0;if(\"HZ\"==(\"\"+this.ClockUnit).toUpperCase()){e=this.Clock;" << endl;
		ss << "t+=\"<td>\"+GetFreqWithUnit(e)+\"</td>\"}else t+=\"<td>\"+this.Clock+\" \"+this.ClockUnit+\"</td>\";t+=\"</tr>\";t+=\"<tr>\";t+=\"<td>Number of samples</td>\";t+=\"<td>\"+this.Samples+\"</td>\";t+=\"</tr>\";var a=0;" << endl;
		ss << "if(this.Clock>0&&\"HZ\"==(\"\"+this.ClockUnit).toUpperCase()){a=this.Samples/this.Clock;t+=\"<tr>\";t+=\"<td>Duration of signal</td>\";t+=\"<td>\";t+=GetDurationWithUnit(a);t+=\"</td>\";t+=\"</tr>\"}t+=\"<tr>\";" << endl;
		ss << "t+=\"<td>Data format</td>\";t+=\"<td>\"+this.Format+\", \"+this.DataType+\"</td>\";t+=\"</tr>\";if(this.DataFilename){t+=\"<tr>\";t+=\"<td>Data filename</td>\";t+=\"<td>\"+this.DataFilename+\"</td>\";" << endl;
		ss << "t+=\"</tr>\"}t+=\"<tr>\";t+=\"<td>Scaling factor</td>\";if(\"V\"==(\"\"+this.ScalingFactorUnit).toUpperCase()){var i=this.ScalingFactor;" << endl;
		ss << "t+=Math.abs(i)>1e3?\"<td>\"+i/1e3+\" kV</td>\":Math.abs(i)<.001?\"<td>\"+1e6*i+\" uV</td>\":Math.abs(i)<1?\"<td>\"+1e3*i+\" mV</td>\":\"<td>\"+i+\" V</td>\"}else t+=\"<td>\"+this.ScalingFactor+\" \"+this.ScalingFactorUnit+\"</td>\";" << endl;
		ss << "t+=\"</tr>\";if(this.NumberOfChannels>1){t+=\"<tr>\";t+=\"<td>Number of channels</td>\";t+=\"<td>\"+this.NumberOfChannels+\"</td>\";t+=\"</tr>\"}t+=\"</tbody>\";t+=\"</table>\";" << endl;
		ss << "var r=this.UserData.RohdeSchwarz.DataImportExport_MandatoryData;if(Object.keys(r).length>0){t+='<table class=\"Top\" >';t+=\"<tbody>\";" << endl;
		ss << "t+='<tr><th colspan=\"2\" class=\"ChHd\">DataImportExport_MandatoryData</th></tr>';for(var n in r)if(r.hasOwnProperty(n)){var l=r[n];t+=\"<tr>\";t+=\"<td>\"+n+\"</td>\";" << endl;
		ss << "if(\"[object Array]\"===Object.prototype.toString.call(l)){t+=\"<td>\";for(var h=0;h<l.length;h++)t+=\"<div>\"+l[h]+\"</div>\";t+=\"</td>\"}else t+=\"<td>\"+l+\"</td>\";t+=\"</tr>\"}t+=\"</tbody>\";" << endl;
		ss << "t+=\"</table>\"}var s=this.UserData.RohdeSchwarz.DataImportExport_OptionalData;if(Object.keys(s).length>0){t+='<table class=\"Top\" >';t+=\"<tbody>\";t+='<tr><th colspan=\"2\" class=\"ChHd\">DataImportExport_OptionalData</th></tr>';" << endl;
		ss << "for(var n in s)if(s.hasOwnProperty(n)){var l=s[n];t+=\"<tr>\";t+=\"<td>\"+n+\"</td>\";t+=\"<td>\"+l+\"</td>\";t+=\"</tr>\"}t+=\"</tbody>\";t+=\"</table>\"}if(this.PreviewData&&this.PreviewData.Channel)" << endl;
		ss << "for(var o=0;o<this.PreviewData.Channel.length;o++){t+='<table class=\"Top\" >';t+=\"<tbody>\";t+='<tr><th colspan=\"2\" class=\"ChHd\">';" << endl;
		ss << "t+=this.PreviewData.Channel[o].Name?this.PreviewData.Channel[o].Name:\"Channel \"+(o+1);t+=\"</th></tr>\";if(this.PreviewData.Channel[o].Comment){t+=\"<tr>\";t+=\"<td>Comment</td>\";" << endl;
		ss << "t+=\"<td>\"+this.PreviewData.Channel[o].Comment+\"</td>\";t+=\"</tr>\"}if(this.PreviewData.Channel[o].PowerVsTime){t+=\"<tr>\";t+='<td><div>Power vs time</div><div class=\"perDiv\" id=\"divLabelpvt'+o+'\" /></td>';" << endl;
		ss << "t+='<td><div id=\"divpvt'+o+'\" ></div></td>';t+=\"</tr>\"}if(this.PreviewData.Channel[o].Spectrum){t+=\"<tr>\";t+='<td><div>Spectrum</div><div class=\"perDiv\" id=\"divLabelspec'+o+'\" /></td>';" << endl;
		ss << "t+='<td><div id=\"divspec'+o+'\" ></div></td>';t+=\"</tr>\"}if(this.PreviewData.Channel[o].IQ){t+=\"<tr>\";t+='<td><div>I/Q</div><div class=\"perDiv\" id=\"divLabeliq'+o+'\" /></td>';" << endl;
		ss << "t+='<td><div id=\"diviq'+o+'\" ></div></td>';t+=\"</tr>\"}t+=\"</tbody>\";t+=\"</table>\"}t+=\"<footer>\";t+='<div>E-mail: <a href=\"mailto:info@rohde-schwarz.com\">info@rohde-schwarz.com</a></div>';" << endl;
		ss << "t+='<div>Internet: <a href=\"http://www.rohde-schwarz.com\" >http://www.rohde-schwarz.com</a></div>';this.fileFormatVersion&&(t+=\"<div>Fileformat version: \"+this.fileFormatVersion+\"</div>\");" << endl;
		ss << "document.getElementById(\"AddJavaScriptGeneratedContentHere\").innerHTML=t;if(this.PreviewData&&this.PreviewData.Channel)for(var o=0;o<this.PreviewData.Channel.length;o++)" << endl;
		ss << "{this.PreviewData.Channel[o].PowerVsTime&&drawPreview(\"pvt\"+o,this.PreviewData.Channel[o].PowerVsTime.Min,this.PreviewData.Channel[o].PowerVsTime.Max,a);" << endl;
		ss << "this.PreviewData.Channel[o].Spectrum&&drawPreview(\"spec\"+o,this.PreviewData.Channel[o].Spectrum.Min,this.PreviewData.Channel[o].Spectrum.Max,e);" << endl;
		ss << "this.PreviewData.Channel[o].IQ&&drawIqPreview(\"iq\"+o,this.PreviewData.Channel[o].IQ)}};function drawPreview(t,e,a,i){if(e.length==a.length&&e.length>0)" << endl;
		ss << "{var r=e.length,n=r/2,l=document.createElement(\"canvas\");l.setAttribute(\"width\",r+1);l.setAttribute(\"height\",n+1);l.setAttribute(\"id\",t);document.getElementById(\"div\"+t).appendChild(l);" << endl;
		ss << "var h=l.getContext(\"2d\"),s=e.min(),o=a.max(),d=.025,m=o-s;s-=d/(1-2*d)*m;o+=d/(1-2*d)*m;m=o-s;isNaN(s)&&(s=-150);isNaN(o)&&(o=50);var c=0,v=.5*n;if(o>s){c=1/(s-o)*n;v=-c*o}h.strokeStyle=\"#AEB5BB\";h.fillStyle=\"#AEB5BB\";" << endl;
		ss << "var p=getPerDivision(s,o),f=\"\";if(p>0){f=\"<div>y-axis: \"+p+\" dB /div</div>\";for(var g=Math.ceil(s/p)*p;o>g;g+=p)h.fillRect(.5,g*c+v-.5,r,1)}var u=\"\";if(0==t.search(\"pvt\")){var C=getPerDivision(0,i);" << endl;
		ss << "if(C>0){u=\"<div>x-axis: \"+GetTimeWithUnit(C)+\" /div</div>\";for(var y=e.length/i,D=C;i>D;D+=C)h.fillRect(y*D,.5,1,n)}}else if(0==t.search(\"spec\")){var C=getPerDivision(-.5*i,.5*i);" << endl;
		ss << "if(C>0){u=\"<div>x-axis: \"+GetFreqWithUnit(C)+\" /div</div>\";for(var y=e.length/i,b=.5*e.length,D=Math.ceil(-.5*i/C)*C;.5*i>D;D+=C)h.fillRect(y*D+b,.5,1,n)}}h.strokeStyle=\"#0000FF\";h.fillStyle=\"#0000FF\";" << endl;
		ss << "for(var w=0;w<e.length;w++){var T=e[w],N=a[w];isNaN(T)&&(T=s);isNaN(N)&&(N=o);h.fillRect(w,N*c+v,1,(T-N)*c)}h.strokeStyle=\"#000000\";h.fillStyle=\"#000000\";h.strokeRect(.5,.5,r,n);" << endl;
		ss << "document.getElementById(\"divLabel\"+t).innerHTML=\"<br/>\"+f+u}else if(0==a.length||0==e.length);else{var F=\"\";F+='<div class=\"error\">Error: Min and Max preview traces have bad lengths ('+e.length+\", \"+a.length+\").</div>\";" << endl;
		ss << "document.getElementById(\"div\"+t).innerHTML=F}}function drawIqPreview(t,e){if(e.histo.length==e.width*e.height&&e.histo.length>0){var a=2,i=a*e.width,r=a*e.height,n=document.createElement(\"canvas\");" << endl;
		ss << "n.setAttribute(\"width\",i);n.setAttribute(\"height\",r);n.setAttribute(\"id\",t);document.getElementById(\"div\"+t).appendChild(n);for(var l=n.getContext(\"2d\"),h=0,s=0;r>s;s+=a)for(var o=0;i>o;o+=a)" << endl;
		ss << "{var d=e.histo.charAt(h++);switch(d){case\"0\":break;case\"1\":l.fillStyle=\"#E3E3FF\";l.fillRect(o,s,a,a);break;case\"2\":l.fillStyle=\"#C6C6FF\";l.fillRect(o,s,a,a);break;case\"3\":l.fillStyle=\"#AAAAFF\";" << endl;
		ss << "l.fillRect(o,s,a,a);break;case\"4\":l.fillStyle=\"#8E8EFF\";l.fillRect(o,s,a,a);break;case\"5\":l.fillStyle=\"#7171FF\";l.fillRect(o,s,a,a);break;case\"6\":l.fillStyle=\"#5555FF\";l.fillRect(o,s,a,a);break;" << endl;
		ss << "case\"7\":l.fillStyle=\"#3939FF\";l.fillRect(o,s,a,a);break;case\"8\":l.fillStyle=\"#1C1CFF\";l.fillRect(o,s,a,a);break;default:l.fillStyle=\"#0000FF\";l.fillRect(o,s,a,a)}l.strokeStyle=\"#000000\";" << endl;
		ss << "l.fillStyle=\"#000000\";l.strokeRect(0,0,i,r)}}else{var m=\"\";m+='<div class=\"error\">Error: I/Q preview has incorrect length ('+e.histo.length+\" != \"+e.width+\" * \"+e.height+\").</div>\";" << endl;
		ss << "document.getElementById(\"div\"+t).innerHTML=m}}function getPerDivision(t,e){var a=0,i=0,r=0;if(e>t){var n=e-t;if(n>0){var l=14,h=Math.log(n/l)/Math.LN10;r=Math.floor(h);var s=h-r;i=0;if(.3>=s)i=2;else if(.69>=s)i=5;" << endl;
		ss << "else{i=1;r+=1}a=i*Math.pow(10,r)}}return a}function GetTimeWithUnit(t){var e=\"\";e+=t>=1?NiceNo(t)+\" s\":t>=.001?NiceNo(1e3*t)+\" ms\":t>=1e-6?NiceNo(1e6*t)+\" us\":t+\" s\";return e}" << endl;
		ss << "function GetDurationWithUnit(t){var e=\"\",a=31536e3,i=Math.floor(t/a);if(i>0){e+=i+\" year\";i>1&&(e+=\"s\");e+=\"&nbsp;&nbsp;&nbsp;\";t-=i*a}a=86400;i=Math.floor(t/a);if(i>0){e+=i+\" day\";i>1&&(e+=\"s\");" << endl;
		ss << "e+=\"&nbsp;&nbsp;&nbsp;\";t-=i*a}a=3600;i=Math.floor(t/a);if(i>0){e+=i+\" h&nbsp;&nbsp;&nbsp;\";t-=i*a}a=60;i=Math.floor(t/a);if(i>0){e+=i+\" min&nbsp;&nbsp;&nbsp;\";t-=i*a}e+=GetTimeWithUnit(t);" << endl;
		ss << "e=e.replace(/(&nbsp;)+$/,\"\");return e}function NiceNo(t){var e=3,a=\"\"+t.toFixed(e);a=a.replace(/[0]+$/,\"\");a=a.replace(/[.]$/,\"\");return a}function GetFreqWithUnit(t){var e=\"\";" << endl;
		ss << "e+=t>=1e9?t/1e9+\" GHz\":t>=1e6?t/1e6+\" MHz\":t>=1e3?t/1e3+\" kHz\":t+\" Hz\";return e}Array.prototype.max=function(){return Math.max.apply({},this)};Array.prototype.min=function(){return Math.min.apply({},this)};          " << endl;
		ss << "          ]]>" << endl;
		ss << "        </xsl:text>" << endl;
		ss << "        </script>" << endl;
		ss << "" << endl;
		ss << "" << endl;
		ss << "      <script>" << endl;
		ss << "        window.onload = function()" << endl;
		ss << "        {" << endl;
		ss << "          // Generate html from xml data" << endl;
		ss << "          var obj = new Iqtar();" << endl;
		ss << "" << endl;
		ss << "          // Render html preview document" << endl;
		ss << "          obj.toHtml();" << endl;
		ss << "" << endl;
		ss << "          // Uncomment for debugging" << endl;
		ss << "          console.log(\"FINE @ \" + Date().toString() );" << endl;
		ss << "        };" << endl;
		ss << "      </script>" << endl;
		ss << "      </head>" << endl;
		ss << "      <body>" << endl;
		ss << "" << endl;
		ss << "        <!-- Embedding original iq-tar xml content -->" << endl;
		ss << "        <xml id=\"xmldata\">" << endl;
		ss << "          <xsl:copy-of select=\"/RS_IQ_TAR_FileFormat\" />" << endl;
		ss << "        </xml>" << endl;
		ss << "" << endl;
		ss << "        <div id=\"AddJavaScriptGeneratedContentHere\"></div>" << endl;
		ss << "" << endl;
		ss << "      </body>" << endl;
		ss << "    </html>" << endl;
		ss << "" << endl;
		ss << "  </xsl:template>" << endl;
		ss << "" << endl;
		ss << "</xsl:stylesheet>" << endl;

        return ss.str();
      }
    }
  }
}